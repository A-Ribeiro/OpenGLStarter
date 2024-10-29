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

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

                bool skinning_dirty;

                bool countBonesLinkedToTransforms(std::shared_ptr<Transform> element, void *userData);

                void computeSkinningInformation();

                void OnTransformVisited(std::shared_ptr<Transform> t);

                void applySkinning();

            public:
                static const ComponentType Type;

                AnimationMixer mixer;
                
                std::shared_ptr<Transform> model_base;

                ResourceHelper *resourceHelper;

                std::shared_ptr<ComponentMesh> componentMesh;
                std::shared_ptr<ComponentMaterial> componentMaterial;

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
                std::vector<std::weak_ptr<Transform>> transform_list;
                std::vector<ITKExtension::Model::Bone *> model_bone_list;

                //
                // GPU Skinning
                //
                bool isGPUSkinning;
                uint32_t SkinnedMesh_GPU_VBO_Upload_Bitflag; // model::CONTAINS_VERTEX_WEIGHT16;

                // MathCore::vec3f clip_motion;

                ComponentSkinnedMesh(ResourceHelper *_resourceHelper = nullptr, bool isGPUSkinning = true);

                void loadModelBase(const std::string &filename);

                void moveMeshToTransform();

                void loadAnimation(const std::string &clip_name, const std::string &filename);

                void done();

                void start();
                void OnPreUpdate(Platform::Time *time);

                ~ComponentSkinnedMesh();

                // always clone 
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone);
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);

            };

        }

    }

}