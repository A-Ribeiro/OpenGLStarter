#include "./ScreenMain.h"
#include "./ScreenManager.h"

namespace ui
{
    void ScreenMain::layoutElements(const MathCore::vec2i &size)
    {
        float total_size = ScreenMain::height * (float)uiComponent->items.size() + ScreenMain::gap * (float)(uiComponent->items.size() - 1);
        float total_size_half = total_size * 0.5f;
        float current_y = total_size_half - ScreenMain::height * 0.5f;

        current_y -= (float)size.height * 0.5f;

        current_y += margin_bottom + total_size_half;

        for (const auto &item : uiComponent->items)
        {
            item.transform->setLocalPosition(MathCore::vec3f(0, current_y, 0));
            current_y -= ScreenMain::height + ScreenMain::gap;
        }
    }
    void ScreenMain::addButton(const std::string &text)
    {

        auto ui = uiComponent->addComponentUI(MathCore::vec2f(0, 0), 0, text).get<AppKit::GLEngine::Components::ComponentUI>();

        ui->addRectangle(
            MathCore::vec2f(0, 0),                                  // pos
            MathCore::vec2f(ScreenMain::width, ScreenMain::height), // size
            screenManager->colorPalette.primary,                    // color
            MathCore::vec4f(32),                                    // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,     // stroke mode
            screenManager->colorPalette.stroke_thickness,           // stroke thickness
            screenManager->colorPalette.primary_stroke,             // stroke color
            0,                                                      // drop shadow thickness
            MathCore::vec4f(0),                                     // drop shadow color
            0,                                                      // z
            "bg");

        ui->addTextureText(
            "resources/Roboto-Regular-100.basof2",                            // font_path
            MathCore::vec2f(0, 0),                                            // pos
            -1,                                                               // z
            text,                                                             // text
            ScreenMain::height * 0.5f,                                        // size
            ScreenMain::width,                                                // max_width
            screenManager->colorPalette.text,                                 // faceColor
            colorFromHex("#000000", 0.0f),                                    // strokeColor
            MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // strokeOffset
            AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // horizontalAlign
            AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // verticalAlign
            1.0f,                                                             // lineHeight
            AppKit::OpenGL::GLFont2WrapMode_Word,                             // wrapMode
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // firstLineHeightMode
            U' ',                                                             // wordSeparatorChar
            "text");
    }
    void ScreenMain::previousButton()
    {
        if (change_screen)
            return;
        selected_button = MathCore::OP<int>::clamp(selected_button - 1, 0, uiComponent->items.size() - 1);
        setPrimaryColorAll();
    }
    void ScreenMain::nextButton()
    {
        if (change_screen)
            return;
        selected_button = MathCore::OP<int>::clamp(selected_button + 1, 0, uiComponent->items.size() - 1);
        setPrimaryColorAll();
    }
    void ScreenMain::backButton()
    {
        if (change_screen)
            return;
        selected_button = uiComponent->items.size() - 1;
        setPrimaryColorAll();
    }
    void ScreenMain::selectOption(const std::string &name)
    {
        printf("Selected name: %s\n", name.c_str());
        setPrimaryColorAll();
        if (name == "Exit Game")
            AppKit::GLEngine::Engine::Instance()->app->exitApp();
        else
            screenManager->open_screen("ScreenMain");
    }
    void ScreenMain::setPrimaryColorAll()
    {
        for (auto &entry : uiComponent->items)
        {
            auto rect = entry.get<AppKit::GLEngine::Components::ComponentUI>()->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
            rect->setColor(
                screenManager->colorPalette.primary,
                screenManager->colorPalette.primary_stroke,
                0);
        }
    }

    std::string ScreenMain::name() const
    {
        return "ScreenMain";
    }

    void ScreenMain::resize(const MathCore::vec2i &size)
    {
        printf("    [ScreenMain] resize %i %i\n", size.width, size.height);
        layoutElements(size);
    }

    void ScreenMain::update(Platform::Time *elapsed)
    {
        if (uiComponent->items.size() == 0)
            return;

        float speed = osciloscope_normal_hz;

        if (increase_speed_for_secs_and_trigger_action > 0.0f)
        {
            speed = osciloscope_selected_hz;
            increase_speed_for_secs_and_trigger_action -= elapsed->unscaledDeltaTime;
            if (increase_speed_for_secs_and_trigger_action < 0.0f)
            {
                increase_speed_for_secs_and_trigger_action = -1.0f;
                printf("Action at selection end...");
                selectOption(uiComponent->items[selected_button].transform->getName());
            }
        }

        if (!change_screen || increase_speed_for_secs_and_trigger_action > 0.0f)
        {

            const float _360_pi = MathCore::CONSTANT<float>::PI * 2.0f;
            osciloscope = MathCore::OP<float>::fmod(osciloscope + elapsed->unscaledDeltaTime * speed * _360_pi, _360_pi);
            float sin = MathCore::OP<float>::sin(osciloscope) * 0.5f + 0.5f;

            auto rect = uiComponent->items[selected_button].get<AppKit::GLEngine::Components::ComponentUI>()->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
            rect->setColor(
                screenManager->colorPalette.lrp_active(sin),
                screenManager->colorPalette.lrp_active_stroke(sin),
                0);
        }
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ScreenMain::initializeTransform(
        AppKit::GLEngine::Engine *engine,
        AppKit::GLEngine::ResourceMap *resourceMap,
        MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
        ui::ScreenManager *screenManager)
    {
        if (uiNode)
            return uiNode;
        this->screenManager = screenManager;
        selected_button = 0;
        osciloscope = 0.0f;
        increase_speed_for_secs_and_trigger_action = -1.0f;
        change_screen = false;
        printf("    [ScreenMain] initializeTransform\n");
        uiNode = AppKit::GLEngine::Transform::CreateShared("ScreenMain");
        uiNode->skip_traversing = true;

        uiComponent = uiNode->addNewComponent<AppKit::GLEngine::Components::ComponentUI>();
        uiComponent->Initialize(resourceMap);

        addButton("Continue");
        addButton("New Game");
        addButton("Options");
        addButton("Exit Game");

        return uiNode;
    }

    void ScreenMain::triggerEvent(ui::UIEventEnum event)
    {
        printf("    [ScreenMain] %s\n", UIEventToStr(event));
        if (event == UIEventEnum::UIEvent_ScreenPush)
        {
            uiNode->skip_traversing = false;

            osciloscope = 0.0f;
            increase_speed_for_secs_and_trigger_action = -1.0f;
            change_screen = false;
        }
        else if (event == UIEventEnum::UIEvent_ScreenPop)
        {
            uiNode->skip_traversing = true;
        }
        else if (!change_screen && increase_speed_for_secs_and_trigger_action < 0.0f)
        {
            if (event == UIEventEnum::UIEvent_InputActionEnter)
            {
                increase_speed_for_secs_and_trigger_action = 0.5f;
                change_screen = true;
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