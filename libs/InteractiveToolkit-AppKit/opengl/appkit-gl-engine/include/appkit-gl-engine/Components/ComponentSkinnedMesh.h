#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/util/Interpolators.h>
#include <appkit-gl-engine/util/ResourceHelper.h>

#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>

// #include <aRibeiroData/Animation.h>

#include <appkit-gl-engine/util/Animation.h>
// #include "../util/Animation.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            const uint32_t SkinnedMesh_VBO_Upload_Bitflag = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_NORMAL | ITKExtension::Model::CONTAINS_TANGENT | ITKExtension::Model::CONTAINS_BINORMAL;

            struct SkinnedMesh_vertexWeight
            {
                int bone_index;
                float weight;
            };

            class ComponentSkinnedMesh : public Component
            {

                RenderWindowRegion *renderWindowRegion;

                bool skinning_dirty;

                bool countBonesLinkedToTransforms(Transform *element, void *userData);

                void computeSkinningInformation();

                void OnTransformVisited(Transform *t);

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

                std::vector<MathCore::mat4f> bone_gradient;
                // std::vector<MathCore::mat4f> bone_target;
                std::vector<MathCore::mat4f> bone_inverse;

                std::vector<std::vector<SkinnedMesh_vertexWeight>> vertex_weight;

                std::vector<MathCore::vec4f> src_pos;
                std::vector<MathCore::vec4f> src_normals;
                std::vector<MathCore::vec4f> src_tangent;
                std::vector<MathCore::vec4f> src_binormal;

                std::vector<int> transform_to_bone;
                std::vector<Transform *> transform_list;
                std::vector<ITKExtension::Model::Bone *> model_bone_list;

                //
                // GPU Skinning
                //
                bool isGPUSkinning;
                uint32_t SkinnedMesh_GPU_VBO_Upload_Bitflag; // model::CONTAINS_VERTEX_WEIGHT16;

                // MathCore::vec3f clip_motion;

                ComponentSkinnedMesh(ResourceHelper *_resourceHelper, bool isGPUSkinning);

                void loadModelBase(const std::string &filename);

                void moveMeshToTransform();

                void loadAnimation(const std::string &clip_name, const std::string &filename);

                void done();

                void start();
                void OnPreUpdate(Platform::Time *time);

                ~ComponentSkinnedMesh();
            };

        }

    }

}