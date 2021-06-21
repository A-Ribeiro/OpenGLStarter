#include "avfoundation.h"
#include <rtvideo/RTVideoInput.h>
#include "videotoolbox.h"

#import <AVFoundation/AVFoundation.h>

AF_Device processDevice(AVCaptureDevice *captureDevice, bool muxed){
    
    NSAutoreleasePool* _lpool = [[NSAutoreleasePool alloc] init];
    
    AF_Device device;
    
    device.muxed = muxed;
    const char *name = [[captureDevice localizedName] UTF8String];
    device.name = name;
    
    printf("AVFoundation: %s (muxed:%i)\n", name, muxed);
    
    NSArray<AVCaptureDeviceFormat *> *formatArray = [captureDevice formats];
    if(formatArray == nil)
        return device;
    
    for(int i=0;i<[formatArray count];i++)
    {
        AVCaptureDeviceFormat *format =[formatArray objectAtIndex:i];
        CMFormatDescriptionRef formatDescription = [format formatDescription];
        
        AF_DeviceResolution resolution;
        
        FourCharCode mediaSubType = CMFormatDescriptionGetMediaSubType(formatDescription);
        
        uint32_t mediaSubType_print = CFSwapInt32BigToHost(mediaSubType);
        char codecCode[5] = "\0\0\0\0";
        memcpy(codecCode, &mediaSubType_print, sizeof(FourCharCode));
        resolution.codec_code = codecCode;
        
        //mediaSubType examples
        // kCMVideoCodecType_JPEG_OpenDML -> 'dmb1'
        // kCMPixelFormat_422YpCbCr8 -> '2vuy'
        // kCMPixelFormat_422YpCbCr8_yuvs -> 'yuvs'
        
        //skip yuvs codec type
        if (mediaSubType == kCMPixelFormat_422YpCbCr8_yuvs){
            continue;
        }
        
        CMVideoDimensions dimension = CMVideoFormatDescriptionGetDimensions(formatDescription);
        
        resolution.width = dimension.width;
        resolution.height = dimension.height;
        
        //printf("%i %i -> %s\n", resolution.width, resolution.height, codecCode);
        
        NSArray<AVFrameRateRange *> *framerates = [format videoSupportedFrameRateRanges];
        if (framerates == nil)
            continue;
        
        for(int j=0;j<[framerates count];j++){
            AVFrameRateRange *framerate =[framerates objectAtIndex:j];
            
            AF_DeviceFramerate af_framerate;
            
            af_framerate.fps = framerate.maxFrameRate;
            af_framerate.numerator = framerate.maxFrameDuration.value;
            af_framerate.denominator = framerate.maxFrameDuration.timescale;
            
            resolution.framerates.push_back(af_framerate);
        }
        
        device.resolutions.push_back(resolution);
    }
    
    [_lpool drain];
    return device;
}

std::vector<AF_Device> AF_Device::listDevices(){
    NSAutoreleasePool* _lpool = [[NSAutoreleasePool alloc] init];
    std::vector<AF_Device> result;
    
    NSArray *videoDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    NSArray *muxedDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeMuxed];
    
    AVCaptureDevice *captureDevice = NULL;
    
    for (int i=0;i< [videoDevices count];i++){
        captureDevice = [videoDevices objectAtIndex:i];
        AF_Device device = processDevice(captureDevice, false);
        if (device.resolutions.size() > 0)
            result.push_back(device);
    }
    
    for (int i=0;i< [muxedDevices count];i++){
        captureDevice = [videoDevices objectAtIndex:i];
        AF_Device device = processDevice(captureDevice, true);
        if (device.resolutions.size() > 0)
            result.push_back(device);
    }
    
    [_lpool drain];
    return result;
}


bool AF_queryDevice(const std::string &device_name, int width, int height, std::string codec_code, double fps,
                    AVCaptureDevice **captureDevice, AVCaptureDeviceFormat **captureDeviceFormat,
                    AVFrameRateRange **frameRateRange){
    NSAutoreleasePool* _lpool = [[NSAutoreleasePool alloc] init];
    
    FourCharCode codec_FourCharCode = CFSwapInt32HostToBig(*(uint32_t*)codec_code.c_str());
    
    *captureDevice = nil;
    *captureDeviceFormat = nil;
    *frameRateRange = nil;
    
    NSArray *allDevices = [[AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo]
                           arrayByAddingObjectsFromArray: [AVCaptureDevice devicesWithMediaType:AVMediaTypeMuxed]];
    
    
    
    //find the device
    for (int i=0;i< [allDevices count];i++){
        AVCaptureDevice *captureDevice_aux = [allDevices objectAtIndex:i];
        const char *name = [[captureDevice_aux localizedName] UTF8String];
        if (strcmp(name, device_name.c_str())==0){
            *captureDevice = captureDevice_aux;
            *captureDeviceFormat = nil;
            *frameRateRange = nil;
            //find the resolution
            NSArray<AVCaptureDeviceFormat *> *formatArray = [captureDevice_aux formats];
            for(int i=0;i<[formatArray count];i++)
            {
                AVCaptureDeviceFormat *format =[formatArray objectAtIndex:i];
                CMFormatDescriptionRef formatDescription = [format formatDescription];
                FourCharCode mediaSubType = CMFormatDescriptionGetMediaSubType(formatDescription);
                CMVideoDimensions dimension = CMVideoFormatDescriptionGetDimensions(formatDescription);
                
                if (mediaSubType == codec_FourCharCode && dimension.width == width && dimension.height == height){
                    *captureDeviceFormat = format;
                    *frameRateRange = nil;
                    
                    double selectedDistance = DBL_MAX;
                    
                    //find the framerate
                    NSArray<AVFrameRateRange *> *framerates = [format videoSupportedFrameRateRanges];
                    for(int j=0;j<[framerates count];j++){
                        AVFrameRateRange *framerate =[framerates objectAtIndex:j];
                        double distance = abs([framerate maxFrameRate] - fps);
                        if ( distance < selectedDistance ) {
                            selectedDistance = distance;
                            *frameRateRange = framerate;
                        }
                    }
                    
                    if (*frameRateRange != nil)
                        break;
                }
                
            }
            
            if (*captureDeviceFormat != nil)
                break;
        }
    }

    [_lpool drain];
    return (*captureDevice != nil && *captureDeviceFormat != nil && *frameRateRange != nil);
}

