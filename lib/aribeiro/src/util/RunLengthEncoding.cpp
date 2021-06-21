#include "RunLengthEncoding.h"


namespace aRibeiro {

#define NORMAL 0
#define BLACK 1
#define WHITE 2

    RunLengthEncodingBlackWhite::RunLengthEncodingBlackWhite(const aribeiro_OnDataMethodPtrType &OnData) {
        this->OnData = OnData;
        rle_count = 0;
        state = NORMAL;
    }

    RunLengthEncodingBlackWhite::~RunLengthEncodingBlackWhite(){
        endStream();
    }

    void RunLengthEncodingBlackWhite::putByte(uint8_t byte){
        if (byte==0x00){
            if (state != BLACK){
                if (state != NORMAL){
                    //flush
                    OnData(&rle_count, 1);
                }
                OnData(&byte, 1);
                state = BLACK;
                rle_count = 1;
            }else{
                //state = black
                if (rle_count == 255){
                    //flush
                    OnData(&rle_count, 1);
                    uint8_t black=0x00;
                    OnData(&black, 1);
                    rle_count = 0;
                }
                rle_count++;
            }
        }else if (byte==0xff){
            if (state != WHITE){
                if (state != NORMAL){
                    //flush
                    OnData(&rle_count, 1);
                }
                OnData(&byte, 1);
                state = WHITE;
                rle_count = 1;
            }else{
                //state = white
                if (rle_count == 255){
                    //flush
                    OnData(&rle_count, 1);
                    uint8_t white=0xff;
                    OnData(&white, 1);
                    rle_count = 0;
                }
                rle_count++;
            }
        }else{
            if (state != NORMAL){
                //flush
                OnData(&rle_count, 1);
                state = NORMAL;
            }
            //normal case
            OnData(&byte, 1);
        }
    }

    void RunLengthEncodingBlackWhite::endStream(){
        if (state != NORMAL){
            //flush
            OnData(&rle_count, 1);
            state = NORMAL;
        }
    }

    void RunLengthEncodingBlackWhite::readFromFile(const char* file) {
        FILE* in = fopen(file, "rb");
        if (in) {
            while (!feof(in)) {
                uint8_t c;
                fread(&c, sizeof(uint8_t), 1, in);
                putByte(c);
            }
            fclose(in);
            endStream();
        }
    }

    void RunLengthEncodingBlackWhite::readFromBuffer(const char* data, size_t size) {
        for (size_t i=0;i<size;i++){
            putByte(data[i]);
        }
        endStream();
    }


    RunLengthDecodingBlackWhite::RunLengthDecodingBlackWhite(const aribeiro_OnDataMethodPtrType &OnData) {
        this->OnData = OnData;
        original_byte = 0;
        rle_will_be_next = false;
    }

    void RunLengthDecodingBlackWhite::putByte(const uint8_t &byte){
        if (rle_will_be_next){
            rle_will_be_next = false;
            for(int i=0;i<byte;i++)
                OnData(&original_byte,1);
        } else if (byte == 0x00 || byte == 0xff) {
            original_byte = byte;
            rle_will_be_next = true;
        }
        else {
            OnData(&byte,1);
        }
    }

    void RunLengthDecodingBlackWhite::readFromFile(const char* file) {
        FILE* in = fopen(file, "rb");
        if (in) {
            while (!feof(in)) {
                uint8_t c;
                fread(&c, sizeof(uint8_t), 1, in);
                putByte(c);
            }
            fclose(in);
        }
    }

    void RunLengthDecodingBlackWhite::readFromBuffer(const char* data, size_t size) {
        for (size_t i=0;i<size;i++){
            putByte(data[i]);
        }
    }

}