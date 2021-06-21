#ifndef model_node_animation_h_
#define model_node_animation_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

#include "Vec3Key.h"
#include "QuatKey.h"

namespace model {

    // Defines how an animation channel behaves outside the defined time range.
    enum AnimBehaviour
    {
        // use the default node transformation
        AnimBehaviour_DEFAULT  = 0x0,
        // The nearest key value is used without interpolation
        AnimBehaviour_CONSTANT = 0x1,
        // The value of the nearest two keys is linearly extrapolated for the current time value.
        AnimBehaviour_LINEAR   = 0x2,
        // The animation is repeated.
        //  If the animation key go from n to m and the current
        //  time is t, use the value at (t-n) % (|m-n|).
        AnimBehaviour_REPEAT   = 0x3,
    };

    class _SSE2_ALIGN_PRE NodeAnimation {
    public:
        std::string nodeName;
        aRibeiro::aligned_vector<Vec3Key> positionKeys;
        aRibeiro::aligned_vector<QuatKey> rotationKeys;
        aRibeiro::aligned_vector<Vec3Key> scalingKeys;
        
        AnimBehaviour preState;
        AnimBehaviour postState;

        void write(aRibeiro::BinaryWriter* writer) const{
            writer->writeString(nodeName);
            writer->writeUInt8(preState);
            writer->writeUInt8(postState);
            
            aRibeiro::BinaryWriter_WriteAlignedVector<Vec3Key>(writer,positionKeys);
            aRibeiro::BinaryWriter_WriteAlignedVector<QuatKey>(writer,rotationKeys);
            aRibeiro::BinaryWriter_WriteAlignedVector<Vec3Key>(writer,scalingKeys);
        }

        void read(aRibeiro::BinaryReader* reader) {
            nodeName = reader->readString();
            preState = (AnimBehaviour)(reader->readUInt8());
            postState = (AnimBehaviour)(reader->readUInt8());
            
            aRibeiro::BinaryReader_ReadAlignedVector<Vec3Key>(reader,&positionKeys);
            aRibeiro::BinaryReader_ReadAlignedVector<QuatKey>(reader,&rotationKeys);
            aRibeiro::BinaryReader_ReadAlignedVector<Vec3Key>(reader,&scalingKeys);
        }
        
        NodeAnimation() {
            preState = AnimBehaviour_DEFAULT;
            postState = AnimBehaviour_DEFAULT;
        }
        
        NodeAnimation(const NodeAnimation& v) {
            (*this) = v;
        }
        
        void operator=(const NodeAnimation& v) {
            nodeName = v.nodeName;
            
            positionKeys = v.positionKeys;
            rotationKeys = v.rotationKeys;
            scalingKeys = v.scalingKeys;
            
            preState = v.preState;
            postState = v.postState;
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif