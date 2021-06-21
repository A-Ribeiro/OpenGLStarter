#ifndef RunLengthEncoding__H__
#define RunLengthEncoding__H__

#include <aribeiro/common.h>

namespace aRibeiro {

    class RunLengthEncodingBlackWhite {
        aribeiro_OnDataMethodPtrType OnData;

        uint8_t rle_count;
        int state;
    public:
        RunLengthEncodingBlackWhite(const aribeiro_OnDataMethodPtrType &OnData);
        ~RunLengthEncodingBlackWhite();
        void putByte(uint8_t byte);
        void endStream();
        void readFromFile(const char* file);
        void readFromBuffer(const char* data, size_t size);
    };

    class RunLengthDecodingBlackWhite {
        aribeiro_OnDataMethodPtrType OnData;

        uint8_t original_byte;
        bool rle_will_be_next;
    public:

        RunLengthDecodingBlackWhite(const aribeiro_OnDataMethodPtrType &OnData);
        void putByte(const uint8_t &byte);
        void readFromFile(const char* file);
        void readFromBuffer(const char* data, size_t size);
    };
}

#endif
