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

        auto bg = uiComponent->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        bg->setQuad(
            uiComponent->resourceMap,
            MathCore::vec2f(text_size.x + ScreenMessageBox::text_margin * 2.0f, text_size.y + ScreenMessageBox::text_margin * 2.0f), // size
            screenManager->colorPalette.primary,                                                                                     // color
            MathCore::vec4f(32, 32, 32, 32),                                                                                         // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                                                                      // stroke mode
            screenManager->colorPalette.stroke_thickness,                                                                            // stroke thickness
            screenManager->colorPalette.primary_stroke,                                                                              // stroke color
            16.0f,                                                                                                                   // drop shadow thickness
            colorFromHex("#2a2a2a3d"),                                                                                               // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct                                                                      // meshUploadMode,
        );
    }

    void ScreenMessageBox::previousButton()
    {
    }
    void ScreenMessageBox::nextButton()
    {
    }
    void ScreenMessageBox::backButton()
    {
    }
    void ScreenMessageBox::selectOption(const std::string &name)
    {
    }
    void ScreenMessageBox::setPrimaryColorAll()
    {
    }

    void ScreenMessageBox::showMessageBox(
        const std::string &rich_message,
        const std::vector<std::string> &options,
        EventCore::Callback<void(const std::string &)> onOptionSelected)
    {
        this->text = rich_message;
        this->onOptionSelected = onOptionSelected;
        printf("Show message box: %s\n", rich_message.c_str());

        layoutElements(screenManager->current_size);

        screenManager->push_screen("ScreenMessageBox");
    }

    const char *ScreenMessageBox::Name = "ScreenMessageBox";
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

        uiNode = AppKit::GLEngine::Transform::CreateShared("ScreenOptions");
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
                                 screenManager->colorPalette.primary,                // color
                                 MathCore::vec4f(32, 32, 32, 32),                    // radius
                                 AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                                 screenManager->colorPalette.stroke_thickness,       // stroke thickness
                                 screenManager->colorPalette.primary_stroke,         // stroke color
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

        return uiNode;
    }

    void ScreenMessageBox::triggerEvent(ui::UIEventEnum event)
    {
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
        else if (event == UIEventEnum::UIEvent_InputActionEnter)
        {
            if (onOptionSelected)
                onOptionSelected("Yes");
        }
        else if (event == UIEventEnum::UIEvent_InputActionBack)
        {
        }
    }
}