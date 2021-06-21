#include "videotoolbox.h"

float __hidden__mac_encode_to_callback_time = 0.0f;

void VTEncoder::didCompressH264(void *outputCallbackRefCon, void *sourceFrameRefCon,
                     OSStatus status, VTEncodeInfoFlags infoFlags,
                     CMSampleBufferRef sampleBuffer ) {
    
    if (!CMSampleBufferDataIsReady(sampleBuffer))
        return;
    
    VTEncoder *encoder = (VTEncoder*)outputCallbackRefCon;
    
    //encoder->time.update();
    //if (encoder->time.deltaTime > __hidden__mac_encode_to_callback_time)
        //__hidden__mac_encode_to_callback_time = encoder->time.deltaTime;
    
    bool keyframe = !CFDictionaryContainsKey( (CFDictionaryRef)(CFArrayGetValueAtIndex(CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, true), 0)), kCMSampleAttachmentKey_NotSync);
    
    if (keyframe)
    {
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
        
        size_t spsSize, spsCount;
        const uint8_t *sps;
        OSStatus statusCode =
        CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 0,
                                                           &sps, &spsSize, &spsCount, 0 );
        if (statusCode == noErr)
        {
            size_t ppsSize, ppsCount;
            const uint8_t *pps;
            OSStatus statusCode =
            CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 1,
                                                               &pps, &ppsSize, &ppsCount, 0 );
            if (statusCode == noErr)
            {
                // Found sps/pps
                uint32_t annexbStart = 0x01000000;//CFSwapInt32HostToBig(0x00000001);
                encoder->callback((uint8_t*)&annexbStart,4,encoder->userData);
                encoder->callback((uint8_t*)sps,spsSize,encoder->userData);
                
                encoder->callback((uint8_t*)&annexbStart,4,encoder->userData);
                encoder->callback((uint8_t*)pps,ppsSize,encoder->userData);
                
            }
        }
    }
    
    CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    size_t length, totalLength;
    char *dataPointer;
    OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, &length, &totalLength, &dataPointer);
    if (statusCodeRet == noErr) {
        
        uint32_t annexbStart = 0x01000000;//CFSwapInt32HostToBig(0x00000001);
        
        uint32_t size = CFSwapInt32BigToHost(*(uint32_t*)dataPointer);
        uint32_t offset = 4;
        
        while (offset < totalLength) {
            
            encoder->callback((uint8_t*)&annexbStart,4,encoder->userData);
            encoder->callback((uint8_t*)&dataPointer[offset],size,encoder->userData);
            
            offset += size;
            
            if (offset < totalLength){
                size = CFSwapInt32BigToHost(*(uint32_t*)&dataPointer[offset]);
                offset += 4;
            }
        }
        
    }
    
}
    
VTEncoder::VTEncoder(int width, int height,
              void (*_callback)(const uint8_t *data, size_t size, void* userData),
              void* _userData
          ) {
    this->callback = _callback;
    this->userData = _userData;
    frameCount = 0;
    commandQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    encodeSession = NULL;
    
    dispatch_sync(commandQueue, ^{
        OSStatus status = VTCompressionSessionCreate(NULL,
                                                     width, height, kCMVideoCodecType_H264,
                                                     NULL, NULL, NULL,
                                                     &VTEncoder::didCompressH264,
                                                     this,
                                                     &encodeSession);
        
        
        if(status != 0) {
            printf( "H264: Unable to create a H264 session\n" );
            encodeSession = NULL;
            return;
        }
        
        VTSessionSetProperty(encodeSession, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
         // change the frame number between 2 I frame
        VTSessionSetProperty(encodeSession, kVTCompressionPropertyKey_MaxKeyFrameInterval, (__bridge CFNumberRef)@(60.0));
        VTSessionSetProperty(encodeSession, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Main_AutoLevel);
        //VTSessionSetProperty(encodeSession,
        //                     kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Baseline_AutoLevel);
        
        // Tell the encoder to start encoding
        VTCompressionSessionPrepareToEncodeFrames(encodeSession);
    });
    
    ARIBEIRO_ABORT(encodeSession == NULL, "Error to create encoder session");
}
    
VTEncoder::~VTEncoder(){
    VTCompressionSessionCompleteFrames(encodeSession, kCMTimeInvalid);
    VTCompressionSessionInvalidate(encodeSession);
    CFRelease(encodeSession);
    encodeSession = NULL;
}
    
void VTEncoder::encode(CMSampleBufferRef sampleBuffer) {
    //time.update();
    
    dispatch_sync(commandQueue, ^{
        CVImageBufferRef imageBuffer = (CVImageBufferRef)CMSampleBufferGetImageBuffer(sampleBuffer);
        
        // Create properties
        frameCount++;
        CMTime presentationTimeStamp = CMTimeMake(frameCount, 30);
        //CMTime duration = CMTimeMake(1, DURATION);
        VTEncodeInfoFlags flags;
        
        // Pass it to the encoder
        OSStatus statusCode = VTCompressionSessionEncodeFrame(encodeSession,
                                                              imageBuffer,
                                                              presentationTimeStamp,
                                                              kCMTimeInvalid ,
                                                              NULL, NULL, &flags);
        if (statusCode != noErr) {
            printf("VTCompressionSessionEncodeFrame failed\n");
            return;
        }
    });
}

