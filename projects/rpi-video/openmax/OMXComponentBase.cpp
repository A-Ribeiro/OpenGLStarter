#include "OMXComponentBase.h"

OMX_CALLBACKTYPE OMXComponentBase::callbacks = {
    OMXComponentBase::GlobalEventHandler,
    OMXComponentBase::GlobalEmptyBufferDone,
    OMXComponentBase::GlobalFillBufferDone
};

//
// static processors
//
OMX_ERRORTYPE OMXComponentBase::GlobalEventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_EVENTTYPE eEvent,OMX_U32 nData1,OMX_U32 nData2,OMX_PTR pEventData) {
    OMX::printEvent(hComponent, eEvent, nData1, nData2);
    OMXComponentBase *componentBase = (OMXComponentBase *)pAppData;
    return componentBase->eventHandler(hComponent,pAppData,eEvent,nData1,nData2,pEventData);
}

OMX_ERRORTYPE OMXComponentBase::GlobalEmptyBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer) {
    OMXComponentBase *componentBase = (OMXComponentBase *)pAppData;
    return componentBase->emptyBufferDone(hComponent,pAppData,pBuffer);
}

OMX_ERRORTYPE OMXComponentBase::GlobalFillBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer) {
    OMXComponentBase *componentBase = (OMXComponentBase *)pAppData;
    return componentBase->fillBufferDone(hComponent,pAppData,pBuffer);
}
//
// Virtual methods
//
OMX_ERRORTYPE OMXComponentBase::eventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_EVENTTYPE eEvent,OMX_U32 nData1,OMX_U32 nData2,OMX_PTR pEventData){
    switch(eEvent) {
        case OMX_EventCmdComplete:
            if(nData1 == OMX_CommandFlush) {
                mutex.lock();
                flushed = true;
                mutex.unlock();
            }
            break;
        case OMX_EventError:
            fprintf(stderr, "OMX_EventError: 0x%08x -> %s\n", nData1, OMX::getError((OMX_ERRORTYPE)nData1) );
            exit(-1);
            break;
        default:
            break;
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE OMXComponentBase::emptyBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE OMXComponentBase::fillBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer) {
    return OMX_ErrorNone;
}

OMXComponentBase::OMXComponentBase(){

    OMX::Instance()->init();

    //handle = 0;
    flushed = false;

    //memset(&callbacks, 0, sizeof(callbacks));
    //callbacks.EventHandler    = OMXComponentBase::GlobalEventHandler;
    //callbacks.EmptyBufferDone = OMXComponentBase::GlobalEmptyBufferDone;
    //callbacks.FillBufferDone  = OMXComponentBase::GlobalFillBufferDone;

}

OMXComponentBase::~OMXComponentBase() {

}
