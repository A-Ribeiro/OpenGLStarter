#pragma once

#include "../common.h"
#include <appkit-gl-engine/util/SpriteAtlas.h>

namespace ui
{

    enum DialogAppearModeType
    {
        DialogAppearModeType_Scroll = 0,
        DialogAppearModeType_Fade,
    };

    enum DialogAvatarSideType
    {
        DialogAvatarSideType_None = 0,
        DialogAvatarSideType_Left,
        DialogAvatarSideType_Right,
    };

    enum DialogTextModeType
    {
        DialogTextModeType_CharAppear = 0,
        DialogTextModeType_AppearAtOnce,
    };

    class InGameDialog
    {

        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> parent;

        float avatar_size;
        float continue_button_size;
        float text_size;
        float screen_margin;
        float text_margin; // margin from main_box

        void createAllComponents();

        void releaseAllComponents();

        bool components_created;

        std::string rich_message;
        std::string rich_continue_char;

        int min_line_count;

    public:
        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> node_ui;

        // std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> main_box;
        // std::shared_ptr<AppKit::GLEngine::Components::ComponentRectangle> main_box_bg;
        // std::shared_ptr<AppKit::GLEngine::Components::ComponentFont> main_box_text;

        // std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> avatar_box;
        // std::shared_ptr<AppKit::GLEngine::Components::ComponentRectangle> avatar_box_bg;
        // std::shared_ptr<AppKit::GLEngine::Components::ComponentSprite> avatar_box_sprite;

        // std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> continue_box;
        // std::shared_ptr<AppKit::GLEngine::Components::ComponentRectangle> continue_box_bg;
        // std::shared_ptr<AppKit::GLEngine::Components::ComponentFont> continue_box_text;

        std::shared_ptr<AppKit::GLEngine::SpriteAtlas> avatarAtlas;


        ScreenManager *screenManager;

        InGameDialog() = default;
        ~InGameDialog() = default;

        void setParent(std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> &parent, ScreenManager *screenManager);
        void setProperties(float avatar_size, float continue_button_size, float text_size, float screen_margin, float text_margin);
        
        void blinkAmount(float lerp_factor);

        void layoutVisibleElements(const MathCore::vec2i &size);// called on resize

        CollisionCore::AABB<MathCore::vec3f> computeAABB();

        void setSpriteAvatars(const std::vector<std::string> &sprite_list);

        int countLinesForText(const std::string &rich_message);

        void setMinLineCount(int min_line_count); // start dialog with empty lines to reserve space

        void showDialog(

            DialogAppearModeType appear_mode,

            DialogAvatarSideType side,
            const std::string &avatar,

            DialogTextModeType mode,
            const std::string &rich_message,
            const std::string &rich_continue_char,

            EventCore::Callback<void()> onAppeared,
            EventCore::Callback<void()> onContinuePressed);

        void hideDialog(DialogAppearModeType appear_mode,
            EventCore::Callback<void()> onDisapeared);

        void resetColors();
    };

}
