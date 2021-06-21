#include "OMXComponentBase.h"

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#define ROUND_UP_2(num) (((num)+1)&~1)
#define ROUND_UP_4(num) (((num)+3)&~3)

class BroadcomH264Encode : public OMXComponentBase {

    OMX_U32 width;
    OMX_U32 height;

    OMX_U32 widthUV;
    OMX_U32 heightUV;


    OMX_U32 strideY;
    OMX_U32 strideUV;

    OMX_U32 sizeY;
    OMX_U32 sizeUV;

    OMX_U32 sizeYUV;


public:
    OMX_HANDLETYPE encoder;
    OMX_BUFFERHEADERTYPE *inputBuffer;
    OMX_BUFFERHEADERTYPE *outputBuffer;

    volatile bool canFillInputBuffer;
    volatile bool needOutBuffer;

    PlatformTime time;

    int stdout_fd;

    BroadcomH264Encode(OMX_U32 width, OMX_U32 height, OMX_U32 framerate, OMX_U32 bitrate):OMXComponentBase() {

        stdout_fd = fileno(stdout);

        this->width = width;
        this->height = height;

        needOutBuffer = false;
        canFillInputBuffer = true;

        encoder = OMX::createHandle("OMX.broadcom.video_encode",this);

        fprintf(stderr,"\nBroadcomH264Encode\n\n");

        //input port
        OMX::printPort(encoder,200);
        OMX::printPortSupportedFormats(encoder,200);

        //output port
        OMX::printPort(encoder,201);
        OMX::printPortSupportedFormats(encoder,201);


        OMX_PARAM_PORTDEFINITIONTYPE portdef = OMX::getPortDefinition(encoder,200);
        portdef.format.video.nFrameWidth  = width;
        portdef.format.video.nFrameHeight = height;
        portdef.format.video.xFramerate   = framerate << 16;
        portdef.format.video.nStride      = (portdef.format.video.nFrameWidth + portdef.nBufferAlignment - 1) & (~(portdef.nBufferAlignment - 1));
        portdef.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
        OMX::setPortDefinition(encoder, portdef);

        //set output port using the input as basis
        portdef = OMX::getPortDefinition(encoder,200);
        portdef.nPortIndex = 201;
        portdef.format.video.eColorFormat = OMX_COLOR_FormatUnused;
        portdef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
        portdef.format.video.nBitrate     = bitrate;
        OMX::setPortDefinition(encoder, portdef);

        OMX::setVideoBitrate(encoder, 201, OMX_Video_ControlRateVariable, bitrate);
        OMX::setVideoPortFormat(encoder, 201, OMX_VIDEO_CodingAVC);

        OMX::setState(encoder, OMX_StateIdle);

        OMX::setEnablePort(encoder, 200, OMX_TRUE);
        OMX::setEnablePort(encoder, 201, OMX_TRUE);

        //allocate buffers
        inputBuffer = OMX::allocateBuffer(encoder, 200);
        outputBuffer = OMX::allocateBuffer(encoder, 201);

        //clear input buffer
        memset(inputBuffer->pBuffer, 0, inputBuffer->nAllocLen);

        OMX::setState(encoder, OMX_StateExecuting);

        fprintf(stderr,"\nAfter Initialization\n\n");
        OMX::printPort(encoder,200);
        OMX::printPort(encoder,201);


        widthUV = ROUND_UP_2(width) / 2;
        heightUV = ROUND_UP_2(height) / 2;

        strideY = ROUND_UP_4(width);
        strideUV = ROUND_UP_4(widthUV);

        sizeY = strideY * height;
        sizeUV = strideUV * heightUV;
        sizeYUV = sizeY + sizeUV * 2;


        portdef = OMX::getPortDefinition(encoder,200);

        if ( strideY != portdef.format.video.nStride ){
            fprintf(stderr,"buffer stride calculation differs from the codec ( %i %i )\n",strideY, portdef.format.video.nStride);
            exit(-1);
        }

        if ( sizeYUV != portdef.nBufferSize ){
            fprintf(stderr,"buffer size calculation differs from the codec ( %i %i )\n",sizeYUV, portdef.nBufferSize);
            exit(-1);
        }

        OMX::postFillThisBuffer(encoder, outputBuffer);

    }

    virtual ~BroadcomH264Encode() {

        OMX::portFlush(this, encoder, 200);
        OMX::portFlush(this, encoder, 201);

        OMX::setEnablePort(encoder, 200, OMX_FALSE);
        OMX::setEnablePort(encoder, 201, OMX_FALSE);

        OMX::freeBuffer(encoder, 200, inputBuffer);
        OMX::freeBuffer(encoder, 201, outputBuffer);

        OMX::setState(encoder, OMX_StateIdle);
        OMX::setState(encoder, OMX_StateLoaded);

        OMX::freeHandle(encoder);
    }


    void postYUV(int8_t *yuv, int length) {
        if (!canFillInputBuffer)
            return;

        time.update();

        canFillInputBuffer = false;

        for (int i=0;i<1200;i++)
            inputBuffer->pBuffer[rand() % width + (rand() % height ) * strideY ] = rand() % 256;

        inputBuffer->nOffset = 0;
        inputBuffer->nFilledLen = sizeYUV;

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
        if (!needOutBuffer)
            return;
        needOutBuffer = false;

        //printf("[BroadcomVideoEncode] makeOutBufferAvailable\n");

        OMX::postFillThisBuffer(encoder, outputBuffer);
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
