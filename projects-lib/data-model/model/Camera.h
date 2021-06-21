#ifndef model_camera_h_
#define model_camera_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

namespace model {

    class _SSE2_ALIGN_PRE Camera {
    public:
        std::string name;
        
        aRibeiro::vec3 pos;
        aRibeiro::vec3 up;
        aRibeiro::vec3 forward;
        
        float horizontalFOVrad;
        float nearPlane;
        float farPlane;
        float aspect;
        
        float verticalFOVrad;
        
        float computeHeight(float width){
            return width / aspect;
        }
        float computeWidth(float height){
            return height * aspect;
        }

        void write(aRibeiro::BinaryWriter* writer)const {
            
            writer->writeString(name);

            writer->writeVec3(pos);
            writer->writeVec3(up);
            writer->writeVec3(forward);

            writer->writeFloat(horizontalFOVrad);
            writer->writeFloat(nearPlane);
            writer->writeFloat(farPlane);
            writer->writeFloat(aspect);

            writer->writeFloat(verticalFOVrad);
        }

        void read(aRibeiro::BinaryReader* reader) {
            name = reader->readString();

            pos = reader->readVec3();
            up = reader->readVec3();
            forward = reader->readVec3();

            horizontalFOVrad = reader->readFloat();
            nearPlane = reader->readFloat();
            farPlane = reader->readFloat();
            aspect = reader->readFloat();

            verticalFOVrad = reader->readFloat();
        }
        
        Camera() {
            pos = aRibeiro::vec3(0,0,0);
            up = aRibeiro::vec3(0,1,0);
            forward = aRibeiro::vec3(0,0,1);
            
            horizontalFOVrad = DEG2RAD(60.0f);
            nearPlane = 0.1f;
            farPlane = 100.0f;
            aspect = 1.0f;
            
            verticalFOVrad = DEG2RAD(60.0f);
        }
        
        Camera(const Camera& v) {
            (*this) = v;
        }
        
        void operator=(const Camera& v) {
            name = v.name;
            pos = v.pos;
            up = v.up;
            forward = v.forward;
            horizontalFOVrad = v.horizontalFOVrad;
            nearPlane = v.nearPlane;
            farPlane = v.farPlane;
            aspect = v.aspect;
            verticalFOVrad = v.verticalFOVrad;
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif