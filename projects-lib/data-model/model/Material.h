#ifndef model_material_h_
#define model_material_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

#include "Texture.h"

namespace model {

    class _SSE2_ALIGN_PRE Material {
    public:
        std::string name;

        std::map<std::string, float> floatValue;
        aRibeiro::aligned_map<std::string, aRibeiro::vec2> vec2Value;
        aRibeiro::aligned_map<std::string, aRibeiro::vec3> vec3Value;
        aRibeiro::aligned_map<std::string, aRibeiro::vec4> vec4Value;
        std::map<std::string, int> intValue;

        aRibeiro::aligned_vector<Texture> textures;

        //std::string textureDiffuse;
        //std::string textureNormal;

        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeString(name);
            writer->writeStringMapFloat(floatValue);
            writer->writeStringMapVec2(vec2Value);
            writer->writeStringMapVec3(vec3Value);
            writer->writeStringMapVec4(vec4Value);
            writer->writeStringMapInt32(intValue);
            
            aRibeiro::BinaryWriter_WriteAlignedVector<Texture>(writer,textures);
        }

        void read(aRibeiro::BinaryReader* reader) {
            name = reader->readString();
            reader->readStringMapFloat(&floatValue);
            reader->readStringMapVec2(&vec2Value);
            reader->readStringMapVec3(&vec3Value);
            reader->readStringMapVec4(&vec4Value);
            reader->readStringMapInt32(&intValue);
            
            aRibeiro::BinaryReader_ReadAlignedVector<Texture>(reader,&textures);
        }

        Material() {

        }

        Material(const Material& v) {
            (*this) = v;
        }
        void operator=(const Material& v) {
            name = v.name;
            floatValue = v.floatValue;

            vec2Value = v.vec2Value;
            vec3Value = v.vec3Value;
            vec4Value = v.vec4Value;
            intValue = v.intValue;
            textures = v.textures;
        }

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif