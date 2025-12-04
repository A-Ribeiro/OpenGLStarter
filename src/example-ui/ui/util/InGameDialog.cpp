#include "InGameDialog.h"
#include "../ScreenManager.h"

namespace ui
{

    void InGameDialog::createAllComponents()
    {
        if (components_created)
            return;

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

        node_ui->addTextureText(
            "resources/Roboto-Regular-100.basof2",                            // font_path
            MathCore::vec2f(0, 0),                                            // pos
            -5,                                                               // z
            this->rich_message,                                               // text
            text_size,                                                        // size
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
            "main_box_text");

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

        min_line_count = 0;
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

        auto main_box_text = node_ui->getItemByName("main_box_text").get<AppKit::GLEngine::Components::ComponentFont>();

        MathCore::vec2f max_box_size = (MathCore::vec2f)size - 2.0f * screen_margin;
        max_box_size = MathCore::OP<MathCore::vec2f>::maximum(0, max_box_size);
        float reserved_height = text_margin * 2.0f;
        max_box_size.y = (reserved_height < max_box_size.y) ? reserved_height : max_box_size.y;

        // set text
        auto engine = AppKit::GLEngine::Engine::Instance();
        {
            float text_max_width = MathCore::OP<float>::maximum(max_box_size.x - text_margin * 2.0f, 0.0f);

            main_box_text->setText( //
                node_ui->resourceMap,
                "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
                // 0 = texture, > 0 = polygon
                0,                                                                // float polygon_size,
                0,                                                                // float polygon_distance_tolerance,
                nullptr,                                                          // Platform::ThreadPool *polygon_threadPool,
                engine->sRGBCapable,                                              // bool is_srgb,
                this->rich_message,                                               // const std::string &text,
                text_size,                                                        // float size, ///< current state of the font size
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
        }

        auto fontResource = node_ui->resourceMap->getTextureFont("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);

        auto main_box_text_box = main_box_text->currentBox();
        float size_text_y = main_box_text_box.max_box.y - main_box_text_box.min_box.y;

        // set min_line_count
        size_text_y = MathCore::OP<float>::maximum(size_text_y, min_line_count * fontResource->fontBuilder->glFont2.new_line_height * (text_size / fontResource->fontBuilder->glFont2.size));

        max_box_size.y += MathCore::OP<float>::maximum(0.0f, size_text_y);

        main_box->setQuad(
            node_ui->resourceMap,
            max_box_size,                                                                                 // size
            screenManager->colorPalette.bg,                                                               // color
            screenManager->colorPalette.button_radius_squared ? MathCore::vec4f(0) : MathCore::vec4f(16), // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                                           // stroke mode
            screenManager->colorPalette.dialog_stroke_thickness,                                          // stroke thickness
            screenManager->colorPalette.bg_stroke,                                                        // stroke color
            0,                                                                                            // drop shadow thickness
            MathCore::vec4f(0),                                                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct                                           // meshUploadMode,
        );

        avatar_box->setQuad(
            node_ui->resourceMap,
            MathCore::vec2f(avatar_size),                                                                 // size
            screenManager->colorPalette.primary,                                                          // color
            screenManager->colorPalette.button_radius_squared ? MathCore::vec4f(0) : MathCore::vec4f(16), // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                                           // stroke mode
            screenManager->colorPalette.avatar_stroke_thickness,                                          // stroke thickness
            screenManager->colorPalette.primary_stroke,                                                   // stroke color
            0,                                                                                            // drop shadow thickness
            MathCore::vec4f(0),                                                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct                                           // meshUploadMode,
        );

        continue_box->setQuad(
            node_ui->resourceMap,
            MathCore::vec2f(continue_button_size),                                                        // size
            screenManager->colorPalette.primary,                                                          // color
            screenManager->colorPalette.button_radius_squared ? MathCore::vec4f(0) : MathCore::vec4f(16), // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,                                           // stroke mode
            screenManager->colorPalette.dialog_stroke_thickness,                                          // stroke thickness
            screenManager->colorPalette.primary_stroke,                                                   // stroke color
            0,                                                                                            // drop shadow thickness
            MathCore::vec4f(0),                                                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct                                           // meshUploadMode,
        );

        MathCore::vec2f avatar_offset = MathCore::vec2f(
            -max_box_size.x * 0.5f + avatar_size * 0.5f + text_margin * 0.75f,
            max_box_size.y * 0.5f + avatar_size * 0.5f - text_margin * 0.75f);

        MathCore::vec2f continue_offset = MathCore::vec2f(
            max_box_size.x * 0.5f - continue_button_size * 0.5f - text_margin * 0.75f,
            -max_box_size.y * 0.5f - continue_button_size * 0.5f + text_margin * 0.75f);

        CollisionCore::AABB<MathCore::vec3f> aabb_main_box = CollisionCore::AABB<MathCore::vec3f>(
            -max_box_size * 0.5f,
            max_box_size * 0.5f);

        CollisionCore::AABB<MathCore::vec3f> aabb_avatar_box = CollisionCore::AABB<MathCore::vec3f>(
            -MathCore::vec2f(avatar_size) * 0.5f + avatar_offset,
            MathCore::vec2f(avatar_size) * 0.5f + avatar_offset);

        CollisionCore::AABB<MathCore::vec3f> aabb_continue_box = CollisionCore::AABB<MathCore::vec3f>(
            -MathCore::vec2f(continue_button_size) * 0.5f + continue_offset,
            MathCore::vec2f(continue_button_size) * 0.5f + continue_offset);

        CollisionCore::AABB<MathCore::vec3f> all_box;
        all_box = CollisionCore::AABB<MathCore::vec3f>::joinAABB(aabb_main_box, aabb_avatar_box);
        all_box = CollisionCore::AABB<MathCore::vec3f>::joinAABB(all_box, aabb_continue_box);

        auto center_all = (all_box.min_box + all_box.max_box) * 0.5f;
        auto size_all = (all_box.max_box - all_box.min_box);

        MathCore::vec2f main_offset = MathCore::vec2f(-center_all.x, -center_all.y);

        avatar_offset += main_offset;
        continue_offset += main_offset;

        main_box->getTransform()->setLocalPosition(MathCore::vec3f(
            main_offset.x,
            main_offset.y,
            0.0f));

        avatar_box->getTransform()->setLocalPosition(MathCore::vec3f(
            avatar_offset.x,
            avatar_offset.y,
            -10.0f));

        continue_box->getTransform()->setLocalPosition(MathCore::vec3f(
            continue_offset.x,
            continue_offset.y,
            -10.0f));

        node_ui->getTransform()->setLocalPosition(MathCore::vec3f(
            0.0f,
            -size.y * 0.5f + size_all.y * 0.5f + screen_margin,
            //-size.y * 0.5f + screen_margin + max_box_size.y * 0.5f,
            0.0f));

        main_box_text->getTransform()->setLocalPosition(MathCore::vec3f(
            main_offset.x,
            main_offset.y,
            -5.0f));
    }

    CollisionCore::AABB<MathCore::vec3f> InGameDialog::computeAABB()
    {
        auto size = screenManager->current_size;

        MathCore::vec2f max_box_size = (MathCore::vec2f)size - 2.0f * screen_margin;
        max_box_size = MathCore::OP<MathCore::vec2f>::maximum(0, max_box_size);
        float reserved_height = text_margin * 2.0f;
        max_box_size.y = (reserved_height < max_box_size.y) ? reserved_height : max_box_size.y;

        MathCore::vec2f avatar_offset = MathCore::vec2f(
            -max_box_size.x * 0.5f + avatar_size * 0.5f + text_margin * 0.75f,
            max_box_size.y * 0.5f + avatar_size * 0.5f - text_margin * 0.75f);

        MathCore::vec2f continue_offset = MathCore::vec2f(
            max_box_size.x * 0.5f - continue_button_size * 0.5f - text_margin * 0.75f,
            -max_box_size.y * 0.5f - continue_button_size * 0.5f + text_margin * 0.75f);

        CollisionCore::AABB<MathCore::vec3f> aabb_main_box = CollisionCore::AABB<MathCore::vec3f>(
            -max_box_size * 0.5f,
            max_box_size * 0.5f);

        CollisionCore::AABB<MathCore::vec3f> aabb_avatar_box = CollisionCore::AABB<MathCore::vec3f>(
            -MathCore::vec2f(avatar_size) * 0.5f + avatar_offset,
            MathCore::vec2f(avatar_size) * 0.5f + avatar_offset);

        CollisionCore::AABB<MathCore::vec3f> aabb_continue_box = CollisionCore::AABB<MathCore::vec3f>(
            -MathCore::vec2f(continue_button_size) * 0.5f + continue_offset,
            MathCore::vec2f(continue_button_size) * 0.5f + continue_offset);

        CollisionCore::AABB<MathCore::vec3f> all_box;
        all_box = CollisionCore::AABB<MathCore::vec3f>::joinAABB(aabb_main_box, aabb_avatar_box);
        all_box = CollisionCore::AABB<MathCore::vec3f>::joinAABB(all_box, aabb_continue_box);

        // auto center_all = (all_box.min_box + all_box.max_box) * 0.5f;
        auto size_all = (all_box.max_box - all_box.min_box);

        // auto main_box_text = node_ui->getItemByName("main_box_text").get<AppKit::GLEngine::Components::ComponentFont>();

        {
            float text_max_width = MathCore::OP<float>::maximum(max_box_size.x - text_margin * 2.0f, 0.0f);
            auto engine = AppKit::GLEngine::Engine::Instance();

            auto main_box_text_box = AppKit::GLEngine::Components::ComponentFont::computeBox(
                node_ui->resourceMap,                  // AppKit::GLEngine::ResourceMap *resourceMap,
                "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
                // 0 = texture, > 0 = polygon
                0,                   // float polygon_size,
                0,                   // float polygon_distance_tolerance,
                nullptr,             // Platform::ThreadPool *polygon_threadPool,
                engine->sRGBCapable, // bool is_srgb,
                this->rich_message,  // const std::string &text,
                text_size,           // float size, ///< current state of the font size
                text_max_width,      // float max_width,

                AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
                AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
                1.0f,                                                             // float lineHeight,
                AppKit::OpenGL::GLFont2WrapMode_Word,                             // AppKit::OpenGL::GLFont2WrapMode wrapMode,
                AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
                U' '                                                              // char32_t wordSeparatorChar
            );
            float size_text_y = main_box_text_box.max_box.y - main_box_text_box.min_box.y;

            auto fontResource = node_ui->resourceMap->getTextureFont("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);

            // set min_line_count
            size_text_y = MathCore::OP<float>::maximum(size_text_y, min_line_count * fontResource->fontBuilder->glFont2.new_line_height * (text_size / fontResource->fontBuilder->glFont2.size));

            max_box_size.y += MathCore::OP<float>::maximum(0.0f, size_text_y);
        }

        return CollisionCore::AABB<MathCore::vec3f>(-size_all * 0.5f, size_all * 0.5f);
    }

    void InGameDialog::setSpriteAvatars(const std::vector<std::string> &sprite_list)
    {
        using namespace AppKit::GLEngine;

        SpriteAtlasGenerator gen;

        for(const auto &sprite_path : sprite_list)
            gen.addEntry(sprite_path);

        auto engine = AppKit::GLEngine::Engine::Instance();
        avatarAtlas = gen.generateAtlas(*node_ui->resourceMap, engine->sRGBCapable, true, 10);
    }

    int InGameDialog::countLinesForText(const std::string &rich_message)
    {
        auto size = screenManager->current_size;
        MathCore::vec2f max_box_size = (MathCore::vec2f)size - 2.0f * screen_margin;
        max_box_size = MathCore::OP<MathCore::vec2f>::maximum(0, max_box_size);

        float text_max_width = MathCore::OP<float>::maximum(max_box_size.x - text_margin * 2.0f, 0.0f);
        auto engine = AppKit::GLEngine::Engine::Instance();

        return AppKit::GLEngine::Components::ComponentFont::countLines(
            node_ui->resourceMap,                  // AppKit::GLEngine::ResourceMap *resourceMap,
            "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
            engine->sRGBCapable,                   // bool is_srgb,
            rich_message,                          // const std::string &text,
            text_size,                             // float size, ///< current state of the font size
            text_max_width,                        // float max_width,
            AppKit::OpenGL::GLFont2WrapMode_Word,  // AppKit::OpenGL::GLFont2WrapMode wrapMode,
            U' '                                   // char32_t wordSeparatorChar
        );
    }

    void InGameDialog::setMinLineCount(int min_line_count)
    {
        // start dialog with empty lines to reserve space
        this->min_line_count = min_line_count;
    }

    void InGameDialog::showDialog(

        DialogAppearModeType appear_mode,

        DialogAvatarSideType side,
        const std::string &avatar,

        DialogTextModeType mode,
        const std::string &rich_message,
        const std::string &rich_continue_char,

        EventCore::Callback<void()> onAppeared,
        EventCore::Callback<void()> onContinuePressed)
    {
        this->rich_message = rich_message;
        this->rich_continue_char = rich_continue_char;

        createAllComponents();
        layoutVisibleElements(screenManager->current_size);
        node_ui->getTransform()->skip_traversing = false;
        onAppeared();
    }

    void InGameDialog::hideDialog(DialogAppearModeType appear_mode,
                                  EventCore::Callback<void()> onDisapeared)
    {
        node_ui->getTransform()->skip_traversing = true;
        releaseAllComponents();
        onDisapeared();
    }

    void InGameDialog::resetColors()
    {
    }

}
