#ifndef model_animation_h_
#define model_animation_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

#include "NodeAnimation.h"

namespace model {

    class _SSE2_ALIGN_PRE Animation {
    public:
        std::string name;
        float durationTicks;
        float ticksPerSecond;
        aRibeiro::aligned_vector<NodeAnimation> channels;

        void write(aRibeiro::BinaryWriter* writer) const
        {
            writer->writeString(name);
            writer->writeFloat(durationTicks);
            writer->writeFloat(ticksPerSecond);
            
            aRibeiro::BinaryWriter_WriteAlignedVector<NodeAnimation>(writer,channels);
        }

        void read(aRibeiro::BinaryReader* reader) 
        {
            name = reader->readString();
            durationTicks = reader->readFloat();
            ticksPerSecond = reader->readFloat();
            
            aRibeiro::BinaryReader_ReadAlignedVector<NodeAnimation>(reader,&channels);
        }
        
        Animation() {
            durationTicks = 0;
            ticksPerSecond = 0;
        }
        
        Animation(const Animation& v) {
            (*this) = v;
        }
        
        void operator=(const Animation& v) {
            name = v.name;
            
            durationTicks = v.durationTicks;
            
            ticksPerSecond = v.ticksPerSecond;
            channels = v.channels;
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;
    

}

#endif