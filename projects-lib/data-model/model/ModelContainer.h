#ifndef model_model_container_h_
#define model_model_container_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

#include "Animation.h"
#include "Light.h"
#include "Camera.h"
#include "Material.h"
#include "Geometry.h"
#include "Node.h"

namespace model {

    class _SSE2_ALIGN_PRE ModelContainer {
    public:
        aRibeiro::aligned_vector<Animation> animations;
        aRibeiro::aligned_vector<Light> lights;
        aRibeiro::aligned_vector<Camera> cameras;
        aRibeiro::aligned_vector<Material> materials;
        aRibeiro::aligned_vector<Geometry> geometries;
        aRibeiro::aligned_vector<Node> nodes;//the node[0] is the root
        
        void write(const char* filename)const {
            
            aRibeiro::BinaryWriter writer;
            writer.writeToFile(filename, true);
            
            aRibeiro::BinaryWriter_WriteAlignedVector<Animation>(&writer,animations);
            aRibeiro::BinaryWriter_WriteAlignedVector<Light>(&writer,lights);
            aRibeiro::BinaryWriter_WriteAlignedVector<Camera>(&writer,cameras);
            aRibeiro::BinaryWriter_WriteAlignedVector<Material>(&writer,materials);
            aRibeiro::BinaryWriter_WriteAlignedVector<Geometry>(&writer,geometries);
            aRibeiro::BinaryWriter_WriteAlignedVector<Node>(&writer,nodes);
            
            writer.close();
        }

        void read(const char* filename) {
            
            aRibeiro::BinaryReader reader;
            reader.readFromFile(filename, true);

            aRibeiro::BinaryReader_ReadAlignedVector<Animation>(&reader,&animations);
            aRibeiro::BinaryReader_ReadAlignedVector<Light>(&reader,&lights);
            aRibeiro::BinaryReader_ReadAlignedVector<Camera>(&reader,&cameras);
            aRibeiro::BinaryReader_ReadAlignedVector<Material>(&reader,&materials);
            aRibeiro::BinaryReader_ReadAlignedVector<Geometry>(&reader,&geometries);
            aRibeiro::BinaryReader_ReadAlignedVector<Node>(&reader,&nodes);
            
            reader.close();
        }
        
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif