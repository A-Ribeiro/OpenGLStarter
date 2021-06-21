#include "BinaryReader.h"

#include <zlib-wrapper/zlib-wrapper.h>
#include <string.h> // memcmp

namespace aRibeiro {

    bool BinaryReader::eof() {
        return readPos >= buffer.size();
    }

    BinaryReader::BinaryReader() {
    }

    void BinaryReader::readFromBuffer(const uint8_t* data, size_t size, bool compressed) {
        if (compressed){
            zlibWrapper::ZLIB zlib;
            zlib.uncompress(data, size);
            buffer = zlib.zlibOutput;
        } else {
            buffer.resize(size);
            memcpy(&buffer[0], data, size);
        }
        readPos = 0;
    }

    void BinaryReader::readFromFile(const char* filename, bool compressed) {
        buffer.resize(0);
        FILE* in = fopen(filename, "rb");
        if (in) {
            /*
            while (!feof(in)) {
                uint8_t c;
                if (fread(&c, sizeof(uint8_t), 1, in) == 1)
                    buffer.push_back(c);
                else
                    break;
            }
            */

            //
            // optimized reading
            //
            fseek(in, 0, SEEK_END);
            buffer.resize(ftell(in));
            fseek(in, 0, SEEK_SET);
            fread(&buffer[0], sizeof(uint8_t), buffer.size(), in);
            
            fclose(in);
        }

        //printf("reading size: %u\n", buffer.size());

        if (compressed){
            zlibWrapper::ZLIB zlib;
            zlib.uncompress(&buffer[0], buffer.size());
            buffer = zlib.zlibOutput;
        }

        readPos = 0;
    }
    

    BinaryReader::~BinaryReader() {
        close();
    }

    void BinaryReader::close() {
    }

    void BinaryReader::read( void* data, int size ) {

        ARIBEIRO_ABORT( (readPos + size) > this->buffer.size(), "Error to read buffer. Size greater than the actuan buffer is...");

        if (eof()) {
            memset(data, 0 , size);
            return;
        }
        memcpy(data, &buffer[readPos], size);
        readPos += size;
    }

    uint8_t BinaryReader::readUInt8() {
        uint8_t result;
        read( &result, sizeof(uint8_t) );
        return result;
    }

    int8_t BinaryReader::readInt8() {
        int8_t result;
        read( &result, sizeof(int8_t) );
        return result;
    }

    uint16_t BinaryReader::readUInt16() {
        uint16_t result;
        read( &result, sizeof(uint16_t) );
        return result;
    }

    int16_t BinaryReader::readInt16() {
        int16_t result;
        read( &result, sizeof(int16_t) );
        return result;
    }

    uint32_t BinaryReader::readUInt32() {
        uint32_t result;
        read( &result, sizeof(uint32_t) );
        return result;
    }

    int32_t BinaryReader::readInt32() {
        int32_t result;
        read( &result, sizeof(int32_t) );
        return result;
    }

    float BinaryReader::readFloat() {
        float result;
        read( &result, sizeof(float) );
        return result;
    }

    quat BinaryReader::readQuat() {
        quat result;

        result.x = readFloat();
        result.y = readFloat();
        result.z = readFloat();
        result.w = readFloat();

        return result;
    }

    vec2 BinaryReader::readVec2() {
        vec2 result;

        result.x = readFloat();
        result.y = readFloat();

        return result;
    }

    vec3 BinaryReader::readVec3() {
        vec3 result;

        result.x = readFloat();
        result.y = readFloat();
        result.z = readFloat();

        return result;
    }

    vec4 BinaryReader::readVec4() {
        vec4 result;

        result.x = readFloat();
        result.y = readFloat();
        result.z = readFloat();
        result.w = readFloat();

        return result;
    }

    mat4 BinaryReader::readMat4() {
        mat4 result;

        result[0] = readVec4();
        result[1] = readVec4();
        result[2] = readVec4();
        result[3] = readVec4();

        return result;
    }

