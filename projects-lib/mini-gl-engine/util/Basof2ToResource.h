#ifndef Basof2ToResource_h_
#define Basof2ToResource_h_

#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/ComponentMesh.h>
#include <mini-gl-engine/ComponentMaterial.h>

#include <data-model/data-model.h>
#include <aribeiro/aribeiro.h>

namespace GLEngine {
    
    class _SSE2_ALIGN_PRE Basof2ToResource {
        
        static Transform* nodeTraverse(const std::string &spaces,
                                       int currentIndex,
                                       const model::Node &node,
                                       const model::ModelContainer *container,
                                       std::map<int, Components::ComponentMesh *> &geometryCache,
                                       std::map<int, Components::ComponentMaterial *> &materialCache,
                                       Transform *root,
                                       Components::ComponentMaterial *defaultMaterial = NULL,
                                       uint32_t model_dynamic_upload=0, uint32_t model_static_upload=0xffffffff);
        
    public:
        static Transform* loadAndConvert( 
            const std::string &filename,
            Components::ComponentMaterial *defaultMaterial = NULL, 
            Transform *rootNode = NULL ,
            uint32_t model_dynamic_upload = 0, uint32_t model_static_upload = 0xffffffff);

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}


#endif
