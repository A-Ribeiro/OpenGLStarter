#ifndef SkinnedMesh__H__
#define SkinnedMesh__H__

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>
#include <mini-gl-engine/interpolators.h>
#include <mini-gl-engine/ResourceHelper.h>
#include <mini-gl-engine/ComponentMesh.h>
#include <mini-gl-engine/ComponentMaterial.h>
//#include <data-model/Animation.h>

#include "../util/Animation.h"



namespace GLEngine {
    namespace Components {

        const uint32_t SkinnedMesh_VBO_Upload_Bitflag = model::CONTAINS_POS | model::CONTAINS_NORMAL | model::CONTAINS_TANGENT | model::CONTAINS_BINORMAL;

        struct SkinnedMesh_vertexWeight {
            int bone_index;
            float weight;
        };

        class SkinnedMesh:public Component {

            bool skinning_dirty;

            bool countBonesLinkedToTransforms(Transform *element, void* userData);

            void computeSkinningInformation();

            void OnTransformVisited(Transform* t);

            void applySkinning();

        public:
            static const ComponentType Type;

            AnimationMixer mixer;
            Transform *model_base;
            ResourceHelper *resourceHelper;

            ComponentMesh *componentMesh;
            ComponentMaterial *componentMaterial;

            //
            // bone processing aux structure
            //
            std::vector<bool> bone_processed;

            aRibeiro::aligned_vector<aRibeiro::mat4> bone_gradient;
            //aRibeiro::aligned_vector<aRibeiro::mat4> bone_target;
            aRibeiro::aligned_vector<aRibeiro::mat4> bone_inverse;

            std::vector< std::vector< SkinnedMesh_vertexWeight > > vertex_weight;

            aRibeiro::aligned_vector<aRibeiro::vec4> src_pos;
            aRibeiro::aligned_vector<aRibeiro::vec4> src_normals;
            aRibeiro::aligned_vector<aRibeiro::vec4> src_tangent;
            aRibeiro::aligned_vector<aRibeiro::vec4> src_binormal;

            std::vector< int > transform_to_bone;
            std::vector< Transform * > transform_list;
            std::vector< model::Bone* > model_bone_list;

            //
            // GPU Skinning
            //
            bool isGPUSkinning;
            uint32_t SkinnedMesh_GPU_VBO_Upload_Bitflag;//model::CONTAINS_VERTEX_WEIGHT16;

            //aRibeiro::vec3 clip_motion;

            SkinnedMesh(ResourceHelper *_resourceHelper, bool isGPUSkinning);

            void loadModelBase(const std::string &filename);

            void moveMeshToTransform();

            void loadAnimation(const std::string &clip_name, const std::string &filename);

            void done();

            void start();
            void OnPreUpdate(aRibeiro::PlatformTime *time);

            virtual ~SkinnedMesh();

        };


    }

}

#endif