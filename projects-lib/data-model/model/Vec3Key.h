#ifndef model_vec3key_h_
#define model_vec3key_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

namespace model {

    class _SSE2_ALIGN_PRE Vec3Key{
    public:
        float time;
        aRibeiro::vec3 value;
        
        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeFloat(time);
            writer->writeVec3(value);
        }
        
        void read(aRibeiro::BinaryReader* reader) {
            time = reader->readFloat();
            value = reader->readVec3();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif