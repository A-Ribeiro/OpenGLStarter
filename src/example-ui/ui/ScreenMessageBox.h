#pragma once

#include "./common.h"
#include "./util/ButtonManager.h"

namespace ui
{
    class ScreenMessageBox : public ui::Screen
    {
        void layoutElements(const MathCore::vec2i &size);
        
        void previousButton();
        void nextButton();
        void backButton();
        void selectOption(const std::string &name);
        void setPrimaryColorAll();

        bool change_screen;
        float increase_speed_for_secs_and_trigger_action;
        float osciloscope;
        int selected_button;
        ui::ScreenManager *screenManager;

        EventCore::Callback<void(const std::string &)> onOptionSelected;

        std::string text;

        ButtonManager buttonManager;

    public:

        enum MessageBoxType
        {
            MessageBoxType_OneOption = 0,
            MessageBoxType_TwoOption
        };

        void showMessageBox(
            const std::string &rich_message,
            const std::vector<std::string> &options = {"OK", "Cancel"},
            EventCore::Callback<void(const std::string &)> onOptionSelected = nullptr
        );

        static constexpr float button_width = 256;
        static constexpr float button_height = 64;
        static constexpr float button_gap = 10;
        static constexpr float button_text_size = 32;

        static constexpr float text_size = 32;

        static constexpr float screen_margin = 64;
        static constexpr float text_margin = 32;

        static constexpr float osciloscope_normal_hz = 1.0f;
        static constexpr float osciloscope_selected_hz = 6.0f;

        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent;
        std::shared_ptr<AppKit::GLEngine::Transform> uiNode;

        const static char *Name;
        std::string name() const;
        void resize(const MathCore::vec2i &size);
        void update(Platform::Time *elapsed);

        std::shared_ptr<AppKit::GLEngine::Transform> initializeTransform(
            AppKit::GLEngine::Engine *engine,
            AppKit::GLEngine::ResourceMap *resourceMap,
            MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
            ui::ScreenManager *screenManager,
            const MathCore::vec2i &size);

        void triggerEvent(ui::UIEventEnum event);
    };
}