#pragma once

#include "./common.h"
#include "./util/OsciloscopeWithTrigger.h"
#include "./util/ButtonManager.h"
#include "./util/InGameDialog.h"

namespace ui
{

    class ScreenHUD : public ui::Screen, public OsciloscopeWithTrigger
    {
        void layoutElements(const MathCore::vec2i &size);

        ui::ScreenManager *screenManager;

    protected:
        void onOsciloscopeAction();
        void onOsciloscopeSinLerp(Platform::Time *elapsed, float osciloscope, float sin);

    public:
        static constexpr float avatar_size = 256;
        static constexpr float continue_button_size = 64;

        static constexpr float text_size = 32;

        static constexpr float screen_margin = 64;
        static constexpr float text_margin = 32; // margin from main_box

        static constexpr float osciloscope_normal_hz = 1.0f;
        static constexpr float osciloscope_selected_hz = 6.0f;
        static constexpr float osciloscope_countdown_trigger_secs = 0.5f;

        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent;
        std::shared_ptr<AppKit::GLEngine::Transform> uiNode;

        ui::InGameDialog inGameDialog;

        const static char *Name;

        ScreenHUD();

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
    };
}