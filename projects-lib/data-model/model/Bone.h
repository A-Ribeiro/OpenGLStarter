#ifndef model_bone_h_
#define model_bone_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

namespace model {

    class _SSE2_ALIGN_PRE VertexWeight {
    public:
        uint32_t vertexID;
        float weight;
        
        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeUInt32(vertexID);
            writer->writeFloat(weight);
        }
        
        void read(aRibeiro::BinaryReader* reader) {
            vertexID = reader->readUInt32();
            weight = reader->readFloat();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    class _SSE2_ALIGN_PRE Bone {
    public:
        std::string name;
        aRibeiro::aligned_vector<VertexWeight> weights;

        //aRibeiro::mat4 offset;

        void write(aRibeiro::BinaryWriter* writer) const{
            writer->writeString(name);
            //writer->writeMat4(offset);
            aRibeiro::BinaryWriter_WriteAlignedVector<VertexWeight>(writer, weights);
        }

        void read(aRibeiro::BinaryReader* reader) {
            name = reader->readString();
            //offset = reader->readMat4();
            aRibeiro::BinaryReader_ReadAlignedVector<VertexWeight>(reader, &weights);
        }

        Bone() {

        }

        Bone(const Bone& v) {
            (*this) = v;
        }
        void operator=(const Bone& v) {
            name = v.name;
            weights = v.weights;
            //offset = v.offset;
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif