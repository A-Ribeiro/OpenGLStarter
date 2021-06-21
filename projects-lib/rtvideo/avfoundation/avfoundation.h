#ifndef av_foundation_h__
#define av_foundation_h__

#include <aribeiro/aribeiro.h>

struct AF_DeviceFramerate
{
    double fps;
    uint32_t   numerator;
    uint32_t   denominator;
};

struct AF_DeviceResolution {
    int width;
    int height;
    std::vector<AF_DeviceFramerate> framerates;
    
    std::string codec_code;
};

class AF_Device {
public:
    bool muxed;
    std::string name;
    std::vector<AF_DeviceResolution> resolutions;
    
    
    static std::vector<AF_Device> listDevices();
    
};

void AF_startStream(const std::string &device_name,
                    int width, int height, const std::string &codec_code, double fps,
                    void (*callback)(const uint8_t *data, size_t size, void* userData), void* userData );

#endif