    std::string BinaryReader::readString() {
        if (eof())
            return "";
        std::string result;
        uint16_t size = readUInt16();
        //result.resize(size + 1, '\0');
        if (size > 0) {
            result.resize(size);
            memcpy(&result[0], &buffer[readPos], size);
            readPos += size;
        }
        return &result[0];
    }


    void BinaryReader::readVectorFloat(std::vector<float> *v){
        v->resize(readUInt32());
        for (size_t i=0;i<v->size();i++){
            (*v)[i] = readFloat();
        }
    }

    void BinaryReader::readVectorUInt16(std::vector<uint16_t> *v) {
        v->resize(readUInt32());
        for (size_t i = 0; i < v->size(); i++) {
            (*v)[i] = readUInt16();
        }
    }

    void BinaryReader::readVectorUInt32(std::vector<uint32_t> *v){
        v->resize(readUInt32());
        for (size_t i=0;i<v->size();i++){
            (*v)[i] = readUInt32();
        }
    }

    void BinaryReader::readVectorVec2(aligned_vector<vec2> *v){
        v->resize(readUInt32());
        for (size_t i=0;i<v->size();i++){
            (*v)[i] = readVec2();
        }
    }

    void BinaryReader::readVectorVec3(aligned_vector<vec3> *v){
        v->resize(readUInt32());
        for (size_t i=0;i<v->size();i++){
            (*v)[i] = readVec3();
        }
    }

    void BinaryReader::readVectorVec4(aligned_vector<vec4> *v){
        v->resize(readUInt32());
        for (size_t i=0;i<v->size();i++){
            (*v)[i] = readVec4();
        }
    }


    void BinaryReader::readStringMapFloat(std::map<std::string,float> *result){
        if (eof())
            return;
        uint32_t size = readUInt32();
        //std::map<std::string, T> result;
        (*result).clear();
        for (int i = 0; i < size; i++) {
            std::string aux = readString();
            float value = readFloat();
            (*result)[aux] = value;
        }
    }

    void BinaryReader::readStringMapInt32(std::map<std::string,int32_t> *result){
        if (eof())
            return;
        uint32_t size = readUInt32();
        //std::map<std::string, T> result;
        (*result).clear();
        for (int i = 0; i < size; i++) {
            std::string aux = readString();
            int32_t value = readInt32();
            (*result)[aux] = value;
        }
    }

    void BinaryReader::readStringMapVec2(aligned_map<std::string,vec2> *result){
        if (eof())
            return;
        uint32_t size = readUInt32();
        //std::map<std::string, T> result;
        (*result).clear();
        for (int i = 0; i < size; i++) {
            std::string aux = readString();
            vec2 value = readVec2();
            (*result)[aux] = value;
        }
    }

    void BinaryReader::readStringMapVec3(aligned_map<std::string,vec3> *result){
        if (eof())
            return;
        uint32_t size = readUInt32();
        //std::map<std::string, T> result;
        (*result).clear();
        for (int i = 0; i < size; i++) {
            std::string aux = readString();
            vec3 value = readVec3();
            (*result)[aux] = value;
        }
    }

    void BinaryReader::readStringMapVec4(aligned_map<std::string,vec4> *result){
        if (eof())
            return;
        uint32_t size = readUInt32();
        //std::map<std::string, T> result;
        (*result).clear();
        for (int i = 0; i < size; i++) {
            std::string aux = readString();
            vec4 value = readVec4();
            (*result)[aux] = value;
        }
    }

    void BinaryReader::readBuffer(uint8_t **buffer, uint32_t *size) {
        *size = readUInt32();
        //read((*buffer), *size);
        *buffer = &this->buffer[readPos];
        //memcpy(data, &buffer[readPos], size);
        readPos += *size;

        ARIBEIRO_ABORT( readPos > this->buffer.size(), "Error to load Buffer. Size greater than the actuan buffer is..." );
    }

}
