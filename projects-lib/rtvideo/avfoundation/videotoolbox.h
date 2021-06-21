#ifndef videotoolbox__h__
#define videotoolbox__h__

#include "avfoundation.h"
#include <rtvideo/RTVideoInput.h>

#import <VideoToolbox/VideoToolbox.h>
#import <CoreFoundation/CoreFoundation.h>

extern float __hidden__mac_encode_to_callback_time;

class VTEncoder {
    
    //aRibeiro::PlatformTime time;
    
    void (*callback)(const uint8_t *data, size_t size, void* userData);
    void* userData;
    
    static void didCompressH264(void *outputCallbackRefCon, void *sourceFrameRefCon,
                         OSStatus status, VTEncodeInfoFlags infoFlags,
                                CMSampleBufferRef sampleBuffer );
public:
    dispatch_queue_t commandQueue;
    VTCompressionSessionRef encodeSession;
    int  frameCount;
    
    VTEncoder(int width, int height,
              void (*_callback)(const uint8_t *data, size_t size, void* userData),
              void* _userData
              );
    
    virtual ~VTEncoder();
    
    void encode(CMSampleBufferRef sampleBuffer);
    
};

#endif
