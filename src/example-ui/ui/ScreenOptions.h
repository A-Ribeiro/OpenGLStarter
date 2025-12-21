#pragma once

#include "./common.h"
#include "./util/OsciloscopeWithTrigger.h"
#include "../util/AppOptions.h"

namespace ui
{
    class TopBar;
    class OptionSet;

    class ScreenOptions : public ui::Screen, public OsciloscopeWithTrigger
    {
        ui::ScreenManager *screenManager;

        void layoutElements(const MathCore::vec2i &size);

        void activeCurrentTab();

        std::unique_ptr<AppOptions::OptionsManager> localContext;

        void releaseLocalContext();
        void loadOptionsFromGlobalContext();

        EventCore::Callback<void(AppOptions::OptionsManager *localOptions)> onTryToExitAction;

    protected:
        void onOsciloscopeAction();
        void onOsciloscopeSinLerp(Platform::Time *elapsed, float osciloscope, float sin);
    public:

        static constexpr float margin = 64;

        static constexpr float button_gap = 8.0f;
        static constexpr float top_bar_height = 64.0f;

        static constexpr float item_height = 64.0f;
        static constexpr float item_hmargin = 16.0f;
        static constexpr float item_vmargin = 16.0f;


        static constexpr float osciloscope_normal_hz = 1.0f;
        static constexpr float osciloscope_selected_hz = 6.0f;
        static constexpr float osciloscope_countdown_trigger_secs = 0.5f;

        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent;
        std::shared_ptr<AppKit::GLEngine::Transform> uiNode;

        std::shared_ptr<TopBar> topBar;
        std::unordered_map<std::string, std::unique_ptr<OptionSet>> optionMap;

        const static char *Name;

        ScreenOptions();

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

        void updateColorPalette();

        void showOptions(
            EventCore::Callback<void(AppOptions::OptionsManager *localOptions)> onTryToExitAction
        );
    };

}

#include "./Options/TopBar.h"
#include "./Options/OptionSet.h"
