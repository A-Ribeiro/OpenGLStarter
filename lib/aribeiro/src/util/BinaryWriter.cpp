#include "BinaryWriter.h"

#include <zlib-wrapper/zlib-wrapper.h>
#include <string.h> // memcmp

namespace aRibeiro {

    BinaryWriter::BinaryWriter() {
        _writeToFile = false;
    }
    
    void BinaryWriter::writeToFile(const char* filename, bool compress) {
        name = filename;
        this->compress = compress;
        _writeToFile = true;
        //out = fopen(filename, "wb");
    }

    void BinaryWriter::writeToBuffer(bool compress) {
        this->compress = compress;
        _writeToFile = false;
        //out = fopen(filename, "wb");
    }

    BinaryWriter::~BinaryWriter() {
        //close();
    }

    void BinaryWriter::reset() {
        buffer.clear();
    }

    void BinaryWriter::close() {
        if (compress) {
            zlibWrapper::ZLIB zlib;
            zlib.compress(&buffer[0],buffer.size());
            buffer = zlib.zlibOutput;
        }

        if (_writeToFile){
            FILE *out = fopen(name.c_str(), "wb");
            if (out != NULL) {
                //printf("writing size: %u\n", buffer.size());
                if (buffer.size() > 0)
                    fwrite(&buffer[0], sizeof(uint8_t), buffer.size(), out);
                fclose(out);
            }
            reset();
        }

    }

    void BinaryWriter::write(void *data, size_t size) {
        int startWrite = buffer.size();
        buffer.resize(startWrite + size);
        memcpy(&buffer[startWrite], data, size);
    }

    void BinaryWriter::writeUInt8(uint8_t v) {
        write(&v,sizeof(uint8_t));
    }
    
    void BinaryWriter::writeInt8(int8_t v) {
        write(&v,sizeof(int8_t));
    }

    void BinaryWriter::writeUInt16(uint16_t v) {
        write(&v,sizeof(uint16_t));
    }

    void BinaryWriter::writeInt16(int16_t v) {
        write(&v,sizeof(int16_t));
    }

    void BinaryWriter::writeUInt32(uint32_t v) {
        write(&v,sizeof(uint32_t));
    }

    void BinaryWriter::writeInt32(int32_t v) {
        write(&v,sizeof(int32_t));
    }

    void BinaryWriter::writeFloat(float v) {
        write(&v,sizeof(float));
    }

    void BinaryWriter::writeQuat(const quat &v) {
        writeFloat(v.x);
        writeFloat(v.y);
        writeFloat(v.z);
        writeFloat(v.w);
    }

    void BinaryWriter::writeVec2(const vec2 &v) {
        writeFloat(v.x);
        writeFloat(v.y);
    }

    void BinaryWriter::writeVec3(const vec3 &v) {
        writeFloat(v.x);
        writeFloat(v.y);
        writeFloat(v.z);
    }

    void BinaryWriter::writeVec4(const vec4 &v) {
        writeFloat(v.x);
        writeFloat(v.y);
        writeFloat(v.z);
        writeFloat(v.w);
    }

    void BinaryWriter::writeMat4(const mat4 &v) {
        writeVec4(v[0]);
        writeVec4(v[1]);
        writeVec4(v[2]);
        writeVec4(v[3]);
    }

    void BinaryWriter::writeString(const std::string &s) {
        writeUInt16((uint16_t)s.size());
        if (s.size() > 0) {
            int startWrite = buffer.size();
            buffer.resize(startWrite + s.size());
            memcpy(&buffer[startWrite], s.c_str(), s.size());
        }
    }


    void BinaryWriter::writeVectorFloat(const std::vector<float> &v){
        writeUInt32(v.size());
        for (size_t i=0;i<v.size();i++){
            writeFloat(v[i]);
        }
    }

    void BinaryWriter::writeVectorUInt16(const std::vector<uint16_t> &v) {
        writeUInt32(v.size());
        for (size_t i = 0; i < v.size(); i++) {
            writeUInt16(v[i]);
        }
    }

    void BinaryWriter::writeVectorUInt32(const std::vector<uint32_t> &v){
        writeUInt32(v.size());
        for (size_t i=0;i<v.size();i++){
            writeUInt32(v[i]);
        }
    }

    void BinaryWriter::writeVectorVec2(const aligned_vector<vec2> &v){
        writeUInt32(v.size());
        for (size_t i=0;i<v.size();i++){
            writeVec2(v[i]);
        }
    }

    void BinaryWriter::writeVectorVec3(const aligned_vector<vec3> &v){
        writeUInt32(v.size());
        for (size_t i=0;i<v.size();i++){
            writeVec3(v[i]);
        }
    }

    void BinaryWriter::writeVectorVec4(const aligned_vector<vec4> &v){
        writeUInt32(v.size());
        for (size_t i=0;i<v.size();i++){
            writeVec4(v[i]);
        }
    }


    void BinaryWriter::writeStringMapFloat(const std::map<std::string,float> &v){
        writeUInt32(v.size());
        //typename 
        std::map<std::string, float>::const_iterator it;
        for (it = v.begin(); it != v.end(); it++) {
            writeString(it->first);
            writeFloat(it->second);
        }
    }

    void BinaryWriter::writeStringMapInt32(const std::map<std::string,int32_t> &v){
        writeUInt32(v.size());
        //typename 
        std::map<std::string, int32_t>::const_iterator it;
        for (it = v.begin(); it != v.end(); it++) {
            writeString(it->first);
            writeInt32(it->second);
        }
    }

    void BinaryWriter::writeStringMapVec2(const aligned_map<std::string,vec2> &v){
        writeUInt32(v.size());
        
        //typename 
        aligned_map<std::string,vec2>::const_iterator it;
        for (it = v.begin(); it != v.end(); it++) {
            writeString(it->first);
            writeVec2(it->second);
        }
    }

    void BinaryWriter::writeStringMapVec3(const aligned_map<std::string,vec3> &v){
        writeUInt32(v.size());
        //typename 
        aligned_map<std::string, vec3>::const_iterator it;
        for (it = v.begin(); it != v.end(); it++) {
            writeString(it->first);
            writeVec3(it->second);
        }
    }

    void BinaryWriter::writeStringMapVec4(const aligned_map<std::string,vec4> &v){
        writeUInt32(v.size());
        //typename 
        aligned_map<std::string, vec4>::const_iterator it;
        for (it = v.begin(); it != v.end(); it++) {
            writeString(it->first);
            writeVec4(it->second);
        }
    }

    void BinaryWriter::writeBuffer(const uint8_t* buffer, uint32_t size){
        writeUInt32(size);
        write((void*)buffer, size);
    }

}
