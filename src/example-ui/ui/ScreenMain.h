#pragma once

#include "./common.h"
#include "./util/OsciloscopeWithTrigger.h"

namespace ui
{
    class ScreenMain : public ui::Screen, public OsciloscopeWithTrigger
    {
        void layoutElements(const MathCore::vec2i &size);
        void addButton(const std::string &text);
        void previousButton();
        void nextButton();
        void backButton();
        void selectOption(const std::string &name);
        void setPrimaryColorAll();

        int selected_button;
        ui::ScreenManager *screenManager;

    protected:
        void onOsciloscopeAction();
        void onOsciloscopeSinLerp(float osciloscope, float sin);

    public:
        static constexpr float width = 256;
        static constexpr float height = 64;
        static constexpr float gap = 10;

        static constexpr float margin_bottom = 64;

        static constexpr float osciloscope_normal_hz = 1.0f;
        static constexpr float osciloscope_selected_hz = 6.0f;
        static constexpr float osciloscope_countdown_trigger_secs = 0.5f;

        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent;
        std::shared_ptr<AppKit::GLEngine::Transform> uiNode;

        const static char *Name;

        ScreenMain();
        
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