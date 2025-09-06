#include "./ScreenOptions.h"
#include "./ScreenManager.h"
#include "./Options/TopBar.h"
#include "./Options/OptionSet.h"

void save_options();

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
            MathCore::vec4f(32, 32, 32, 32),                              // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,           // stroke mode
            screenManager->colorPalette.stroke_thickness,                 // stroke thickness
            screenManager->colorPalette.primary_stroke,                   // stroke color
            0,                                                            // drop shadow thickness
            MathCore::vec4f(0),                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct           // meshUploadMode,
        );

        if (topBar)
            topBar->layoutElements(size);

        for (auto &item : optionMap)
            item.second->layoutElements(size);
    }

    void ScreenOptions::activeCurrentTab()
    {
        auto tab_name = topBar->getSelectedButtonName();
        for (auto &option : this->optionMap)
        {
            if (option.first == tab_name)
                option.second->show();
            else
                option.second->hide();
        }
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

        float speed = osciloscope_normal_hz;

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

        if (!change_screen || increase_speed_for_secs_and_trigger_action > 0.0f)
        {

            const float _360_pi = MathCore::CONSTANT<float>::PI * 2.0f;
            osciloscope = MathCore::OP<float>::fmod(osciloscope + elapsed->unscaledDeltaTime * speed * _360_pi, _360_pi);
            float sin = MathCore::OP<float>::sin(osciloscope) * 0.25f + 0.75f;

            // auto rect = uiComponent->items[selected_button].get<AppKit::GLEngine::Components::ComponentUI>()->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
            // rect->setColor(
            //     screenManager->colorPalette.lrp_active(sin),
            //     screenManager->colorPalette.lrp_active_stroke(sin),
            //     0);
            auto tab_name = topBar->getSelectedButtonName();
            optionMap[tab_name]->update(elapsed, osciloscope, sin);
        }
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
        auto mask = uiComponent->addRectangle(
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
                                   "bg")
                        .get<AppKit::GLEngine::Components::ComponentRectangle>();

        topBar = std::make_shared<TopBar>();
        topBar->initialize({"Control",
                            "Audio",
                            "Video",
                            "Extra"},
                           uiComponent, screenManager);

        // Control
        {
            std::string btn_set_name = "Control";

            auto optSet = STL_Tools::make_unique<OptionSet>();
            optSet->initialize(btn_set_name, uiComponent, screenManager, mask);

            optSet->addOption("Input", {"Steam 1", "Keyboard", "Steam 1 + Keyboard"}, "Steam 1 + Keyboard");
            optSet->addOption("Movement", {"Fluid", "Legacy"}, "Fluid");

            optionMap[btn_set_name] = std::move(optSet);
        }
        // Audio
        {
            std::string btn_set_name = "Audio";

            auto optSet = STL_Tools::make_unique<OptionSet>();
            optSet->initialize(btn_set_name, uiComponent, screenManager, mask);

            optSet->addOption("Effects Volume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100");
            optSet->addOption("Music Volume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100");

            optionMap[btn_set_name] = std::move(optSet);
        }
        // Video
        {
            std::string btn_set_name = "Video";

            auto optSet = STL_Tools::make_unique<OptionSet>();
            optSet->initialize(btn_set_name, uiComponent, screenManager, mask);

            optSet->addOption("Window Mode", {"Window", "Borderless", "Fullscreen"}, "Borderless");
            optSet->addOption("Resolution", {"2560x1440", "1920x1080", "1280x720", "854x480"}, "1920x1080");
            optSet->addOption("Aspect", {"16:9", "16:10"}, "16:9");
            optSet->addOption("Anti-Aliasing", {"MSAA", "OFF"}, "MSAA");
            optSet->addOption("VSync", {"ON", "OFF"}, "ON");

            optionMap[btn_set_name] = std::move(optSet);
        }
        // Extra
        {
            std::string btn_set_name = "Extra";

            auto optSet = STL_Tools::make_unique<OptionSet>();
            optSet->initialize(btn_set_name, uiComponent, screenManager, mask);

            optSet->addOption("Language", {"English", "Português (BR)"}, "English");
            optSet->addOption("ColorScheme", {"Purple", "Orange", "Green", "Blue"}, "Purple");
            optSet->addOption("MeshCrusher", {"ON", "OFF"}, "ON");
            optSet->addOption("Particles", {"Low", "Medium", "High", "Ultra"}, "High");
            optSet->addOption("On Game Stats", {"FPS", "OFF"}, "OFF");

            optionMap[btn_set_name] = std::move(optSet);
        }

        activeCurrentTab();

        // static constexpr float item_height = 64.0f;
        // static constexpr float item_margin = 16.0f;
        // float item_width = valid_size.width * 0.5f - item_margin * 2.0f;

        // MathCore::vec2f center = MathCore::vec2f(0, -top_bar_height * 0.5f);
        // auto rect = uiComponent->addRectangle(
        //                            center,                                             // pos
        //                            MathCore::vec2f(valid_size.x, item_height),         // size
        //                            screenManager->colorPalette.active,                 // color
        //                            MathCore::vec4f(32, 32, 32, 32),                    // radius
        //                            AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
        //                            screenManager->colorPalette.stroke_thickness,       // stroke thickness
        //                            screenManager->colorPalette.active_stroke,          // stroke color
        //                            0,                                                  // drop shadow thickness
        //                            MathCore::vec4f(0),                                 // drop shadow color
        //                            -1,                                                 // z
        //                            "selection")
        //                 .get<AppKit::GLEngine::Components::ComponentRectangle>();
        // rect->setMask(uiComponent->resourceMap, screenManager->camera, mask);

        // auto option_text_center = center + MathCore::vec2f(-valid_size.width * 0.25f, 0);
        // auto txt = uiComponent->addTextureText(
        //                           "resources/Roboto-Regular-100.basof2",                            // font_path
        //                           option_text_center,                                               // pos
        //                           -3,                                                               // z
        //                           "VSync",                                                          // text
        //                           item_height * 0.5f,                                               // size
        //                           -1,                                                               // max_width
        //                           screenManager->colorPalette.text,                                 // faceColor
        //                           colorFromHex("#000000", 0.0f),                                    // strokeColor
        //                           MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // strokeOffset
        //                           AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // horizontalAlign
        //                           AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // verticalAlign
        //                           1.0f,                                                             // lineHeight
        //                           AppKit::OpenGL::GLFont2WrapMode_Word,                             // wrapMode
        //                           AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // firstLineHeightMode
        //                           U' ',                                                             // wordSeparatorChar
        //                           "option1-text")
        //                .get<AppKit::GLEngine::Components::ComponentFont>();
        // txt->setMask(uiComponent->resourceMap, screenManager->camera, mask);

        // // auto option_center = center + MathCore::vec2f(item_margin + item_width * 0.5f, 0);
        // auto option_center = center + MathCore::vec2f(valid_size.width * 0.25f, 0);
        // rect = uiComponent->addRectangle(
        //                       option_center,                                                                         // pos
        //                       MathCore::vec2f(item_width, item_height * 0.6f),                                       // size
        //                       (MathCore::vec4f)screenManager->colorPalette.primary * MathCore::vec4f(1, 1, 1, 0.4f), // color
        //                       MathCore::vec4f(16),                                                                   // radius
        //                       AppKit::GLEngine::Components::StrokeModeGrowInside,                                    // stroke mode
        //                       screenManager->colorPalette.stroke_thickness,                                          // stroke thickness
        //                       screenManager->colorPalette.primary_stroke,                                            // stroke color
        //                       0,                                                                                     // drop shadow thickness
        //                       MathCore::vec4f(0),                                                                    // drop shadow color
        //                       -2,                                                                                    // z
        //                       "selection")
        //            .get<AppKit::GLEngine::Components::ComponentRectangle>();
        // rect->setMask(uiComponent->resourceMap, screenManager->camera, mask);

        // txt = uiComponent->addTextureText(
        //                      "resources/Roboto-Regular-100.basof2",                            // font_path
        //                      option_center,                                                    // pos
        //                      -3,                                                               // z
        //                      "ON",                                                             // text
        //                      item_height * 0.5f,                                               // size
        //                      -1,                                                               // max_width
        //                      screenManager->colorPalette.text,                                 // faceColor
        //                      colorFromHex("#000000", 0.0f),                                    // strokeColor
        //                      MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // strokeOffset
        //                      AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // horizontalAlign
        //                      AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // verticalAlign
        //                      1.0f,                                                             // lineHeight
        //                      AppKit::OpenGL::GLFont2WrapMode_Word,                             // wrapMode
        //                      AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // firstLineHeightMode
        //                      U' ',                                                             // wordSeparatorChar
        //                      "option1-sel")
        //           .get<AppKit::GLEngine::Components::ComponentFont>();
        // txt->setMask(uiComponent->resourceMap, screenManager->camera, mask);

        // txt = uiComponent->addTextureText(
        //                      "resources/Roboto-Regular-100.basof2",                                // font_path
        //                      option_center + MathCore::vec2f(-item_width * 0.5f + item_margin, 0), // pos
        //                      -3,                                                                   // z
        //                      "<",                                                                  // text
        //                      item_height * 0.5f,                                                   // size
        //                      -1,                                                                   // max_width
        //                      screenManager->colorPalette.text,                                     // faceColor
        //                      colorFromHex("#000000", 0.0f),                                        // strokeColor
        //                      MathCore::vec3f(0.0f, 0.0f, -0.02f),                                  // strokeOffset
        //                      AppKit::OpenGL::GLFont2HorizontalAlign_left,                          // horizontalAlign
        //                      AppKit::OpenGL::GLFont2VerticalAlign_middle,                          // verticalAlign
        //                      1.0f,                                                                 // lineHeight
        //                      AppKit::OpenGL::GLFont2WrapMode_Word,                                 // wrapMode
        //                      AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,     // firstLineHeightMode
        //                      U' ',                                                                 // wordSeparatorChar
        //                      "option1-<")
        //           .get<AppKit::GLEngine::Components::ComponentFont>();
        // txt->setMask(uiComponent->resourceMap, screenManager->camera, mask);

        // txt = uiComponent->addTextureText(
        //                      "resources/Roboto-Regular-100.basof2",                               // font_path
        //                      option_center + MathCore::vec2f(item_width * 0.5f - item_margin, 0), // pos
        //                      -3,                                                                  // z
        //                      ">",                                                                 // text
        //                      item_height * 0.5f,                                                  // size
        //                      -1,                                                                  // max_width
        //                      screenManager->colorPalette.text,                                    // faceColor
        //                      colorFromHex("#000000", 0.0f),                                       // strokeColor
        //                      MathCore::vec3f(0.0f, 0.0f, -0.02f),                                 // strokeOffset
        //                      AppKit::OpenGL::GLFont2HorizontalAlign_right,                        // horizontalAlign
        //                      AppKit::OpenGL::GLFont2VerticalAlign_middle,                         // verticalAlign
        //                      1.0f,                                                                // lineHeight
        //                      AppKit::OpenGL::GLFont2WrapMode_Word,                                // wrapMode
        //                      AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,    // firstLineHeightMode
        //                      U' ',                                                                // wordSeparatorChar
        //                      "option1->")
        //           .get<AppKit::GLEngine::Components::ComponentFont>();
        // txt->setMask(uiComponent->resourceMap, screenManager->camera, mask);

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
                // increase_speed_for_secs_and_trigger_action = 0.5f;
                // change_screen = true;
            }
            else if (event == UIEventEnum::UIEvent_InputDown)
            {
                auto tab_name = topBar->getSelectedButtonName();
                optionMap[tab_name]->downButton();
            }
            else if (event == UIEventEnum::UIEvent_InputUp)
            {
                auto tab_name = topBar->getSelectedButtonName();
                optionMap[tab_name]->upButton();
            }
            else if (event == UIEventEnum::UIEvent_InputLeft)
            {
                auto tab_name = topBar->getSelectedButtonName();
                optionMap[tab_name]->leftButton();
            }
            else if (event == UIEventEnum::UIEvent_InputRight)
            {
                auto tab_name = topBar->getSelectedButtonName();
                optionMap[tab_name]->rightButton();
            }
            else if (event == UIEventEnum::UIEvent_InputActionBack)
            {
                // backButton();
                save_options();
                screenManager->open_screen("ScreenMain");
            }
            else if (event == UIEventEnum::UIEvent_InputShoulderRight)
            {
                topBar->shoulderNext();
                activeCurrentTab();
            }
            else if (event == UIEventEnum::UIEvent_InputShoulderLeft)
            {
                topBar->shoulderPrevious();
                activeCurrentTab();
            }
        }
    }

}