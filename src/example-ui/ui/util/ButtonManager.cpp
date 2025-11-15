#include "ButtonManager.h"
#include "../ScreenManager.h"

namespace ui
{

    void ButtonManager::setParent(std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> &parent, ScreenManager *screenManager)
    {
        ITK_ABORT(this->parent != nullptr, "Parent already set");
        this->parent = parent;
        this->screenManager = screenManager;

        node_ui = this->parent->addComponentUI(MathCore::vec2f(0, 0), 0, "button_manager").get<AppKit::GLEngine::Components::ComponentUI>();
        node_ui->getTransform()->skip_traversing = true; // start hidden

        visible_count = 0;
    }

    void ButtonManager::setButtonProperties(float button_width, float button_height, float button_gap, float font_size)
    {
        this->button_width = button_width;
        this->button_height = button_height;
        this->button_gap = button_gap;
        this->font_size = font_size;
    }

    void ButtonManager::reserveButtonData(int count)
    {
        while (buttons.size() < count)
        {
            auto button = this->node_ui->addComponentUI(MathCore::vec2f(0, 0), 0, "button").get<AppKit::GLEngine::Components::ComponentUI>();
            button->getTransform()->skip_traversing = true; // start hidden

            button->addRectangle(
                MathCore::vec2f(0, 0),                              // pos
                MathCore::vec2f(button_width, button_height),       // size
                screenManager->colorPalette.primary,                // color
                screenManager->colorPalette.button_radius,          // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                screenManager->colorPalette.stroke_thickness,       // stroke thickness
                screenManager->colorPalette.primary_stroke,         // stroke color
                0,                                                  // drop shadow thickness
                MathCore::vec4f(0),                                 // drop shadow color
                0,                                                  // z
                "bg");

            button->addTextureText(
                "resources/Roboto-Regular-100.basof2",                            // font_path
                MathCore::vec2f(0, 0),                                            // pos
                -1,                                                               // z
                "dummy text",                                                     // text
                font_size,                                                        // size
                button_width,                                                     // max_width
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

            buttons.push_back(button);
        }
    }

    void ButtonManager::setButtonVisibleCount(int count)
    {
        reserveButtonData(count);
        for (int i = 0; i < buttons.size(); i++)
            buttons[i]->getTransform()->skip_traversing = (i >= count);
        node_ui->getTransform()->skip_traversing = count == 0;
        visible_count = count;
    }

    const std::string &ButtonManager::getButtonText(int idx) const
    {
        ITK_ABORT(idx < 0 || idx >= buttons.size(), "Index out of range");
        auto text_component = buttons[idx]->getItemByName("text").get<AppKit::GLEngine::Components::ComponentFont>();
        return text_component->getText();
    }

    void ButtonManager::setButtonText(int idx, const std::string &text)
    {
        ITK_ABORT(idx < 0 || idx >= buttons.size(), "Index out of range");

        auto text_component = buttons[idx]->getItemByName("text").get<AppKit::GLEngine::Components::ComponentFont>();
        auto engine = AppKit::GLEngine::Engine::Instance();

        if (text_component->getText() == text)
            return;

        text_component->setText( //
            node_ui->resourceMap,
            "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
            // 0 = texture, > 0 = polygon
            0,                                                                // float polygon_size,
            0,                                                                // float polygon_distance_tolerance,
            nullptr,                                                          // Platform::ThreadPool *polygon_threadPool,
            engine->sRGBCapable,                                              // bool is_srgb,
            text,                                                             // const std::string &text,
            font_size,                                                        // float size, ///< current state of the font size
            button_width,                                                     // float max_width,
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
    }

    void ButtonManager::resetButtonColors()
    {
        for (auto &button : buttons)
        {
            auto bg = button->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
            bg->setColor(
                screenManager->colorPalette.primary,
                screenManager->colorPalette.primary_stroke,
                colorFromHex("#000000", 0.0f));

            auto text = button->getItemByName("text").get<AppKit::GLEngine::Components::ComponentFont>();
            text->setColor(
                screenManager->colorPalette.text,
                colorFromHex("#000000", 0.0f));
        }
    }

    void ButtonManager::setButtonColor(int idx, const MathCore::vec4f &color, const MathCore::vec4f &stroke_color)
    {
        ITK_ABORT(idx < 0 || idx >= buttons.size(), "Index out of range");

        auto bg = buttons[idx]->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        bg->setColor(
            color,
            stroke_color,
            colorFromHex("#000000", 0.0f));
    }

    void ButtonManager::layoutVisibleElements(ButtonDirectionEnum direction)
    {
        int visible_count = 0;
        for (int i = 0; i < buttons.size(); i++)
        {
            if (!buttons[i]->getTransform()->skip_traversing)
                visible_count++;
            else
                break;
        }

        if (visible_count == 0)
            return;

        float total_width = button_width * visible_count + button_gap * (visible_count - 1);
        float total_height = button_height * visible_count + button_gap * (visible_count - 1);

        MathCore::vec2f start_pos;
        if (direction == ButtonDirection_horizontal)
            start_pos = MathCore::vec2f(-total_width * 0.5f + button_width * 0.5f, 0);
        else
            start_pos = MathCore::vec2f(0, total_height * 0.5f - button_height * 0.5f);

        for (int i = 0; i < visible_count; i++)
        {
            MathCore::vec2f pos;
            if (direction == ButtonDirection_horizontal)
                pos = start_pos + MathCore::vec2f((button_width + button_gap) * (float)i, 0);
            else
                pos = start_pos + MathCore::vec2f(0, -(button_height + button_gap) * (float)i);

            buttons[i]->getTransform()->setLocalPosition(MathCore::vec3f(pos, 0.0f));
        }
    }

    CollisionCore::AABB<MathCore::vec3f> ButtonManager::computeAABB(
        ButtonDirectionEnum direction)
    {
        int visible_count = 0;
        for (int i = 0; i < buttons.size(); i++)
        {
            if (!buttons[i]->getTransform()->skip_traversing)
                visible_count++;
            else
                break;
        }

        if (visible_count == 0)
            return CollisionCore::AABB<MathCore::vec3f>();

        float total_width = button_width * visible_count + button_gap * (visible_count - 1);
        float total_height = button_height * visible_count + button_gap * (visible_count - 1);

        if (direction == ButtonDirection_horizontal)
            return CollisionCore::AABB<MathCore::vec3f>(
                MathCore::vec3f(-total_width * 0.5f, -button_height * 0.5f, 0.0f),
                MathCore::vec3f(total_width * 0.5f, button_height * 0.5f, 0.0f));
        else
            return CollisionCore::AABB<MathCore::vec3f>(
                MathCore::vec3f(-button_width * 0.5f, -total_height * 0.5f, 0.0f),
                MathCore::vec3f(button_width * 0.5f, total_height * 0.5f, 0.0f));
    }

}
