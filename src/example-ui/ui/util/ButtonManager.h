#pragma once

#include "../common.h"

namespace ui
{

    enum ButtonDirectionEnum
    {
        ButtonDirection_horizontal = 0,
        ButtonDirection_vertical,
    };

    class ButtonManager
    {
        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> parent;

        float button_width, button_height, button_gap, font_size;

    public:
        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> node_ui;
        std::vector<std::shared_ptr<AppKit::GLEngine::Components::ComponentUI>> buttons;
        ScreenManager *screenManager;

        ButtonManager() = default;
        ~ButtonManager() = default;

        void setParent(std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> &parent, ScreenManager *screenManager);

        void setButtonProperties(float button_width, float button_height, float button_gap, float font_size);
        void reserveButtonData(int count);

        void setButtonVisibleCount(int count);

        void setButtonText(int idx, const std::string &text);

        void resetButtonColors();
        void setButtonColor(int idx, const MathCore::vec4f &color, const MathCore::vec4f &stroke_color);

        void layoutVisibleElements(ButtonDirectionEnum direction);

        CollisionCore::AABB<MathCore::vec3f> computeAABB(ButtonDirectionEnum direction);
    };

}
