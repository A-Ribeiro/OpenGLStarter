#pragma once

#include "./common.h"

namespace ui
{
    class TopBar;
    class OptionSet;

    class ScreenOptions : public ui::Screen
    {
        // void addButton(const std::string &text);
        // void previousButton();
        // void nextButton();
        // void backButton();
        // void selectOption(const std::string &name);
        // void setPrimaryColorAll();

        bool change_screen;
        float increase_speed_for_secs_and_trigger_action;
        float osciloscope;
        int selected_button;
        ui::ScreenManager *screenManager;

        void layoutElements(const MathCore::vec2i &size);

        void activeCurrentTab();

    public:
        // static constexpr float width = 256;
        // static constexpr float height = 64;
        // static constexpr float gap = 10;

        static constexpr float margin = 64;

        static constexpr float button_gap = 8.0f;
        static constexpr float top_bar_height = 64.0f;

        static constexpr float item_height = 64.0f;
        static constexpr float item_hmargin = 16.0f;
        static constexpr float item_vmargin = 32.0f;


        static constexpr float osciloscope_normal_hz = 1.0f;
        static constexpr float osciloscope_selected_hz = 6.0f;

        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent;
        std::shared_ptr<AppKit::GLEngine::Transform> uiNode;

        std::shared_ptr<TopBar> topBar;
        std::unordered_map<std::string, std::unique_ptr<OptionSet>> optionMap;

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

#include "./Options/TopBar.h"
#include "./Options/OptionSet.h"
