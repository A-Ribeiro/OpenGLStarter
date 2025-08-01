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
                    self<ComponentUI>()
                );
                items.push_back(item);
                return item;
            }

            UIItem ComponentUI::addRectangle(
                const MathCore::vec2f &min,
                const MathCore::vec2f &max,
                const MathCore::vec4f &color,
                const MathCore::vec4f &radius,
                float z,
                const std::string &name)
            {
                auto transform = Transform::CreateShared(name);
                auto item = UIItem(
                    getTransform()->addChild(transform),
                    self<ComponentUI>()
                );
                items.push_back(item);
                return item;
            }

            UIItem ComponentUI::addSprite(
                const MathCore::vec2f &pos,
                std::shared_ptr<AppKit::OpenGL::GLTexture> texture,
                const MathCore::vec2f &pivot,
                const MathCore::vec4f &color,
                const MathCore::vec2f &size_constraint,
                float z,
                const std::string &name)
            {
                auto transform = Transform::CreateShared(name);
                auto item = UIItem(
                    getTransform()->addChild(transform),
                    self<ComponentUI>()
                );
                items.push_back(item);
                return item;
            }

            void ComponentUI::setVisible(bool visible)
            {
                getTransform()->skip_traversing = !visible;
            }

            void ComponentUI::clear()
            {
                items.clear();
            }

            void ComponentUI::Initialize(AppKit::GLEngine::ResourceMap *resourceMap,
                                         std::shared_ptr<SpriteShader> spriteShader)
            {
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
