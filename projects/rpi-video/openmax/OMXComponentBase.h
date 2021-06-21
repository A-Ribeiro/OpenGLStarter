#ifndef OMX_COMPONENT_BASE_H
#define OMX_COMPONENT_BASE_H

#include "OMX.h"

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

class OMXComponentBase {

    //static processors
    static OMX_ERRORTYPE GlobalEventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_EVENTTYPE eEvent,OMX_U32 nData1,OMX_U32 nData2,OMX_PTR pEventData);
    static OMX_ERRORTYPE GlobalEmptyBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer);
    static OMX_ERRORTYPE GlobalFillBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer);

    public:
    //OMX_HANDLETYPE handle;
    bool flushed;
    PlatformMutex mutex;
    std::string name;

    static OMX_CALLBACKTYPE callbacks;

    virtual OMX_ERRORTYPE eventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_EVENTTYPE eEvent,OMX_U32 nData1,OMX_U32 nData2,OMX_PTR pEventData);
    virtual OMX_ERRORTYPE emptyBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer);
    virtual OMX_ERRORTYPE fillBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer);

    OMXComponentBase();
    virtual ~OMXComponentBase();
};

#endif
