#include "OMXComponentBase.h"

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;


struct NALu {
  unsigned int pos, size, type;
};

static unsigned int FindAllNALu(unsigned char * source, unsigned int size, NALu * table)
{
  unsigned int num = 0;
  for (unsigned int i = 0; i < size; i++)
  {
    if (source[i]     == 0x00)
    if (source[i + 1] == 0x00)
    if (source[i + 2] == 0x00)
    if (source[i + 3] == 0x01)
    {
      table[num] = (NALu){i, size - i, ((unsigned int)(source[i + 4] & 31)) };
      if (num != 0)
        table[num - 1].size = i - table[num - 1].pos;
      num++;
    }
  }
  return num;
}

class BroadcomH264Decode : public OMXComponentBase {

    PlatformMutex mutex;

    OMX_BUFFERHEADERTYPE * fifoRemove() {

        OMX_BUFFERHEADERTYPE *result = NULL;

        while (result == NULL && !OMX::Instance()->appExitRequested) {
            mutex.lock();
            if (availableBuffers.size() > 0) {
                result = availableBuffers[0];
                availableBuffers.erase(availableBuffers.begin());
            }
            mutex.unlock();
            if (result == NULL)
                usleep(100);
        }

        return result;
    }

    void fifoInsert(OMX_BUFFERHEADERTYPE *buffer) {
        mutex.lock();
        availableBuffers.push_back(buffer);
        mutex.unlock();
    }


public:
    OMX_HANDLETYPE decoder;

    std::vector<OMX_BUFFERHEADERTYPE *> ibuffers;
    std::vector<OMX_BUFFERHEADERTYPE *> availableBuffers;

    OMX_BUFFERHEADERTYPE *outputBuffer;

    bool firstFrame;
    volatile bool outputPortChanged;
    volatile bool CanReadOutputBuffer;

    BroadcomH264Decode():OMXComponentBase() {

        outputBuffer = NULL;

        decoder = OMX::createHandle("OMX.broadcom.video_decode",this);

        fprintf(stderr,"\nBroadcomH264Decode\n\n");


        OMX::setVideoPortFormat(decoder, 130, OMX_VIDEO_CodingAVC);
        OMX::setVideoPortFormat(decoder, 131, OMX_VIDEO_CodingUnused, OMX_COLOR_FormatYUV420Planar);


        //input port
        OMX::printPort(decoder,130);
        OMX::printPortSupportedFormats(decoder,130);

        //output port
        OMX::printPort(decoder,131);
        OMX::printPortSupportedFormats(decoder,131);

        OMX::setState(decoder, OMX_StateIdle);

        OMX::setEnablePort(decoder, 130, OMX_TRUE);
        //OMX::setEnablePort(decoder, 131, OMX_TRUE);

        //allocate input buffers
        OMX_PARAM_PORTDEFINITIONTYPE inputPortDef = OMX::getPortDefinition(decoder, 130);
        for (int i=0;i<inputPortDef.nBufferCountActual;i++)
            ibuffers.push_back( OMX::allocateBuffer(decoder, 130) );

        for (int i=0;i<ibuffers.size();i++){
            memset(ibuffers[i]->pBuffer, 0, ibuffers[i]->nAllocLen);
            fifoInsert(ibuffers[i]);
        }

        CanReadOutputBuffer = false;
        outputPortChanged = false;
        firstFrame = true;

        OMX::setState(decoder, OMX_StateExecuting);

        fprintf(stderr, "Initialized...\n");
    }


    virtual ~BroadcomH264Decode() {

        OMX::portFlush(this, decoder, 130);
        OMX::portFlush(this, decoder, 131);

        OMX::setEnablePort(decoder, 130, OMX_FALSE);
        if (outputBuffer != NULL)
            OMX::setEnablePort(decoder, 131, OMX_FALSE);

        for (int i=0;i<20;i++)
            OMX::freeBuffer(decoder, 130, ibuffers[i]);
        ibuffers.clear();
        availableBuffers.clear();

        if (outputBuffer != NULL)
            OMX::freeBuffer(decoder, 131, outputBuffer);

        OMX::setState(decoder, OMX_StateIdle);
        OMX::setState(decoder, OMX_StateLoaded);

        OMX::freeHandle(decoder);
    }


    void writeH264Buffer(unsigned char* buffer, int size){


        static NALu nalu[20];
        unsigned int nSize = FindAllNALu(buffer, size, nalu);

        //fprintf(stderr,"Number of nsizes: %i\n", nSize);
        for (unsigned int i = 0; i < nSize; i++) {
            OMX_BUFFERHEADERTYPE * inputBuffer = fifoRemove();
            if (inputBuffer == NULL)
                return;

            void * pStart = &buffer[nalu[i].pos];
            memcpy(inputBuffer->pBuffer, pStart, nalu[i].size);
            inputBuffer->nOffset = 0;
            inputBuffer->nFilledLen = nalu[i].size;

            if (firstFrame) {
                inputBuffer->nFlags = OMX_BUFFERFLAG_STARTTIME;
                firstFrame = false;
            } else
                inputBuffer->nFlags = 0;

            fprintf(stderr,"+");fflush(stderr);

            OMX::postEmptyThisBuffer(decoder, inputBuffer);



            if (outputPortChanged){
                outputPortChanged = false;

                if (outputBuffer != NULL) {
                    OMX::setEnablePort(decoder, 131, OMX_FALSE);
                    OMX::freeBuffer(decoder, 131, outputBuffer);
                    outputBuffer = NULL;
                }
                OMX::setEnablePort(decoder, 131, OMX_TRUE);
                outputBuffer = OMX::allocateBuffer(decoder, 131);

                OMX::postFillThisBuffer(decoder, outputBuffer);
                fprintf(stderr,"*******\nOUTPUT PORT CHANGED!!!\n*******\n");
                OMX::printPort(decoder, 131);
            }


            //get all frames
            if (CanReadOutputBuffer){
                CanReadOutputBuffer = false;
                //
                // Read OutputBuffer
                //
                OMX::postFillThisBuffer(decoder, outputBuffer);
                fprintf(stderr,"-");fflush(stderr);
            }

        }

    }

    OMX_ERRORTYPE eventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_EVENTTYPE eEvent,OMX_U32 nData1,OMX_U32 nData2,OMX_PTR pEventData){
        switch(eEvent) {
            case OMX_EventPortSettingsChanged:
                if (nData1 == 131) {
                    outputPortChanged = true;
                    fprintf(stderr," -- output port changed --\n");
                }
                break;
        }
        return OMXComponentBase::eventHandler(hComponent,pAppData,eEvent,nData1,nData2,pEventData);
    }

    // OMX callbacks
    OMX_ERRORTYPE emptyBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer){
        fifoInsert(pBuffer);
        return OMX_ErrorNone;
    }

    // OMX callbacks
    OMX_ERRORTYPE fillBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer){

        if(pBuffer->nFlags & OMX_BUFFERFLAG_ENDOFFRAME) {
            //fprintf(stderr,"[BroadcomVideoEncode] fillBufferDone / OMX_BUFFERFLAG_ENDOFFRAME\n");



        }

        CanReadOutputBuffer = true;

        return OMX_ErrorNone;
    }

};