/*
@interface User : NSObject
@property int value;
@end

@implementation User
- (int)value {
    printf("get identifier");
    return 0;
}

- (void)setValue:(int)newValue {
    printf("set identifier");
}
@end
 */

// delegate for AVCaptureSession
@interface ObjC_FrameReceiver : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    //void (*callback)(const uint8_t *data, size_t size, void* userData);
    //void* userData;
    VTEncoder *encoder;
    
    bool checkLatency;
    aRibeiro::PlatformTime latency;
}

- (void)resetLatency;

- (void)close;

- (void)dealloc;

- (id)initWithCallback:(void (*)(const uint8_t *data, size_t size, void* userData))callback
           andUserData:(void*)userData
              andWidth:(int) width
             andHeight:(int) height;

- (void)  captureOutput:(AVCaptureOutput *)captureOutput
  didOutputSampleBuffer:(CMSampleBufferRef)videoFrame
         fromConnection:(AVCaptureConnection *)connection;

@end

@implementation ObjC_FrameReceiver

- (void)resetLatency {
    self->checkLatency = true;
    self->latency.update();
}

- (void)close {
    if (encoder != NULL)
        delete encoder;
    encoder = NULL;
}

-(void)dealloc {
    if (encoder != NULL)
        delete encoder;
    encoder = NULL;
    [super dealloc];
}

- (id)initWithCallback:(void (*)(const uint8_t *data, size_t size, void* userData))_callback
           andUserData:(void*)_userData
              andWidth:(int) width
             andHeight:(int) height
{
    if (self = [super init]) {
        //self->callback = _callback;
        //self->userData = _userData;
        self->checkLatency = true;
        self->encoder = new VTEncoder(width,height,_callback, _userData);
    }
    return self;
}

- (void)  captureOutput:(AVCaptureOutput *)captureOutput
  didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
         fromConnection:(AVCaptureConnection *)connection
{
    if (self->checkLatency){
        self->checkLatency = false;
        self->latency.update();
        __hidden__mac_encode_to_callback_time = self->latency.deltaTime;
        
        printf("ObjC_FrameReceiver Encode latency: %f secs\n", __hidden__mac_encode_to_callback_time);
    }
        
    encoder->encode(sampleBuffer);
    
    /*
     
    CMBlockBufferRef data = CMSampleBufferGetDataBuffer(sampleBuffer);
    //size_t size = CMBlockBufferGetDataLength(data);
    
    size_t lengthAtOffset;
    size_t totalLengthOut;
    char* dataPointer;
    
    if (CMBlockBufferGetDataPointer(data, 0, &lengthAtOffset, &totalLengthOut, &dataPointer) == kCMBlockBufferNoErr){
        
        
        uint32_t annexbStart = 0x01000000;//CFSwapInt32HostToBig(0x00000001);
        
        uint32_t size = CFSwapInt32BigToHost(*(uint32_t*)dataPointer);
        uint32_t offset = 4;
        
        while (offset < totalLengthOut) {
            
            self->callback((uint8_t*)&annexbStart,4,self->userData);
            self->callback((uint8_t*)&dataPointer[offset],size,self->userData);
            
            offset += size;
            
            if (offset < totalLengthOut){
                size = CFSwapInt32BigToHost(*(uint32_t*)&dataPointer[offset]);
                offset += 4;
            }
        }
         
        
        printf("size:%u %u total:%lu\n",
               *(uint32_t*)dataPointer,
               CFSwapInt32BigToHost(*(uint32_t*)dataPointer),
               totalLengthOut);
        
        //self->callback((uint8_t*)dataPointer,totalLengthOut,self->userData);
    }
    */
    
    /*
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    CVPixelBufferRef pixels = imageBuffer;
    //@synchronized (self){
        //pixels = CVBufferRetain(imageBuffer);
    //}
    
    //CVBufferRetain(imageBuffer);
    
    CVPixelBufferLockBaseAddress(pixels, 0);
    uint32_t* baseaddress = (uint32_t*)CVPixelBufferGetBaseAddress(pixels);
    size_t width = CVPixelBufferGetWidth(pixels);
    size_t height = CVPixelBufferGetHeight(pixels);
    size_t rowBytes = CVPixelBufferGetBytesPerRow(pixels);
    size_t size = CVPixelBufferGetDataSize(pixels);
    
    //make image copy
    //BGRAVideoFrame frame = {(int)width, (int)height, (int)stride, (int)size, baseAddress};
    //self->callback((uint8_t*)baseaddress,0,self->userData);
    
    //release locks
    CVPixelBufferUnlockBaseAddress(pixels, 0);
    //CVBufferRelease(pixels);
    
    
    */
    
    /*
    static int cunt=0;
    static aRibeiro::PlatformTime time;
    time.update();
    if (time.deltaTime > 0.00002f)
        printf("data %i (%f)\n", cunt++, 1.0f/time.deltaTime);
    */
    
}

