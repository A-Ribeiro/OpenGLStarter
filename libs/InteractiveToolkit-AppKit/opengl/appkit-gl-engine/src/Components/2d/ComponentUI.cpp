#include <appkit-gl-engine/Components/2d/ComponentUI.h>

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

            UIItem &ComponentUI::getItemByName(const std::string &name)
            {
                for (size_t i = 0; i < items.size(); i++)
                {
                    if (items[i].transform->getName() == name)
                    {
                        return items[i];
                    }
                }
                static UIItem dummy;
                return dummy;
            }

            UIItem ComponentUI::addComponentUI(const MathCore::vec2f &pos,
                                               float z,
                                               const std::string &name)
            {
                auto transform = Transform::CreateShared(name);
                transform->setLocalPosition(MathCore::vec3f(pos.x, pos.y, z));
                auto ui = transform->addNewComponent<ComponentUI>();
                ui->Initialize(resourceMap);
                auto item = UIItem(
                    getTransform()->addChild(transform),
                    self<ComponentUI>());
                item.set<ComponentUI>(ui);
                items.push_back(item);
                return item;
            }

            UIItem ComponentUI::addTextureText(
                const std::string &font_path,
                const MathCore::vec2f &pos,
                float z,

                const std::string &text,
                float size, ///< current state of the font size
                float max_width,

                const MathCore::vec4f &faceColor,   ///< current state of the face color
                const MathCore::vec4f &strokeColor, ///< current state of the stroke color

                // .a == 0 turn off the drawing
                // bool drawFace;                          ///< should draw face
                // bool drawStroke;                        ///< should draw stroke

                const MathCore::vec3f &strokeOffset,
                AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
                AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
                float lineHeight,

                AppKit::OpenGL::GLFont2WrapMode wrapMode,
                AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
                char32_t wordSeparatorChar,

                const std::string &name)
            {
                auto engine = AppKit::GLEngine::Engine::Instance();
                auto transform = Transform::CreateShared(name);
                transform->setLocalPosition(MathCore::vec3f(pos.x, pos.y, z));

                auto font = transform->addNewComponent<ComponentFont>();
                font->setText(
                    this->resourceMap,
                    font_path,
                    0, 0, nullptr,
                    engine->sRGBCapable,
                    text,
                    size,
                    max_width,
                    faceColor,
                    strokeColor,
                    strokeOffset,
                    horizontalAlign,
                    verticalAlign,
                    lineHeight,
                    wrapMode,
                    firstLineHeightMode,
                    wordSeparatorChar,
                    MeshUploadMode_Direct);

                auto item = UIItem(
                    getTransform()->addChild(transform),
                    self<ComponentUI>());
                item.set<ComponentFont>(font);
                items.push_back(item);
                return item;
            }

            UIItem ComponentUI::addPolygonText(
                const std::string &font_path,
                float base_polygon_size,
                float base_polygon_distance_tolerance,
                Platform::ThreadPool *polygon_threadPool,
                const MathCore::vec2f &pos,
                float z,

                const std::string &text,
                float size, ///< current state of the font size
                float max_width,

                // .a == 0 turn off the drawing
                const MathCore::vec4f &faceColor,   ///< current state of the face color
                const MathCore::vec4f &strokeColor, ///< current state of the stroke color

                const MathCore::vec3f &strokeOffset,
                AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
                AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
                float lineHeight,

                AppKit::OpenGL::GLFont2WrapMode wrapMode,
                AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
                char32_t wordSeparatorChar,

                const std::string &name)
            {
                auto engine = AppKit::GLEngine::Engine::Instance();
                auto transform = Transform::CreateShared(name);
                transform->setLocalPosition(MathCore::vec3f(pos.x, pos.y, z));

                auto font = transform->addNewComponent<ComponentFont>();
                font->setText(
                    this->resourceMap,
                    font_path,
                    base_polygon_size, base_polygon_distance_tolerance, polygon_threadPool,
                    engine->sRGBCapable,
                    text,
                    size,
                    max_width,
                    faceColor,
                    strokeColor,
                    strokeOffset,
                    horizontalAlign,
                    verticalAlign,
                    lineHeight,
                    wrapMode,
                    firstLineHeightMode,
                    wordSeparatorChar,
                    MeshUploadMode_Direct);

                auto item = UIItem(
                    getTransform()->addChild(transform),
                    self<ComponentUI>());
                item.set<ComponentFont>(font);
                items.push_back(item);
                return item;
            }

            // color.a == 0, skip this draw
            UIItem ComponentUI::addRectangle(

                const MathCore::vec2f &pos,
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
                transform->setLocalPosition(MathCore::vec3f(pos.x, pos.y, z));
                auto rect = transform->addNewComponent<ComponentRectangle>();
                rect->setQuad(
                    this->resourceMap,
                    size,
                    color,
                    radius,
                    stroke_mode,
                    stroke_thickness,
                    stroke_color,
                    drop_shadow_thickness,
                    drop_shadow_color,
                    MeshUploadMode_Direct);
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

            UIItem ComponentUI::addSpriteFromAtlas(
                const MathCore::vec2f &pos,
                std::shared_ptr<SpriteAtlas> atlas,
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
                sprite->setTextureFromAtlas(
                    this->resourceMap,
                    atlas,
                    texture_path,
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

            void ComponentUI::Initialize(AppKit::GLEngine::ResourceMap *resourceMap)
            {
                this->resourceMap = resourceMap;
            }

            std::shared_ptr<Component> ComponentUI::duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentUI>();

                result->always_clone = this->always_clone;

                result->resourceMap = this->resourceMap;

                for (auto &item : items)
                    result->items.push_back(item);

                return result;
            }
            void ComponentUI::fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
            {
                for (auto &item : items)
                {
                    if (transformMap.find(item.transform) != transformMap.end())
                        item.transform = transformMap[item.transform];
                    item.self_ui = self<ComponentUI>();
                    if (item.type == UIItemFont && componentMap.find(item.get<ComponentFont>()) != componentMap.end())
                        item.set<ComponentFont>(std::dynamic_pointer_cast<ComponentFont>(componentMap[item.get<ComponentFont>()]));
                    else if (item.type == UIItemRectangle && componentMap.find(item.get<ComponentRectangle>()) != componentMap.end())
                        item.set<ComponentRectangle>(std::dynamic_pointer_cast<ComponentRectangle>(componentMap[item.get<ComponentRectangle>()]));
                    else if (item.type == UIItemSprite && componentMap.find(item.get<ComponentSprite>()) != componentMap.end())
                        item.set<ComponentSprite>(std::dynamic_pointer_cast<ComponentSprite>(componentMap[item.get<ComponentSprite>()]));
                    else if (item.type == UIItemUI && componentMap.find(item.get<ComponentUI>()) != componentMap.end())
                        item.set<ComponentUI>(std::dynamic_pointer_cast<ComponentUI>(componentMap[item.get<ComponentUI>()]));
                }
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
