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
#include <appkit-gl-engine/util/SpriteAtlas.h>

#include <appkit-gl-engine/Components/Core/ComponentMesh.h>
#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>

#include "./common.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            class ComponentRectangle;

            class ComponentSprite : public Component
            {
            public:
                static const ComponentType Type;

                bool always_clone;

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;
                std::shared_ptr<ComponentMeshWrapper> meshWrapper;

                std::shared_ptr<ComponentRectangle> mask;

                std::shared_ptr<AppKit::OpenGL::GLTexture> last_texture;

                void checkOrCreateAuxiliaryComponents(
                    AppKit::GLEngine::ResourceMap *resourceMap,
                    std::shared_ptr<AppKit::OpenGL::GLTexture> texture
                );

                void setTexture(
                    AppKit::GLEngine::ResourceMap *resourceMap,
                    std::shared_ptr<AppKit::OpenGL::GLTexture> texture,
                    const MathCore::vec2f &pivot,
                    const MathCore::vec4f &color,
                    const MathCore::vec2f &size_constraint = MathCore::vec2f(-1, -1),
                    MeshUploadMode meshUploadMode = MeshUploadMode_Static
                );

                void setTextureFromAtlas(
                    AppKit::GLEngine::ResourceMap *resourceMap,
                    std::shared_ptr<SpriteAtlas> atlas,
                    const std::string &name,
                    const MathCore::vec2f &pivot,
                    const MathCore::vec4f &color,
                    const MathCore::vec2f &size_constraint = MathCore::vec2f(-1, -1),
                    MeshUploadMode meshUploadMode = MeshUploadMode_Static
                );

                // void setTextureFromAtlas(
                //     std::shared_ptr<AppKit::OpenGL::GLTexture> altas_texture,
                //     const SpriteAtlas::Entry &altas_entry,
                //     const MathCore::vec2f &pivot,
                //     const MathCore::vec4f &color,
                //     bool staticMesh
                // );

                void setMask(AppKit::GLEngine::ResourceMap *resourceMap,
                             std::shared_ptr<ComponentCamera> &camera,
                             std::shared_ptr<ComponentRectangle> &mask);

                ComponentSprite();

                ~ComponentSprite();

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
