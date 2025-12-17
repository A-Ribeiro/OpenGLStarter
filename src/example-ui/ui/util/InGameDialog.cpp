#include "InGameDialog.h"
#include "../ScreenManager.h"

//
// Auto Generated: Exported Bitmaps inside the Font
//
#define Font_xbox_a u8"\U00010000"
#define Font_xbox_b u8"\U00010001"
#define Font_xbox_x u8"\U00010002"
#define Font_xbox_y u8"\U00010003"
#define Font_ps_circle_color u8"\U00010004"
#define Font_ps_cross_color u8"\U00010005"
#define Font_ps_square_color u8"\U00010006"
#define Font_ps_triangle_color u8"\U00010007"
#define Font_ps_circle_white u8"\U00010008"
#define Font_ps_cross_white u8"\U00010009"
#define Font_ps_square_white u8"\U0001000a"
#define Font_ps_triangle_white u8"\U0001000b"
#define Font_L_stick u8"\U0001000c"
#define Font_R_stick u8"\U0001000d"
#define Font_Key_arrows u8"\U0001000e"
#define Font_Key_z u8"\U0001000f"
#define Font_Key_x u8"\U00010010"
#define Font_Key_c u8"\U00010011"

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

        auto avatar_pivot = node_ui->addComponentUI(
                                       MathCore::vec2f(0, 0), // pos
                                       -10,                   // z
                                       "avatar_pivot")
                                .get<AppKit::GLEngine::Components::ComponentUI>();

        auto rect_mask = avatar_pivot->addRectangle(
                                         MathCore::vec2f(0, 0),                              // pos
                                         MathCore::vec2f(avatar_size),                       // size
                                         colorFromHex("#000000", 0.4f),                      // color
                                         MathCore::vec4f(0, 0, 0, 0),                        // radius
                                         AppKit::GLEngine::Components::StrokeModeGrowInside, // stroke mode
                                         0,                                                  // stroke thickness
                                         colorFromHex("#000000", 0.0f),                      // stroke color
                                         0,                                                  // drop shadow thickness
                                         MathCore::vec4f(0),                                 // drop shadow color
                                         0,                                                  // z
                                         "avatar_box")
                             .get<AppKit::GLEngine::Components::ComponentRectangle>();

        auto avatar_sprite = avatar_pivot->addSpriteFromAtlas(
                                             MathCore::vec2f(0, 0),         // pos
                                             avatarAtlas,                   // atlas
                                             "NON_EXISTING_SPRITE_NAME]",   // texture
                                             MathCore::vec2f(0.5f, 0.5f),   // pivot
                                             ui::colorFromHex("#ffffffff"), // color
                                             MathCore::vec2f(avatar_size),  // size
                                             false,                         // x_invert
                                             false,                         // y_invert
                                             -2,                            // z
                                             "avatar_sprite")
                                 .get<AppKit::GLEngine::Components::ComponentSprite>();
        avatar_sprite->setMask(
            node_ui->resourceMap,
            screenManager->camera,
            rect_mask);

        auto continue_pivot = node_ui->addComponentUI(
                                         MathCore::vec2f(0, 0), // pos
                                         -10,                   // z
                                         "continue_pivot")
                                  .get<AppKit::GLEngine::Components::ComponentUI>();

        continue_pivot->addRectangle(
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

        continue_pivot->addTextureText(
            "resources/Roboto-Regular-100.basof2",                    // font_path
            MathCore::vec2f(0, 0),                                    // pos
            -15,                                                      // z
            this->rich_continue_char,                                 // text
            text_size,                                                // size
            -1,                                                       // max_width
            screenManager->colorPalette.text,                         // faceColor
            colorFromHex("#000000", 0.0f),                            // strokeColor
            MathCore::vec3f(0.0f, 0.0f, -0.02f),                      // strokeOffset
            AppKit::OpenGL::GLFont2HorizontalAlign_center,            // horizontalAlign
            AppKit::OpenGL::GLFont2VerticalAlign_middle,              // verticalAlign
            1.0f,                                                     // lineHeight
            AppKit::OpenGL::GLFont2WrapMode_Word,                     // wrapMode
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseLineHeight, // firstLineHeightMode
            U' ',                                                     // wordSeparatorChar
            "continue_char");

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
        min_text_height = 0.0f;
    }

    void InGameDialog::setProperties(float avatar_size, float continue_button_size, float text_size, float screen_margin, float text_margin, float char_per_sec, float char_per_sec_fast)
    {
        this->avatar_size = avatar_size;
        this->continue_button_size = continue_button_size;
        this->text_size = text_size;
        this->screen_margin = screen_margin;
        this->text_margin = text_margin;
        this->char_per_sec = char_per_sec;
        this->char_per_sec_fast = char_per_sec_fast;
        this->seconds_per_character = 1.0f / char_per_sec;

        this->time_to_appear_main_box_sec = 0.2f;
        this->curr_lrp_main_box = 0.0f;
        this->dialogAppearUpdateState = DialogAppearUpdateState_None;
    }

    void InGameDialog::update(Platform::Time *elapsed, float blink_01_lerp_factor)
    {
        // auto avatar_pivot = node_ui->getItemByName("avatar_pivot").get<AppKit::GLEngine::Components::ComponentUI>();

        // float posicao_esquerda = -max_box_size.x * 0.5f + avatar_size * 0.5f + text_margin * 0.75f;
        // float posicao_direita = max_box_size.x * 0.5f - avatar_size * 0.5f - text_margin * 0.75f;
        // //float pos_offset = MathCore::OP<float>::lerp(posicao_esquerda, posicao_direita, side_percentage); // posicao_esquerda * ( 1.0f - side_percentage ) + posicao_direita * side_percentage;

        // float pos_offset = MathCore::OP<float>::lerp(posicao_esquerda, posicao_direita, blink_01_lerp_factor);
        // pos_offset += main_offset.x;

        // avatar_offset.x = pos_offset;

        // avatar_pivot->getTransform()->setLocalPosition(MathCore::vec3f(
        //     avatar_offset.x,
        //     avatar_offset.y,
        //     -10.0f));

        if (this->dialogAppearUpdateState == DialogAppearUpdateState_ToOne)
        {
            // this->time_to_appear_main_box_sec = 5.0f;
            this->curr_lrp_main_box += elapsed->deltaTime / this->time_to_appear_main_box_sec;
            if (this->curr_lrp_main_box >= 1.0f)
            {
                this->curr_lrp_main_box = 1.0f;
                this->dialogAppearUpdateState = DialogAppearUpdateState_None;
                if (this->onAppeared != nullptr)
                {
                    auto tmp_onAppeared = this->onAppeared;
                    this->onAppeared = nullptr;
                    tmp_onAppeared();
                }
            }

            auto size = screenManager->current_size;
            auto size_all = (all_box.max_box - all_box.min_box);

            float dialog_position_y = MathCore::OP<float>::lerp(
                -size.y * 0.5f - size_all.y * 0.5f,
                -size.y * 0.5f + size_all.y * 0.5f + screen_margin,
                curr_lrp_main_box);

            node_ui->getTransform()->setLocalPosition(MathCore::vec3f(
                0.0f,
                dialog_position_y,
                0.0f));
        }
        if (this->dialogAppearUpdateState == DialogAppearUpdateState_ToZero)
        {
            this->curr_lrp_main_box -= elapsed->deltaTime / this->time_to_appear_main_box_sec;
            if (this->curr_lrp_main_box <= 0.0f)
            {
                this->curr_lrp_main_box = 0.0f;
                this->dialogAppearUpdateState = DialogAppearUpdateState_None;
                node_ui->getTransform()->skip_traversing = true;
                releaseAllComponents();
                if (this->onDisapeared != nullptr)
                {
                    auto tmp_onDisapeared = this->onDisapeared;
                    this->onDisapeared = nullptr;
                    tmp_onDisapeared();
                }
            }
            else
            {
                auto size = screenManager->current_size;
                auto size_all = (all_box.max_box - all_box.min_box);

                float dialog_position_y = MathCore::OP<float>::lerp(
                    -size.y * 0.5f - size_all.y * 0.5f,
                    -size.y * 0.5f + size_all.y * 0.5f + screen_margin,
                    curr_lrp_main_box);

                node_ui->getTransform()->setLocalPosition(MathCore::vec3f(
                    0.0f,
                    dialog_position_y,
                    0.0f));
            }
        }
        else if (text_mode == DialogTextModeType_AppearAtOnce)
        {
            // this->rich_message = rich_message_source;
        }
        else if (text_mode == DialogTextModeType_CharAppear)
        {
            bool rich_message_changed = false;
            this->count_up_aux += elapsed->deltaTime;
            while (count_up_aux > seconds_per_character)
            {
                count_up_aux -= seconds_per_character;
                // add next char to rich_message
                bool ended = false;
                auto element_str = text_tokenizer.nextChar(&ended);
                if (ended)
                {
                    text_mode = DialogTextModeType_None;
                    if (components_created)
                    {
                        auto continue_pivot = node_ui->getItemByName("continue_pivot").get<AppKit::GLEngine::Components::ComponentUI>();
                        // auto continue_box = continue_pivot->getItemByName("continue_box").get<AppKit::GLEngine::Components::ComponentRectangle>();
                        auto continue_char = continue_pivot->getItemByName("continue_char").get<AppKit::GLEngine::Components::ComponentFont>();
                        continue_char->getTransform()->skip_traversing = false;
                    }
                }
                else
                {
                    rich_message += element_str;
                    rich_message_changed = true;
                }
            }
            if (rich_message_changed && components_created)
            {
                auto engine = AppKit::GLEngine::Engine::Instance();
                auto main_box_text = node_ui->getItemByName("main_box_text").get<AppKit::GLEngine::Components::ComponentFont>();
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
        }
    }

    void InGameDialog::layoutVisibleElements(const MathCore::vec2i &size)
    {
        if (!components_created)
            return;

        // called on resize

        auto main_box = node_ui->getItemByName("main_box").get<AppKit::GLEngine::Components::ComponentRectangle>();
        auto main_box_text = node_ui->getItemByName("main_box_text").get<AppKit::GLEngine::Components::ComponentFont>();

        auto avatar_pivot = node_ui->getItemByName("avatar_pivot").get<AppKit::GLEngine::Components::ComponentUI>();
        auto avatar_box = avatar_pivot->getItemByName("avatar_box").get<AppKit::GLEngine::Components::ComponentRectangle>();
        auto avatar_sprite = avatar_pivot->getItemByName("avatar_sprite").get<AppKit::GLEngine::Components::ComponentSprite>();

        auto continue_pivot = node_ui->getItemByName("continue_pivot").get<AppKit::GLEngine::Components::ComponentUI>();
        auto continue_box = continue_pivot->getItemByName("continue_box").get<AppKit::GLEngine::Components::ComponentRectangle>();
        auto continue_char = continue_pivot->getItemByName("continue_char").get<AppKit::GLEngine::Components::ComponentFont>();

        avatar_pivot->getTransform()->skip_traversing = side_percentage < 0.0f || side_percentage > 1.0f;

        if (text_mode == DialogTextModeType_CharAppear)
        {
            // auto continue_pivot = node_ui->getItemByName("continue_pivot").get<AppKit::GLEngine::Components::ComponentUI>();
            // auto continue_box = continue_pivot->getItemByName("continue_box").get<AppKit::GLEngine::Components::ComponentRectangle>();
            // auto continue_char = continue_pivot->getItemByName("continue_char").get<AppKit::GLEngine::Components::ComponentFont>();
            continue_char->getTransform()->skip_traversing = true;
        }
        else
            continue_char->getTransform()->skip_traversing = false;

        max_box_size = (MathCore::vec2f)size - 2.0f * screen_margin;
        max_box_size = MathCore::OP<MathCore::vec2f>::maximum(0, max_box_size);
        float reserved_height = text_margin * 2.0f;
        max_box_size.y = (reserved_height < max_box_size.y) ? reserved_height : max_box_size.y;

        // set text
        auto engine = AppKit::GLEngine::Engine::Instance();
        float text_max_width = MathCore::OP<float>::maximum(max_box_size.x - text_margin * 2.0f, 0.0f);

        {

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

        // auto main_box_text_box = main_box_text->currentBox();
        // size_text_y = main_box_text_box.max_box.y - main_box_text_box.min_box.y;

        {
            auto main_box_text_box = AppKit::GLEngine::Components::ComponentFont::computeBox(
                node_ui->resourceMap,                  // AppKit::GLEngine::ResourceMap *resourceMap,
                "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
                // 0 = texture, > 0 = polygon
                0,                         // float polygon_size,
                0,                         // float polygon_distance_tolerance,
                nullptr,                   // Platform::ThreadPool *polygon_threadPool,
                engine->sRGBCapable,       // bool is_srgb,
                this->rich_message_source, // const std::string &text,
                text_size,                 // float size, ///< current state of the font size
                text_max_width,            // float max_width,

                AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
                AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
                1.0f,                                                             // float lineHeight,
                AppKit::OpenGL::GLFont2WrapMode_Word,                             // AppKit::OpenGL::GLFont2WrapMode wrapMode,
                AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
                U' '                                                              // char32_t wordSeparatorChar
            );

            size_text_y = main_box_text_box.max_box.y - main_box_text_box.min_box.y;
        }

        // set min_line_count
        size_text_y = MathCore::OP<float>::maximum(size_text_y, min_line_count * fontResource->fontBuilder->glFont2.new_line_height * (text_size / fontResource->fontBuilder->glFont2.size));
        size_text_y = MathCore::OP<float>::maximum(size_text_y, min_text_height);

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

        avatar_sprite->setTextureFromAtlas(
            node_ui->resourceMap,
            avatarAtlas,                                        // atlas,
            this->avatar,                                       // name,
            MathCore::vec2f(0.5f, 0.5f),                        // pivot,
            ui::colorFromHex("#ffffffff"),                      // color,
            MathCore::vec2f(avatar_size),                       // size_constraint = MathCore::vec2f(-1, -1),
            x_invert,                                           // x_invert
            false,                                              // y_invert
            AppKit::GLEngine::Components::MeshUploadMode_Direct // meshUploadMode
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

        float posicao_esquerda = -max_box_size.x * 0.5f + avatar_size * 0.5f + text_margin * 0.75f;
        float posicao_direita = max_box_size.x * 0.5f - avatar_size * 0.5f - text_margin * 0.75f;
        float pos_offset = MathCore::OP<float>::lerp(posicao_esquerda, posicao_direita,
                                                     MathCore::OP<float>::clamp(side_percentage, 0, 1)); // posicao_esquerda * ( 1.0f - side_percentage ) + posicao_direita * side_percentage;
        avatar_offset = MathCore::vec2f(
            pos_offset,
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

        // CollisionCore::AABB<MathCore::vec3f> all_box;
        all_box = CollisionCore::AABB<MathCore::vec3f>::joinAABB(aabb_main_box, aabb_avatar_box);
        all_box = CollisionCore::AABB<MathCore::vec3f>::joinAABB(all_box, aabb_continue_box);

        auto center_all = (all_box.min_box + all_box.max_box) * 0.5f;
        auto size_all = (all_box.max_box - all_box.min_box);

        main_offset = MathCore::vec2f(-center_all.x, -center_all.y);

        avatar_offset += main_offset;
        continue_offset += main_offset;

        main_box->getTransform()->setLocalPosition(MathCore::vec3f(
            main_offset.x,
            main_offset.y,
            0.0f));

        avatar_pivot->getTransform()->setLocalPosition(MathCore::vec3f(
            avatar_offset.x,
            avatar_offset.y,
            -10.0f));

        continue_pivot->getTransform()->setLocalPosition(MathCore::vec3f(
            continue_offset.x,
            continue_offset.y,
            -10.0f));

        main_box_text->getTransform()->setLocalPosition(MathCore::vec3f(
            main_offset.x,
            main_offset.y,
            -5.0f));

        float dialog_position_y = MathCore::OP<float>::lerp(
            -size.y * 0.5f - size_all.y * 0.5f,
            -size.y * 0.5f + size_all.y * 0.5f + screen_margin,
            curr_lrp_main_box);

        node_ui->getTransform()->setLocalPosition(MathCore::vec3f(
            0.0f,
            dialog_position_y,
            0.0f));
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
            size_text_y = MathCore::OP<float>::maximum(size_text_y, min_text_height);

            max_box_size.y += MathCore::OP<float>::maximum(0.0f, size_text_y);
        }

        return CollisionCore::AABB<MathCore::vec3f>(-size_all * 0.5f, size_all * 0.5f);
    }

    void InGameDialog::setSpriteAvatars(const std::vector<std::string> &sprite_list)
    {
        using namespace AppKit::GLEngine;

        SpriteAtlasGenerator gen;

        for (const auto &sprite_path : sprite_list)
            gen.addEntry(sprite_path);

        auto engine = AppKit::GLEngine::Engine::Instance();
        avatarAtlas = gen.generateAtlas(*node_ui->resourceMap, engine->sRGBCapable, true, 10);
    }

    int InGameDialog::countLinesForText(const std::string &rich_message) const
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

    CollisionCore::AABB<MathCore::vec3f> InGameDialog::computeTextAABB(const std::string &rich_message) const
    {
        auto size = screenManager->current_size;

        MathCore::vec2f max_box_size = (MathCore::vec2f)size - 2.0f * screen_margin;
        max_box_size = MathCore::OP<MathCore::vec2f>::maximum(0, max_box_size);
        // float reserved_height = text_margin * 2.0f;
        // max_box_size.y = (reserved_height < max_box_size.y) ? reserved_height : max_box_size.y;

        auto engine = AppKit::GLEngine::Engine::Instance();
        float text_max_width = MathCore::OP<float>::maximum(max_box_size.x - text_margin * 2.0f, 0.0f);

        auto main_box_text_box = AppKit::GLEngine::Components::ComponentFont::computeBox(
            node_ui->resourceMap,                  // AppKit::GLEngine::ResourceMap *resourceMap,
            "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
            // 0 = texture, > 0 = polygon
            0,                   // float polygon_size,
            0,                   // float polygon_distance_tolerance,
            nullptr,             // Platform::ThreadPool *polygon_threadPool,
            engine->sRGBCapable, // bool is_srgb,
            rich_message,        // const std::string &text,
            text_size,           // float size, ///< current state of the font size
            text_max_width,      // float max_width,

            AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
            AppKit::OpenGL::GLFont2VerticalAlign_middle,                      // AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
            1.0f,                                                             // float lineHeight,
            AppKit::OpenGL::GLFont2WrapMode_Word,                             // AppKit::OpenGL::GLFont2WrapMode wrapMode,
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
            U' '                                                              // char32_t wordSeparatorChar
        );
        return main_box_text_box;
    }

    float InGameDialog::computeTextHeight(const std::string &rich_message) const
    {
        auto aabb = computeTextAABB(rich_message);
        return aabb.max_box.y - aabb.min_box.y;
    }

    void InGameDialog::setMinTextHeight(float h)
    {
        this->min_text_height = h;
    }

    void InGameDialog::showDialog(

        DialogAppearModeType appear_mode,

        float side_percentage, // DialogAvatarSideType
        const std::string &avatar,
        bool x_invert,

        DialogTextModeType text_mode,
        const std::string &rich_message,
        const std::string &rich_continue_char,

        EventCore::Callback<void()> onAppeared,
        EventCore::Callback<void()> onContinuePressed)
    {

        this->onAppeared = onAppeared;
        this->onContinuePressed = onContinuePressed;

        // this->rich_message = rich_message;
        this->rich_continue_char = rich_continue_char;
        this->avatar = avatar;
        this->side_percentage = side_percentage;
        this->x_invert = x_invert;

        createAllComponents();
        // set line count before layout

        int line_count = 1;
        rich_message_source = "";
        AppKit::OpenGL::RichMessageTokenizer line_tokenizer(rich_message);
        std::vector<std::string> line_output;
        bool ended = false;
        line_tokenizer.nextLine(&line_output, &ended);
        while (!ended)
        {
            if (!rich_message_source.empty())
            {
                rich_message_source += "\n";
                line_count++;
            }

            std::string aux_current_line;
            bool first_word = true;
            for (const auto &word : line_output)
            {
                if (first_word)
                {
                    first_word = false;
                    aux_current_line = word;
                }
                else
                {
                    std::string line_with_word = aux_current_line + " " + word;
                    int new_line_count = countLinesForText(line_with_word);
                    if (new_line_count > 1)
                    {
                        // line break needed
                        rich_message_source += aux_current_line + "\n";
                        line_count++;
                        aux_current_line = word;
                    }
                    else
                        aux_current_line = line_with_word;
                }
            }
            if (!aux_current_line.empty())
                rich_message_source += aux_current_line;

            line_tokenizer.nextLine(&line_output, &ended);
        }

        // setMinLineCount(line_count);

        this->text_mode = text_mode;
        if (text_mode == DialogTextModeType_AppearAtOnce)
        {
            this->rich_message = rich_message_source;
        }
        else if (text_mode == DialogTextModeType_CharAppear)
        {
            this->rich_message = "";
            this->text_tokenizer = AppKit::OpenGL::RichMessageTokenizer(rich_message_source);
            this->count_up_aux = 0.0f;
            this->seconds_per_character = 1.0f / char_per_sec;
        }

        layoutVisibleElements(screenManager->current_size);
        node_ui->getTransform()->skip_traversing = false;

        // this->time_to_appear_main_box_sec = 5.0f;
        // this->curr_lrp_main_box = 0.0f;
        this->dialogAppearUpdateState = DialogAppearUpdateState_ToOne;

        // if (this->onAppeared != nullptr)
        // {
        //     auto tmp_onAppeared = this->onAppeared;
        //     this->onAppeared = nullptr;
        //     tmp_onAppeared();
        // }
    }

    void InGameDialog::smartShowDialog(
        DialogAppearModeType appear_mode,
        DialogAppearModeType disappear_mode,
        const std::string &rich_continue_char,
        const std::vector<DialogProperties> &dialog_pages,
        EventCore::Callback<void()> onDialogEnded)
    {
        smart_dialog_pages.assign(dialog_pages.begin(), dialog_pages.end());

        float max_height = 0;
        for (const auto &v : smart_dialog_pages)
            max_height = MathCore::OP<float>::maximum(max_height, computeTextHeight(v.rich_message));
        setMinTextHeight(max_height);

        this->appear_mode = appear_mode;
        this->disappear_mode = disappear_mode;
        this->rich_continue_char = rich_continue_char;
        this->smart_OnDialogEnded = onDialogEnded;
        show_next_page();
    }
    void InGameDialog::show_next_page()
    {
        if (smart_dialog_pages.empty())
        {
            hideDialog(
                this->disappear_mode,
                [this]()
                {
                    if (this->smart_OnDialogEnded != nullptr)
                    {
                        auto tmp_onDialogEnded = this->smart_OnDialogEnded;
                        this->smart_OnDialogEnded = nullptr;
                        tmp_onDialogEnded();
                    }
                });
            return;
        }
        auto current_page = smart_dialog_pages.front();
        smart_dialog_pages.erase(smart_dialog_pages.begin());

        showDialog(
            this->appear_mode,
            current_page.side_percentage,
            current_page.avatar,
            current_page.x_invert,
            current_page.text_mode,
            current_page.rich_message,
            this->rich_continue_char,
            // onAppeared
            nullptr,
            // onContinuePressed
            EventCore::CallbackWrapper(&InGameDialog::show_next_page, this));
    }

    void InGameDialog::hideDialog(DialogAppearModeType appear_mode,
                                  EventCore::Callback<void()> onDisapeared)
    {
        this->onDisapeared = onDisapeared;
        this->dialogAppearUpdateState = DialogAppearUpdateState_ToZero;

        // node_ui->getTransform()->skip_traversing = true;
        // releaseAllComponents();
        // if (this->onDisapeared != nullptr)
        // {
        //     auto tmp_onDisapeared = this->onDisapeared;
        //     this->onDisapeared = nullptr;
        //     tmp_onDisapeared();
        // }
    }

    void InGameDialog::pressContinue()
    {
        if (text_mode == DialogTextModeType_CharAppear)
        {
            this->seconds_per_character = 1.0f / char_per_sec_fast;
        }

        if (text_mode == DialogTextModeType_None)
        {
            // continue pressed
            if (this->onContinuePressed != nullptr)
            {
                auto tmp_onContinuePressed = this->onContinuePressed;
                this->onContinuePressed = nullptr;
                tmp_onContinuePressed();
            }
        }
    }

    void InGameDialog::resetColors()
    {
    }

    void InGameDialog::pushScreen_ResetAll()
    {
        this->curr_lrp_main_box = 0.0f;
    }

}
