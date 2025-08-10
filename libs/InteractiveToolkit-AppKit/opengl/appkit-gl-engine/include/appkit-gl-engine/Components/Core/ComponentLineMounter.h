#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

// #include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
// #include <appkit-gl-engine/util/ReferenceCounter.h>
// #include <aRibeiroData/aRibeiroData.h>

// #include <appkit-gl-engine/DefaultEngineShader.h>
// #include "SpriteAtlas.h"

#include <appkit-gl-engine/Components/Core/ComponentMesh.h>
#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>

#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {
        class ResourceMap;
        namespace Components
        {

            class ComponentLineMounter : public Component
            {
                // make possible to the component to fiz the aabb according the pixel size of the 
                // line segments and the camera projection type
                void OnBeforeComputeFinalPositions(ComponentMeshWrapper* meshWrapper);

                void OnCameraTransformVisit(std::shared_ptr<Transform> transform);

                

                bool dirty;
                // for orthographic camera, the last max scaled value
                float last_max_scaled;


                // for perspective camera, the last camera projection matrix
                MathCore::vec3f last_dir;
                MathCore::vec3f last_cam_pos;
                float last_near_plane;
                float last_far_plane;
                float last_tan_over_viewport_height;
                MathCore::mat4f last_local_to_world;

                std::shared_ptr<ComponentCamera> camera;

                // this calculates the max size the point can be in any projection.
                // for orthographic camera, it is the max scaled value
                // for perspective camera, it is the max scaled value at the far plane
                bool use_max_scale_update_on_change_self_scale;
                float one_time_set_scale;
                float one_time_word_to_local_scale;
                float one_time_camera_scale;
                void one_time_set(std::shared_ptr<ComponentCamera> camera);

            public:
                static const ComponentType Type;

                bool always_clone;

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;
                std::shared_ptr<ComponentMeshWrapper> meshWrapper;
                

                using AABBType = CollisionCore::AABB<MathCore::vec3f>;

                AABBType aabb;

                void checkOrCreateAuxiliaryComponents(AppKit::GLEngine::ResourceMap *resourceMap);

                void setCamera(AppKit::GLEngine::ResourceMap *resourceMap, std::shared_ptr<ComponentCamera> camera, bool one_time_set);

                void force_refresh();

                ComponentLineMounter();

                ~ComponentLineMounter();

                void clear();

                void addLine(const MathCore::vec3f &a, const MathCore::vec3f &b,
                             float thickness,
                             const MathCore::vec4f &color);
                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone) { return nullptr; }
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap) {}

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {}
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet) {}
            };
        }
    }
}
