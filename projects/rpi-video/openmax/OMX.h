#ifndef OMX_H
#define OMX_H

#include "BCMHostInit.h"

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VA_ARGS_TO_STRING(strResult, formatParameter) \
    std::string strResult; \
    { \
        va_list length_args; \
        va_start(length_args, formatParameter); \
        va_list result_args; \
        va_copy(result_args, length_args); \
        size_t length = vsnprintf(NULL, 0U, formatParameter, length_args); \
        strResult = std::string(length, '\0'); \
        vsnprintf(&(strResult[0]), result.length() , formatParameter, result_args); \
        va_end(result_args); \
        va_end(length_args); \
    }

#define OMX_INIT_STRUCTURE(a) \
    memset(&(a), 0, sizeof(a)); \
    (a).nSize = sizeof(a); \
    (a).nVersion.nVersion = OMX_VERSION; \
    (a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
    (a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
    (a).nVersion.s.nRevision = OMX_VERSION_REVISION; \
    (a).nVersion.s.nStep = OMX_VERSION_STEP


class OMXComponentBase;

class OMX {
    OMX();
    bool initialized;
public:

    volatile bool appExitRequested;

    static OMX* Instance();
    void init();
    ~OMX();

    /*
    All Raspberry PI Components

    Audio Domain Components
        OMX.broadcom.audio_capture
        OMX.broadcom.audio_decode
        OMX.broadcom.audio_encode
        OMX.broadcom.audio_lowpower
        OMX.broadcom.audio_mixer
        OMX.broadcom.audio_processor
        OMX.broadcom.audio_render
        OMX.broadcom.audio_splitter

    Image Domain Components
        OMX.broadcom.image_decode
        OMX.broadcom.image_encode
        OMX.broadcom.image_fx
        OMX.broadcom.image_read
        OMX.broadcom.image_write
        OMX.broadcom.resize
        OMX.broadcom.source
        OMX.broadcom.transition
        OMX.broadcom.write_still

    Miscellaneous Components
        OMX.broadcom.clock
        OMX.broadcom.null_sink
        OMX.broadcom.text_scheduler
        OMX.broadcom.visualisation

    Multiplexers / Demultiplexer Components
        OMX.broadcom.read_media
        OMX.broadcom.write_media

    Video Domain Components
        OMX.broadcom.camera
        OMX.broadcom.egl_render
        OMX.broadcom.video_decode
        OMX.broadcom.video_encode
        OMX.broadcom.video_render
        OMX.broadcom.video_scheduler
        OMX.broadcom.video_splitter

    Documentation at: http://home.nouwen.name/RaspberryPi/documentation/ilcomponents/
    */

    //OMX.broadcom.video_encode, OMX.broadcom.camera, OMX.broadcom.null_sink
    static OMX_HANDLETYPE createHandle( const char *name, OMXComponentBase *componentBase);
    static void freeHandle(OMX_HANDLETYPE &handle);

    // OMX_StateIdle, OMX_StateLoaded, OMX_StateExecuting
    static void setState(OMX_HANDLETYPE handle, OMX_STATETYPE state);
    static void setEnablePort(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_BOOL enabled);
    static void portFlush(OMXComponentBase *component, OMX_HANDLETYPE handle, OMX_U32 portIndex);
    static OMX_PARAM_PORTDEFINITIONTYPE getPortDefinition(OMX_HANDLETYPE handle, OMX_U32 portIndex);
    static void setPortDefinition(OMX_HANDLETYPE handle, OMX_PARAM_PORTDEFINITIONTYPE &portdef);

    //
    // Video Calls
    //
    static void setVideoBitrate(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_VIDEO_CONTROLRATETYPE controlrate, OMX_U32 bitrate);
    static void setVideoPortFormat(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_VIDEO_CODINGTYPE codingType, OMX_COLOR_FORMATTYPE colorType = OMX_COLOR_FormatUnused);

    //
    // Audio Calls
    //
    static void setAudioPCMParam(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_U32 inputSamplingRate);
    static void setAudioAACParam(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_U32 bitrate, OMX_U32 samplerate);

    //
    // Allocating/Releasing buffers
    //
    static OMX_BUFFERHEADERTYPE *allocateBuffer(OMX_HANDLETYPE handle, OMX_U32 portIndex);
    static void freeBuffer(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_BUFFERHEADERTYPE *buffer);
    static void postEmptyThisBuffer(OMX_HANDLETYPE handle, OMX_BUFFERHEADERTYPE *buffer);
    static void postFillThisBuffer(OMX_HANDLETYPE handle, OMX_BUFFERHEADERTYPE *buffer);

    //print routines
    static void printEvent(OMX_HANDLETYPE handle, OMX_EVENTTYPE event, OMX_U32 data1, OMX_U32 data2);
    static void printPortDefinition(OMX_PARAM_PORTDEFINITIONTYPE &portdef);
    static std::vector<OMX_VIDEO_PARAM_PORTFORMATTYPE> portGetVideoSupportedFormats(OMX_HANDLETYPE handle, OMX_U32 portIndex);
    static std::vector<OMX_IMAGE_PARAM_PORTFORMATTYPE> portGetImageSupportedFormats(OMX_HANDLETYPE handle, OMX_U32 portIndex);
    static std::vector<OMX_AUDIO_PARAM_PORTFORMATTYPE> portGetAudioSupportedFormats(OMX_HANDLETYPE handle, OMX_U32 portIndex);
    static void printPortSupportedFormats(OMX_HANDLETYPE handle, OMX_U32 portIndex);
    static void printPort(OMX_HANDLETYPE handle, OMX_U32 portIndex);
    //string codes
    static const char* getError(OMX_ERRORTYPE error);
    static const char* getEvent(OMX_EVENTTYPE event);
    static const char* getVideoCoding(OMX_VIDEO_CODINGTYPE c);
    static const char* getImageCoding(OMX_IMAGE_CODINGTYPE c);
    static const char* getAudioCoding(OMX_AUDIO_CODINGTYPE c);
    static const char* getColorFormat(OMX_COLOR_FORMATTYPE c);
};

#endif
