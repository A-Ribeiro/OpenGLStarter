#ifndef model_quatkey_h_
#define model_quatkey_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

namespace model {

    class _SSE2_ALIGN_PRE QuatKey{
    public:
        float time;
        aRibeiro::quat value;
        
        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeFloat(time);
            writer->writeQuat(value);
        }
        
        void read(aRibeiro::BinaryReader* reader) {
            time = reader->readFloat();
            value = reader->readQuat();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif