#include "./ScreenMain.h"
#include "./ScreenManager.h"

namespace ui
{
    std::string ScreenMain::name() const
    {
        return "ScreenMain";
    }

    void ScreenMain::resize(const MathCore::vec2i &size)
    {
        printf("    [ScreenMain] resize %i %i\n", size.width, size.height);
    }

    void ScreenMain::update(Platform::Time *elapsed)
    {
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ScreenMain::initializeTransform(
        AppKit::GLEngine::Engine *engine,
        AppKit::GLEngine::ResourceMap *resourceMap,
        MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
        ui::ScreenManager *screenManager)
    {
        if (uiNode)
            return uiNode;
        printf("    [ScreenMain] initializeTransform\n");
        uiNode = AppKit::GLEngine::Transform::CreateShared("ScreenMain");
        uiNode->skip_traversing = true;

        uiComponent = uiNode->addNewComponent<AppKit::GLEngine::Components::ComponentUI>();
        uiComponent->Initialize(resourceMap);

        float width = 256;

        uiComponent->addRectangle(
            MathCore::vec2f(0, 0),                              // pos
            MathCore::vec2f(width, 64),                         // size
            colorFromHex("#bbbbff", 1.0f),                      // color
            MathCore::vec4f(32),                                // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
            3.0f,                                               // stroke thickness
            colorFromHex("#610081", 1.0f),                      // stroke color
            0,                                                  // drop shadow thickness
            MathCore::vec4f(0),                                 // drop shadow color
            0,                                                  // z
            "bg");

        uiComponent->addTextureText(
            "resources/Roboto-Regular-100.basof2",                            // font_path
            MathCore::vec2f(0, 0),                                            // pos
            -1,                                                               // z
            "Option Nº1!",                                                    // text
            32.0f,                                                            // size
            width,                                                            // max_width
            colorFromHex("#000000", 1.0f),                                    // faceColor
            colorFromHex("#000000", 0.0f),                                    // strokeColor
            MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // strokeOffset
            AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // horizontalAlign
            AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // verticalAlign
            1.0f,                                                             // lineHeight
            AppKit::OpenGL::GLFont2WrapMode_Word,                             // wrapMode
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // firstLineHeightMode
            U' ',                                                             // wordSeparatorChar
            "text");

        return uiNode;
    }

    void ScreenMain::triggerEvent(ui::UIEventEnum event)
    {
        printf("    [ScreenMain] %s\n", UIEventToStr(event));
        if (event == UIEventEnum::UIEvent_ScreenPush)
        {
            uiNode->skip_traversing = false;
        }
        else if (event == UIEventEnum::UIEvent_ScreenPop)
        {
            uiNode->skip_traversing = true;
        }
    }

}