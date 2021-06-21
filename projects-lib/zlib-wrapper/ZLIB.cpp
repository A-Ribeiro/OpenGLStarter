#include "ZLIB.h"

#include <zlib/zlib.h>
#include <md5-wrapper/md5-wrapper.h>
using namespace md5Wrapper;

namespace zlibWrapper {
    void ZLIB::compress(const uint8_t* data, uint32_t size,
        const aRibeiro::aribeiro_OnDataMethodPtrType &OnData) {
        uLongf zlibOutput_Length = compressBound(size);
        zlibOutput.resize(16 + zlibOutput_Length + sizeof(uint32_t));

        memcpy(&zlibOutput[16], &size, sizeof(uint32_t));

        int result = ::compress2((Bytef *)&zlibOutput[16 + sizeof(uint32_t)],
            &zlibOutput_Length,
            (Bytef *)&data[0],
            size, Z_BEST_COMPRESSION);

        ARIBEIRO_ABORT(
            (result != Z_OK),
            "Error to compress data\n");

        //if (result != Z_OK) {
        //    fprintf(stderr, "[%s:%i]\n", __FILE__, __LINE__ );
        //    fprintf(stderr, "Error to compress data\n");
        //    exit(-1);
        //}

        //after compression, the final size could be less than the limit bounds calculated
        zlibOutput.resize(16 + zlibOutput_Length + sizeof(uint32_t));

        MD5::get16bytesHashFromBytes((char*)&zlibOutput[16],
            zlibOutput_Length + sizeof(uint32_t),
            &zlibOutput[0]);

        //printf("Zlib compress MD5: %s\n", MD5::_16BytesToHexString(&zlibOutput[0]).c_str() );

        ARIBEIRO_ABORT(zlibOutput.size() != (16 + zlibOutput_Length + sizeof(uint32_t)), "wrong buffer size after compression");

        if (OnData != NULL)
            OnData(&zlibOutput[0], zlibOutput.size());
    }

    void ZLIB::uncompress(const uint8_t* data, uint32_t size,
        const aRibeiro::aribeiro_OnDataMethodPtrType &OnData) {

        ARIBEIRO_ABORT(
            (size < 16 + sizeof(uint32_t)),
            "Error to decompress stream\n");

        //if (size < 16 + sizeof(uint32_t) )
        //{
        //    fprintf(stderr, "[%s:%i]\n", __FILE__, __LINE__);
        //    fprintf(stderr,"Error to decompress stream\n");
        //    exit(-1);
        //}

        unsigned char *md5_from_file = (unsigned char *)&data[0];
        //
        // Check the MD5 before create the uncompressed buffer
        //
        unsigned char md5[16];
        MD5::get16bytesHashFromBytes((char*)&data[16], size - 16, md5);

        /*
        printf("Zlib uncompress MD5 from file: %s MD5 calculated: %s\n",
            MD5::_16BytesToHexString(md5_from_file).c_str(),
            MD5::_16BytesToHexString(md5).c_str()
        );
        */

        ARIBEIRO_ABORT(
            (memcmp(md5_from_file, md5, 16) != 0),
            "Stream is corrupted\n");

        //if (memcmp(md5_from_file, md5,16) != 0){
        //    fprintf(stderr, "[%s:%i]\n", __FILE__, __LINE__);
        //    fprintf(stderr,"Stream is corrupted\n");
        //    exit(-1);
        //}

        uLongf zlibUncompressed_Length = (uLongf)(*((uint32_t*)&data[16]));

        zlibOutput.resize(zlibUncompressed_Length);
        int result = ::uncompress((Bytef *)&zlibOutput[0],
            &zlibUncompressed_Length,
            (Bytef *)&data[16 + sizeof(uint32_t)],
            size - 16 - sizeof(uint32_t));

        ARIBEIRO_ABORT(
            (result != Z_OK || zlibOutput.size() != zlibUncompressed_Length),
            "Error to uncompress input stream\n");

        //if (result != Z_OK || zlibOutput.size() != zlibUncompressed_Length) {
        //    fprintf(stderr, "[%s:%i]\n", __FILE__, __LINE__);
        //    fprintf(stderr, "Error to uncompress input stream\n");
        //    exit(-1);
        //}

        if (OnData != NULL)
            OnData(&zlibOutput[0], zlibOutput.size());
    }
}