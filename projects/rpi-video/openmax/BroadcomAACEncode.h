#include "OMXComponentBase.h"

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

class BroadcomAACEncode : public OMXComponentBase {

public:
    OMX_HANDLETYPE encoder;

    OMX_BUFFERHEADERTYPE *inputBuffer;
    OMX_BUFFERHEADERTYPE *outputBuffer;

    volatile bool canFillInputBuffer;
    volatile bool needOutBuffer;
    volatile bool outputPortChanged;

    PlatformTime time;

    int stdout_fd;

    BroadcomAACEncode(OMX_U32 samplingrate, OMX_U32 bitrate):OMXComponentBase() {

        stdout_fd = fileno(stdout);

        needOutBuffer = false;
        canFillInputBuffer = true;

        encoder = OMX::createHandle("OMX.broadcom.audio_encode",this);

        fprintf(stderr,"\nBroadcomAACEncode\n\n");

        //input port
        OMX::printPort(encoder,160);
        OMX::printPortSupportedFormats(encoder,160);

        //output port
        OMX::printPort(encoder,161);
        OMX::printPortSupportedFormats(encoder,161);

		//
		// Audio Input setup
		//
        OMX_PARAM_PORTDEFINITIONTYPE portdef = OMX::getPortDefinition(encoder,160);

        //OMX_INIT_STRUCTURE(portdef);

        portdef.nPortIndex = 160;
        portdef.format.audio.eEncoding  = OMX_AUDIO_CodingPCM;
        OMX::setPortDefinition(encoder, portdef);

        fprintf(stderr, " setPortDefinition 160, OK\n");

        //input port configuration (2 channels, signed interleaved 16 bits, little endian, linear
        OMX::setAudioPCMParam(encoder, 160, samplingrate);

        fprintf(stderr, " setAudioPCMParam 160, OK\n");

        OMX::printPort(encoder,160);

        //
		// Audio Output setup
		//

        //set output port using the input as basis
        portdef = OMX::getPortDefinition(encoder,161);
        portdef.nPortIndex = 161;
        portdef.format.audio.eEncoding  = OMX_AUDIO_CodingAAC;
        OMX::setPortDefinition(encoder, portdef);
        fprintf(stderr, " setPortDefinition 161, OK\n");

        //OMX::setVideoBitrate(encoder, 201, OMX_Video_ControlRateVariable, bitrate);
        //OMX::setVideoPortFormat(encoder, 201, OMX_VIDEO_CodingAVC);

        OMX::setAudioAACParam(encoder, 161, bitrate, samplingrate);
        fprintf(stderr, " setAudioAACParam 161, OK\n");

        OMX::printPort(encoder,161);

        fprintf(stderr,"\nBefore OMX_StateIdle\n\n");
        OMX::setState(encoder, OMX_StateIdle);

        fprintf(stderr,"\nOMX::setEnablePort(encoder, 160, OMX_TRUE)\n\n");
        OMX::setEnablePort(encoder, 160, OMX_TRUE);

        fprintf(stderr,"\nOMX::setEnablePort(encoder, 161, OMX_TRUE)\n\n");
        OMX::setEnablePort(encoder, 161, OMX_TRUE);

        //allocate buffers
        fprintf(stderr,"\nBefore allocate 1\n\n");
        inputBuffer = OMX::allocateBuffer(encoder, 160);
        fprintf(stderr,"\nBefore allocate 2\n\n");
        outputBuffer = OMX::allocateBuffer(encoder, 161);
        //outputBuffer = NULL;

        //clear input buffer
        memset(inputBuffer->pBuffer, 0, inputBuffer->nAllocLen);

        outputPortChanged = false;

        fprintf(stderr,"\nBefore Initialization\n\n");

        OMX::setState(encoder, OMX_StateExecuting);

        fprintf(stderr,"\nAfter Initialization\n\n");
        OMX::printPort(encoder,160);
        OMX::printPort(encoder,161);

        OMX::postFillThisBuffer(encoder, outputBuffer);
    }

    virtual ~BroadcomAACEncode() {

        OMX::portFlush(this, encoder, 160);
        OMX::portFlush(this, encoder, 161);

        OMX::setEnablePort(encoder, 160, OMX_FALSE);
        if (outputBuffer != NULL)
			OMX::setEnablePort(encoder, 161, OMX_FALSE);

        OMX::freeBuffer(encoder, 160, inputBuffer);
        if (outputBuffer != NULL)
			OMX::freeBuffer(encoder, 161, outputBuffer);

        OMX::setState(encoder, OMX_StateIdle);
        OMX::setState(encoder, OMX_StateLoaded);

        OMX::freeHandle(encoder);
    }


