#pragma once

#include "../ScreenOptions.h"

namespace ui
{

    class OptionSet
    {
        void set_selected_rect_pos();
        // float get_selection_percentagem_related_to_valid_area();
        void set_selection_percentagem_related_to_valid_area();
    public:
        struct ItemDefinition
        {
            std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> ui;

            std::string option;
            std::vector<std::string> choices;
            std::string selected;

            int selected_index;
        };

        // std::shared_ptr<AppKit::GLEngine::Transform> uiNode;
        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> ui;

        std::shared_ptr<AppKit::GLEngine::Components::ComponentRectangle> mask;
        std::shared_ptr<AppKit::GLEngine::Components::ComponentRectangle> selection_rect;

        ScreenManager *screenManager;

        std::vector<ItemDefinition> items;
        int selected_item_index;

        void initialize(
            const std::string &buttonSetName,
            std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent,
            ScreenManager *screenManager,
            std::shared_ptr<AppKit::GLEngine::Components::ComponentRectangle> &mask);

        void layoutElements(const MathCore::vec2i &size);

        void leftButton();
        void rightButton();

        void upButton();
        void downButton();

        void show();
        void hide();

        void addOption(const std::string &option, const std::vector<std::string> &choices, const std::string &selected);

        void update(Platform::Time *elapsed, float osciloscope, float sin);
    };
}