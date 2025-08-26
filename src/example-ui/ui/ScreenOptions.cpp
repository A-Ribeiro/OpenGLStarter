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

        if (topBar)
            topBar->layoutElements(size);
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

        topBar = std::make_shared<TopBar>();
        topBar->initialize({"Game",
                            "Audio",
                            "Graphics"},
                           uiComponent, screenManager);

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

    void TopBar::initialize(
        std::vector<std::string> buttonNames,
        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent,
        ScreenManager *screenManager)
    {
        this->screenManager = screenManager;
        ui = uiComponent->addComponentUI(MathCore::vec2f(0, 0), -1, "top_bar").get<AppKit::GLEngine::Components::ComponentUI>();

        addShoulder(0);
        addShoulder(1);

        for (const auto &buttonName : buttonNames)
            addButton(buttonName);
    }

    void TopBar::layoutElements(const MathCore::vec2i &size)
    {
        if (btns.size() == 0)
            return;
        int topButtonsCount = (int)btns.size();

        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f);

        ui->getTransform()->setLocalPosition(
            MathCore::vec3f(0, valid_size.height * 0.5f - ScreenOptions::top_bar_height * 0.5f, -1));

        auto shoulder_size = MathCore::vec2f(ScreenOptions::top_bar_height, ScreenOptions::top_bar_height);
        float available_width = (valid_size.width - (shoulder_size.x * 2.0f + ScreenOptions::button_gap * 2.0f));
        auto button_size = MathCore::vec2f(
            (available_width - ScreenOptions::button_gap * (float)(topButtonsCount - 1)) / (float)topButtonsCount,
            ScreenOptions::top_bar_height);

        MathCore::vec2f pos(-valid_size.width * 0.5f + shoulder_size.x + ScreenOptions::button_gap + button_size.x * 0.5f,
                            0);

        for (int i = 0; i < topButtonsCount; i++)
        {
            auto btn = btns[i];
            btn->getTransform()->setLocalPosition(MathCore::vec3f(pos, 0));

            auto bg = btn->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
            bg->setQuad(
                ui->resourceMap,
                button_size,                                        // size
                screenManager->colorPalette.disabled,               // color
                MathCore::vec4f(32, 0, 0, 32),                      // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                screenManager->colorPalette.stroke_thickness,       // stroke thickness
                screenManager->colorPalette.disabled_stroke,        // stroke color
                0,                                                  // drop shadow thickness
                MathCore::vec4f(0),                                 // drop shadow color
                AppKit::GLEngine::Components::MeshUploadMode_Direct // meshUploadMode,
            );

            pos.x += button_size.x + ScreenOptions::button_gap;
        }

        if (shoulders.size() != 2)
            return;
        shoulders[0]->getTransform()->setLocalPosition(MathCore::vec3f(-valid_size.width * 0.5f + shoulder_size.x * 0.5f, 0, 0));
        shoulders[1]->getTransform()->setLocalPosition(MathCore::vec3f(valid_size.width * 0.5f - shoulder_size.x * 0.5f, 0, 0));
    }

    void TopBar::addShoulder(int side)
    {
        std::string shoulder_name = (side == 0) ? "shoulder_left" : "shoulder_right";
        std::string shoulder_text = (side == 0) ? "L" : "R";
        MathCore::vec4f shoulder_border = (side == 0) ? MathCore::vec4f(32, 32, 32, 32) : MathCore::vec4f(32, 32, 32, 32);

        auto btn_ui = ui->addComponentUI(MathCore::vec2f(0, 0), 0, shoulder_name).get<AppKit::GLEngine::Components::ComponentUI>();
        shoulders.push_back(btn_ui);

        // auto size = screenManager->current_size;
        auto shoulder_size = MathCore::vec2f(ScreenOptions::top_bar_height, ScreenOptions::top_bar_height);

        btn_ui->addRectangle(
            MathCore::vec2f(0),                                 // pos
            shoulder_size,                                      // size
            screenManager->colorPalette.disabled,               // color
            shoulder_border,                                    // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
            screenManager->colorPalette.stroke_thickness,       // stroke thickness
            screenManager->colorPalette.disabled_stroke,        // stroke color
            0,                                                  // drop shadow thickness
            MathCore::vec4f(0),                                 // drop shadow color
            0,                                                  // z
            "bg");

        btn_ui->addTextureText(
            "resources/Roboto-Regular-100.basof2",                            // font_path
            MathCore::vec2f(0, 0),                                            // pos
            -1,                                                               // z
            shoulder_text,                                                    // text
            ScreenOptions::top_bar_height * 0.5f,                             // size
            -1,                                                               // max_width
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

    void TopBar::addButton(const std::string &text)
    {
        auto btn_ui = ui->addComponentUI(MathCore::vec2f(0, 0), 0, text).get<AppKit::GLEngine::Components::ComponentUI>();
        btns.push_back(btn_ui);

        auto size = screenManager->current_size;

        btn_ui->addRectangle(
            MathCore::vec2f(0),                                 // pos
            MathCore::vec2f(size.width, 32),                    // size
            screenManager->colorPalette.active,                 // color
            MathCore::vec4f(32, 0, 0, 32),                      // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
            screenManager->colorPalette.stroke_thickness,       // stroke thickness
            screenManager->colorPalette.primary_stroke,         // stroke color
            0,                                                  // drop shadow thickness
            MathCore::vec4f(0),                                 // drop shadow color
            0,                                                  // z
            "bg");

        btn_ui->addTextureText(
            "resources/Roboto-Regular-100.basof2",                            // font_path
            MathCore::vec2f(0, 0),                                            // pos
            -1,                                                               // z
            text,                                                             // text
            ScreenOptions::top_bar_height * 0.5f,                             // size
            -1,                                                               // max_width
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

}