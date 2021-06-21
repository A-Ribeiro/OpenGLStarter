#ifndef RTAudioOutput__H
#define RTAudioOutput__H

#include <rtaudio/RtAudio.h>
#include "AudioResampler.h"
namespace rtaudioWrapper {

    template <typename T>
    class _PoolThreadSafe {

        struct Element{
            bool available;
            T *data;
        };

        aRibeiro::PlatformMutex mutex;

        std::vector<Element> list;


        //std::vector<T*> used;
        //std::vector<T*> available;
        int pos;
        int posRelease;
        int count;

        bool released;
    public:

        _PoolThreadSafe() {
            released = false;
            pos = 0;
            posRelease = 0;
            count = 0;
        }

        virtual ~_PoolThreadSafe(){
            mutex.lock();
            for(size_t i=0;i<list.size();i++){
                //avoid linux address sanitizer wrong releasing count
                if (list[i].available)
                    new (list[i].data) T();
                delete list[i].data;
            }
            list.clear();

            //used.clear();
            //available.clear();

            released = true;
            mutex.unlock();
        }

        T* create() {
            mutex.lock();
            count++;

            ARIBEIRO_ABORT(released, "ERROR: trying to create element from a deleted pool");
            int listSize = (int)list.size();
            for(int i=0;i<listSize;i++){
                int index = (i + pos) % listSize;
                if (list[index].available){
                    list[index].available = false;
                    pos = (index + 1) % listSize;

                    //placement new operator
                    new (list[index].data) T();

                    mutex.unlock();
                    return list[index].data;
                }
            }
            Element newElement;
            newElement.available = false;
            newElement.data = new T();
            list.push_back(newElement);
            pos = 0;
            mutex.unlock();

            printf("Pool size: %lu count: %i\n", list.size(), count);
            return newElement.data;
        }

        void release( T* data ) {
            mutex.lock();
            ARIBEIRO_ABORT(released, "ERROR: trying to release element from a deleted pool");
            int listSize = (int)list.size();
            for(int i=0;i<listSize;i++){
                int index = (i + posRelease) % listSize;
                if (list[index].data == data){
                    list[index].available = true;
                    posRelease = (index + 1) % listSize;
                    count--;

                    //placement delete operator
                    list[index].data->~T();

                    mutex.unlock();
                    return;
                }
            }
            mutex.unlock();
        }
    };


    #include <queue>

    template <typename T>
    class _QueueThreadSafe {
        aRibeiro::PlatformMutex mutex;

        std::queue<T> list;
    public:

        void enqueue(const T &v) {
            mutex.lock();
            list.push(v);
            mutex.unlock();
        }

        int size() {
            int result;
            mutex.lock();
            result = list.size();
            mutex.unlock();
            return result;
        }

        T peek() {
            mutex.lock();
            if (list.size() > 0) {
                T result = list.front();
                //list.erase(list.begin());
                mutex.unlock();
                return result;
            }
            mutex.unlock();
            return T();
        }

        T dequeue() {
            mutex.lock();
            if (list.size() > 0) {
                T result = list.front();
                list.pop();
                //list.erase(list.begin());
                mutex.unlock();
                return result;
            }
            mutex.unlock();
            return T();
        }
    };

    struct FloatFrames {
        std::vector<float> data;
        int current;

        FloatFrames() {
            current = 0;
        }

    };

    template<typename C>
    class RTAudioOutput {

    public:

        RTAUDIO_OnAudioDataMethodPtrT OnData;

    private:
        RtAudio audioBUS;
        uint32_t bufferFrames;

        uint32_t samplerateRequested;
        uint32_t realSamplerate;
        uint32_t realChannel;

        //in the case we need to resample the buffer from the hardware
        //AudioResampler<C> audioResampler;

        rtaudioWrapper::_PoolThreadSafe<FloatFrames> audioFramesPool;
        rtaudioWrapper::_QueueThreadSafe<FloatFrames*> audioFramesQueue;

        bool wait_2_samples_on_buffer;

