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
            class ComponentRectangle;

            class ComponentFont : public Component
            {

                std::string last_text;

            public:
                static const ComponentType Type;

                bool always_clone;

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;
                std::shared_ptr<ComponentMeshWrapper> meshWrapper;

                std::shared_ptr<ComponentCamera> camera;
                std::shared_ptr<ComponentRectangle> mask;

                std::shared_ptr<AppKit::GLEngine::ResourceMap::FontResource> last_fontResource;

                void checkOrCreateAuxiliaryComponents(
                    AppKit::GLEngine::ResourceMap *resourceMap);

                // if color.a == 0, skip this draw
                const std::string &getText() const;

                void setText(
                    AppKit::GLEngine::ResourceMap *resourceMap,

                    const std::string &font_path,

                    // 0 = texture, > 0 = polygon
                    float polygon_size,
                    float polygon_distance_tolerance,
                    Platform::ThreadPool *polygon_threadPool,

                    bool is_srgb,

                    const std::string &text,
                    float size = 60.0f, ///< current state of the font size
                    float max_width = -1.0f,

                    // .a == 0 turn off the drawing
                    const MathCore::vec4f &faceColor = MathCore::vec4f(1.0f, 1.0f, 1.0f, 1.0f),   ///< current state of the face color
                    const MathCore::vec4f &strokeColor = MathCore::vec4f(0.0f, 0.0f, 0.0f, 1.0f), ///< current state of the stroke color

                    const MathCore::vec3f &strokeOffset = MathCore::vec3f(0.0f, 0.0f, -0.02f),
                    AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_center,
                    AppKit::OpenGL::GLFont2VerticalAlign verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_middle,
                    float lineHeight = 1.5f,

                    AppKit::OpenGL::GLFont2WrapMode wrapMode = AppKit::OpenGL::GLFont2WrapMode_Word,
                    AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode = AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,
                    char32_t wordSeparatorChar = U' ',
                    MeshUploadMode meshUploadMode = MeshUploadMode_Direct);

                void setMask(AppKit::GLEngine::ResourceMap *resourceMap,
                             std::shared_ptr<ComponentCamera> &camera,
                             std::shared_ptr<ComponentRectangle> &mask);

                CollisionCore::AABB<MathCore::vec3f> computeBox(
                    AppKit::GLEngine::ResourceMap *resourceMap,
                    const std::string &font_path,
                    // 0 = texture, > 0 = polygon
                    float polygon_size,
                    float polygon_distance_tolerance,
                    Platform::ThreadPool *polygon_threadPool,
                    bool is_srgb,

                    const std::string &text,
                    float size = 60.0f, ///< current state of the font size
                    float max_width = -1.0f,

                    AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_center,
                    AppKit::OpenGL::GLFont2VerticalAlign verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_middle,
                    float lineHeight = 1.5f,

                    AppKit::OpenGL::GLFont2WrapMode wrapMode = AppKit::OpenGL::GLFont2WrapMode_Word,
                    AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode = AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,
                    char32_t wordSeparatorChar = U' ');

                ComponentFont();

                ~ComponentFont();

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
