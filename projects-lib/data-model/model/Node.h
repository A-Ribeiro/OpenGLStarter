#ifndef model_node_h_
#define model_node_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

namespace model {

    class _SSE2_ALIGN_PRE Node {
    public:
        std::string name;
        std::vector<uint32_t> geometries;
        std::vector<uint32_t> children;
        aRibeiro::mat4 transform;

        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeString(name);
            writer->writeMat4(transform);
            writer->writeVectorUInt32(geometries);
            writer->writeVectorUInt32(children);
        }

        void read(aRibeiro::BinaryReader* reader) {
            name = reader->readString();
            transform = reader->readMat4();
            reader->readVectorUInt32(&geometries);
            reader->readVectorUInt32(&children);
        }

        Node() {

        }

        //copy constructores
        Node(const Node& v) {
            (*this) = v;
        }
        void operator=(const Node& v) {
            name = v.name;
            geometries = v.geometries;
            children = v.children;
            transform = v.transform;
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif