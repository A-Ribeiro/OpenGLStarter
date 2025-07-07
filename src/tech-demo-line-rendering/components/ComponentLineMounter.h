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

#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

#include "../shaders/LineShader.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            class ComponentLineMounter : public Component
            {
                // make possible to the component to fiz the aabb according the pixel size of the 
                // line segments and the camera projection type
                void OnBeforeComputeFinalPositions(ComponentMeshWrapper* meshWrapper);

            public:
                static const ComponentType Type;

                bool always_clone;

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;
                std::shared_ptr<ComponentMeshWrapper> meshWrapper;
                std::shared_ptr<LineShader> lineShader;
                std::shared_ptr<ComponentCamera> camera;

                using AABBType = CollisionCore::AABB<MathCore::vec3f>;

                AABBType aabb;

                void checkOrCreateAuxiliaryComponents();

                void setLineShader(std::shared_ptr<LineShader> lineShader);
                void setCamera(std::shared_ptr<ComponentCamera> camera);


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
