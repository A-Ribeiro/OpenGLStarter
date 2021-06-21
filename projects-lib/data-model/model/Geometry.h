#ifndef model_geometry_h_
#define model_geometry_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

#include "Bone.h"

namespace model {

    enum VertexFormat {
        CONTAINS_POS = (1 << 0), // float3 position
        CONTAINS_NORMAL = (1 << 1), // float3 normal
        CONTAINS_BINORMAL = (1 << 2), // float3 binormal
        CONTAINS_TANGENT = (1 << 3), // float3 tangent
        //CONTAINS_COL = (1 << 4), // unsigned byte 4 color

        CONTAINS_UV0 = (1 << 5), // float2
        CONTAINS_UV1 = (1 << 6), // float2
        CONTAINS_UV2 = (1 << 7), // float2
        CONTAINS_UV3 = (1 << 8), // float2
        CONTAINS_UV4 = (1 << 9), // float2
        CONTAINS_UV5 = (1 << 10), // float2
        CONTAINS_UV6 = (1 << 11), // float2
        CONTAINS_UV7 = (1 << 12), // float2

        CONTAINS_COLOR0 = (1 << 13), // byte4
        CONTAINS_COLOR1 = (1 << 14), // byte4
        CONTAINS_COLOR2 = (1 << 15), // byte4
        CONTAINS_COLOR3 = (1 << 16), // byte4
        CONTAINS_COLOR4 = (1 << 17), // byte4
        CONTAINS_COLOR5 = (1 << 18), // byte4
        CONTAINS_COLOR6 = (1 << 19), // byte4
        CONTAINS_COLOR7 = (1 << 20), // byte4

        // PAIR MATRIX[*] + PAIR float4(vertexindex) float4(weight)
        CONTAINS_VERTEX_WEIGHT16 = (1 << 27),
        CONTAINS_VERTEX_WEIGHT32 = (1 << 28),
        CONTAINS_VERTEX_WEIGHT64 = (1 << 29),
        CONTAINS_VERTEX_WEIGHT96 = (1 << 30),
        CONTAINS_VERTEX_WEIGHT128 = (1 << 31),

        CONTAINS_VERTEX_WEIGHT_ANY = CONTAINS_VERTEX_WEIGHT16 | CONTAINS_VERTEX_WEIGHT32 | CONTAINS_VERTEX_WEIGHT64 | CONTAINS_VERTEX_WEIGHT96 | CONTAINS_VERTEX_WEIGHT128

    };

    class _SSE2_ALIGN_PRE Geometry {

    public:
        std::string name;

        //VertexFormat: CONTAINS_POS | CONTAINS_NORMAL | ...
        uint32_t format;
        uint32_t vertexCount;
        uint32_t indiceCountPerFace;// 1 - points, 2 - lines, 3 - triangles, 4 - quads

        aRibeiro::aligned_vector<aRibeiro::vec3> pos;
        aRibeiro::aligned_vector<aRibeiro::vec3> normals;
        aRibeiro::aligned_vector<aRibeiro::vec3> tangent;
        aRibeiro::aligned_vector<aRibeiro::vec3> binormal;
        aRibeiro::aligned_vector<aRibeiro::vec3> uv[8];
        aRibeiro::aligned_vector<aRibeiro::vec4> color[8];//RGBA
        //std::vector<uint32_t> color[8];//RGBA

        std::vector<uint16_t> indice;

        uint32_t materialIndex;

        aRibeiro::aligned_vector<Bone> bones;

        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeString(name);

            //VertexFormat: CONTAINS_POS | CONTAINS_NORMAL | ...
            writer->writeUInt32(format);
            writer->writeUInt32(vertexCount);
            writer->writeUInt32(indiceCountPerFace);// 1 - points, 2 - lines, 3 - triangles, 4 - quads
            writer->writeUInt32(materialIndex);

            writer->writeVectorVec3(pos);
            writer->writeVectorVec3(normals);
            writer->writeVectorVec3(tangent);

            //for (size_t i = 0; i < normals.size(); i++)
                //printf("%f\n", distance(binormal[i], cross(normals[i], tangent[i])));
            writer->writeVectorVec3(binormal);

            for (int i = 0; i < 8; i++)
                writer->writeVectorVec3(uv[i]);
            for (int i = 0; i < 8; i++)
                writer->writeVectorVec4(color[i]);
            //writer->writeVectorUInt32(color[i]);//RGBA
            writer->writeVectorUInt16(indice);

            aRibeiro::BinaryWriter_WriteAlignedVector<Bone>(writer, bones);
        }

        void read(aRibeiro::BinaryReader* reader) {
            name = reader->readString();

            //VertexFormat: CONTAINS_POS | CONTAINS_NORMAL | ...
            format = reader->readUInt32();
            vertexCount = reader->readUInt32();
            indiceCountPerFace = reader->readUInt32();// 1 - points, 2 - lines, 3 - triangles, 4 - quads
            materialIndex = reader->readUInt32();

            reader->readVectorVec3(&pos);
            reader->readVectorVec3(&normals);
            reader->readVectorVec3(&tangent);

            //binormal.resize(normals.size());
            //for (size_t i = 0; i < normals.size(); i++)
                //binormal[i] = cross(normals[i], tangent[i]);
            reader->readVectorVec3(&binormal);

            for (int i = 0; i < 8; i++)
                reader->readVectorVec3(&uv[i]);
            for (int i = 0; i < 8; i++)
                reader->readVectorVec4(&color[i]);
            //reader->readVectorUInt32(&color[i]);//RGBA
            reader->readVectorUInt16(&indice);

            aRibeiro::BinaryReader_ReadAlignedVector<Bone>(reader, &bones);
        }

        Geometry() {
            format = 0; //CONTAINS_POS | CONTAINS_NORMAL | ...
            vertexCount = 0;
            indiceCountPerFace = 0;// 1 - points, 2 - lines, 3 - triangles, 4 - quads
            materialIndex = 0;
        }

        //copy constructores
        Geometry(const Geometry& v) {
            (*this) = v;
        }
        void operator=(const Geometry& v) {

            name = v.name;

            format = v.format;
            vertexCount = v.vertexCount;
            indiceCountPerFace = v.indiceCountPerFace;

            pos = v.pos;
            normals = v.normals;
            tangent = v.tangent;
            binormal = v.binormal;
            for (int i = 0; i < 8; i++) {
                uv[i] = v.uv[i];
                color[i] = v.color[i];
            }

            indice = v.indice;

            materialIndex = v.materialIndex;

            bones = v.bones;

        }

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
