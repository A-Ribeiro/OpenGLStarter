#include "ComponentUI.h"

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentUI::Type = "ComponentUI";

            ComponentUI::ComponentUI() : Component(ComponentUI::Type)
            {
                always_clone = true;
            }

            ComponentUI::~ComponentUI()
            {
            }

            UIItem ComponentUI::addText(
                const std::string &font_name,
                const std::string &text,
                int h_align,
                int v_align,
                const MathCore::vec2f &pos,
                const MathCore::vec4f &color,
                float z,
                const std::string &name)
            {
                auto transform = Transform::CreateShared(name);
                auto item = UIItem(
                    getTransform()->addChild(transform),
                    self<ComponentUI>());
                items.push_back(item);
                return item;
            }

            // UIItem ComponentUI::addRectangleMinMax(
            //     const MathCore::vec2f &min,
            //     const MathCore::vec2f &max,
            //     const MathCore::vec4f &color,
            //     const MathCore::vec4f &radius,
            //     float z,
            //     const std::string &name)
            // {
            //     auto transform = Transform::CreateShared(name);
            //     transform->setLocalPosition(MathCore::vec3f(0, 0, z));
            //     auto rect = transform->addNewComponent<ComponentRectangle>();
            //     rect->setQuadFromMinMax(this->resourceMap, min, max, color, radius);
            //     auto item = UIItem(
            //         getTransform()->addChild(transform),
            //         self<ComponentUI>());
            //     item.set<ComponentRectangle>(rect);
            //     items.push_back(item);
            //     return item;
            // }

            // color.a == 0, skip this draw
            UIItem ComponentUI::addRectangleCenterSize(
                
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
                const std::string &name)

            {
                auto transform = Transform::CreateShared(name);
                transform->setLocalPosition(MathCore::vec3f(0, 0, z));
                auto rect = transform->addNewComponent<ComponentRectangle>();
                rect->setQuadFromCenterSize(
                    this->resourceMap,
                    center,
                    size,
                    color,
                    radius,
                    stroke_mode,
                    stroke_thickness,
                    stroke_color,
                    drop_shadow_thickness,
                    drop_shadow_color
                );
                auto item = UIItem(
                    getTransform()->addChild(transform),
                    self<ComponentUI>());
                item.set<ComponentRectangle>(rect);
                items.push_back(item);
                return item;
            }

            UIItem ComponentUI::addSprite(
                const MathCore::vec2f &pos,
                const std::string &texture_path,
                const MathCore::vec2f &pivot,
                const MathCore::vec4f &color,
                const MathCore::vec2f &size_constraint,
                float z,
                const std::string &name)
            {
                auto engine = AppKit::GLEngine::Engine::Instance();

                auto transform = Transform::CreateShared(name);
                transform->setLocalPosition(MathCore::vec3f(pos.x, pos.y, z));
                auto sprite = transform->addNewComponent<ComponentSprite>();
                sprite->setTexture(
                    this->resourceMap,
                    this->spriteShader,
                    resourceMap->getTexture(texture_path, engine->sRGBCapable),
                    pivot,
                    color,
                    size_constraint,
                    MeshUploadMode_Direct);
                auto item = UIItem(
                    getTransform()->addChild(transform),
                    self<ComponentUI>());
                item.set<ComponentSprite>(sprite);
                items.push_back(item);
                return item;
            }

            void ComponentUI::setVisible(bool visible)
            {
                getTransform()->skip_traversing = !visible;
            }

            void ComponentUI::clear()
            {
                for (auto &item : items)
                    item.transform->removeSelf();
                items.clear();
            }

            void ComponentUI::Initialize(AppKit::GLEngine::ResourceMap *resourceMap,
                                         std::shared_ptr<SpriteShader> spriteShader)
            {
                this->resourceMap = resourceMap;
                this->spriteShader = spriteShader;
            }

            std::shared_ptr<Component> ComponentUI::duplicate_ref_or_clone(bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentUI>();

                result->always_clone = this->always_clone;

                return result;
            }
            void ComponentUI::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentUI::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }

            void ComponentUI::Deserialize(rapidjson::Value &_value,
                                          std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                          std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                          ResourceSet &resourceSet)
            {
            }

        }
    }
}
