#include "InGameDialog.h"
#include "../ScreenManager.h"

namespace ui
{

    void InGameDialog::createAllComponents()
    {
        if (components_created)
            return;

        if (node_ui->getItemByName("main_box").type == AppKit::GLEngine::Components::UIItemNone)
        {
            node_ui->addRectangle(
                MathCore::vec2f(0, 0),                              // pos
                MathCore::vec2f(256, 256),                          // size
                colorFromHex("#000000", 0.4f),                      // color
                MathCore::vec4f(0, 0, 0, 0),                        // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                0,                                                  // stroke thickness
                colorFromHex("#000000", 0.0f),                      // stroke color
                0,                                                  // drop shadow thickness
                MathCore::vec4f(0),                                 // drop shadow color
                0,                                                  // z
                "main_box");
        }
        if (node_ui->getItemByName("avatar_box").type == AppKit::GLEngine::Components::UIItemNone)
        {
            node_ui->addRectangle(
                MathCore::vec2f(0, 0),                              // pos
                MathCore::vec2f(256, 256),                          // size
                colorFromHex("#000000", 0.4f),                      // color
                MathCore::vec4f(0, 0, 0, 0),                        // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                0,                                                  // stroke thickness
                colorFromHex("#000000", 0.0f),                      // stroke color
                0,                                                  // drop shadow thickness
                MathCore::vec4f(0),                                 // drop shadow color
                -10,                                                // z
                "avatar_box");
        }
        if (node_ui->getItemByName("continue_box").type == AppKit::GLEngine::Components::UIItemNone)
        {
            node_ui->addRectangle(
                MathCore::vec2f(0, 0),                              // pos
                MathCore::vec2f(256, 256),                          // size
                colorFromHex("#000000", 0.4f),                      // color
                MathCore::vec4f(0, 0, 0, 0),                        // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                0,                                                  // stroke thickness
                colorFromHex("#000000", 0.0f),                      // stroke color
                0,                                                  // drop shadow thickness
                MathCore::vec4f(0),                                 // drop shadow color
                -10,                                                // z
                "continue_box");
        }

        components_created = true;
    }

    void InGameDialog::releaseAllComponents()
    {
        node_ui->clear();
        components_created = false;
    }

    void InGameDialog::setParent(std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> &parent, ScreenManager *screenManager)
    {
        ITK_ABORT(this->parent != nullptr, "Parent already set");
        this->parent = parent;
        this->screenManager = screenManager;

        node_ui = this->parent->addComponentUI(MathCore::vec2f(0, 0), 0, "button_manager").get<AppKit::GLEngine::Components::ComponentUI>();
        node_ui->getTransform()->skip_traversing = true; // start hidden

        components_created = false;
    }

    void InGameDialog::setProperties(float avatar_size, float continue_button_size, float text_size, float screen_margin, float text_margin)
    {
        this->avatar_size = avatar_size;
        this->continue_button_size = continue_button_size;
        this->text_size = text_size;
        this->screen_margin = screen_margin;
        this->text_margin = text_margin;
    }

    void InGameDialog::blinkAmount(float lerp_factor)
    {
    }

    void InGameDialog::layoutVisibleElements(const MathCore::vec2i &size)
    {
        if (!components_created)
            return;

        // called on resize

        auto main_box = node_ui->getItemByName("main_box").get<AppKit::GLEngine::Components::ComponentRectangle>();
        auto avatar_box = node_ui->getItemByName("avatar_box").get<AppKit::GLEngine::Components::ComponentRectangle>();
        auto continue_box = node_ui->getItemByName("continue_box").get<AppKit::GLEngine::Components::ComponentRectangle>();

        MathCore::vec2f max_box_size = (MathCore::vec2f)size - 2.0f * screen_margin;
        max_box_size = MathCore::OP<MathCore::vec2f>::maximum(0, max_box_size);
        float reserved_height = text_margin * 2.0f;
        max_box_size.y = (reserved_height < max_box_size.y) ? reserved_height : max_box_size.y;

        main_box->setQuad(
            node_ui->resourceMap,
            max_box_size,                                                                                 // size
            screenManager->colorPalette.bg,                                                               // color
            screenManager->colorPalette.button_radius_squared ? MathCore::vec4f(0) : MathCore::vec4f(32), // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                                           // stroke mode
            screenManager->colorPalette.dialog_stroke_thickness,                                          // stroke thickness
            screenManager->colorPalette.bg_stroke,                                                        // stroke color
            0,                                                                                            // drop shadow thickness
            MathCore::vec4f(0),                                                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct                                           // meshUploadMode,
        );


        avatar_box->setQuad(
            node_ui->resourceMap,
            MathCore::vec2f(avatar_size),                                                                                 // size
            screenManager->colorPalette.primary,                                                               // color
            screenManager->colorPalette.button_radius_squared ? MathCore::vec4f(0) : MathCore::vec4f(32), // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                                           // stroke mode
            screenManager->colorPalette.avatar_stroke_thickness,                                          // stroke thickness
            screenManager->colorPalette.primary_stroke,                                                        // stroke color
            0,                                                                                            // drop shadow thickness
            MathCore::vec4f(0),                                                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct                                           // meshUploadMode,
        );


        continue_box->setQuad(
            node_ui->resourceMap,
            MathCore::vec2f(continue_button_size),                                                                                 // size
            screenManager->colorPalette.primary,                                                               // color
            screenManager->colorPalette.button_radius_squared ? MathCore::vec4f(0) : MathCore::vec4f(32), // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                                           // stroke mode
            screenManager->colorPalette.avatar_stroke_thickness,                                          // stroke thickness
            screenManager->colorPalette.primary_stroke,                                                        // stroke color
            0,                                                                                            // drop shadow thickness
            MathCore::vec4f(0),                                                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct                                           // meshUploadMode,
        );
        

    }

    CollisionCore::AABB<MathCore::vec3f> InGameDialog::computeAABB()
    {
        return CollisionCore::AABB<MathCore::vec3f>(MathCore::vec3f(0), MathCore::vec3f(0));
    }

    void InGameDialog::setSpriteAvatars(const std::vector<std::string> &sprite_list)
    {
    }

    int InGameDialog::countLinesForText(const std::string &rich_message)
    {
        return 0;
    }

    void InGameDialog::setMinLineCount(int min_line_count)
    {
        // start dialog with empty lines to reserve space
    }

    void InGameDialog::showDialog(

        DialogAppearModeType appear_mode,

        DialogAvatarSideType side,
        const std::string &avatar,

        DialogTextModeType mode,
        const std::string &rich_message,
        const std::string &rich_continue_char,

        EventCore::Callback<void(const std::string &)> onContinuePressed)
    {
    }

    void InGameDialog::hideDialog(DialogAppearModeType appear_mode)
    {
    }

    void InGameDialog::resetColors()
    {
    }

}