        static int outputWrite(void * outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData)
        {
            RTAudioOutput *rtAudioOutput = (RTAudioOutput *)userData;
            if (rtAudioOutput->endStream_called)
                return 2;

            int framesWritten = 0;

            float* outputBufferf = (float*)outputBuffer;

            FloatFrames *frames = NULL;
            if (rtAudioOutput->wait_2_samples_on_buffer) {
                if (rtAudioOutput->audioFramesQueue.size() >= 2) {
                    rtAudioOutput->wait_2_samples_on_buffer = false;
                    frames = rtAudioOutput->audioFramesQueue.peek();
                }
            }
            else
                if (rtAudioOutput->audioFramesQueue.size() > 0)
                    frames = rtAudioOutput->audioFramesQueue.peek();

            while (frames != NULL) {
                while ((frames != NULL) && framesWritten < nBufferFrames) {
                    outputBufferf[framesWritten * 2 + 0] = frames->data[frames->current * 2 + 0];
                    outputBufferf[framesWritten * 2 + 1] = frames->data[frames->current * 2 + 1];

                    frames->current++;
                    framesWritten++;

                    if (frames->current >= (frames->data.size() / 2)) {
                        rtAudioOutput->audioFramesPool.release(rtAudioOutput->audioFramesQueue.dequeue());
                        frames = NULL;
                        if (rtAudioOutput->audioFramesQueue.size() > 0)
                            frames = rtAudioOutput->audioFramesQueue.peek();
                    }
                }
                if (framesWritten >= nBufferFrames)
                    break;
            }

            while (framesWritten < nBufferFrames) {
                outputBufferf[framesWritten * 2 + 0] = 0;
                outputBufferf[framesWritten * 2 + 1] = 0;
                framesWritten++;
            }

            if (frames == NULL)
                rtAudioOutput->wait_2_samples_on_buffer = true;

            return 0;
        }

        bool initialized;
        volatile bool endStream_called;

    public:

        RTAudioOutput(RtAudio::Api api = RtAudio::UNSPECIFIED) :
            audioBUS(api)
        {
            initialized = false;
            endStream_called = false;
            wait_2_samples_on_buffer = true;

            samplerateRequested = 0;
            realSamplerate = 0;
        }

        bool isStreaming() {
            if (!initialized)
                return false;

            return audioBUS.isStreamRunning();
        }

        void endStream() {
            endStream_called = true;

            if (!initialized)
                return;

            while (audioBUS.isStreamRunning()) {
                PlatformSleep::sleepMillis(100);
            }
            if (audioBUS.isStreamOpen()) audioBUS.closeStream();
        }

        void writeBufferStereo(const float* data, int frames) {

            FloatFrames *frameToWrite = audioFramesPool.create();

            frameToWrite->data.resize(frames * 2);
            frameToWrite->current = 0;
            memcpy(&frameToWrite->data[0], data, frames * 2 * sizeof(float));

            audioFramesQueue.enqueue(frameToWrite);
        }

        void initOutputFromDeviceName(const std::string &devicename,
            uint32_t _bufferFrames = 1024,
            RtAudioFormat format = RTAUDIO_FLOAT32,
            uint32_t samplerate = 44100,
            int channels = 2,
            int offset = 0) {

            if (initialized)
                return;
            initialized = true;

            this->OnData = OnData;

            samplerateRequested = samplerate;

            int count = audioBUS.getDeviceCount();
            bool found = false;
            RtAudio::DeviceInfo info;
            int deviceID = 0;
            for (int i = 0; i < count; i++) {
                info = audioBUS.getDeviceInfo(i);
                if (info.name.compare(devicename) == 0) {
                    found = true;
                    deviceID = i;
                    break;
                }
            }

            ARIBEIRO_ABORT(
                !found,
                "cannot find the device: %s\n", devicename.c_str());

            //if (!found) {
            //    fprintf(stderr, "cannot find the device: %s\n", devicename.c_str());
            //    exit(1);
            //}


            // Let RtAudio print messages to stderr.
            audioBUS.showWarnings(true);

            RtAudio::StreamParameters oParams;
            oParams.deviceId = deviceID;
            oParams.nChannels = channels;
            oParams.firstChannel = offset;


            bufferFrames = _bufferFrames;

            try {
                audioBUS.openStream(&oParams, NULL, format, samplerate, &bufferFrames,
                    &RTAudioOutput::outputWrite,
                    (void *)this);
            }
            catch (RtAudioError& e) {
                endStream();

                ARIBEIRO_ABORT(
                    true,
                    "%s\n", e.getMessage().c_str());

                //std::cout << '\n' << e.getMessage() << '\n' << std::endl;
                //exit(1);
            }

            realSamplerate = audioBUS.getStreamSampleRate();
            realChannel = channels;
            printf("[RTAudioOutput] need resample: %i requested: %u opened: %i \n", samplerateRequested != realSamplerate, samplerateRequested, realSamplerate);
            /*
            if (samplerateRequested != realSamplerate) {
                audioResampler.init(
                    format != RTAUDIO_FLOAT32 && format != RTAUDIO_FLOAT64,
                    bufferFrames, channels, realSamplerate, samplerateRequested,
                    OnData);
            }
            */

            try {
                audioBUS.startStream();
            }
            catch (RtAudioError& e) {
                endStream();

                ARIBEIRO_ABORT(
                    true,
                    "%s\n", e.getMessage().c_str());

                //std::cout << '\n' << e.getMessage() << '\n' << std::endl;
                //exit(1);
            }

        }
    };
}

#endif
