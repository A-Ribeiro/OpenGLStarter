#include "./ScreenOptions.h"
#include "./ScreenManager.h"

namespace ui
{
    void ScreenOptions::layoutElements(const MathCore::vec2i &size)
    {
        // float total_size = ScreenOptions::height * (float)uiComponent->items.size() + ScreenOptions::gap * (float)(uiComponent->items.size() - 1);
        // float total_size_half = total_size * 0.5f;
        // float current_y = total_size_half - ScreenOptions::height * 0.5f;

        // current_y -= (float)size.height * 0.5f;

        // current_y += margin_bottom + total_size_half;

        // for (const auto &item : uiComponent->items)
        // {
        //     // item.transform->setLocalPosition(MathCore::vec3f(0, current_y, 0));
        //     // current_y -= ScreenOptions::height + ScreenOptions::gap;
        // }

        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f);

        auto bg = uiComponent->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        bg->getTransform()->setLocalPosition(MathCore::vec3f(0, -top_bar_height * 0.5f, 0));
        bg->setQuad(
            uiComponent->resourceMap,
            MathCore::vec2f(valid_size.x, valid_size.y - top_bar_height), // size
            screenManager->colorPalette.primary,                          // color
            MathCore::vec4f(0, 32, 32, 0),                                // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,           // stroke mode
            screenManager->colorPalette.stroke_thickness,                 // stroke thickness
            screenManager->colorPalette.primary_stroke,                   // stroke color
            0,                                                            // drop shadow thickness
            MathCore::vec4f(0),                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct           // meshUploadMode,
        );
    }
    // void ScreenOptions::addButton(const std::string &text)
    // {

    //     auto ui = uiComponent->addComponentUI(MathCore::vec2f(0, 0), 0, text).get<AppKit::GLEngine::Components::ComponentUI>();

    //     ui->addRectangle(
    //         MathCore::vec2f(0, 0),                                  // pos
    //         MathCore::vec2f(ScreenOptions::width, ScreenOptions::height), // size
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
    //         ScreenOptions::height * 0.5f,                                        // size
    //         ScreenOptions::width,                                                // max_width
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
    // void ScreenOptions::previousButton()
    // {
    //     if (change_screen)
    //         return;
    //     selected_button = MathCore::OP<int>::clamp(selected_button - 1, 0, uiComponent->items.size() - 1);
    //     setPrimaryColorAll();
    // }
    // void ScreenOptions::nextButton()
    // {
    //     if (change_screen)
    //         return;
    //     selected_button = MathCore::OP<int>::clamp(selected_button + 1, 0, uiComponent->items.size() - 1);
    //     setPrimaryColorAll();
    // }
    // void ScreenOptions::backButton()
    // {
    //     if (change_screen)
    //         return;
    //     selected_button = uiComponent->items.size() - 1;
    //     setPrimaryColorAll();
    // }
    // void ScreenOptions::selectOption(const std::string &name)
    // {
    //     printf("Selected name: %s\n", name.c_str());
    //     setPrimaryColorAll();
    //     if (name == "Exit Game")
    //         AppKit::GLEngine::Engine::Instance()->app->exitApp();
    //     else
    //         screenManager->open_screen("ScreenOptions");
    // }
    // void ScreenOptions::setPrimaryColorAll()
    // {
    //     for (auto &entry : uiComponent->items)
    //     {
    //         auto rect = entry.get<AppKit::GLEngine::Components::ComponentUI>()->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
    //         rect->setColor(
    //             screenManager->colorPalette.primary,
    //             screenManager->colorPalette.primary_stroke,
    //             0);
    //     }
    // }

    std::string ScreenOptions::name() const
    {
        return "ScreenOptions";
    }

    void ScreenOptions::resize(const MathCore::vec2i &size)
    {
        printf("    [ScreenOptions] resize %i %i\n", size.width, size.height);
        layoutElements(size);
    }

    void ScreenOptions::update(Platform::Time *elapsed)
    {
        if (uiComponent->items.size() == 0)
            return;

        // float speed = osciloscope_normal_hz;

        // if (increase_speed_for_secs_and_trigger_action > 0.0f)
        // {
        //     speed = osciloscope_selected_hz;
        //     increase_speed_for_secs_and_trigger_action -= elapsed->unscaledDeltaTime;
        //     if (increase_speed_for_secs_and_trigger_action < 0.0f)
        //     {
        //         increase_speed_for_secs_and_trigger_action = -1.0f;
        //         printf("Action at selection end...");
        //         // selectOption(uiComponent->items[selected_button].transform->getName());
        //     }
        // }

        // if (!change_screen || increase_speed_for_secs_and_trigger_action > 0.0f)
        // {

        //     const float _360_pi = MathCore::CONSTANT<float>::PI * 2.0f;
        //     osciloscope = MathCore::OP<float>::fmod(osciloscope + elapsed->unscaledDeltaTime * speed * _360_pi, _360_pi);
        //     float sin = MathCore::OP<float>::sin(osciloscope) * 0.5f + 0.5f;

        //     auto rect = uiComponent->items[selected_button].get<AppKit::GLEngine::Components::ComponentUI>()->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        //     rect->setColor(
        //         screenManager->colorPalette.lrp_active(sin),
        //         screenManager->colorPalette.lrp_active_stroke(sin),
        //         0);
        // }
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ScreenOptions::initializeTransform(
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
        osciloscope = 0.0f;
        increase_speed_for_secs_and_trigger_action = -1.0f;
        change_screen = false;
        printf("    [ScreenOptions] initializeTransform\n");
        uiNode = AppKit::GLEngine::Transform::CreateShared("ScreenOptions");
        uiNode->skip_traversing = true;

        uiComponent = uiNode->addNewComponent<AppKit::GLEngine::Components::ComponentUI>();
        uiComponent->Initialize(resourceMap);

        // addButton("Continue");
        // addButton("New Game");
        // addButton("Options");
        // addButton("Exit Game");

        // auto ui = uiComponent->addComponentUI(MathCore::vec2f(0, 0), 0, "game").get<AppKit::GLEngine::Components::ComponentUI>();
        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f);
        uiComponent->addRectangle(
            MathCore::vec2f(0, -top_bar_height * 0.5f),                   // pos
            MathCore::vec2f(valid_size.x, valid_size.y - top_bar_height), // size
            screenManager->colorPalette.primary,                          // color
            MathCore::vec4f(0, 32, 32, 0),                                // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,           // stroke mode
            screenManager->colorPalette.stroke_thickness,                 // stroke thickness
            screenManager->colorPalette.primary_stroke,                   // stroke color
            0,                                                            // drop shadow thickness
            MathCore::vec4f(0),                                           // drop shadow color
            0,                                                            // z
            "bg");

        const char *top_buttons[] = {
            "Game",
            "Audio",
            "Graphics"};
        int topButtonsCount = sizeof(top_buttons) / sizeof(top_buttons[0]);

        auto shoulder_size = MathCore::vec2f(top_bar_height, top_bar_height);
        float available_width = (valid_size.width - (shoulder_size.x * 2.0f + button_gap * 2.0f));
        auto button_size = MathCore::vec2f(
            (available_width - button_gap * (float)(topButtonsCount - 1)) / (float)topButtonsCount,
            top_bar_height); // game, audio, graphics
        auto ui = uiComponent->addComponentUI(MathCore::vec2f(0, 0), -1, "top_bar").get<AppKit::GLEngine::Components::ComponentUI>();
        ui->addRectangle(
            MathCore::vec2f(-valid_size.width * 0.5f + shoulder_size.x * 0.5f,
                            valid_size.height * 0.5f - top_bar_height * 0.5f), // pos
            shoulder_size,                                                     // size
            screenManager->colorPalette.active,                                // color
            MathCore::vec4f(0, 0, 0, 32),                                      // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                // stroke mode
            screenManager->colorPalette.stroke_thickness,                      // stroke thickness
            screenManager->colorPalette.primary_stroke,                        // stroke color
            0,                                                                 // drop shadow thickness
            MathCore::vec4f(0),                                                // drop shadow color
            0,                                                                 // z
            "shoulder_left");
        ui->addRectangle(
            MathCore::vec2f(valid_size.width * 0.5f - shoulder_size.x * 0.5f,
                            valid_size.height * 0.5f - top_bar_height * 0.5f), // pos
            shoulder_size,                                                     // size
            screenManager->colorPalette.active,                                // color
            MathCore::vec4f(32, 0, 0, 0),                                      // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                // stroke mode
            screenManager->colorPalette.stroke_thickness,                      // stroke thickness
            screenManager->colorPalette.primary_stroke,                        // stroke color
            0,                                                                 // drop shadow thickness
            MathCore::vec4f(0),                                                // drop shadow color
            0,                                                                 // z
            "shoulder_right");

        MathCore::vec2f pos(-valid_size.width * 0.5f + shoulder_size.x + button_gap + button_size.x * 0.5f,
                            valid_size.height * 0.5f - top_bar_height * 0.5f);
        for (int i = 0; i < topButtonsCount; i++)
        {
            ui->addRectangle(
                pos,                                                // pos
                button_size,                                        // size
                screenManager->colorPalette.active,                 // color
                MathCore::vec4f(32, 0, 0, 32),                      // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                screenManager->colorPalette.stroke_thickness,       // stroke thickness
                screenManager->colorPalette.primary_stroke,         // stroke color
                0,                                                  // drop shadow thickness
                MathCore::vec4f(0),                                 // drop shadow color
                0,                                                  // z
                top_buttons[i]);
            pos.x += button_size.x + button_gap;
        }

        return uiNode;
    }

    void ScreenOptions::triggerEvent(ui::UIEventEnum event)
    {
        printf("    [ScreenOptions] %s\n", UIEventToStr(event));
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
                // nextButton();
            }
            else if (event == UIEventEnum::UIEvent_InputUp)
            {
                // previousButton();
            }
            else if (event == UIEventEnum::UIEvent_InputActionBack)
            {
                // backButton();
                screenManager->open_screen("ScreenMain");
            }
        }
    }

}