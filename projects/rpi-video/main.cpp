#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include "openmax/BroadcomCamera.h"
#include "openmax/BroadcomH264Decode.h"
#include "openmax/BroadcomH264Encode.h"

#include "openmax/BroadcomAACEncode.h"

#include "v4l2/v4l2.h"


#include <signal.h>

volatile bool exit_requested = false;
void signal_handler(int signal) {
    exit_requested = true;
    //
    // Force internal loops to exit nicely
    //
    OMX::Instance()->appExitRequested = true;
}
//
// Show info v4l2-ctl
//
// list parameters
// v4l2-ctl --device=/dev/video0 --all
//
// list resolutions and framerates
// v4l2-ctl --device=/dev/video0 --list-formats-ext
//
// Command line:
//
// ./rpi-video | cvlc - --demux h264
//
// mkfifo h264.fifo
// ./rpi-video > h264.fifo
// cvlc h264.fifo --demux h264
//
// cvlc v4l2:///dev/video0:chroma=h264:width=320:height=240 --demux h264
//
// mkfifo h264.fifo
// ./rpi-video > h264.fifo & omxplayer --adev hdmi h264.fifo --video_queue 1 --threshold 1 --win 0,0,640,360
//
int main(int argc, char* argv[]) {
	PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));

	/* AAC HW Encoder from RPI does not work...

	signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);

    BroadcomAACEncode encoder(44100, 128);


    while (!exit_requested) {

        encoder.postPCM16bitSignedInterleaved(NULL,0);

        encoder.makeOutBufferAvailable();

        PlatformSleep::sleepMillis(40);

    }


    signal(SIGINT,  SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

	/*/

	// VIdeo input from RPI...
    signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);

	Device device;
	v4l2::loadDeviceInfo("/dev/video0",&device);

	if ( !(device.capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) ){
        fprintf(stderr,"device is not a video capture device.\n");
        exit(-1);
	}

	if ( !(device.capability.capabilities & V4L2_CAP_STREAMING) ){
        fprintf(stderr,"device doesn't support streaming.\n");
        exit(-1);
	}

	v4l2_fmtdesc formatDescription;
    if (! device.queryPixelFormat(V4L2_PIX_FMT_H264, &formatDescription) ){
    //if (! device.queryPixelFormat(V4L2_PIX_FMT_MJPEG, &formatDescription) ){
        fprintf(stderr,"device doesn't support H264 reading.\n");
        exit(-1);
    }

    v4l2_frmsizeenum res;
    if (!device.queryNearResolutionForFormat(formatDescription, 1280, 720, &res)){
    //if (!device.queryNearResolutionForFormat(formatDescription, 176, 144, &res)){
        fprintf(stderr,"could not query resolution.\n");
        exit(-1);
    }

    v4l2_frmivalenum interval;
    if (!device.queryNearInterval(res, 30.0f, &interval)){
        fprintf(stderr,"could not query interval of 24 fps.\n");
        exit(-1);
    }

    fprintf(stderr,"%s -> %s\n", device.path.c_str(), device.capability.card );
    fprintf(stderr,"  %s", fcc2s(formatDescription.pixelformat).c_str() );
    fprintf(stderr," / %u x %u", res.discrete.width, res.discrete.height );
    fprintf(stderr," @ %s fps\n", fract2fps( interval.discrete ).c_str() );


    device.open();

    //v4l2_queryctrl ctrl;
    //if ( device.queryControlByName("Brightness", &ctrl ) ){
        //device.setCtrlValue(ctrl, 128);
    //}


    device.printControls();
	//fgetc(stdin);


    device.setFormat(formatDescription, res, interval);

    // buffer allocation and information retrieve
    const int BUFFER_COUNT = 3; // max 32

    device.setNumberOfInputBuffers(BUFFER_COUNT);
    v4l2_buffer bufferInfo[BUFFER_COUNT];
    void* bufferPtr[BUFFER_COUNT];
    for(int i=0;i<BUFFER_COUNT;i++) {
        bufferInfo[i] = device.getBufferInformationFromDevice(i);
        bufferPtr[i] = device.getBufferPointer(bufferInfo[i]);
        memset(bufferPtr[i], 0, bufferInfo[i].length);
        device.queueBuffer(i, &bufferInfo[i]);
    }

    //main loop
    device.streamON();

    int fd_stdout = fileno(stdout);
    v4l2_buffer bufferQueue;

    while (!exit_requested){
        device.dequeueBuffer(&bufferQueue);

        //fprintf(stderr,"index: %i\n",bufferQueue.index);

        // cannot use AUD on this kind of stream....
        // internally it can deliver different kinds of pkt sequences...
        // -- it is safer to detect new frame using the DetectionClass
        //
        //char h264_aud[] = {0x00,0x00,0x00,0x01,0x09};
        //write(fd_stdout,h264_aud,sizeof(h264_aud));

        //output to stdout
        write(fd_stdout,bufferPtr[bufferQueue.index],bufferQueue.bytesused);

        //unsigned char* buffer = (unsigned char*)bufferPtr[bufferQueue.index];
        //static NALu nalu[20];
        //unsigned int nSize = FindAllNALu(buffer, bufferQueue.bytesused, nalu);
        //for (unsigned int i = 0; i < nSize; i++) {
        //    void * pStart = &buffer[nalu[i].pos];
        //    write(fd_stdout,pStart,nalu[i].size);
        //};

        device.queueBuffer(bufferQueue.index, &bufferQueue);
    }

    device.streamOFF();


    device.close();

    signal(SIGINT,  SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

/* /
    signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);

    BroadcomH264Encode encoder(1280, 720, 25, 2700);


    while (!exit_requested) {

        encoder.postYUV(NULL,0);

        encoder.makeOutBufferAvailable();

        PlatformSleep::sleepMillis(40);

    }


    signal(SIGINT,  SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

//* /


    int fd_stdin = fileno(stdin);

    signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);

    BroadcomH264Decode decoder;

    unsigned char buffer[65536];

    while (!exit_requested) {

        int readedSize = read(fd_stdin,buffer,65536);
        decoder.writeH264Buffer(buffer,readedSize);

    }

    signal(SIGINT,  SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);


//*/

	return 0;
}
