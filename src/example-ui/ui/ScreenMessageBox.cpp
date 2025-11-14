#include "./ScreenMessageBox.h"
#include "./ScreenManager.h"

namespace ui
{
    void ScreenMessageBox::layoutElements(const MathCore::vec2i &size)
    {
        auto max_size = MathCore::vec2f(size.width - ScreenMessageBox::screen_margin * 2.0f,
                                        size.height - ScreenMessageBox::screen_margin * 2.0f);

        auto fade = uiComponent->getItemByName("fade").get<AppKit::GLEngine::Components::ComponentRectangle>();
        fade->setQuad(
            uiComponent->resourceMap,
            MathCore::vec2f(size.x, size.y),                    // size
            colorFromHex("#000000", 0.4f),                      // color
            MathCore::vec4f(0, 0, 0, 0),                        // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
            0,                                                  // stroke thickness
            colorFromHex("#000000", 0.0f),                      // stroke color
            0,                                                  // drop shadow thickness
            MathCore::vec4f(0),                                 // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct // meshUploadMode,
        );

        auto text = uiComponent->getItemByName("text").get<AppKit::GLEngine::Components::ComponentFont>();
        auto engine = AppKit::GLEngine::Engine::Instance();

        float text_max_width = MathCore::OP<float>::maximum(max_size.width - ScreenMessageBox::text_margin * 2.0f, 0.0f);

        text->setText( //
            uiComponent->resourceMap,
            "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
            // 0 = texture, > 0 = polygon
            0,                                                                // float polygon_size,
            0,                                                                // float polygon_distance_tolerance,
            nullptr,                                                          // Platform::ThreadPool *polygon_threadPool,
            engine->sRGBCapable,                                              // bool is_srgb,
            this->text,                                                       // const std::string &text,
            ScreenMessageBox::text_size,                                      // float size, ///< current state of the font size
            text_max_width,                                                   // float max_width,
            screenManager->colorPalette.text,                                 // const MathCore::vec4f &faceColor,   ///< current state of the face color // .a == 0 turn off the drawing
            colorFromHex("#000000", 0.0f),                                    // const MathCore::vec4f &strokeColor, ///< current state of the stroke color
            MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // const MathCore::vec3f &strokeOffset,
            AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
            AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
            1.0f,                                                             // float lineHeight,
            AppKit::OpenGL::GLFont2WrapMode_Word,                             // AppKit::OpenGL::GLFont2WrapMode wrapMode,
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
            U' ',                                                             // char32_t wordSeparatorChar,
            AppKit::GLEngine::Components::MeshUploadMode_Direct               // MeshUploadMode meshUploadMode
        );

        CollisionCore::AABB<MathCore::vec3f> text_aabb = text->computeBox(
            uiComponent->resourceMap,
            "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
            // 0 = texture, > 0 = polygon
            0,                                                                // float polygon_size,
            0,                                                                // float polygon_distance_tolerance,
            nullptr,                                                          // Platform::ThreadPool *polygon_threadPool,
            engine->sRGBCapable,                                              // bool is_srgb,
            this->text,                                                       // const std::string &text,
            ScreenMessageBox::text_size,                                      // float size, ///< current state of the font size
            text_max_width,                                                   // float max_width,
            AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
            AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
            1.0f,                                                             // float lineHeight,
            AppKit::OpenGL::GLFont2WrapMode_Word,                             // AppKit::OpenGL::GLFont2WrapMode wrapMode,
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
            U' '                                                              // char32_t wordSeparatorChar
        );
        MathCore::vec3f text_size = text_aabb.max_box - text_aabb.min_box;

        CollisionCore::AABB<MathCore::vec3f> button_aabb = buttonManager.computeAABB(ButtonDirection_horizontal);
        MathCore::vec3f button_size = button_aabb.max_box - button_aabb.min_box;

        MathCore::vec2f box_bg_size = MathCore::vec2f(
            MathCore::OP<float>::maximum(button_size.x, text_size.x) + ScreenMessageBox::text_margin * 2.0f,
            text_size.y + button_size.y + ScreenMessageBox::text_margin * 3.0f);

        auto bg = uiComponent->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        bg->setQuad(
            uiComponent->resourceMap,
            box_bg_size,                                        // size
            screenManager->colorPalette.bg,                     // color
            MathCore::vec4f(32, 32, 32, 32),                    // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
            0,                                                  // stroke thickness
            colorFromHex("#000000", 0.0f),                      // stroke color
            16.0f,                                              // drop shadow thickness
            colorFromHex("#2a2a2a3d"),                          // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct // meshUploadMode,
        );

        buttonManager.layoutVisibleElements(ButtonDirection_horizontal);

        text->getTransform()->setLocalPosition(
            MathCore::vec3f(0, box_bg_size.y * 0.5f - ScreenMessageBox::text_margin - text_size.y * 0.5f, -102));

        buttonManager.node_ui->getTransform()->setLocalPosition(
            MathCore::vec3f(0, -box_bg_size.y * 0.5f + ScreenMessageBox::text_margin + button_size.y * 0.5f, -103));
    }

