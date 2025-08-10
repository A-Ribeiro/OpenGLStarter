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
// #include "../../util/SpriteAtlas.h"

#include <appkit-gl-engine/Components/Core/ComponentMesh.h>
#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>

// #include "../../shaders/SpriteShader.h"
#include "./common.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            enum StrokeModeEnum
            {
                StrokeModeGrowMiddle,
                StrokeModeGrowInside,
                StrokeModeGrowOutside
            };

            class ComponentRectangle : public Component
            {

                void clearMesh();

                void drawInside(const MathCore::vec2f &center,
                                const MathCore::vec2f &size,
                                const MathCore::vec4f &color_internal,
                                const MathCore::vec4f &color_external,
                                const MathCore::vec4f &radius,
                                StrokeModeEnum stroke_mode,
                                float ignore_stroke_thickness,
                                uint32_t segment_count);

                void drawStroke(const MathCore::vec2f &center,
                                const MathCore::vec2f &size,
                                const MathCore::vec4f &color_internal,
                                const MathCore::vec4f &color_external,
                                const MathCore::vec4f &radius,
                                StrokeModeEnum stroke_mode,
                                float stroke_thickness,
                                float stroke_offset,
                                uint32_t segment_count);

            public:
                static const ComponentType Type;

                bool always_clone;

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;
                std::shared_ptr<ComponentMeshWrapper> meshWrapper;

                void checkOrCreateAuxiliaryComponents(
                    AppKit::GLEngine::ResourceMap *resourceMap);

                // void setQuadFromMinMax(
                //     AppKit::GLEngine::ResourceMap *resourceMap,
                //     const MathCore::vec2f &min,
                //     const MathCore::vec2f &max,
                //     const MathCore::vec4f &color,
                //     const MathCore::vec4f &radius,
                //     StrokeModeEnum stroke_mode = StrokeModeGrowMiddle,
                //     float ignore_stroke_thickness = 0.0f,
                //     uint32_t segment_count = 10);


                // if color.a == 0, skip this draw
                void setQuadFromCenterSize(
                    AppKit::GLEngine::ResourceMap *resourceMap,
                    const MathCore::vec2f &center,
                    const MathCore::vec2f &size,
                    const MathCore::vec4f &color,
                    const MathCore::vec4f &radius,
                    StrokeModeEnum stroke_mode,
                    float stroke_thickness,
                    const MathCore::vec4f &stroke_color,
                    float drop_shadow_thickness,
                    const MathCore::vec4f &drop_shadow_color,
                    MeshUploadMode meshUploadMode,
                    uint32_t segment_count = 10);

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
