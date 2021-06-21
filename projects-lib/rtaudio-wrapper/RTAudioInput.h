#ifndef RTAudioInput__H
#define RTAudioInput__H

#include <rtaudio/RtAudio.h>
#include "AudioResampler.h"
namespace rtaudioWrapper {
    /// \brief This class is a wrapper to the rtaudio library (input mode)
    ///
    /// It opens the audio device for reading and call the registered callback method/function.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <rtaudio-wrapper/rtaudio-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace rtaudioWrapper;
    ///
    /// ...
    ///
    /// void OnDataFromAudioCard(const void* samples, uint32_t frames){
    ///     ...
    /// }
    ///
    /// RTAudioInput<float> rtaudio_input;
    ///
    /// rtaudio_input.initInputFromDeviceName(
    ///     "Microfone (HD Pro Webcam C920)", // devicename
    ///     1024, // _bufferFrames
    ///     RTAUDIO_FLOAT32, // format
    ///     44100, // samplerate
    ///     2, //channels
    ///     0, //offset
    ///     &OnDataFromAudioCard, // RTAUDIO_OnAudioDataMethodPtrT
    /// );
    ///
    /// ...
    ///
    /// rtaudio_input.endStream();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template<typename C>
    class RTAudioInput {

    public:

        RTAUDIO_OnAudioDataMethodPtrT OnData;

    private:
        RtAudio audioBUS;
        uint32_t bufferFrames;

        uint32_t samplerateRequested;
        uint32_t realSamplerate;
        uint32_t realChannel;

        //in the case we need to resample the buffer from the hardware
        AudioResampler<C> audioResampler;

        static int inputRead(void * /*outputBuffer*/, void *inputBuffer, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *userData)
        {
            RTAudioInput *rtAudioInput = (RTAudioInput *)userData;
            if (rtAudioInput->endStream_called)
                return 2;
            //printf("readed frames: %i\n",nBufferFrames);

            if (rtAudioInput->samplerateRequested != rtAudioInput->realSamplerate) {
                //resample
                const C* fbuffer = (C*)inputBuffer;
                for (int i = 0; i < nBufferFrames; i++) {
                    rtAudioInput->audioResampler.putSample(&fbuffer[i*(rtAudioInput->realChannel)]);
                }
            }
            else
                rtAudioInput->OnData(inputBuffer, nBufferFrames);

            return 0;
        }

        bool initialized;
        volatile bool endStream_called;

    public:

        /// \brief Initialize this instance with an audio API
        ///
        /// The supported audio APIs are: RtAudio::UNSPECIFIED, RtAudio::LINUX_ALSA,<br />
        /// RtAudio::LINUX_PULSE, RtAudio::LINUX_OSS, RtAudio::UNIX_JACK,<br />
        /// RtAudio::MACOSX_CORE, RtAudio::WINDOWS_WASAPI, RtAudio::WINDOWS_ASIO,<br />
        /// RtAudio::WINDOWS_DS
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <rtaudio-wrapper/rtaudio-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace rtaudioWrapper;
        ///
        /// ...
        ///
        /// // initialize direct sound
        /// RTAudioInput<float> rtaudio_input(RtAudio::WINDOWS_DS);
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param api the RtAudio:: api
        ///
        RTAudioInput(RtAudio::Api api = RtAudio::UNSPECIFIED) :
            audioBUS(api)
        {
            initialized = false;
            endStream_called = false;

            samplerateRequested = 0;
            realSamplerate = 0;
        }

        /// \brief Check if the current audio input is setup and running
        ///
        /// \author Alessandro Ribeiro
        /// \return true if the streaming thread is running
        ///
        bool isStreaming() {
            if (!initialized)
                return false;

            return audioBUS.isStreamRunning();
        }

        /// \brief Closes the current running stream
        ///
        /// \author Alessandro Ribeiro
        ///
        void endStream() {
            endStream_called = true;

            if (!initialized)
                return;

            while (audioBUS.isStreamRunning()) {
                PlatformSleep::sleepMillis(100);
            }
            if (audioBUS.isStreamOpen()) audioBUS.closeStream();
        }

        /// \brief Initialize this audio input with the specified parameters
        ///
        /// This method handle the stream resample automatically by checking<br />
        /// the samplerate of the created input stream.
        ///
        /// \author Alessandro Ribeiro
        /// \param devicename the device to open as read
        /// \param _bufferFrames number of samples in the input callback
        /// \param format the internal data format
        /// \param samplerate the audio device input sample rate
        /// \param channels the audio device input number of channels
        /// \param offset the audio device input channel offset
        /// \param OnData callback. Will call this with the configured: #_bufferFrames, #samplerate, #channels and #format
        ///
        void initInputFromDeviceName(const std::string &devicename,
            uint32_t _bufferFrames = 1024,
            RtAudioFormat format = RTAUDIO_FLOAT32,
            uint32_t samplerate = 44100,
            int channels = 2,
            int offset = 0,
            const RTAUDIO_OnAudioDataMethodPtrT &OnData = NULL) {
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

            RtAudio::StreamParameters iParams;
            iParams.deviceId = deviceID;
            iParams.nChannels = channels;
            iParams.firstChannel = offset;


            bufferFrames = _bufferFrames;

            try {
                audioBUS.openStream(NULL, &iParams, format, samplerate, &bufferFrames,
                    &RTAudioInput::inputRead,
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
            printf("[RTAudioInput] need resample: %i requested: %u opened: %i \n", samplerateRequested != realSamplerate, samplerateRequested, realSamplerate);
            if (samplerateRequested != realSamplerate) {
                audioResampler.init(
                    format != RTAUDIO_FLOAT32 && format != RTAUDIO_FLOAT64,
                    bufferFrames, channels, realSamplerate, samplerateRequested,
                    OnData);
            }

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