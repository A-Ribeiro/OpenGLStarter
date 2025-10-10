#include "./ScreenMain.h"
#include "./ScreenManager.h"

namespace ui
{
    void ScreenMain::layoutElements(const MathCore::vec2i &size)
    {
        // float total_size = ScreenMain::height * (float)uiComponent->items.size() + ScreenMain::gap * (float)(uiComponent->items.size() - 1);
        // float total_size_half = total_size * 0.5f;
        // float current_y = total_size_half - ScreenMain::height * 0.5f;

        // current_y -= (float)size.height * 0.5f;

        // current_y += margin_bottom + total_size_half;

        // for (const auto &item : uiComponent->items)
        // {
        //     item.transform->setLocalPosition(MathCore::vec3f(0, current_y, 0));
        //     current_y -= ScreenMain::height + ScreenMain::gap;
        // }

        float y_start = -size.height * 0.5f + ScreenMain::margin_bottom;

        buttonManager.layoutVisibleElements(ButtonDirection_vertical);

        CollisionCore::AABB<MathCore::vec3f> button_aabb = buttonManager.computeAABB(ButtonDirection_vertical);

        buttonManager.node_ui->getTransform()->setLocalPosition(
            MathCore::vec3f(0, -button_aabb.min_box.y + y_start, 0));
    }
    // void ScreenMain::addButton(const std::string &text)
    // {

    //     auto ui = uiComponent->addComponentUI(MathCore::vec2f(0, 0), 0, text).get<AppKit::GLEngine::Components::ComponentUI>();

    //     ui->addRectangle(
    //         MathCore::vec2f(0, 0),                                  // pos
    //         MathCore::vec2f(ScreenMain::width, ScreenMain::height), // size
    //         screenManager->colorPalette.primary,                    // color
    //         MathCore::vec4f(32),                                    // radius
    //         AppKit::GLEngine::Components::StrokeModeGrowInside,     // stroke mode
    //         screenManager->colorPalette.stroke_thickness,           // stroke thickness
    //         screenManager->colorPalette.primary_stroke,             // stroke color
    //         0,                                                      // drop shadow thickness
    //         MathCore::vec4f(0),                                     // drop shadow color
    //         0,                                                      // z
    //         "bg");

    //     ui->addTextureText(
    //         "resources/Roboto-Regular-100.basof2",                            // font_path
    //         MathCore::vec2f(0, 0),                                            // pos
    //         -1,                                                               // z
    //         text,                                                             // text
    //         ScreenMain::height * 0.5f,                                        // size
    //         ScreenMain::width,                                                // max_width
    //         screenManager->colorPalette.text,                                 // faceColor
    //         colorFromHex("#000000", 0.0f),                                    // strokeColor
    //         MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // strokeOffset
    //         AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // horizontalAlign
    //         AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // verticalAlign
    //         1.0f,                                                             // lineHeight
    //         AppKit::OpenGL::GLFont2WrapMode_Word,                             // wrapMode
    //         AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // firstLineHeightMode
    //         U' ',                                                             // wordSeparatorChar
    //         "text");
    // }
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
        if (name == "Exit Game")
            AppKit::GLEngine::Engine::Instance()->app->exitApp();
        else if (name == "Options")
            screenManager->open_screen("ScreenOptions");
        else
            screenManager->open_screen("ScreenMain");
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
        // for (auto &entry : uiComponent->items)
        // {
        //     auto rect = entry.get<AppKit::GLEngine::Components::ComponentUI>()->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        //     rect->setColor(
        //         screenManager->colorPalette.primary,
        //         screenManager->colorPalette.primary_stroke,
        //         0);
        // }
    }

    const char *ScreenMain::Name = "ScreenMain";

    void ScreenMain::onOsciloscopeAction()
    {
        printf("Action at selection end...");
        // selectOption(uiComponent->items[selected_button].transform->getName());
        selectOption(buttonManager.getButtonText(selected_button));
    }
    void ScreenMain::onOsciloscopeSinLerp(float osciloscope, float sin)
    {
        buttonManager.setButtonColor(selected_button,
                                     screenManager->colorPalette.lrp_active(sin),
                                     screenManager->colorPalette.lrp_active_stroke(sin));
        // auto rect = uiComponent->items[selected_button].get<AppKit::GLEngine::Components::ComponentUI>()->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        // rect->setColor(
        //     screenManager->colorPalette.lrp_active(sin),
        //     screenManager->colorPalette.lrp_active_stroke(sin),
        //     0);
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
        // if (uiComponent->items.size() == 0)
        //     return;
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
        uiNode = AppKit::GLEngine::Transform::CreateShared("ScreenMain");
        uiNode->skip_traversing = true;

        uiComponent = uiNode->addNewComponent<AppKit::GLEngine::Components::ComponentUI>();
        uiComponent->Initialize(resourceMap);

        // addButton("Continue");
        // addButton("New Game");
        // addButton("Options");
        // addButton("Exit Game");

        buttonManager.setParent(uiComponent, screenManager);
        buttonManager.setButtonProperties(
            ScreenMain::button_width,
            ScreenMain::button_height,
            ScreenMain::button_gap,
            ScreenMain::button_text_size);
        buttonManager.reserveButtonData(4);

        buttonManager.setButtonVisibleCount(4);
        buttonManager.setButtonText(0, "Continue");
        buttonManager.setButtonText(1, "New Game");
        buttonManager.setButtonText(2, "Options");
        buttonManager.setButtonText(3, "Exit Game");

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

}