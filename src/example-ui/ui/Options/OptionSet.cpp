#include "./OptionSet.h"
#include "../ScreenManager.h"

namespace ui
{
    void OptionSet::initialize(
        const std::string &buttonSetName,
        std::shared_ptr<AppKit::GLEngine::Components::ComponentUI> uiComponent,
        ScreenManager *screenManager,
        std::shared_ptr<AppKit::GLEngine::Components::ComponentRectangle> &mask)
    {
        selected_item_index = 0;
        this->mask = mask;
        this->screenManager = screenManager;

        auto size = screenManager->current_size;

        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f - ScreenOptions::top_bar_height);
        float y_pos =
            -ScreenOptions::top_bar_height * 0.5f + valid_size.height * 0.5f - ScreenOptions::item_height * 0.5f;

        // ui at center
        ui = uiComponent->addComponentUI(MathCore::vec2f(0, y_pos), -1, buttonSetName).get<AppKit::GLEngine::Components::ComponentUI>();
        ui->getTransform()->skip_traversing = true;

        MathCore::vec2f center = MathCore::vec2f(0, 0);
        selection_rect = ui->addRectangle(
                               center,                                                    // pos
                               MathCore::vec2f(valid_size.x, ScreenOptions::item_height), // size
                               screenManager->colorPalette.active,                        // color
                               MathCore::vec4f(32, 32, 32, 32),                           // radius
                               AppKit::GLEngine::Components::StrokeModeGrowInside,        // stroke mode
                               screenManager->colorPalette.stroke_thickness,              // stroke thickness
                               screenManager->colorPalette.active_stroke,                 // stroke color
                               0,                                                         // drop shadow thickness
                               MathCore::vec4f(0),                                        // drop shadow color
                               -1,                                                        // z
                               "selection_rect")
                             .get<AppKit::GLEngine::Components::ComponentRectangle>();
        selection_rect->setMask(ui->resourceMap, screenManager->camera, mask);

        auto rect = ui->addRectangle(
                          center,                                                    // pos
                          MathCore::vec2f(valid_size.x, ScreenOptions::item_height), // size
                          MathCore::vec4f(0, 0, 0, 0.4f),                            // color
                          MathCore::vec4f(32, 0, 0, 32),                             // radius
                          AppKit::GLEngine::Components::StrokeModeGrowInside,        // stroke mode
                          screenManager->colorPalette.stroke_thickness,              // stroke thickness
                          screenManager->colorPalette.active_stroke,                 // stroke color
                          0,                                                         // drop shadow thickness
                          MathCore::vec4f(0),                                        // drop shadow color
                          -100,                                                      // z
                          "top_rect")
                        .get<AppKit::GLEngine::Components::ComponentRectangle>();
        rect->setMask(ui->resourceMap, screenManager->camera, mask);