    void ScreenMessageBox::previousButton()
    {
        if (osciloscopeIsLocked())
            return;
        selected_button = MathCore::OP<int>::clamp(selected_button - 1, 0, (int)buttonManager.visible_count - 1);
        setPrimaryColorAll();
    }
    void ScreenMessageBox::nextButton()
    {
        if (osciloscopeIsLocked())
            return;
        selected_button = MathCore::OP<int>::clamp(selected_button + 1, 0, (int)buttonManager.visible_count - 1);
        setPrimaryColorAll();
    }
    void ScreenMessageBox::backButton()
    {
        if (osciloscopeIsLocked())
            return;
        selected_button = (int)buttonManager.visible_count - 1;
        setPrimaryColorAll();
    }
    void ScreenMessageBox::selectOption(const std::string &name)
    {
        printf("Selected name: %s\n", name.c_str());
        setPrimaryColorAll();
        // if (name == "Exit Game")
        //     AppKit::GLEngine::Engine::Instance()->app->exitApp();
        // else if (name == "Options")
        //     screenManager->open_screen("ScreenOptions");
        // else
        //     screenManager->open_screen("ScreenMain");

        if (onOptionSelected != nullptr)
            onOptionSelected(name);
    }
    void ScreenMessageBox::setPrimaryColorAll()
    {
        for (int i = 0; i < buttonManager.visible_count; i++)
        {
            buttonManager.setButtonColor(
                i,
                screenManager->colorPalette.primary,
                screenManager->colorPalette.primary_stroke);
        }
    }

    void ScreenMessageBox::showMessageBox(
        const std::string &rich_message,
        const std::vector<std::string> &options,
        const std::string &init_selected,
        EventCore::Callback<void(const std::string &)> onOptionSelected)
    {
        this->text = rich_message;
        this->onOptionSelected = onOptionSelected;
        printf("Show message box: %s\n", rich_message.c_str());

        buttonManager.setButtonVisibleCount((int)options.size());
        selected_button = 0;
        for (int i = 0; i < (int)options.size(); i++)
        {
            if (options[i] == init_selected)
                selected_button = i;
            buttonManager.setButtonText(i, options[i]);
        }

        layoutElements(screenManager->current_size);

        screenManager->push_screen("ScreenMessageBox");
    }

    const char *ScreenMessageBox::Name = "ScreenMessageBox";

    void ScreenMessageBox::onOsciloscopeAction()
    {
        printf("Action at selection end...");
        // selectOption(uiComponent->items[selected_button].transform->getName());
        selectOption(buttonManager.getButtonText(selected_button));
    }

    void ScreenMessageBox::onOsciloscopeSinLerp(Platform::Time *elapsed, float osciloscope, float sin)
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

    ScreenMessageBox::ScreenMessageBox() : OsciloscopeWithTrigger(
                                               ScreenMessageBox::osciloscope_normal_hz,
                                               ScreenMessageBox::osciloscope_selected_hz,
                                               ScreenMessageBox::osciloscope_countdown_trigger_secs)
    {
        selected_button = 0;
        screenManager = nullptr;
    }

