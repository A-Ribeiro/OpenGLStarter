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
#include "../util/SpriteAtlas.h"

#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

#include "../shaders/SpriteShader.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            class ComponentRectangle : public Component
            {
            public:
                static const ComponentType Type;

                bool always_clone;

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;
                std::shared_ptr<ComponentMeshWrapper> meshWrapper;

                void checkOrCreateAuxiliaryComponents(
                    AppKit::GLEngine::ResourceMap *resourceMap
                );

                void setQuadFromMinMax(
                    AppKit::GLEngine::ResourceMap *resourceMap,
                    const MathCore::vec2f &min,
                    const MathCore::vec2f &max,
                    const MathCore::vec4f &color,
                    const MathCore::vec4f &radius
                );

                void setQuadFromCenterSize(
                    AppKit::GLEngine::ResourceMap *resourceMap,
                    const MathCore::vec2f &center,
                    const MathCore::vec2f &size,
                    const MathCore::vec4f &color,
                    const MathCore::vec4f &radius
                );

                ComponentRectangle();

                ~ComponentRectangle();

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
