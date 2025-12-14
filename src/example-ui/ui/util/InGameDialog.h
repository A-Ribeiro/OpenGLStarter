#pragma once

#include "../common.h"
#include <appkit-gl-engine/util/SpriteAtlas.h>
#include "RichMessageTokenizer.h"

namespace ui
{

    enum DialogAppearModeType
    {
        DialogAppearModeType_None = 0,
        DialogAppearModeType_Scroll,
        DialogAppearModeType_Fade,
    };

    // enum DialogAvatarSideType
    // {
    //     DialogAvatarSideType_None = 0,
    //     DialogAvatarSideType_Left,
    //     DialogAvatarSideType_Right,
    // };

    enum DialogTextModeType
    {
        DialogTextModeType_None = 0,
        DialogTextModeType_CharAppear,
        DialogTextModeType_AppearAtOnce,
    };

    struct DialogProperties
    {
        float side_percentage;
        std::string avatar;
        DialogTextModeType text_mode;
        std::string rich_message;
    };

    class InGameDialog : public EventCore::HandleCallback
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
        std::string avatar;
        float side_percentage;
        float size_text_y;
        MathCore::vec2f avatar_offset;
        MathCore::vec2f max_box_size;
        MathCore::vec2f main_offset;


        int min_line_count;
        float min_text_height;

        std::string rich_message_source;
        AppKit::OpenGL::RichMessageTokenizer text_tokenizer;

        DialogTextModeType text_mode;

        float count_up_aux;
        float seconds_per_character;
        float char_per_sec;
        float char_per_sec_fast;

        EventCore::Callback<void()> onAppeared;
        EventCore::Callback<void()> onContinuePressed;
        EventCore::Callback<void()> onDisapeared;

        Platform::SmartVector<DialogProperties> smart_dialog_pages;
        DialogAppearModeType appear_mode;
        DialogAppearModeType disappear_mode;

        EventCore::Callback<void()> smart_OnDialogEnded;

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
        void setProperties(float avatar_size, float continue_button_size, float text_size, float screen_margin, float text_margin, float char_per_sec, float char_per_sec_fast);
        
        void update(Platform::Time *elapsed, float blink_01_lerp_factor);

        void layoutVisibleElements(const MathCore::vec2i &size);// called on resize

        CollisionCore::AABB<MathCore::vec3f> computeAABB();

        void setSpriteAvatars(const std::vector<std::string> &sprite_list);

        int countLinesForText(const std::string &rich_message) const;
        void setMinLineCount(int min_line_count); // start dialog with empty lines to reserve space

        CollisionCore::AABB<MathCore::vec3f> computeTextAABB(const std::string &rich_message) const;
        float computeTextHeight(const std::string &rich_message) const;
        void setMinTextHeight(float h);
        

        void showDialog(

            DialogAppearModeType appear_mode,

            float side_percentage, // DialogAvatarSideType
            const std::string &avatar,

            DialogTextModeType text_mode,
            const std::string &rich_message,
            const std::string &rich_continue_char,

            EventCore::Callback<void()> onAppeared,
            EventCore::Callback<void()> onContinuePressed);

        void hideDialog(DialogAppearModeType appear_mode,
            EventCore::Callback<void()> onDisapeared);


        void smartShowDialog(
            DialogAppearModeType appear_mode,
            DialogAppearModeType disappear_mode,
            const std::string &rich_continue_char,
            const std::vector<DialogProperties> &dialog_pages,
            EventCore::Callback<void()> onDialogEnded);
        

        void pressContinue();

        void resetColors();

        private:
        void show_next_page();
    };

}
