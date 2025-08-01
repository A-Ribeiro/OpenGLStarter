#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/Platform/Core/SmartVector.h>

// #include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

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
            class ComponentUI;

            enum UIItemType
            {
                UIItemNone,
                UIItemFont,
                UIItemRectangle,
                UIItemSprite,
                UIItemSpriteAtlas,
                UIItemUI,
            };

            struct UIItem
            {
                UIItemType type;

                std::shared_ptr<Transform> transform;
                std::weak_ptr<ComponentUI> self_ui;

            private:
                std::shared_ptr<ComponentFont> font;
                std::shared_ptr<ComponentRectangle> rectangle;
                std::shared_ptr<ComponentSprite> sprite;
                std::shared_ptr<ComponentSpriteAtlas> sprite_atlas;
                std::shared_ptr<ComponentUI> ui;

            public:
                template <typename T>
                std::shared_ptr<T> &get();
                template <typename T>
                void set(std::shared_ptr<T> v);

                UIItem(std::shared_ptr<Transform> transform,
                       std::weak_ptr<ComponentUI> self_ui);

                void removeSelf();
                bool operator==(const UIItem &other) const;
            };

        }
    }
}

// #include "../shaders/ComponentFont.h"
// #include "../shaders/ComponentRectangle.h"
// #include "../shaders/ComponentSprite.h"
// #include "../shaders/ComponentSpriteAtlas.h"
// #include "../shaders/ComponentUI.h"

