#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/Platform/Core/SmartVector.h>

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

            class ComponentFont;
            class ComponentRectangle;
            class ComponentSprite;
            class ComponentSpriteAtlas;

            enum UIItemType
            {
                UIItemNone,
                UIItemFont,
                UIItemRectangle,
                UIItemSprite,
                UIItemSpriteAtlas,
            };

            class ComponentUI : public Component
            {
            public:
                static const ComponentType Type;

                bool always_clone;

                struct Item
                {
                    std::shared_ptr<Transform> transform;

                    UIItemType type;

                    std::shared_ptr<ComponentFont> font;
                    std::shared_ptr<ComponentRectangle> rectangle;
                    std::shared_ptr<ComponentSprite> sprite;
                    std::shared_ptr<ComponentSpriteAtlas> sprite_atlas;
                };

                Platform::SmartVector<Item> items;

                void Initialize(AppKit::GLEngine::ResourceMap *resourceMap,
                                std::shared_ptr<SpriteShader> spriteShader)
                {
                }

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