    std::string ScreenMessageBox::name() const
    {
        return Name;
    }
    void ScreenMessageBox::resize(const MathCore::vec2i &size)
    {
        printf("    [ScreenMessageBox] resize %i %i\n", size.width, size.height);
        layoutElements(size);
    }
    void ScreenMessageBox::update(Platform::Time *elapsed)
    {
        if (buttonManager.visible_count == 0)
            return;

        osciloscopeUpdate(elapsed);
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ScreenMessageBox::initializeTransform(
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

        uiNode = AppKit::GLEngine::Transform::CreateShared(name());
        uiNode->skip_traversing = true;

        uiComponent = uiNode->addNewComponent<AppKit::GLEngine::Components::ComponentUI>();
        uiComponent->Initialize(resourceMap);

        auto max_size = MathCore::vec2f(size.width - ScreenMessageBox::screen_margin * 2.0f,
                                        size.height - ScreenMessageBox::screen_margin * 2.0f);

        auto fade = uiComponent->addRectangle(
                                   MathCore::vec2f(0, 0),                              // pos
                                   MathCore::vec2f(size.x, size.y),                    // size
                                   colorFromHex("#000000", 0.4f),                      // color
                                   MathCore::vec4f(0, 0, 0, 0),                        // radius
                                   AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                                   0,                                                  // stroke thickness
                                   colorFromHex("#000000", 0.0f),                      // stroke color
                                   0,                                                  // drop shadow thickness
                                   MathCore::vec4f(0),                                 // drop shadow color
                                   -100,                                               // z
                                   "fade")
                        .get<AppKit::GLEngine::Components::ComponentRectangle>();
        auto bg = uiComponent->addRectangle(
                                 MathCore::vec2f(0, 0),                              // pos
                                 MathCore::vec2f(max_size.x, max_size.y),            // size
                                 screenManager->colorPalette.bg,                     // color
                                 MathCore::vec4f(32, 32, 32, 32),                    // radius
                                 AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                                 0,                                                  // stroke thickness
                                 colorFromHex("#000000", 0.0f),                      // stroke color
                                 0,                                                  // drop shadow thickness
                                 MathCore::vec4f(0),                                 // drop shadow color
                                 -101,                                               // z
                                 "bg")
                      .get<AppKit::GLEngine::Components::ComponentRectangle>();

        auto txt = uiComponent->addTextureText(
                                  "resources/Roboto-Regular-100.basof2",                            // font_path
                                  MathCore::vec2f(0, 0),                                            // pos
                                  -102,                                                             // z
                                  "dummy-text",                                                     // text
                                  ScreenMessageBox::text_size,                                      // size
                                  max_size.width,                                                   // max_width
                                  screenManager->colorPalette.text,                                 // faceColor
                                  colorFromHex("#000000", 0.0f),                                    // strokeColor
                                  MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // strokeOffset
                                  AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // horizontalAlign
                                  AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // verticalAlign
                                  1.0f,                                                             // lineHeight
                                  AppKit::OpenGL::GLFont2WrapMode_Word,                             // wrapMode
                                  AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // firstLineHeightMode
                                  U' ',                                                             // wordSeparatorChar
                                  "text")
                       .get<AppKit::GLEngine::Components::ComponentFont>();

        buttonManager.setParent(uiComponent, screenManager);
        buttonManager.setButtonProperties(
            ScreenMessageBox::button_width,
            ScreenMessageBox::button_height,
            ScreenMessageBox::button_gap,
            ScreenMessageBox::button_text_size);
        buttonManager.reserveButtonData(2);

        return uiNode;
    }

    void ScreenMessageBox::triggerEvent(ui::UIEventEnum event)
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
        else if (!osciloscopeIsLocked())
        {
            if (event == UIEventEnum::UIEvent_InputActionEnter)
            {
                osciloscopeTriggerAction();
            }
            else if (event == UIEventEnum::UIEvent_InputRight)
            {
                nextButton();
            }
            else if (event == UIEventEnum::UIEvent_InputLeft)
            {
                previousButton();
            }
            else if (event == UIEventEnum::UIEvent_InputActionBack)
            {
                backButton();
            }
        }
    }

    void ScreenMessageBox::updateColorPalette()
    {
        buttonManager.resetButtonColors();

        auto bg = uiComponent->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        bg->setColor(
            screenManager->colorPalette.primary,
            screenManager->colorPalette.primary_stroke,
            colorFromHex("#000000", 0.0f));

        auto text = uiComponent->getItemByName("text").get<AppKit::GLEngine::Components::ComponentFont>();
        text->setColor(
            screenManager->colorPalette.text,
            colorFromHex("#000000", 0.0f));
    }
}