@end


void AF_startStream(const std::string &device_name,
                    int width, int height, const std::string &codec_code, double fps,
                    void (*callback)(const uint8_t *data, size_t size, void* userData), void* userData ) {
    
    NSAutoreleasePool* _lpool = [[NSAutoreleasePool alloc] init];
    
    AVCaptureDevice *captureDevice = nil;
    AVCaptureDeviceFormat *captureDeviceFormat = nil;
    AVFrameRateRange *frameRateRange = nil;
    bool result = AF_queryDevice(device_name,
                                 width, height,codec_code,fps,
                                 &captureDevice, &captureDeviceFormat,&frameRateRange);
    
    if (!result){
        [_lpool drain];
        return;
    }
    
    //setup device input
    @try {
        if([captureDevice lockForConfiguration:nil]){
            captureDevice.activeFormat = captureDeviceFormat;
            captureDevice.activeVideoMinFrameDuration = frameRateRange.minFrameDuration;
            captureDevice.activeVideoMaxFrameDuration = frameRateRange.maxFrameDuration;
            [captureDevice unlockForConfiguration];
        }
    } @catch(NSException *e) {
        printf("Error to Setup Video Device Format\n");
        [_lpool drain];
        return;
    }
    
    NSError *error = nil;
    AVCaptureDeviceInput *captureDeviceInput = [[AVCaptureDeviceInput alloc]
                                                initWithDevice:captureDevice
                                                error:&error];
    
    AVCaptureVideoDataOutput *captureVideoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
    captureVideoDataOutput.alwaysDiscardsLateVideoFrames = true;
    
    ObjC_FrameReceiver *frameReceiver =
    [[ObjC_FrameReceiver alloc] initWithCallback:callback
                                     andUserData:userData
                                        andWidth:width
                                       andHeight:height];
    
    dispatch_queue_t queue = dispatch_queue_create("_queue", NULL);
    [captureVideoDataOutput setSampleBufferDelegate:frameReceiver queue:queue];
    dispatch_release(queue);
    
    captureVideoDataOutput.videoSettings =
    [NSDictionary dictionaryWithObjectsAndKeys:
     //AVVideoCodecTypeH264,AVVideoCodecKey,
     //[NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],(id)kCVPixelBufferPixelFormatTypeKey,
     //[NSNumber numberWithUnsignedInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange],(id)kCVPixelBufferPixelFormatTypeKey,
     //[NSNumber numberWithUnsignedInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange],(id)kCVPixelBufferPixelFormatTypeKey,
     [NSNumber numberWithUnsignedInt:kCVPixelFormatType_422YpCbCr8],(id)kCVPixelBufferPixelFormatTypeKey,
     //[NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],(id)kCVPixelBufferPixelFormatTypeKey,
     nil];
    
    AVCaptureSession *captureSession = [[AVCaptureSession alloc] init];
    
    [captureSession addInput:captureDeviceInput];
    [captureSession addOutput:captureVideoDataOutput];
    
    [captureSession beginConfiguration];
    captureSession.sessionPreset = AVCaptureSessionPresetHigh;
    [captureSession commitConfiguration];
    
    //setup device output
    AVCaptureConnection *conn = [captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    if (conn.isVideoMinFrameDurationSupported)
        conn.videoMinFrameDuration = frameRateRange.minFrameDuration;
    if (conn.isVideoMaxFrameDurationSupported)
        conn.videoMaxFrameDuration = frameRateRange.minFrameDuration;
    
    [frameReceiver resetLatency];
    [captureSession startRunning];
    
    //wait external signal to stop
    while ( !aRibeiro::PlatformThread::isCurrentThreadInterrupted() ){
        aRibeiro::PlatformSleep::sleepMillis(1);
    }
    
    [captureSession stopRunning];
    
    [captureSession release];
    
    [frameReceiver close];
    [frameReceiver release];
    
    [captureVideoDataOutput release];
    [captureDeviceInput release];
    
    [_lpool drain];
}
