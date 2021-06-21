#include "OMX.h"
#include "OMXComponentBase.h"

OMX::OMX(){
    initialized = false;
    appExitRequested = false;
}

OMX* OMX::Instance(){
    static OMX instance;
    return &instance;
}

void OMX::init() {
    if (initialized)
        return;
    initialized = true;

    BMCHost::Instance()->init();

    OMX_ERRORTYPE error;
    if((error = OMX_Init()) != OMX_ErrorNone) {
        fprintf(stderr,"OMX initalization failed: %s", OMX::getError(error) );
        exit(-1);
    }
    fprintf(stderr,"OMX::init OK\n");
}

OMX::~OMX(){
    if (!initialized)
        return;

    OMX_ERRORTYPE error;
    if((error = OMX_Deinit()) != OMX_ErrorNone) {
        fprintf(stderr,"OMX un-initalization failed: %s", OMX::getError(error) );
        exit(-1);
    }
}


OMX_HANDLETYPE OMX::createHandle( const char *name, OMXComponentBase *componentBase) {
    char nameNotConst[1024];
    snprintf(nameNotConst,1024,"%s",name);

    OMX_HANDLETYPE handleOut;
    OMX_ERRORTYPE error;

    if((error = OMX_GetHandle(&handleOut, nameNotConst, componentBase, &OMXComponentBase::callbacks)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to create handle for %s: %s\n", name, OMX::getError(error) );
        exit(-1);
    }
    //
    // Disable ports
    //
    OMX_INDEXTYPE types[] = {
        OMX_IndexParamAudioInit,
        OMX_IndexParamVideoInit,
        OMX_IndexParamImageInit,
        OMX_IndexParamOtherInit
    };
    const char* typeNames[] = {
        "Audio",
        "Video",
        "Image",
        "Other"
    };
    OMX_PORT_PARAM_TYPE ports;
    OMX_INIT_STRUCTURE(ports);

    for(int i = 0; i < 4; i++) {
        if(OMX_GetParameter(handleOut, types[i], &ports) == OMX_ErrorNone) {
            for(OMX_U32 portIndex = ports.nStartPortNumber; portIndex < ports.nStartPortNumber + ports.nPorts; portIndex++) {
                fprintf(stderr,"  [ %s ] Port off %i from %s\n", typeNames[i],  portIndex , name );
                OMX::setEnablePort(handleOut, portIndex, OMX_FALSE);
            }
        } else {
            fprintf(stderr, "Failed to get ports from %s: %s\n", name, OMX::getError(error) );
            exit(-1);
        }
    }

    return handleOut;
}

void OMX::freeHandle(OMX_HANDLETYPE &handle) {
    OMX_ERRORTYPE error;
    if((error = OMX_FreeHandle(handle)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to free handle: %s\n", OMX::getError(error) );
        exit(-1);
    }
}



// OMX_StateIdle, OMX_StateLoaded, OMX_StateExecuting
void OMX::setState(OMX_HANDLETYPE handle, OMX_STATETYPE state) {
    //
    // change state
    //
    OMX_ERRORTYPE error;
    if((error = OMX_SendCommand(handle, OMX_CommandStateSet, state, NULL)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed on changeState: %s\n", OMX::getError(error) );
        exit(-1);
    }
    //
    // check state change
    //
    OMX_STATETYPE readedState;
    do{
        if((error = OMX_GetState(handle, &readedState)) != OMX_ErrorNone) {
            fprintf(stderr, "Failed to get handle state: %s\n", OMX::getError(error) );
            exit(-1);
        }
        if(readedState != state)
            usleep(1000);
    } while(readedState != state);
}


void OMX::setEnablePort(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_BOOL enabled) {

    OMX_ERRORTYPE error;
    //
    // Enable/Disable port
    //
    if (enabled){
        if((error = OMX_SendCommand(handle, OMX_CommandPortEnable, portIndex, NULL)) != OMX_ErrorNone) {
            fprintf(stderr, "Failed to enable port %i: %s\n", portIndex, OMX::getError(error) );
            exit(-1);
        }
    } else {
        if((error = OMX_SendCommand(handle, OMX_CommandPortDisable, portIndex, NULL)) != OMX_ErrorNone) {
            fprintf(stderr, "Failed to disable port %i: %s\n", portIndex, OMX::getError(error) );
            exit(-1);
        }
    }
    //
    // Block until port change...
    //
    OMX_PARAM_PORTDEFINITIONTYPE portdefinition;
    OMX_INIT_STRUCTURE(portdefinition);
    portdefinition.nPortIndex = portIndex;
    OMX_U32 i = 0;
    do {
        if((error = OMX_GetParameter(handle, OMX_IndexParamPortDefinition, &portdefinition)) != OMX_ErrorNone) {
            fprintf(stderr, "Failed to get port definition: %s\n", OMX::getError(error) );
            exit(-1);
        }
        if(portdefinition.bEnabled != enabled)
            usleep(1000);
    } while(portdefinition.bEnabled != enabled) ;
}

void OMX::portFlush(OMXComponentBase *component, OMX_HANDLETYPE handle, OMX_U32 portIndex) {
    OMX_ERRORTYPE error;
    if((error = OMX_SendCommand(handle, OMX_CommandFlush, portIndex, NULL)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to flush buffer on port %i: %s\n", portIndex, OMX::getError(error) );
        exit(-1);
    }
    //
    // block until flush
    //
    bool quit = false;
    do{
        component->mutex.lock();
        if(component->flushed) {
            component->flushed = false;
            quit = true;
        }
        component->mutex.unlock();
        if(!quit)
            usleep(1000);
    }while(!quit);
}

OMX_PARAM_PORTDEFINITIONTYPE OMX::getPortDefinition(OMX_HANDLETYPE handle, OMX_U32 portIndex){
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    OMX_INIT_STRUCTURE(portdef);
    portdef.nPortIndex = portIndex;
    OMX_ERRORTYPE error;
    if((error = OMX_GetParameter(handle, OMX_IndexParamPortDefinition, &portdef)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to get port definition on port %i: %s\n", portIndex, OMX::getError(error) );
        exit(-1);
    }
    return portdef;
}

void OMX::setPortDefinition(OMX_HANDLETYPE handle, OMX_PARAM_PORTDEFINITIONTYPE &portdef){
    OMX_ERRORTYPE error;
    if((error = OMX_SetParameter(handle, OMX_IndexParamPortDefinition, &portdef)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to set port definition on port %i: %s\n", portdef.nPortIndex, OMX::getError(error) );
        exit(-1);
    }
}

void OMX::setVideoBitrate(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_VIDEO_CONTROLRATETYPE controlrate, OMX_U32 bitratep){
    OMX_VIDEO_PARAM_BITRATETYPE bitrate;
    OMX_INIT_STRUCTURE(bitrate);
    bitrate.eControlRate = controlrate;//OMX_Video_ControlRateVariable;
    bitrate.nTargetBitrate = bitratep;
    bitrate.nPortIndex = portIndex;
    OMX_ERRORTYPE error;
    if((error = OMX_SetParameter(handle, OMX_IndexParamVideoBitrate, &bitrate)) != OMX_ErrorNone) {
        fprintf(stderr,"Failed to set bitrate on port %u: %s", portIndex, OMX::getError(error) );
        exit(-1);
    }
}

void OMX::setVideoPortFormat(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_VIDEO_CODINGTYPE codingType, OMX_COLOR_FORMATTYPE colorType) {
    OMX_VIDEO_PARAM_PORTFORMATTYPE format;
    OMX_INIT_STRUCTURE(format);
    format.nPortIndex = portIndex;
    format.eColorFormat = colorType;
    format.eCompressionFormat = codingType;//OMX_VIDEO_CodingAVC;
    OMX_ERRORTYPE error;
    if((error = OMX_SetParameter(handle, OMX_IndexParamVideoPortFormat, &format)) != OMX_ErrorNone) {
        fprintf(stderr,"Failed to set video format on port %u: %s", portIndex, OMX::getError(error) );
        exit(-1);
    }
}


void OMX::setAudioPCMParam(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_U32 inputSamplingRate) {
    OMX_AUDIO_PARAM_PCMMODETYPE param;
    OMX_INIT_STRUCTURE(param);

    param.nPortIndex = portIndex;

    // nChannels must be 1, 2, 4, or 8
    param.nChannels = 2;
    // nBitPerSample must be 16 or 32
    param.nBitPerSample = 16;
    // eNumData must be OMX_NumericalDataSigned
    param.eNumData = OMX_NumericalDataSigned;
    // eEndian must be OMX_EndianLittle
    param.eEndian = OMX_EndianLittle;
    // bInterleaved must be OMX_TRUE
    param.bInterleaved = OMX_TRUE;
    // ePCMMode must be OMX_AUDIO_PCMModeLinear
    param.ePCMMode = OMX_AUDIO_PCMModeLinear;

    param.nSamplingRate = inputSamplingRate;

    param.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    param.eChannelMapping[1] = OMX_AUDIO_ChannelRF;


    OMX_ERRORTYPE error;
    if((error = OMX_SetParameter(handle, OMX_IndexParamAudioPcm, &param)) != OMX_ErrorNone) {
        fprintf(stderr,"Failed to set audio format on port %u: %s", portIndex, OMX::getError(error) );
        exit(-1);
    }


}

void OMX::setAudioAACParam(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_U32 bitrate, OMX_U32 samplerate) {
    OMX_AUDIO_PARAM_AACPROFILETYPE param;
    OMX_INIT_STRUCTURE(param);

    param.nPortIndex = portIndex;

    param.nBitRate = bitrate;
    //MP2ADTS or RAW
    param.eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP2ADTS;
    param.nSampleRate = samplerate;

    //nChannels (must be 1 or 2)
    param.nChannels = 2;

    OMX_ERRORTYPE error;
    if((error = OMX_SetParameter(handle, OMX_IndexParamAudioAac, &param)) != OMX_ErrorNone) {
        fprintf(stderr,"Failed to set audio format on port %u: %s", portIndex, OMX::getError(error) );
        exit(-1);
    }
}



OMX_BUFFERHEADERTYPE* OMX::allocateBuffer(OMX_HANDLETYPE handle, OMX_U32 portIndex) {
    OMX_PARAM_PORTDEFINITIONTYPE portdef = OMX::getPortDefinition(handle,portIndex);
    OMX_BUFFERHEADERTYPE *result;
    OMX_ERRORTYPE error;
    if((error = OMX_AllocateBuffer(handle, &result, portIndex, NULL, portdef.nBufferSize)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to allocate buffer on port %i: %s\n", portIndex, OMX::getError(error) );
        exit(-1);
    }
    return result;
}

void OMX::freeBuffer(OMX_HANDLETYPE handle, OMX_U32 portIndex, OMX_BUFFERHEADERTYPE *buffer) {
    OMX_ERRORTYPE error;
    if((error = OMX_FreeBuffer(handle, portIndex, buffer)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to free buffer on port %i: %s\n", portIndex, OMX::getError(error) );
        exit(-1);
    }
}

void OMX::postEmptyThisBuffer(OMX_HANDLETYPE handle, OMX_BUFFERHEADERTYPE *buffer) {
    OMX_ERRORTYPE error;
    if((error = OMX_EmptyThisBuffer(handle, buffer)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to request emptying buffer: %s\n", OMX::getError(error) );
        exit(-1);
    }
}


void OMX::postFillThisBuffer(OMX_HANDLETYPE handle, OMX_BUFFERHEADERTYPE *buffer) {
    OMX_ERRORTYPE error;
    if((error = OMX_FillThisBuffer(handle, buffer)) != OMX_ErrorNone) {
        fprintf(stderr, "Failed to request filling buffer: %s\n", OMX::getError(error) );
        exit(-1);
    }
}


//print routines
void OMX::printEvent(OMX_HANDLETYPE handle, OMX_EVENTTYPE event, OMX_U32 data1, OMX_U32 data2) {
    fprintf(stderr,"Event 0x%08x %s, handle:0x%08x, data1:0x%08x, data2:0x%08x\n", event, OMX::getEvent(event) , handle, data1, data2);
}

void OMX::printPortDefinition(OMX_PARAM_PORTDEFINITIONTYPE &portdef) {
    fprintf(stderr,"Port %d is %s, %s, nBufferCountActual:%d nBufferCountMin:%d, nBufferSize:%d, bPopulated:%d, nBufferAlignment:%d\n",
        portdef.nPortIndex,
        (portdef.eDir ==  OMX_DirInput ? "input" : "output"),
        (portdef.bEnabled == OMX_TRUE ? "enabled" : "disabled"),
        portdef.nBufferCountActual,
        portdef.nBufferCountMin,
        portdef.nBufferSize,
        portdef.bPopulated,
        portdef.nBufferAlignment);

    OMX_VIDEO_PORTDEFINITIONTYPE &viddef = portdef.format.video;
    OMX_IMAGE_PORTDEFINITIONTYPE &imgdef = portdef.format.image;
    OMX_AUDIO_PORTDEFINITIONTYPE &audiodef = portdef.format.audio;
    switch(portdef.eDomain) {
        case OMX_PortDomainVideo:
            fprintf(stderr,"  Video:\n"
                "    Resolution ( %d, %d ) Stride: %d  SliceHeight: %d\n"
                "    Bitrate: %d  Framerate: %.02f  ErrorConcealment: %s\n"
                "    Codec: %s  Color: %s\n",
                viddef.nFrameWidth,
                viddef.nFrameHeight,
                viddef.nStride,
                viddef.nSliceHeight,
                viddef.nBitrate,
                ((float)viddef.xFramerate / (float)65536),
                (viddef.bFlagErrorConcealment == OMX_TRUE ? "yes" : "no"),
                OMX::getVideoCoding(viddef.eCompressionFormat),
                OMX::getColorFormat(viddef.eColorFormat));
            break;
        case OMX_PortDomainImage:
            fprintf(stderr,"  Image:\n"
                "    Resolution( %d, %d )  Stride: %d  SliceHeight: %d\n"
                "    ErrorConcealment: %s  Codec: %s  Color: %s\n",
                imgdef.nFrameWidth,
                imgdef.nFrameHeight,
                imgdef.nStride,
                imgdef.nSliceHeight,
                (imgdef.bFlagErrorConcealment == OMX_TRUE ? "yes" : "no"),
                OMX::getImageCoding(imgdef.eCompressionFormat),
                OMX::getColorFormat(imgdef.eColorFormat));
            break;
        case OMX_PortDomainAudio:
            fprintf(stderr,"  Audio:\n"
                "    MIMEType: %s\n"
                "    ErrorConcealment: %s\n"
                "    Codec: %s\n",
                audiodef.cMIMEType,
                (audiodef.bFlagErrorConcealment == OMX_TRUE ? "yes" : "no"),
                OMX::getAudioCoding(audiodef.eEncoding));
            break;
        default:
            break;
    }
}

std::vector<OMX_VIDEO_PARAM_PORTFORMATTYPE> OMX::portGetVideoSupportedFormats(OMX_HANDLETYPE handle, OMX_U32 portIndex){
    std::vector<OMX_VIDEO_PARAM_PORTFORMATTYPE> result;
    OMX_ERRORTYPE error;
    OMX_VIDEO_PARAM_PORTFORMATTYPE portformat;
    OMX_INIT_STRUCTURE(portformat);
    portformat.nPortIndex = portIndex;
    portformat.nIndex = 0;

    error = OMX_ErrorNone;
    do {
        if((error = OMX_GetParameter(handle, OMX_IndexParamVideoPortFormat, &portformat)) == OMX_ErrorNone) {
            result.push_back( portformat );
            portformat.nIndex++;
        }
    }while(error == OMX_ErrorNone);
    return result;
}

std::vector<OMX_IMAGE_PARAM_PORTFORMATTYPE> OMX::portGetImageSupportedFormats(OMX_HANDLETYPE handle, OMX_U32 portIndex){
    std::vector<OMX_IMAGE_PARAM_PORTFORMATTYPE> result;
    OMX_ERRORTYPE error;
    OMX_IMAGE_PARAM_PORTFORMATTYPE portformat;
    OMX_INIT_STRUCTURE(portformat);
    portformat.nPortIndex = portIndex;
    portformat.nIndex = 0;

    error = OMX_ErrorNone;
    do {
        if((error = OMX_GetParameter(handle, OMX_IndexParamImagePortFormat, &portformat)) == OMX_ErrorNone) {
            result.push_back( portformat );
            portformat.nIndex++;
        }
    }while(error == OMX_ErrorNone);
    return result;
}

std::vector<OMX_AUDIO_PARAM_PORTFORMATTYPE> OMX::portGetAudioSupportedFormats(OMX_HANDLETYPE handle, OMX_U32 portIndex){
    std::vector<OMX_AUDIO_PARAM_PORTFORMATTYPE> result;
    OMX_ERRORTYPE error;
    OMX_AUDIO_PARAM_PORTFORMATTYPE portformat;
    OMX_INIT_STRUCTURE(portformat);
    portformat.nPortIndex = portIndex;
    portformat.nIndex = 0;

    error = OMX_ErrorNone;
    do {
        if((error = OMX_GetParameter(handle, OMX_IndexParamAudioPortFormat, &portformat)) == OMX_ErrorNone) {
            result.push_back( portformat );
            portformat.nIndex++;
        }
    }while(error == OMX_ErrorNone);
    return result;
}

void OMX::printPortSupportedFormats(OMX_HANDLETYPE handle, OMX_U32 portIndex) {
    std::vector<OMX_VIDEO_PARAM_PORTFORMATTYPE> videoSupportedFormats = portGetVideoSupportedFormats(handle, portIndex);
    if (videoSupportedFormats.size() > 0) {
        fprintf(stderr,"  Supported video formats:\n");
        for(int i=0; i<videoSupportedFormats.size();i++){
            OMX_VIDEO_PARAM_PORTFORMATTYPE &portformat = videoSupportedFormats[i];
            fprintf(stderr,"    %s, compression: %s\n", OMX::getColorFormat(portformat.eColorFormat), OMX::getVideoCoding(portformat.eCompressionFormat));
        }
    }
    std::vector<OMX_IMAGE_PARAM_PORTFORMATTYPE> imageSupportedFormats = portGetImageSupportedFormats(handle, portIndex);
    if (imageSupportedFormats.size() > 0) {
        fprintf(stderr,"  Supported image formats:\n");
        for(int i=0; i<imageSupportedFormats.size();i++){
            OMX_IMAGE_PARAM_PORTFORMATTYPE &portformat = imageSupportedFormats[i];
            fprintf(stderr,"    %s, compression: %s\n", OMX::getColorFormat(portformat.eColorFormat), OMX::getImageCoding(portformat.eCompressionFormat));
        }
    }
    std::vector<OMX_AUDIO_PARAM_PORTFORMATTYPE> audioSupportedFormats = portGetAudioSupportedFormats(handle, portIndex);
    if (audioSupportedFormats.size() > 0) {
        fprintf(stderr,"  Supported audio formats:\n");
        for(int i=0; i<audioSupportedFormats.size();i++){
            OMX_AUDIO_PARAM_PORTFORMATTYPE &portformat = audioSupportedFormats[i];
            fprintf(stderr,"    %s\n", OMX::getAudioCoding(portformat.eEncoding));
        }
    }
}

void OMX::printPort(OMX_HANDLETYPE handle, OMX_U32 portIndex) {
    OMX_PARAM_PORTDEFINITIONTYPE portdef = OMX::getPortDefinition(handle, portIndex);
    printPortDefinition(portdef);
}



//string codes
const char* OMX::getError(OMX_ERRORTYPE error){
    switch(error) {
        /*
        case OMX_ErrorNone:                     return "OMX_ErrorNone";
        case OMX_ErrorBadParameter:             return "OMX_ErrorBadParameter";
        case OMX_ErrorIncorrectStateOperation:  return "OMX_ErrorIncorrectStateOperation (invalid state while trying to perform command)";
        case OMX_ErrorIncorrectStateTransition: return "OMX_ErrorIncorrectStateTransition (unallowed state transition)";
        case OMX_ErrorInsufficientResources:    return "OMX_ErrorInsufficientResources";
        case OMX_ErrorBadPortIndex:             return "OMX_ErrorBadPortIndex (incorrect port)";
        case OMX_ErrorHardware:                 return "OMX_ErrorHardware";
        */



        case OMX_ErrorInsufficientResources:              return "OMX_ErrorInsufficientResources";
        case OMX_ErrorUndefined:                          return "OMX_ErrorUndefined";
        case OMX_ErrorInvalidComponentName:               return "OMX_ErrorInvalidComponentName";
        case OMX_ErrorComponentNotFound:                  return "OMX_ErrorComponentNotFound";
        case OMX_ErrorInvalidComponent:                   return "OMX_ErrorInvalidComponent";
        case OMX_ErrorBadParameter:                       return "OMX_ErrorBadParameter";
        case OMX_ErrorNotImplemented:                     return "OMX_ErrorNotImplemented";
        case OMX_ErrorUnderflow:                          return "OMX_ErrorUnderflow";
        case OMX_ErrorOverflow:                           return "OMX_ErrorOverflow";
        case OMX_ErrorHardware:                           return "OMX_ErrorHardware";
        case OMX_ErrorInvalidState:                       return "OMX_ErrorInvalidState";
        case OMX_ErrorStreamCorrupt:                      return "OMX_ErrorStreamCorrupt";
        case OMX_ErrorPortsNotCompatible:                 return "OMX_ErrorPortsNotCompatible";
        case OMX_ErrorResourcesLost:                      return "OMX_ErrorResourcesLost";
        case OMX_ErrorNoMore:                             return "OMX_ErrorNoMore";
        case OMX_ErrorVersionMismatch:                    return "OMX_ErrorVersionMismatch";
        case OMX_ErrorNotReady:                           return "OMX_ErrorNotReady";
        case OMX_ErrorTimeout:                            return "OMX_ErrorTimeout";
        case OMX_ErrorSameState:                          return "OMX_ErrorSameState";
        case OMX_ErrorResourcesPreempted:                 return "OMX_ErrorResourcesPreempted";
        case OMX_ErrorPortUnresponsiveDuringAllocation:   return "OMX_ErrorPortUnresponsiveDuringAllocation";
        case OMX_ErrorPortUnresponsiveDuringDeallocation: return "OMX_ErrorPortUnresponsiveDuringDeallocation";
        case OMX_ErrorPortUnresponsiveDuringStop:         return "OMX_ErrorPortUnresponsiveDuringStop";
        case OMX_ErrorIncorrectStateTransition:           return "OMX_ErrorIncorrectStateTransition";
        case OMX_ErrorIncorrectStateOperation:            return "OMX_ErrorIncorrectStateOperation";
        case OMX_ErrorUnsupportedSetting:                 return "OMX_ErrorUnsupportedSetting";
        case OMX_ErrorUnsupportedIndex:                   return "OMX_ErrorUnsupportedIndex";
        case OMX_ErrorBadPortIndex:                       return "OMX_ErrorBadPortIndex";
        case OMX_ErrorPortUnpopulated:                    return "OMX_ErrorPortUnpopulated";
        case OMX_ErrorComponentSuspended:                 return "OMX_ErrorComponentSuspended";
        case OMX_ErrorDynamicResourcesUnavailable:        return "OMX_ErrorDynamicResourcesUnavailable";
        case OMX_ErrorMbErrorsInFrame:                    return "OMX_ErrorMbErrorsInFrame";
        case OMX_ErrorFormatNotDetected:                  return "OMX_ErrorFormatNotDetected";
        case OMX_ErrorContentPipeOpenFailed:              return "OMX_ErrorContentPipeOpenFailed";
        case OMX_ErrorContentPipeCreationFailed:          return "OMX_ErrorContentPipeCreationFailed";
        case OMX_ErrorSeperateTablesUsed:                 return "OMX_ErrorSeperateTablesUsed";
        case OMX_ErrorTunnelingUnsupported:               return "OMX_ErrorTunnelingUnsupported";
        case OMX_ErrorKhronosExtensions:                  return "OMX_ErrorKhronosExtensions";
        case OMX_ErrorVendorStartUnused:                  return "OMX_ErrorVendorStartUnused";
        case OMX_ErrorDiskFull:                           return "OMX_ErrorDiskFull";
        case OMX_ErrorMaxFileSize:                        return "OMX_ErrorMaxFileSize";
        case OMX_ErrorDrmUnauthorised:                    return "OMX_ErrorDrmUnauthorised";
        case OMX_ErrorDrmExpired:                         return "OMX_ErrorDrmExpired";
        case OMX_ErrorDrmGeneral:                         return "OMX_ErrorDrmGeneral";
        /* That's all I've encountered during hacking so let's not bother with the rest... */
        default:                                          return "(unknown error)";
    }
}

const char* OMX::getEvent(OMX_EVENTTYPE event){
    switch(event) {
        case OMX_EventCmdComplete:          return "OMX_EventCmdComplete";
        case OMX_EventError:                return "OMX_EventError";
        case OMX_EventParamOrConfigChanged: return "OMX_EventParamOrConfigChanged";
        case OMX_EventPortSettingsChanged:  return "OMX_EventPortSettingsChanged";
        default:                            return "(unknown event)";
    }
}

const char* OMX::getVideoCoding(OMX_VIDEO_CODINGTYPE c) {
    switch(c) {
        case OMX_VIDEO_CodingUnused:     return "not used";
        case OMX_VIDEO_CodingAutoDetect: return "autodetect";
        case OMX_VIDEO_CodingMPEG2:      return "MPEG2";
        case OMX_VIDEO_CodingH263:       return "H.263";
        case OMX_VIDEO_CodingMPEG4:      return "MPEG4";
        case OMX_VIDEO_CodingWMV:        return "Windows Media Video";
        case OMX_VIDEO_CodingRV:         return "RealVideo";
        case OMX_VIDEO_CodingAVC:        return "H.264/AVC";
        case OMX_VIDEO_CodingMJPEG:      return "Motion JPEG";
        case OMX_VIDEO_CodingVP6:        return "VP6";
        case OMX_VIDEO_CodingVP7:        return "VP7";
        case OMX_VIDEO_CodingVP8:        return "VP8";
        case OMX_VIDEO_CodingYUV:        return "Raw YUV video";
        case OMX_VIDEO_CodingSorenson:   return "Sorenson";
        case OMX_VIDEO_CodingTheora:     return "OGG Theora";
        case OMX_VIDEO_CodingMVC:        return "H.264/MVC";
        default:
        {
            static char result[] = "( unknown video coding type: 0xffffffff )";
            snprintf(&(result[strlen("( unknown video coding type: ")]), strlen("0xffffffff )") , "0x%08x )", c);
            return result;
        }
    }
}

const char* OMX::getImageCoding(OMX_IMAGE_CODINGTYPE c){
    switch(c) {
        case OMX_IMAGE_CodingUnused:     return "OMX_IMAGE_CodingUnused";      /**< Value when format is N/A */
        case OMX_IMAGE_CodingAutoDetect: return "OMX_IMAGE_CodingAutoDetect";  /**< Auto detection of image format */
        case OMX_IMAGE_CodingJPEG:       return "OMX_IMAGE_CodingJPEG";        /**< JPEG/JFIF image format */
        case OMX_IMAGE_CodingJPEG2K:     return "OMX_IMAGE_CodingJPEG2K";      /**< JPEG 2000 image format */
        case OMX_IMAGE_CodingEXIF:       return "OMX_IMAGE_CodingEXIF";        /**< EXIF image format */
        case OMX_IMAGE_CodingTIFF:       return "OMX_IMAGE_CodingTIFF";        /**< TIFF image format */
        case OMX_IMAGE_CodingGIF:        return "OMX_IMAGE_CodingGIF";         /**< Graphics image format */
        case OMX_IMAGE_CodingPNG:        return "OMX_IMAGE_CodingPNG";         /**< PNG image format */
        case OMX_IMAGE_CodingLZW:        return "OMX_IMAGE_CodingLZW";         /**< LZW image format */
        case OMX_IMAGE_CodingBMP:        return "OMX_IMAGE_CodingBMP";         /**< Windows Bitmap format */
        case OMX_IMAGE_CodingTGA:        return "OMX_IMAGE_CodingTGA";
        case OMX_IMAGE_CodingPPM:        return "OMX_IMAGE_CodingPPM";
        default:
        {
            static char result[] = "( unknown image coding type: 0xffffffff )";
            snprintf(&(result[strlen("( unknown image coding type: ")]), strlen("0xffffffff )") , "0x%08x )", c);
            return result;
        }
    }
}

const char* OMX::getAudioCoding(OMX_AUDIO_CODINGTYPE c) {
    switch(c) {
        case OMX_AUDIO_CodingUnused:     return "OMX_AUDIO_CodingUnused";      /**< Placeholder value when coding is N/A  */
        case OMX_AUDIO_CodingAutoDetect: return "OMX_AUDIO_CodingAutoDetect";  /**< auto detection of audio format */
        case OMX_AUDIO_CodingPCM:        return "OMX_AUDIO_CodingPCM";         /**< Any variant of PCM coding */
        case OMX_AUDIO_CodingADPCM:      return "OMX_AUDIO_CodingADPCM";       /**< Any variant of ADPCM encoded data */
        case OMX_AUDIO_CodingAMR:        return "OMX_AUDIO_CodingAMR";         /**< Any variant of AMR encoded data */
        case OMX_AUDIO_CodingGSMFR:      return "OMX_AUDIO_CodingGSMFR";       /**< Any variant of GSM fullrate (i.e. GSM610) */
        case OMX_AUDIO_CodingGSMEFR:     return "OMX_AUDIO_CodingGSMEFR";      /**< Any variant of GSM Enhanced Fullrate encoded data*/
        case OMX_AUDIO_CodingGSMHR:      return "OMX_AUDIO_CodingGSMHR";       /**< Any variant of GSM Halfrate encoded data */
        case OMX_AUDIO_CodingPDCFR:      return "OMX_AUDIO_CodingPDCFR";       /**< Any variant of PDC Fullrate encoded data */
        case OMX_AUDIO_CodingPDCEFR:     return "OMX_AUDIO_CodingPDCEFR";      /**< Any variant of PDC Enhanced Fullrate encoded data */
        case OMX_AUDIO_CodingPDCHR:      return "OMX_AUDIO_CodingPDCHR";       /**< Any variant of PDC Halfrate encoded data */
        case OMX_AUDIO_CodingTDMAFR:     return "OMX_AUDIO_CodingTDMAFR";      /**< Any variant of TDMA Fullrate encoded data (TIA/EIA-136-420) */
        case OMX_AUDIO_CodingTDMAEFR:    return "OMX_AUDIO_CodingTDMAEFR";     /**< Any variant of TDMA Enhanced Fullrate encoded data (TIA/EIA-136-410) */
        case OMX_AUDIO_CodingQCELP8:     return "OMX_AUDIO_CodingQCELP8";      /**< Any variant of QCELP 8kbps encoded data */
        case OMX_AUDIO_CodingQCELP13:    return "OMX_AUDIO_CodingQCELP13";     /**< Any variant of QCELP 13kbps encoded data */
        case OMX_AUDIO_CodingEVRC:       return "OMX_AUDIO_CodingEVRC";        /**< Any variant of EVRC encoded data */
        case OMX_AUDIO_CodingSMV:        return "OMX_AUDIO_CodingSMV";         /**< Any variant of SMV encoded data */
        case OMX_AUDIO_CodingG711:       return "OMX_AUDIO_CodingG711";        /**< Any variant of G.711 encoded data */
        case OMX_AUDIO_CodingG723:       return "OMX_AUDIO_CodingG723";        /**< Any variant of G.723 dot 1 encoded data */
        case OMX_AUDIO_CodingG726:       return "OMX_AUDIO_CodingG726";        /**< Any variant of G.726 encoded data */
        case OMX_AUDIO_CodingG729:       return "OMX_AUDIO_CodingG729";        /**< Any variant of G.729 encoded data */
        case OMX_AUDIO_CodingAAC:        return "OMX_AUDIO_CodingAAC";         /**< Any variant of AAC encoded data */
        case OMX_AUDIO_CodingMP3:        return "OMX_AUDIO_CodingMP3";         /**< Any variant of MP3 encoded data */
        case OMX_AUDIO_CodingSBC:        return "OMX_AUDIO_CodingSBC";         /**< Any variant of SBC encoded data */
        case OMX_AUDIO_CodingVORBIS:     return "OMX_AUDIO_CodingVORBIS";      /**< Any variant of VORBIS encoded data */
        case OMX_AUDIO_CodingWMA:        return "OMX_AUDIO_CodingWMA";         /**< Any variant of WMA encoded data */
        case OMX_AUDIO_CodingRA:         return "OMX_AUDIO_CodingRA";          /**< Any variant of RA encoded data */
        case OMX_AUDIO_CodingMIDI:       return "OMX_AUDIO_CodingMIDI";        /**< Any variant of MIDI encoded data */
        //OMX_AUDIO_CodingKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
        //OMX_AUDIO_CodingVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
        case OMX_AUDIO_CodingFLAC:       return "OMX_AUDIO_CodingFLAC";        /**< Any variant of FLAC */
        case OMX_AUDIO_CodingDDP:        return "OMX_AUDIO_CodingDDP";         /**< Any variant of Dolby Digital Plus */
        case OMX_AUDIO_CodingDTS:        return "OMX_AUDIO_CodingDTS";         /**< Any variant of DTS */
        case OMX_AUDIO_CodingWMAPRO:     return "OMX_AUDIO_CodingWMAPRO";      /**< Any variant of WMA Professional */
        case OMX_AUDIO_CodingATRAC3:     return "OMX_AUDIO_CodingATRAC3";      /**< Sony ATRAC-3 variants */
        case OMX_AUDIO_CodingATRACX:     return "OMX_AUDIO_CodingATRACX";      /**< Sony ATRAC-X variants */
        case OMX_AUDIO_CodingATRACAAL:   return "OMX_AUDIO_CodingATRACAAL";
        default:
        {
            static char result[] = "( unknown audio format type: 0xffffffff )";
            snprintf(&(result[strlen("( unknown audio format type: ")]), strlen("0xffffffff )") , "0x%08x )", c);
            return result;
        }
    }
}

const char* OMX::getColorFormat(OMX_COLOR_FORMATTYPE c) {
    switch(c) {
        case OMX_COLOR_FormatUnused:                 return "OMX_COLOR_FormatUnused: not used";
        case OMX_COLOR_FormatMonochrome:             return "OMX_COLOR_FormatMonochrome";
        case OMX_COLOR_Format8bitRGB332:             return "OMX_COLOR_Format8bitRGB332";
        case OMX_COLOR_Format12bitRGB444:            return "OMX_COLOR_Format12bitRGB444";
        case OMX_COLOR_Format16bitARGB4444:          return "OMX_COLOR_Format16bitARGB4444";
        case OMX_COLOR_Format16bitARGB1555:          return "OMX_COLOR_Format16bitARGB1555";
        case OMX_COLOR_Format16bitRGB565:            return "OMX_COLOR_Format16bitRGB565";
        case OMX_COLOR_Format16bitBGR565:            return "OMX_COLOR_Format16bitBGR565";
        case OMX_COLOR_Format18bitRGB666:            return "OMX_COLOR_Format18bitRGB666";
        case OMX_COLOR_Format18bitARGB1665:          return "OMX_COLOR_Format18bitARGB1665";
        case OMX_COLOR_Format19bitARGB1666:          return "OMX_COLOR_Format19bitARGB1666";
        case OMX_COLOR_Format24bitRGB888:            return "OMX_COLOR_Format24bitRGB888";
        case OMX_COLOR_Format24bitBGR888:            return "OMX_COLOR_Format24bitBGR888";
        case OMX_COLOR_Format24bitARGB1887:          return "OMX_COLOR_Format24bitARGB1887";
        case OMX_COLOR_Format25bitARGB1888:          return "OMX_COLOR_Format25bitARGB1888";
        case OMX_COLOR_Format32bitBGRA8888:          return "OMX_COLOR_Format32bitBGRA8888";
        case OMX_COLOR_Format32bitARGB8888:          return "OMX_COLOR_Format32bitARGB8888";
        case OMX_COLOR_FormatYUV411Planar:           return "OMX_COLOR_FormatYUV411Planar";
        case OMX_COLOR_FormatYUV411PackedPlanar:     return "OMX_COLOR_FormatYUV411PackedPlanar: Planes fragmented when a frame is split in multiple buffers";
        case OMX_COLOR_FormatYUV420Planar:           return "OMX_COLOR_FormatYUV420Planar: Planar YUV, 4:2:0 (I420)";
        case OMX_COLOR_FormatYUV420PackedPlanar:     return "OMX_COLOR_FormatYUV420PackedPlanar: Planar YUV, 4:2:0 (I420), planes fragmented when a frame is split in multiple buffers";
        case OMX_COLOR_FormatYUV420SemiPlanar:       return "OMX_COLOR_FormatYUV420SemiPlanar, Planar YUV, 4:2:0 (NV12), U and V planes interleaved with first U value";
        case OMX_COLOR_FormatYUV422Planar:           return "OMX_COLOR_FormatYUV422Planar";
        case OMX_COLOR_FormatYUV422PackedPlanar:     return "OMX_COLOR_FormatYUV422PackedPlanar: Planes fragmented when a frame is split in multiple buffers";
        case OMX_COLOR_FormatYUV422SemiPlanar:       return "OMX_COLOR_FormatYUV422SemiPlanar";
        case OMX_COLOR_FormatYCbYCr:                 return "OMX_COLOR_FormatYCbYCr";
        case OMX_COLOR_FormatYCrYCb:                 return "OMX_COLOR_FormatYCrYCb";
        case OMX_COLOR_FormatCbYCrY:                 return "OMX_COLOR_FormatCbYCrY";
        case OMX_COLOR_FormatCrYCbY:                 return "OMX_COLOR_FormatCrYCbY";
        case OMX_COLOR_FormatYUV444Interleaved:      return "OMX_COLOR_FormatYUV444Interleaved";
        case OMX_COLOR_FormatRawBayer8bit:           return "OMX_COLOR_FormatRawBayer8bit";
        case OMX_COLOR_FormatRawBayer10bit:          return "OMX_COLOR_FormatRawBayer10bit";
        case OMX_COLOR_FormatRawBayer8bitcompressed: return "OMX_COLOR_FormatRawBayer8bitcompressed";
        case OMX_COLOR_FormatL2:                     return "OMX_COLOR_FormatL2";
        case OMX_COLOR_FormatL4:                     return "OMX_COLOR_FormatL4";
        case OMX_COLOR_FormatL8:                     return "OMX_COLOR_FormatL8";
        case OMX_COLOR_FormatL16:                    return "OMX_COLOR_FormatL16";
        case OMX_COLOR_FormatL24:                    return "OMX_COLOR_FormatL24";
        case OMX_COLOR_FormatL32:                    return "OMX_COLOR_FormatL32";
        case OMX_COLOR_FormatYUV420PackedSemiPlanar: return "OMX_COLOR_FormatYUV420PackedSemiPlanar: Planar YUV, 4:2:0 (NV12), planes fragmented when a frame is split in multiple buffers, U and V planes interleaved with first U value";
        case OMX_COLOR_FormatYUV422PackedSemiPlanar: return "OMX_COLOR_FormatYUV422PackedSemiPlanar: Planes fragmented when a frame is split in multiple buffers";
        case OMX_COLOR_Format18BitBGR666:            return "OMX_COLOR_Format18BitBGR666";
        case OMX_COLOR_Format24BitARGB6666:          return "OMX_COLOR_Format24BitARGB6666";
        case OMX_COLOR_Format24BitABGR6666:          return "OMX_COLOR_Format24BitABGR6666";
        case OMX_COLOR_Format32bitABGR8888:          return "OMX_COLOR_Format32bitABGR8888";
        case OMX_COLOR_Format8bitPalette:            return "OMX_COLOR_Format8bitPalette";
        case OMX_COLOR_FormatYUVUV128:               return "OMX_COLOR_FormatYUVUV128";
        case OMX_COLOR_FormatRawBayer12bit:          return "OMX_COLOR_FormatRawBayer12bit";
        case OMX_COLOR_FormatBRCMEGL:                return "OMX_COLOR_FormatBRCMEGL";
        case OMX_COLOR_FormatBRCMOpaque:             return "OMX_COLOR_FormatBRCMOpaque";
        case OMX_COLOR_FormatYVU420PackedPlanar:     return "OMX_COLOR_FormatYVU420PackedPlanar";
        case OMX_COLOR_FormatYVU420PackedSemiPlanar: return "OMX_COLOR_FormatYVU420PackedSemiPlanar";
        default:
        {
            static char result[] = "( unknown color format type: 0xffffffff )";
            snprintf(&(result[strlen("( unknown color format type: ")]), strlen("0xffffffff )") , "0x%08x )", c);
            return result;
        }
    }
}
