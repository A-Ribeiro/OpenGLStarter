#include "./ScreenHUD.h"
#include "./ScreenManager.h"

namespace ui
{
    void ScreenHUD::layoutElements(const MathCore::vec2i &size)
    {
        inGameDialog.layoutVisibleElements(size);
    }
    void ScreenHUD::onOsciloscopeAction()
    {
    }
    void ScreenHUD::onOsciloscopeSinLerp(Platform::Time *elapsed, float osciloscope, float sin)
    {
        inGameDialog.update(elapsed, sin);
    }

    const char *ScreenHUD::Name = "ScreenHUD";

    ScreenHUD::ScreenHUD() : OsciloscopeWithTrigger(
                                 ScreenHUD::osciloscope_normal_hz,
                                 ScreenHUD::osciloscope_selected_hz,
                                 ScreenHUD::osciloscope_countdown_trigger_secs)
    {
    }

    std::string ScreenHUD::name() const
    {
        return Name;
    }
    void ScreenHUD::resize(const MathCore::vec2i &size)
    {
        layoutElements(size);
    }
    void ScreenHUD::update(Platform::Time *elapsed)
    {
        osciloscopeUpdate(elapsed);
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ScreenHUD::initializeTransform(
        AppKit::GLEngine::Engine *engine,
        AppKit::GLEngine::ResourceMap *resourceMap,
        MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
        ui::ScreenManager *screenManager,
        const MathCore::vec2i &size)
    {
        if (uiNode)
            return uiNode;
        this->screenManager = screenManager;

        uiNode = AppKit::GLEngine::Transform::CreateShared(name());
        uiNode->skip_traversing = true;

        uiComponent = uiNode->addNewComponent<AppKit::GLEngine::Components::ComponentUI>();
        uiComponent->Initialize(resourceMap);

        inGameDialog.setParent(uiComponent, screenManager);
        inGameDialog.setProperties(
            180, //ScreenHUD::avatar_size,
            64, //ScreenHUD::continue_button_size,
            32, //ScreenHUD::text_size,
            64, //ScreenHUD::screen_margin,
            32, //ScreenHUD::text_margin,
            20,// char per sec
            20*20);// char per sec fast -> 20x normal speed

        return uiNode;
    }

    void ScreenHUD::triggerEvent(ui::UIEventEnum event)
    {
        if (event == UIEventEnum::UIEvent_ScreenPush)
        {
            uiNode->skip_traversing = false;

            osciloscopeResetLock();
        }
        else if (event == UIEventEnum::UIEvent_ScreenPop)
        {
            uiNode->skip_traversing = true;
        }
    }

    void ScreenHUD::updateColorPalette()
    {
        inGameDialog.resetColors();
    }
}