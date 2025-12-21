#pragma once

#include "../ScreenOptions.h"

namespace ui
{

    class TopBar
    {
        void addShoulder(int side);
        void addButton(const std::string &text);

        int selected_button;

        void setButtonColors();
    public:
        // std::shared_ptr<AppKit::GLEngine::Transform> uiNode;
        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> ui;
        std::vector<std::shared_ptr<AppKit::GLEngine::Components::ComponentUI>> btns;
        std::vector<std::shared_ptr<AppKit::GLEngine::Components::ComponentUI>> shoulders;

        ScreenManager *screenManager;

        void initialize(
            std::vector<std::string> buttonNames,
            std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent,
            ScreenManager *screenManager);

        void layoutElements(const MathCore::vec2i &size);

        void shoulderNext();
        void shoulderPrevious();

        const std::string &getSelectedButtonName() const;
    };

}