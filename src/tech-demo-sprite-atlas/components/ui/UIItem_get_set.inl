#pragma once

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            template <>
            inline std::shared_ptr<ComponentFont> &UIItem::get<ComponentFont>()
            {
                ITK_ABORT(type != UIItemFont, "This is not a font item");
                return font;
            }
            template <>
            inline void UIItem::set<ComponentFont>(std::shared_ptr<ComponentFont> v)
            {
                ITK_ABORT(type != UIItemFont, "This is not a font item");
                font = v;
            }
            template <>
            inline std::shared_ptr<ComponentRectangle> &UIItem::get<ComponentRectangle>()
            {
                ITK_ABORT(type != UIItemRectangle, "This is not a rectangle item");
                return rectangle;
            }
            template <>
            inline void UIItem::set<ComponentRectangle>(std::shared_ptr<ComponentRectangle> v)
            {
                ITK_ABORT(type != UIItemRectangle, "This is not a rectangle item");
                rectangle = v;
            }
            template <>
            inline std::shared_ptr<ComponentSprite> &UIItem::get<ComponentSprite>()
            {
                ITK_ABORT(type != UIItemSprite, "This is not a sprite item");
                return sprite;
            }
            template <>
            inline void UIItem::set<ComponentSprite>(std::shared_ptr<ComponentSprite> v)
            {
                ITK_ABORT(type != UIItemSprite, "This is not a sprite item");
                sprite = v;
            }
            template <>
            inline std::shared_ptr<ComponentSpriteAtlas> &UIItem::get<ComponentSpriteAtlas>()
            {
                ITK_ABORT(type != UIItemSpriteAtlas, "This is not a sprite atlas item");
                return sprite_atlas;
            }
            template <>
            inline void UIItem::set<ComponentSpriteAtlas>(std::shared_ptr<ComponentSpriteAtlas> v)
            {
                ITK_ABORT(type != UIItemSpriteAtlas, "This is not a sprite atlas item");
                sprite_atlas = v;
            }
            template <>
            inline std::shared_ptr<ComponentUI> &UIItem::get<ComponentUI>()
            {
                ITK_ABORT(type != UIItemUI, "This is not a UI item");
                return ui;
            }
            template <>
            inline void UIItem::set<ComponentUI>(std::shared_ptr<ComponentUI> v)
            {
                ITK_ABORT(type != UIItemUI, "This is not a UI item");
                ui = v;
            }

        }
    }
}
