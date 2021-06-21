#ifndef model_light_h_
#define model_light_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

namespace model {

    enum LightType {
        LightType_NONE          = 0x0,
        LightType_DIRECTIONAL   = 0x1,
        LightType_POINT         = 0x2,
        LightType_SPOT          = 0x3,
        LightType_AMBIENT       = 0x4,
        LightType_AREA          = 0x5
    };

    static const char* LightTypeToStr(LightType lt) {
        switch(lt) {
            case LightType_NONE:
                return "none";
            case LightType_DIRECTIONAL:
                return "directional";
            case LightType_POINT:
                return "point";
            case LightType_SPOT:
                return "spot";
            case LightType_AMBIENT:
                return "ambient";
            case LightType_AREA:
                return "area";
        }
        return "error to parse";
    }

    struct _SSE2_ALIGN_PRE DirectionalLight {
        aRibeiro::vec3 direction;
        aRibeiro::vec3 up;
        
        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeVec3(direction);
            writer->writeVec3(up);
        }
        
        void read(aRibeiro::BinaryReader* reader) {
            direction = reader->readVec3();
            up = reader->readVec3();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    struct _SSE2_ALIGN_PRE PointLight {
        aRibeiro::vec3 position;
        
        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeVec3(position);
        }
        
        void read(aRibeiro::BinaryReader* reader) {
            position = reader->readVec3();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    struct _SSE2_ALIGN_PRE SpotLight {
        aRibeiro::vec3 position;
        aRibeiro::vec3 direction;
        aRibeiro::vec3 up;
        float angleInnerCone;
        float angleOuterCone;
        
        SpotLight() {
            angleInnerCone = 0;
            angleOuterCone = 0;
        }
        
        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeVec3(position);
            writer->writeVec3(direction);
            writer->writeVec3(up);
            writer->writeFloat(angleInnerCone);
            writer->writeFloat(angleOuterCone);
        }
        
        void read(aRibeiro::BinaryReader* reader) {
            position = reader->readVec3();
            direction = reader->readVec3();
            up = reader->readVec3();
            angleInnerCone = reader->readFloat();
            angleOuterCone = reader->readFloat();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    struct _SSE2_ALIGN_PRE AmbientLight {
        aRibeiro::vec3 position;
        aRibeiro::vec3 direction;
        aRibeiro::vec3 up;
        
        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeVec3(position);
            writer->writeVec3(direction);
            writer->writeVec3(up);
        }
        
        void read(aRibeiro::BinaryReader* reader) {
            position = reader->readVec3();
            direction = reader->readVec3();
            up = reader->readVec3();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    struct _SSE2_ALIGN_PRE AreaLight {
        aRibeiro::vec3 position;
        aRibeiro::vec3 direction;
        aRibeiro::vec3 up;
        aRibeiro::vec2 size;
        
        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeVec3(position);
            writer->writeVec3(direction);
            writer->writeVec3(up);
            writer->writeVec2(size);
        }
        
        void read(aRibeiro::BinaryReader* reader) {
            position = reader->readVec3();
            direction = reader->readVec3();
            up = reader->readVec3();
            size = reader->readVec2();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    class _SSE2_ALIGN_PRE Light {
    public:
        std::string name;
        
        LightType type;
        
        DirectionalLight directional;
        PointLight point;
        SpotLight spot;
        AmbientLight ambient;
        AreaLight area;
        
        // d = distance
        // Atten = 1/( att0 + att1 * d + att2 * d*d)
        float attenuationConstant;
        float attenuationLinear;
        float attenuationQuadratic;

        aRibeiro::vec3 colorDiffuse;
        aRibeiro::vec3 colorSpecular;
        aRibeiro::vec3 colorAmbient;

        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeString(name);
            writer->writeUInt8(type);
            
            directional.write(writer);
            point.write(writer);
            spot.write(writer);
            ambient.write(writer);
            area.write(writer);

            // d = distance
            // Atten = 1/( att0 + att1 * d + att2 * d*d)
            writer->writeFloat(attenuationConstant);
            writer->writeFloat(attenuationLinear);
            writer->writeFloat(attenuationQuadratic);

            writer->writeVec3(colorDiffuse);
            writer->writeVec3(colorSpecular);
            writer->writeVec3(colorAmbient);
        }

        void read(aRibeiro::BinaryReader* reader) {
            name = reader->readString();
            type = (LightType)reader->readUInt8();
            
            directional.read(reader);
            point.read(reader);
            spot.read(reader);
            ambient.read(reader);
            area.read(reader);
            
            // d = distance
            // Atten = 1/( att0 + att1 * d + att2 * d*d)
            attenuationConstant = reader->readFloat();
            attenuationLinear = reader->readFloat();
            attenuationQuadratic = reader->readFloat();

            colorDiffuse = reader->readVec3();
            colorSpecular = reader->readVec3();
            colorAmbient = reader->readVec3();
        }
        
        Light() {
            attenuationConstant = 0;
            attenuationLinear = 0;
            attenuationQuadratic = 0;
        }
        
        Light(const Light& v) {
            (*this) = v;
        }
        
        void operator=(const Light& v) {
            name = v.name;
            
            type = v.type;
            
            directional = v.directional;
            point = v.point;
            spot = v.spot;
            ambient = v.ambient;
            area = v.area;
            
            // d = distance
            // Atten = 1/( att0 + att1 * d + att2 * d*d)
            attenuationConstant = v.attenuationConstant;
            attenuationLinear = v.attenuationLinear;
            attenuationQuadratic = v.attenuationQuadratic;
            
            colorDiffuse = v.colorDiffuse;
            colorSpecular = v.colorSpecular;
            colorAmbient = v.colorAmbient;
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif