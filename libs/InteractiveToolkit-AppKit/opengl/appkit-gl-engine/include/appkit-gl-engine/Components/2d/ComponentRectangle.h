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
        class AddShaderRectangleMask;

        namespace Components
        {

            enum StrokeModeEnum
            {
                StrokeModeGrowMiddle,
                StrokeModeGrowInside,
                StrokeModeGrowOutside
            };

            enum MaskOrder
            {
                MaskOrder_topRight = 0,
                MaskOrder_bottomRight,
                MaskOrder_bottomLeft,
                MaskOrder_topLeft,
                MaskOrder_Count
            };

            class ComponentRectangle : public Component
            {
            protected:
                MathCore::vec2f mask_corner[MaskOrder_Count];
                MathCore::vec4f mask_radius;

                size_t
                    idx_start_inside,
                    idx_start_stroke_external,
                    idx_start_stroke_internal,
                    idx_start_drop_shadow_min_external,
                    idx_start_drop_shadow_min_internal,
                    idx_start_drop_shadow_max_external,
                    idx_start_drop_shadow_max_internal,
                    idx_end;

                enum DrawStrokeEnum {
                    DrawStroke_Stroke,
                    DrawStroke_DropShadow_Internal,
                    DrawStroke_DropShadow_External
                };

            private:
                void clearMesh();

                void precomputeMaskParameters(const MathCore::vec2f &size,
                                              const MathCore::vec4f &radius,
                                              StrokeModeEnum stroke_mode,
                                              float ignore_stroke_thickness);

                void drawInside(const MathCore::vec2f &size,
                                const MathCore::vec4f &color_internal,
                                const MathCore::vec4f &color_external,
                                const MathCore::vec4f &radius,
                                StrokeModeEnum stroke_mode,
                                float ignore_stroke_thickness,
                                uint32_t segment_count);

                void drawStroke(const MathCore::vec2f &size,
                                const MathCore::vec4f &color_internal,
                                const MathCore::vec4f &color_external,
                                const MathCore::vec4f &radius,
                                StrokeModeEnum stroke_mode,
                                float stroke_thickness,
                                float stroke_offset,
                                uint32_t segment_count,
                                DrawStrokeEnum drawStrokeMode);

            public:
                friend class AppKit::GLEngine::AddShaderRectangleMask;

                static const ComponentType Type;

                bool always_clone;

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;
                std::shared_ptr<ComponentMeshWrapper> meshWrapper;

                std::shared_ptr<ComponentCamera> camera;
                std::shared_ptr<ComponentRectangle> mask;

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
                void setQuad(
                    AppKit::GLEngine::ResourceMap *resourceMap,
                    // const MathCore::vec2f &center,
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
                
                void setColor(
                    const MathCore::vec4f &color,
                    const MathCore::vec4f &stroke_color,
                    const MathCore::vec4f &drop_shadow_color);

                void setLinearColorVertical(
                    const MathCore::vec4f &color_bottom,
                    const MathCore::vec4f &color_top);

                void setMask(AppKit::GLEngine::ResourceMap *resourceMap,
                             std::shared_ptr<ComponentCamera> &camera,
                             std::shared_ptr<ComponentRectangle> &mask);

                ComponentRectangle();

                ~ComponentRectangle();

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone);
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);
            };
        }
    }
}
