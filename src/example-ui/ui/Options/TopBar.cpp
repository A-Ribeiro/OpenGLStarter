#include "./TopBar.h"
#include "../ScreenManager.h"

namespace ui
{

    void TopBar::initialize(
        std::vector<std::string> buttonNames,
        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent,
        ScreenManager *screenManager)
    {
        selected_button = 0;
        this->screenManager = screenManager;
        ui = uiComponent->addComponentUI(MathCore::vec2f(0, 0), -1, "top_bar").get<AppKit::GLEngine::Components::ComponentUI>();

        addShoulder(0);
        addShoulder(1);

        for (const auto &buttonName : buttonNames)
            addButton(buttonName);

        setButtonColors();
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
                screenManager->colorPalette.button_radius_squared ? MathCore::vec4f(0) : MathCore::vec4f(32, 0, 0, 32),                      // radius
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

        setButtonColors();
    }

    void TopBar::shoulderNext()
    {
        selected_button = MathCore::OP<int>::clamp(selected_button + 1, 0, (int)btns.size() - 1);
        setButtonColors();
    }

    void TopBar::shoulderPrevious()
    {
        selected_button = MathCore::OP<int>::clamp(selected_button - 1, 0, (int)btns.size() - 1);
        setButtonColors();
    }

    const std::string &TopBar::getSelectedButtonName() const
    {
        return btns[selected_button]->getTransform()->getName();
    }

    void TopBar::addShoulder(int side)
    {
        std::string shoulder_name = (side == 0) ? "shoulder_left" : "shoulder_right";
        std::string shoulder_text = (side == 0) ? "L" : "R";
        MathCore::vec4f shoulder_border = (side == 0) ? MathCore::vec4f(32, 32, 32, 32) : MathCore::vec4f(32, 32, 32, 32);

        shoulder_border = screenManager->colorPalette.button_radius_squared ? MathCore::vec4f(0) : shoulder_border;

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
            screenManager->colorPalette.text_disabled,                        // faceColor
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

    void TopBar::setButtonColors()
    {
        for (size_t i = 0; i < btns.size(); ++i)
        {
            auto btn = btns[i];
            auto bg = btn->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
            auto text = btn->getItemByName("text").get<AppKit::GLEngine::Components::ComponentFont>();
            if ((int)i == selected_button)
            {
                bg->setColor(
                    screenManager->colorPalette.primary,        // color
                    screenManager->colorPalette.primary_stroke, // stroke color
                    MathCore::vec4f(0)                          // drop shadow color
                );
                text->setColor(screenManager->colorPalette.text,
                               colorFromHex("#000000", 0.0f));
            }
            else
            {
                bg->setColor(
                    screenManager->colorPalette.disabled,        // color
                    screenManager->colorPalette.disabled_stroke, // stroke color
                    MathCore::vec4f(0)                           // drop shadow color
                );
                text->setColor(screenManager->colorPalette.text_disabled,
                               colorFromHex("#000000", 0.0f));
            }
        }
    }

}