#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/Platform/Core/SmartVector.h>

// #include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

// #include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
// #include <appkit-gl-engine/GL/GLRenderState.h>
// // #include <appkit-gl-engine/util/ReferenceCounter.h>
// // #include <aRibeiroData/aRibeiroData.h>

// // #include <appkit-gl-engine/DefaultEngineShader.h>
// #include "../util/SpriteAtlas.h"

// #include <appkit-gl-engine/Components/ComponentMesh.h>
// #include <appkit-gl-engine/Components/ComponentMaterial.h>
// #include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

#include "../../shaders/SpriteShader.h"

#include "UIItem.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            // manage raycast algorithms
            class ComponentUI : public Component
            {
            public:
                static const ComponentType Type;

                bool always_clone;

                AppKit::GLEngine::ResourceMap *resourceMap;
                std::shared_ptr<SpriteShader> spriteShader;

                Platform::SmartVector<UIItem> items;

                UIItem addText(
                    const std::string &font_name,
                    const std::string &text,
                    int h_align,
                    int v_align,
                    const MathCore::vec2f &pos,
                    const MathCore::vec4f &color,
                    float z,
                    const std::string &name = "_text");

                // UIItem addRectangleMinMax(
                //     const MathCore::vec2f &min,
                //     const MathCore::vec2f &max,
                //     const MathCore::vec4f &color,
                //     const MathCore::vec4f &radius,
                //     float z,
                //     const std::string &name = "_rect");

                UIItem addRectangleCenterSize(
                    const MathCore::vec2f &center,
                    const MathCore::vec2f &size,
                    const MathCore::vec4f &color,
                    const MathCore::vec4f &radius,
                    StrokeModeEnum stroke_mode,
                    float stroke_thickness,
                    const MathCore::vec4f &stroke_color,
                    float drop_shadow_thickness,
                    const MathCore::vec4f &drop_shadow_color,

                    float z,
                    const std::string &name = "_rect");

                UIItem addSprite(
                    const MathCore::vec2f &pos,
                    std::shared_ptr<AppKit::OpenGL::GLTexture> texture,
                    const MathCore::vec2f &pivot,
                    const MathCore::vec4f &color,
                    const MathCore::vec2f &size_constraint,
                    float z,
                    const std::string &name = "_sprite");

                void setVisible(bool visible);

                void clear();

                void Initialize(AppKit::GLEngine::ResourceMap *resourceMap,
                                std::shared_ptr<SpriteShader> spriteShader);

                ComponentUI();

                ~ComponentUI();

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

#include "UIItem_get_set.inl"