    void postPCM16bitSignedInterleaved(int16_t *buffer, int length) {
        if (!canFillInputBuffer)
            return;

        time.update();

        canFillInputBuffer = false;

        for (int i=0;i<inputBuffer->nAllocLen;i++)
            inputBuffer->pBuffer[rand() % inputBuffer->nAllocLen ] = rand() % 256;

        inputBuffer->nOffset = 0;
        inputBuffer->nFilledLen = inputBuffer->nAllocLen;

        //fprintf(stderr,"postYUV filledLength: %i \n",inputBuffer->nFilledLen);
        //if EOF
        //inputBuffer->nFlags = OMX_BUFFERFLAG_EOS;

        //uint64_t timestamp = (uint64_t)inputBuffer->nTimeStamp.nHighPart;
        //timestamp = timestamp << 32;
        //timestamp |= (uint64_t)inputBuffer->nTimeStamp.nLowPart;
        static uint64_t timestamp = -time.deltaTimeMicro;
        //((uint32_t*)&timestamp)[0] = inputBuffer->nTimeStamp.nLowPart;
        //((uint32_t*)&timestamp)[1] = inputBuffer->nTimeStamp.nHighPart;

        timestamp += time.deltaTimeMicro;

        inputBuffer->nTimeStamp.nLowPart = ((uint32_t*)&timestamp)[0];
        inputBuffer->nTimeStamp.nHighPart = ((uint32_t*)&timestamp)[1];

        //fprintf(stderr,"time: %llu\n", timestamp);
        //fprintf(stderr,"  low: %u\n", inputBuffer->nTimeStamp.nLowPart);
        //fprintf(stderr,"  high: %u\n", inputBuffer->nTimeStamp.nHighPart);

        //inputBuffer->nTimeStamp;

        OMX::postEmptyThisBuffer(encoder, inputBuffer);

    }

    void makeOutBufferAvailable(){


		if (outputPortChanged){
			outputPortChanged = false;

			if (outputBuffer != NULL) {
				OMX::setEnablePort(encoder, 161, OMX_FALSE);
				OMX::freeBuffer(encoder, 161, outputBuffer);
				outputBuffer = NULL;
			}
			OMX::setEnablePort(encoder, 161, OMX_TRUE);
			outputBuffer = OMX::allocateBuffer(encoder, 161);

			OMX::postFillThisBuffer(encoder, outputBuffer);
			fprintf(stderr,"*******\nOUTPUT PORT CHANGED!!!\n*******\n");
			OMX::printPort(encoder, 161);

			return;
		}


        if (!needOutBuffer)
            return;
        needOutBuffer = false;

        //printf("[BroadcomVideoEncode] makeOutBufferAvailable\n");

        OMX::postFillThisBuffer(encoder, outputBuffer);
    }


	OMX_ERRORTYPE eventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_EVENTTYPE eEvent,OMX_U32 nData1,OMX_U32 nData2,OMX_PTR pEventData){

		OMXComponentBase::eventHandler(hComponent,pAppData,eEvent,nData1,nData2,pEventData);

		switch(eEvent) {
			case OMX_EventPortSettingsChanged:
				if (nData1 == 161) {
                    outputPortChanged = true; // the nBufferSize changed
                    fprintf(stderr," -- output port changed --\n");
                }
                break;
			default:
				break;
		}

		return OMX_ErrorNone;
	}


    // OMX callbacks
    OMX_ERRORTYPE emptyBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer){
        //fprintf(stderr,"[BroadcomVideoEncode] emptyBufferDone\n");
        canFillInputBuffer = true;
        return OMX_ErrorNone;
    }

    // OMX callbacks
    OMX_ERRORTYPE fillBufferDone(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_BUFFERHEADERTYPE* pBuffer){

        if(pBuffer->nFlags & OMX_BUFFERFLAG_ENDOFFRAME) {
            //fprintf(stderr,"encoding frameout...\n");
            //frame_out++;
        }

        // Flush buffer to output file
        //fwrite(pBuffer->pBuffer + pBuffer->nOffset, 1, pBuffer->nFilledLen, stdout);
        write(stdout_fd, pBuffer->pBuffer + pBuffer->nOffset, pBuffer->nFilledLen);

        needOutBuffer = true;
        //OMX::postFillThisBuffer(encoder, outputBuffer);

        //fprintf(stderr,"[BroadcomVideoEncode] fillBufferDone\n");
        return OMX_ErrorNone;
    }



};

