#ifndef ___rtvideoinput__H__
#define ___rtvideoinput__H__

#include <aribeiro/aribeiro.h>

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dshow.h>

#elif defined(__linux__)
    #include <rtvideo/v4l2.h>
#endif

class RTVideoInputDeviceFramerate {
public:
#if defined(WIN32)
    REFERENCE_TIME  AvgTimePerFrame;   // Average time per frame (100ns units)
#elif defined(__linux__)
    v4l2_frmivalenum _v4l2_frmivalenum;
#endif
    float fps;
    uint32_t   numerator;
	uint32_t   denominator;
};

class RTVideoInputDeviceResolution {
public:
#ifdef __linux__
    v4l2_frmsizeenum _v4l2_frmsizeenum;
#elif defined(__APPLE__)
    std::string codec_code;
#endif
    uint32_t width;
    uint32_t height;
    std::vector<RTVideoInputDeviceFramerate> framerates;
};

class RTVideoInputDevice {
public:
#if defined(WIN32)
    std::wstring originalDeviceName;
    std::wstring devicePath;
#elif defined(__linux__)
    Device device;
    v4l2_fmtdesc h264_v4l2_fmtdesc;
#endif
    std::string name;
    std::vector<RTVideoInputDeviceResolution> resolutions;
};


class RTVideoInput {

    RTVideoInputDevice *selectedDevice;
    RTVideoInputDeviceResolution *selectedResolution;
    RTVideoInputDeviceFramerate *selectedFramerate;

    aRibeiro::aribeiro_OnDataMethodPtrType OnData;
    aRibeiro::PlatformThread streamingThread;

    void streamingThreadRun();
    
    RTVideoInputDeviceResolution *queryNearResolution(RTVideoInputDevice *device,
                                                      uint32_t width,
                                                      uint32_t height);
    
    RTVideoInputDeviceFramerate *queryNearInterval(RTVideoInputDeviceResolution *resolution,
                                                   float fps);
    
public:

    static void printDevices();

    std::vector<RTVideoInputDevice> &listDevices();

    RTVideoInput();
    virtual ~RTVideoInput();

    void open(const std::string deviceName,
              uint32_t width, uint32_t height,
              float FPS,
              aRibeiro::aribeiro_OnDataMethodPtrType OnData);
    void close();

};


#endif
