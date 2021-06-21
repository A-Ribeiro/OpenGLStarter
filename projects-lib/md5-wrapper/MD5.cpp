#include <aribeiro/aribeiro.h>

#include <fstream>
#include <iostream>

#include "MD5.h"
#include <md5/md5.h>

#include <string.h>
#include <memory.h>
#include <stdio.h>

namespace md5Wrapper {

    std::string MD5::getHexStringHashFromBytes(const char* buffer, int size) {
        unsigned char result[16];
        get16bytesHashFromBytes(buffer, size, result);
        return _16BytesToHexString(result);
    }

    void MD5::get16bytesHashFromBytes(const char* buffer, int size, unsigned char outBuffer[16]) {
        md5_state_t state;
        md5_init(&state);
        md5_append(&state, (const md5_byte_t*)buffer, size);
        md5_finish(&state, (md5_byte_t*)outBuffer);
    }

    std::string MD5::getHexStringHashFromFile(const char* filename) {
        unsigned char result[16];
        get16bytesHashFromFile(filename, result);
        return _16BytesToHexString(result);
    }

    void MD5::get16bytesHashFromFile(const char* filename, unsigned char outBuffer[16]) {
        md5_state_t state;
        md5_init(&state);
        FILE *file = fopen(filename, "rb");
        unsigned char buffer[1024];
        if (file != NULL) {
            int len;
            while ((len = fread(buffer, sizeof(unsigned char), 1024, file)))
                md5_append(&state, (const md5_byte_t*)buffer, len);
            fclose(file);
        }
        else {
            ARIBEIRO_ABORT(
                true,
                "File not found: %s\n", filename);
            //fprintf(stderr, "[%s:%i]\n", __FILE__, __LINE__);
            //fprintf(stderr, "File not found: %s\n", filename);
            //exit(-1);
        }
        md5_finish(&state, (md5_byte_t*)outBuffer);
    }

    std::string MD5::_16BytesToHexString(unsigned char md5[16]) {
        char result[33];
        for (int i = 0; i < 16; i++)
            sprintf(&result[i * 2], "%02x", md5[i]);
        return result;
    }
}
