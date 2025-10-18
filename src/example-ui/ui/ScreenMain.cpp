#include "./ScreenMain.h"
#include "./ScreenManager.h"

namespace ui
{
    void ScreenMain::layoutElements(const MathCore::vec2i &size)
    {
        float y_start = -size.height * 0.5f + ScreenMain::margin_bottom;

        buttonManager.layoutVisibleElements(ButtonDirection_vertical);

        CollisionCore::AABB<MathCore::vec3f> button_aabb = buttonManager.computeAABB(ButtonDirection_vertical);

        buttonManager.node_ui->getTransform()->setLocalPosition(
            MathCore::vec3f(0, -button_aabb.min_box.y + y_start, 0));
    }
    void ScreenMain::previousButton()
    {
        if (osciloscopeIsLocked())
            return;
        selected_button = MathCore::OP<int>::clamp(selected_button - 1, 0, (int)buttonManager.visible_count - 1);
        setPrimaryColorAll();
    }
    void ScreenMain::nextButton()
    {
        if (osciloscopeIsLocked())
            return;
        selected_button = MathCore::OP<int>::clamp(selected_button + 1, 0, (int)buttonManager.visible_count - 1);
        setPrimaryColorAll();
    }
    void ScreenMain::backButton()
    {
        if (osciloscopeIsLocked())
            return;
        selected_button = (int)buttonManager.visible_count - 1;
        setPrimaryColorAll();
    }
    void ScreenMain::selectOption(const std::string &name)
    {
        printf("Selected name: %s\n", name.c_str());
        setPrimaryColorAll();

        if (onOptionSelected != nullptr)
            onOptionSelected(name);
    }
    void ScreenMain::setPrimaryColorAll()
    {
        for (int i = 0; i < buttonManager.visible_count; i++)
        {
            buttonManager.setButtonColor(
                i,
                screenManager->colorPalette.primary,
                screenManager->colorPalette.primary_stroke);
        }
    }

    const char *ScreenMain::Name = "ScreenMain";

    void ScreenMain::onOsciloscopeAction()
    {
        printf("Action at selection end...");
        // selectOption(uiComponent->items[selected_button].transform->getName());
        selectOption(buttonManager.getButtonText(selected_button));
    }
    void ScreenMain::onOsciloscopeSinLerp(Platform::Time *elapsed, float osciloscope, float sin)
    {
        buttonManager.setButtonColor(selected_button,
                                     screenManager->colorPalette.lrp_active(sin),
                                     screenManager->colorPalette.lrp_active_stroke(sin));
    }

    ScreenMain::ScreenMain() : OsciloscopeWithTrigger(
                                   ScreenMain::osciloscope_normal_hz,
                                   ScreenMain::osciloscope_selected_hz,
                                   ScreenMain::osciloscope_countdown_trigger_secs)
    {
        selected_button = 0;
        screenManager = nullptr;
    }

    std::string ScreenMain::name() const
    {
        return Name;
    }

    void ScreenMain::resize(const MathCore::vec2i &size)
    {
        printf("    [ScreenMain] resize %i %i\n", size.width, size.height);
        layoutElements(size);
    }

    void ScreenMain::update(Platform::Time *elapsed)
    {
        if (buttonManager.visible_count == 0)
            return;

        osciloscopeUpdate(elapsed);
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ScreenMain::initializeTransform(
        AppKit::GLEngine::Engine *engine,
        AppKit::GLEngine::ResourceMap *resourceMap,
        MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
        ui::ScreenManager *screenManager,
        const MathCore::vec2i &size)
    {
        if (uiNode)
            return uiNode;
        this->screenManager = screenManager;
        selected_button = 0;

        printf("    [ScreenMain] initializeTransform\n");
        uiNode = AppKit::GLEngine::Transform::CreateShared(name());
        uiNode->skip_traversing = true;

        uiComponent = uiNode->addNewComponent<AppKit::GLEngine::Components::ComponentUI>();
        uiComponent->Initialize(resourceMap);

        buttonManager.setParent(uiComponent, screenManager);
        buttonManager.setButtonProperties(
            ScreenMain::button_width,
            ScreenMain::button_height,
            ScreenMain::button_gap,
            ScreenMain::button_text_size);
        buttonManager.reserveButtonData(4);

        return uiNode;
    }

    void ScreenMain::triggerEvent(ui::UIEventEnum event)
    {
        printf("    [ScreenMain] %s\n", UIEventToStr(event));
        if (event == UIEventEnum::UIEvent_ScreenPush)
        {
            uiNode->skip_traversing = false;

            osciloscopeResetLock();
        }
        else if (event == UIEventEnum::UIEvent_ScreenPop)
        {
            uiNode->skip_traversing = true;
        }
        else if (!osciloscopeIsLocked())
        {
            if (event == UIEventEnum::UIEvent_InputActionEnter)
            {
                osciloscopeTriggerAction();
            }
            else if (event == UIEventEnum::UIEvent_InputDown)
            {
                nextButton();
            }
            else if (event == UIEventEnum::UIEvent_InputUp)
            {
                previousButton();
            }
            else if (event == UIEventEnum::UIEvent_InputActionBack)
            {
                backButton();
            }
        }
    }

    void ScreenMain::show(
        const std::vector<std::string> &options,
        const std::string &init_selected,
        EventCore::Callback<void(const std::string &)> onOptionSelected)
    {
        this->onOptionSelected = onOptionSelected;

        buttonManager.setButtonVisibleCount((int)options.size());
        selected_button = 0;
        for (int i = 0; i < (int)options.size(); i++)
        {
            if (options[i] == init_selected)
                selected_button = i;
            buttonManager.setButtonText(i, options[i]);
        }

        layoutElements(screenManager->current_size);

        screenManager->open_screen("ScreenMain");
    }

}