        rect = ui->addRectangle(
                     center,                                                    // pos
                     MathCore::vec2f(valid_size.x, ScreenOptions::item_height), // size
                     MathCore::vec4f(0, 0, 0, 0.4f),                            // color
                     MathCore::vec4f(0, 32, 32, 0),                             // radius
                     AppKit::GLEngine::Components::StrokeModeGrowInside,        // stroke mode
                     screenManager->colorPalette.stroke_thickness,              // stroke thickness
                     screenManager->colorPalette.active_stroke,                 // stroke color
                     0,                                                         // drop shadow thickness
                     MathCore::vec4f(0),                                        // drop shadow color
                     -100,                                                      // z
                     "bottom_rect")
                   .get<AppKit::GLEngine::Components::ComponentRectangle>();
        rect->setMask(ui->resourceMap, screenManager->camera, mask);
    }

    void OptionSet::update(Platform::Time *elapsed, float osciloscope, float sin)
    {
        selection_rect->setColor(
            screenManager->colorPalette.lrp_active(sin),
            screenManager->colorPalette.lrp_active_stroke(sin),
            0);
    }

    void OptionSet::addOption(const std::string &option, const std::vector<std::string> &choices, const std::string &selected)
    {

        float y_pos = -(float)items.size() * (ScreenOptions::item_height + ScreenOptions::item_vmargin);

        ItemDefinition itemDefinition;

        itemDefinition.ui = ui->addComponentUI(MathCore::vec2f(0, y_pos), 0, option).get<AppKit::GLEngine::Components::ComponentUI>();
        itemDefinition.option = option;
        itemDefinition.choices = choices;
        auto sel_it = std::find(choices.begin(), choices.end(), selected);
        if (sel_it == choices.end())
        {
            itemDefinition.selected = choices[0];
            itemDefinition.selected_index = 0;
        }
        else
        {
            itemDefinition.selected = selected;
            itemDefinition.selected_index = std::distance(choices.begin(), sel_it);
        }

        items.push_back(itemDefinition);

        auto size = screenManager->current_size;

        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f);

        float item_width = valid_size.width * 0.5f - ScreenOptions::item_hmargin * 2.0f;

        MathCore::vec2f center = MathCore::vec2f(0, 0);

        auto option_text_center = center + MathCore::vec2f(-valid_size.width * 0.25f, 0);
        auto txt = itemDefinition.ui->addTextureText(
                                        "resources/Roboto-Regular-100.basof2",                            // font_path
                                        option_text_center,                                               // pos
                                        -3,                                                               // z
                                        itemDefinition.option,                                            // text
                                        ScreenOptions::item_height * 0.5f,                                // size
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
                                        "title")
                       .get<AppKit::GLEngine::Components::ComponentFont>();
        txt->setMask(itemDefinition.ui->resourceMap, screenManager->camera, mask);

        // auto option_center = center + MathCore::vec2f(item_margin + item_width * 0.5f, 0);
        auto option_center = center + MathCore::vec2f(valid_size.width * 0.25f, 0);
        auto rect = itemDefinition.ui->addRectangle(
                                         option_center,                                                                         // pos
                                         MathCore::vec2f(item_width, ScreenOptions::item_height * 0.6f),                        // size
                                         (MathCore::vec4f)screenManager->colorPalette.primary * MathCore::vec4f(1, 1, 1, 0.4f), // color
                                         MathCore::vec4f(16),                                                                   // radius
                                         AppKit::GLEngine::Components::StrokeModeGrowInside,                                    // stroke mode
                                         screenManager->colorPalette.stroke_thickness,                                          // stroke thickness
                                         screenManager->colorPalette.primary_stroke,                                            // stroke color
                                         0,                                                                                     // drop shadow thickness
                                         MathCore::vec4f(0),                                                                    // drop shadow color
                                         -2,                                                                                    // z
                                         "option-bg")
                        .get<AppKit::GLEngine::Components::ComponentRectangle>();
        rect->setMask(itemDefinition.ui->resourceMap, screenManager->camera, mask);

        txt = itemDefinition.ui->addTextureText(
                                   "resources/Roboto-Regular-100.basof2",                            // font_path
                                   option_center,                                                    // pos
                                   -3,                                                               // z
                                   itemDefinition.selected,                                          // text
                                   ScreenOptions::item_height * 0.5f,                                // size
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
                                   "option-text")
                  .get<AppKit::GLEngine::Components::ComponentFont>();
        txt->setMask(itemDefinition.ui->resourceMap, screenManager->camera, mask);

        txt = itemDefinition.ui->addTextureText(
                                   "resources/Roboto-Regular-100.basof2",                                                // font_path
                                   option_center + MathCore::vec2f(-item_width * 0.5f + ScreenOptions::item_hmargin, 0), // pos
                                   -3,                                                                                   // z
                                   "<",                                                                                  // text
                                   ScreenOptions::item_height * 0.5f,                                                    // size
                                   -1,                                                                                   // max_width
                                   screenManager->colorPalette.text,                                                     // faceColor
                                   colorFromHex("#000000", 0.0f),                                                        // strokeColor
                                   MathCore::vec3f(0.0f, 0.0f, -0.02f),                                                  // strokeOffset
                                   AppKit::OpenGL::GLFont2HorizontalAlign_left,                                          // horizontalAlign
                                   AppKit::OpenGL::GLFont2VerticalAlign_middle,                                          // verticalAlign
                                   1.0f,                                                                                 // lineHeight
                                   AppKit::OpenGL::GLFont2WrapMode_Word,                                                 // wrapMode
                                   AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,                     // firstLineHeightMode
                                   U' ',                                                                                 // wordSeparatorChar
                                   "option-less")
                  .get<AppKit::GLEngine::Components::ComponentFont>();
        txt->setMask(itemDefinition.ui->resourceMap, screenManager->camera, mask);

        txt = itemDefinition.ui->addTextureText(
                                   "resources/Roboto-Regular-100.basof2",                                               // font_path
                                   option_center + MathCore::vec2f(item_width * 0.5f - ScreenOptions::item_hmargin, 0), // pos
                                   -3,                                                                                  // z
                                   ">",                                                                                 // text
                                   ScreenOptions::item_height * 0.5f,                                                   // size
                                   -1,                                                                                  // max_width
                                   screenManager->colorPalette.text,                                                    // faceColor
                                   colorFromHex("#000000", 0.0f),                                                       // strokeColor
                                   MathCore::vec3f(0.0f, 0.0f, -0.02f),                                                 // strokeOffset
                                   AppKit::OpenGL::GLFont2HorizontalAlign_right,                                        // horizontalAlign
                                   AppKit::OpenGL::GLFont2VerticalAlign_middle,                                         // verticalAlign
                                   1.0f,                                                                                // lineHeight
                                   AppKit::OpenGL::GLFont2WrapMode_Word,                                                // wrapMode
                                   AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,                    // firstLineHeightMode
                                   U' ',                                                                                // wordSeparatorChar
                                   "option-greater")
                  .get<AppKit::GLEngine::Components::ComponentFont>();
        txt->setMask(itemDefinition.ui->resourceMap, screenManager->camera, mask);
    }

    void OptionSet::layoutElements(const MathCore::vec2i &size)
    {
        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f - ScreenOptions::top_bar_height);
        // float y_center = -ScreenOptions::top_bar_height * 0.5f;
        // float y_pos_start =
        //     y_center + valid_size.height * 0.5f - ScreenOptions::item_height * 0.5f;
        // float y_pos_end =
        //     y_center - valid_size.height * 0.5f + ScreenOptions::item_height * 0.5f;

        // float y_height_items = (float)(items.size() - 1) * (ScreenOptions::item_height + ScreenOptions::item_vmargin);

        // ui->getTransform()->setLocalPosition(MathCore::vec3f(0, (y_pos_start + y_pos_end) * 0.5 + y_height_items * 0.5f, -1));

        set_selection_percentagem_related_to_valid_area();

        float item_width = valid_size.width * 0.5f - ScreenOptions::item_hmargin * 2.0f;

        auto option_text_center = MathCore::vec3f(-valid_size.width * 0.25f, 0, -3);
        auto option_center = MathCore::vec3f(valid_size.width * 0.25f, 0, -3);
        auto option_sel_bg_size = MathCore::vec2f(item_width, ScreenOptions::item_height * 0.6f);
        auto option_less = option_center + MathCore::vec3f(-item_width * 0.5f + ScreenOptions::item_hmargin, 0, 0);
        auto option_greater = option_center + MathCore::vec3f(item_width * 0.5f - ScreenOptions::item_hmargin, 0, 0);

        for (auto &item : items)
        {
            item.ui->getItemByName("title").transform->setLocalPosition(option_text_center);
            item.ui->getItemByName("option-text").transform->setLocalPosition(option_center);
            item.ui->getItemByName("option-less").transform->setLocalPosition(option_less);
            item.ui->getItemByName("option-greater").transform->setLocalPosition(option_greater);

            auto &bg = item.ui->getItemByName("option-bg");
            bg.transform->setLocalPosition(option_center - MathCore::vec3f(0, 0, -1));

            bg.get<AppKit::GLEngine::Components::ComponentRectangle>()->setQuad(
                ui->resourceMap,
                option_sel_bg_size,                                                                    // size
                (MathCore::vec4f)screenManager->colorPalette.primary * MathCore::vec4f(1, 1, 1, 0.4f), // color
                MathCore::vec4f(16),                                                                   // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside,                                    // stroke mode
                screenManager->colorPalette.stroke_thickness,                                          // stroke thickness
                screenManager->colorPalette.primary_stroke,                                            // stroke color
                0,                                                                                     // drop shadow thickness
                MathCore::vec4f(0),                                                                    // drop shadow color
                AppKit::GLEngine::Components::MeshUploadMode_Direct                                    // meshUploadMode,
            );
        }

        selection_rect->setQuad(
            ui->resourceMap,
            MathCore::vec2f(valid_size.x, ScreenOptions::item_height), // size
            screenManager->colorPalette.active,                        // color
            MathCore::vec4f(32, 32, 32, 32),                           // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,        // stroke mode
            screenManager->colorPalette.stroke_thickness,              // stroke thickness
            screenManager->colorPalette.active_stroke,                 // stroke color
            0,                                                         // drop shadow thickness
            MathCore::vec4f(0),                                        // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct        // meshUploadMode,
        );

        // printf("Selection Percent: %f\n", get_selection_percentagem_related_to_valid_area());

        // float y_center = -ScreenOptions::top_bar_height * 0.5f;
        // y_center -= ui->getTransform()->getLocalPosition().y;
        // float y_pos_start =
        //     y_center + valid_size.height * 0.5f - ScreenOptions::item_height * 0.5f;
        // float y_pos_end =
        //     y_center - valid_size.height * 0.5f + ScreenOptions::item_height * 0.5f;

        {
            auto &rect = ui->getItemByName("top_rect");

            // rect.transform->setLocalPosition(MathCore::vec3f(0, y_pos_start, -10));

            rect.get<AppKit::GLEngine::Components::ComponentRectangle>()->setQuad(
                ui->resourceMap,
                MathCore::vec2f(valid_size.x, ScreenOptions::item_height), // size
                MathCore::vec4f(0, 0, 0, 0.4f),                            // color
                MathCore::vec4f(32, 0, 0, 32),                             // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside,        // stroke mode
                screenManager->colorPalette.stroke_thickness,              // stroke thickness
                screenManager->colorPalette.active_stroke,                 // stroke color
                0,                                                         // drop shadow thickness
                MathCore::vec4f(0),                                        // drop shadow color
                AppKit::GLEngine::Components::MeshUploadMode_Direct        // meshUploadMode,
            );

            rect.get<AppKit::GLEngine::Components::ComponentRectangle>()->setLinearColorVertical(
                (MathCore::vec4f)screenManager->colorPalette.scroll_gradient * MathCore::vec4f(1, 1, 1, 0), // bottom
                screenManager->colorPalette.scroll_gradient                                                 // top
            );
        }

        {
            auto &rect = ui->getItemByName("bottom_rect");

            // rect.transform->setLocalPosition(MathCore::vec3f(0, y_pos_end, -10));

            rect.get<AppKit::GLEngine::Components::ComponentRectangle>()->setQuad(
                ui->resourceMap,
                MathCore::vec2f(valid_size.x, ScreenOptions::item_height), // size
                MathCore::vec4f(0, 0, 0, 0.4f),                            // color
                MathCore::vec4f(0, 32, 32, 0),                             // radius
                AppKit::GLEngine::Components::StrokeModeGrowInside,        // stroke mode
                screenManager->colorPalette.stroke_thickness,              // stroke thickness
                screenManager->colorPalette.active_stroke,                 // stroke color
                0,                                                         // drop shadow thickness
                MathCore::vec4f(0),                                        // drop shadow color
                AppKit::GLEngine::Components::MeshUploadMode_Direct        // meshUploadMode,
            );

            rect.get<AppKit::GLEngine::Components::ComponentRectangle>()->setLinearColorVertical(
                screenManager->colorPalette.scroll_gradient,                                               // bottom
                (MathCore::vec4f)screenManager->colorPalette.scroll_gradient * MathCore::vec4f(1, 1, 1, 0) // top
            );
        }

        set_selected_rect_pos();
    }

    void OptionSet::leftButton()
    {
        auto engine = AppKit::GLEngine::Engine::Instance();

        auto &item = items[selected_item_index];
        item.selected_index = (item.selected_index - 1 + (int)item.choices.size()) % (int)item.choices.size();
        item.selected = item.choices[item.selected_index];
        item.ui->getItemByName("option-text").get<AppKit::GLEngine::Components::ComponentFont>()->setText( //
            ui->resourceMap,
            "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
            // 0 = texture, > 0 = polygon
            0,                                                                // float polygon_size,
            0,                                                                // float polygon_distance_tolerance,
            nullptr,                                                          // Platform::ThreadPool *polygon_threadPool,
            engine->sRGBCapable,                                              // bool is_srgb,
            item.selected,                                                    // const std::string &text,
            ScreenOptions::item_height * 0.5f,                                // float size, ///< current state of the font size
            -1,                                                               // float max_width,
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
    void OptionSet::rightButton()
    {
        auto engine = AppKit::GLEngine::Engine::Instance();

        auto &item = items[selected_item_index];
        item.selected_index = (item.selected_index + 1) % item.choices.size();
        item.selected = item.choices[item.selected_index];
        item.ui->getItemByName("option-text").get<AppKit::GLEngine::Components::ComponentFont>()->setText( //
            ui->resourceMap,
            "resources/Roboto-Regular-100.basof2", // const std::string &font_path,
            // 0 = texture, > 0 = polygon
            0,                                                                // float polygon_size,
            0,                                                                // float polygon_distance_tolerance,
            nullptr,                                                          // Platform::ThreadPool *polygon_threadPool,
            engine->sRGBCapable,                                              // bool is_srgb,
            item.selected,                                                    // const std::string &text,
            ScreenOptions::item_height * 0.5f,                                // float size, ///< current state of the font size
            -1,                                                               // float max_width,
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

    void OptionSet::upButton()
    {
        selected_item_index = MathCore::OP<int>::clamp(selected_item_index - 1, 0, (int)items.size() - 1);
        set_selected_rect_pos();
    }
    void OptionSet::downButton()
    {
        selected_item_index = MathCore::OP<int>::clamp(selected_item_index + 1, 0, (int)items.size() - 1);
        set_selected_rect_pos();
    }

    void OptionSet::show()
    {
        ui->getTransform()->skip_traversing = false;
    }
    void OptionSet::hide()
    {
        ui->getTransform()->skip_traversing = true;
    }

    void OptionSet::set_selected_rect_pos()
    {
        // auto size = screenManager->current_size;
        // auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
        //                                   size.height - ScreenOptions::margin * 2.0f - ScreenOptions::top_bar_height);
        // float y_center = -ScreenOptions::top_bar_height * 0.5f;
        // float y_pos_start =
        //     y_center + valid_size.height * 0.5f - ScreenOptions::item_height * 0.5f;

        float rect_y_pos = -(float)selected_item_index * (ScreenOptions::item_height + ScreenOptions::item_vmargin);

        selection_rect->getTransform()->setLocalPosition(MathCore::vec3f(0, rect_y_pos, -1));
        set_selection_percentagem_related_to_valid_area();
    }

    // float OptionSet::get_selection_percentagem_related_to_valid_area()
    // {

    //     auto ui_pos = ui->getTransform()->getLocalPosition();

    //     auto size = screenManager->current_size;
    //     auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
    //                                       size.height - ScreenOptions::margin * 2.0f - ScreenOptions::top_bar_height);
    //     float rect_y_pos = -(float)selected_item_index * (ScreenOptions::item_height + ScreenOptions::item_vmargin);
    //     rect_y_pos += ui_pos.y;

    //     float y_center = -ScreenOptions::top_bar_height * 0.5f;
    //     float y_pos_start =
    //         y_center + valid_size.height * 0.5f - ScreenOptions::item_height * 0.5f;
    //     float y_pos_end =
    //         y_center - valid_size.height * 0.5f + ScreenOptions::item_height * 0.5f;

    //     if (valid_size.height < ScreenOptions::item_height)
    //         return MathCore::FloatTypeInfo<float>::max;

    //     float diff = (rect_y_pos - y_pos_start) / (y_pos_end - y_pos_start);

    //     return diff;
    // }

    void OptionSet::set_selection_percentagem_related_to_valid_area()
    {

        // if (lrp >= MathCore::FloatTypeInfo<float>::max)
        //{
        float rect_y_pos = -(float)selected_item_index * (ScreenOptions::item_height + ScreenOptions::item_vmargin);

        auto size = screenManager->current_size;

        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f - ScreenOptions::top_bar_height);
        float y_center = -ScreenOptions::top_bar_height * 0.5f;
        float y_pos_start =
            y_center + valid_size.height * 0.5f - ScreenOptions::item_height * 0.5f;
        float y_pos_end =
            y_center - valid_size.height * 0.5f + ScreenOptions::item_height * 0.5f;

        float y_height_items = (float)(items.size() - 1) * (ScreenOptions::item_height + ScreenOptions::item_vmargin);

        auto &top_rect = ui->getItemByName("top_rect");
        auto &bottom_rect = ui->getItemByName("bottom_rect");

        if (y_height_items + ScreenOptions::item_height < valid_size.height)
        {
            float y_final = -rect_y_pos;

            ui->getTransform()->setLocalPosition(MathCore::vec3f(0, (y_pos_start + y_pos_end) * 0.5 + //
                                                                        y_height_items * 0.5f,        //
                                                                                                      // y_final,
                                                                 -1));
            // trigger shadow top OFF
            // trigger shadow bottom OFF
            top_rect.transform->skip_traversing = true;
            bottom_rect.transform->skip_traversing = true;
        }
        else
        {
            float y_final = -rect_y_pos;

            // auto ui_pos = ui->getTransform()->getLocalPosition();
            // float first_real_pos = 0;
            // rect_y_pos += ui_pos.y;
            // float last_real_pos = -(float)selected_item_index * (ScreenOptions::item_height + ScreenOptions::item_vmargin);
            // rect_y_pos += ui_pos.y;

            // if (y_final < y_height_items * 0.5f)
            float y_stick_top = valid_size.height * 0.5f - ScreenOptions::item_height * 0.5f; // stick top

            float y_stick_bottom = -valid_size.height * 0.5f + ScreenOptions::item_height * 0.5f + // stick bottom
                                   y_height_items;
            if (y_final < y_stick_top)
            {
                y_final = y_stick_top;
                // trigger shadow top OFF
                // trigger shadow bottom ON
                top_rect.transform->skip_traversing = true;
                bottom_rect.transform->skip_traversing = false;
            }
            else if (y_final > y_stick_bottom)
            {
                y_final = y_stick_bottom;
                // trigger shadow top ON
                // trigger shadow bottom OFF
                top_rect.transform->skip_traversing = false;
                bottom_rect.transform->skip_traversing = true;
            }
            else
            {
                top_rect.transform->skip_traversing = false;
                bottom_rect.transform->skip_traversing = false;
            }

            ui->getTransform()->setLocalPosition(MathCore::vec3f(0, (y_pos_start + y_pos_end) * 0.5 + //
                                                                                                      // y_height_items * 0.5f +       //
                                                                        y_final,
                                                                 -1));
        }

        float current_scroll = ui->getTransform()->getLocalPosition().y;
        top_rect.transform->setLocalPosition(MathCore::vec3f(0, y_pos_start - current_scroll, -10));
        bottom_rect.transform->setLocalPosition(MathCore::vec3f(0, y_pos_end - current_scroll, -10));

        //}
    }

}