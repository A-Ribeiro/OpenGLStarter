// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include "ComponentLineMounter.h"

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            // const ComponentType ComponentSprite::Type = "ComponentSprite";

            // void ComponentSprite::checkOrCreateAuxiliaryComponents()
            // {
            //     if (material != nullptr || mesh != nullptr || meshWrapper != nullptr)
            //         return;
            //     auto transform = getTransform();
            //     if (material == nullptr)
            //     {
            //         material = transform->addNewComponent<ComponentMaterial>();
            //         material->type = MaterialUnlitTexture;
            //         material->unlit.tex = directTexture.texture;
            //         material->unlit.color = directTexture.color;
            //     }
            //     if (mesh == nullptr)
            //     {
            //         mesh = transform->addNewComponent<ComponentMesh>();
            //         mesh->format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0;
            //     }
            //     if (meshWrapper == nullptr)
            //     {
            //         meshWrapper = transform->addNewComponent<ComponentMeshWrapper>();
            //         transform->makeFirstComponent(meshWrapper);
            //         //meshWrapper->updateMeshAABB();
            //     }
            // }

            // void ComponentSprite::setTexture(
            //     std::shared_ptr<AppKit::OpenGL::GLTexture> &texture,
            //     const MathCore::vec2f &pivot,
            //     const MathCore::vec4f &color,
            //     bool staticMesh)
            // {
            //     checkOrCreateAuxiliaryComponents();
            // }

            // void ComponentSprite::setTextureFromAtlas(
            //     std::shared_ptr<SpriteAtlas> &atlas,
            //     const std::string &name,
            //     const MathCore::vec2f &pivot,
            //     const MathCore::vec4f &color,
            //     bool staticMesh)
            // {
            //     checkOrCreateAuxiliaryComponents();
            // }

            // void ComponentSprite::setTextureFromAtlas(
            //     std::shared_ptr<AppKit::OpenGL::GLTexture> &altas_texture,
            //     const SpriteAtlas::Entry &altas_entry,
            //     const MathCore::vec2f &pivot,
            //     const MathCore::vec4f &color,
            //     bool staticMesh)
            // {
            //     checkOrCreateAuxiliaryComponents();
            // }

            // ComponentSprite::ComponentSprite() : Component(ComponentSprite::Type)
            // {
            //     type = SpriteSourceNone;
            //     directTexture.color = MathCore::vec4f(1.0f);
            //     directTexture.pivot = MathCore::vec2f(0.5f, 0.5f);
            //     directTexture.texture = nullptr;

            //     textureFromAtlas.color = MathCore::vec4f(1.0f);
            //     textureFromAtlas.entry = SpriteAtlas::Entry();
            //     textureFromAtlas.texture = nullptr;

            //     always_clone = false;
            // }

            // ComponentSprite::~ComponentSprite()
            // {
            // }

            // // always clone
            // std::shared_ptr<Component> ComponentSprite::duplicate_ref_or_clone(bool force_clone)
            // {
            //     if (!always_clone && !force_clone)
            //         return self();
            //     auto result = Component::CreateShared<ComponentSprite>();

            //     result->type = this->type;
            //     result->directTexture = this->directTexture;
            //     result->textureFromAtlas = this->textureFromAtlas;
            //     result->always_clone = this->always_clone;

            //     return result;
            // }
            // void ComponentSprite::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            // {
            // }

            // void ComponentSprite::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            // {
            // }
            // void ComponentSprite::Deserialize(rapidjson::Value &_value,
            //                                   std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
            //                                   std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
            //                                   ResourceSet &resourceSet)
            // {
            // }

        }
    }
}
