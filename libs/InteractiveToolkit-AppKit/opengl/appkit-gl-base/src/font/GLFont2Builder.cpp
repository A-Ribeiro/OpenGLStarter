#include <appkit-gl-base/font/GLFont2Builder.h>

// using namespace aRibeiro;
#if defined(_WIN32)
#pragma warning(disable : 4996)
#endif

namespace AppKit
{

    namespace OpenGL
    {

        GLFont2Builder_VertexAttrib::GLFont2Builder_VertexAttrib() {}
        GLFont2Builder_VertexAttrib::GLFont2Builder_VertexAttrib(
            const MathCore::vec3f &pos,
            const MathCore::vec2f &uv,
            const MathCore::vec4f &color)
        {

            this->uv = uv;
            this->pos = pos;
            this->color = color;
        }

        void GLFont2Builder::GLFont2BitmapRef_to_VertexAttrib(const MathCore::vec3f &pos, const MathCore::vec4f &color, const GLFont2BitmapRef &bitmapRef, float scale)
        {
            if (bitmapRef.bitmapBounds.w == 0 || bitmapRef.bitmapBounds.h == 0)
                return;
            auto scale_v3 = MathCore::vec3f(scale, scale, 1.0f);
            MathCore::vec3f pos_new = pos + bitmapRef.move_before_draw * scale_v3;
            for (int i = 0; i < 6; i++)
            {
                vertexAttrib.push_back(
                    GLFont2Builder_VertexAttrib(
                        bitmapRef.triangles.position[i] * scale_v3 + pos_new,
                        bitmapRef.triangles.uv[i],
                        color));
            }
        }

        GLFont2Builder::GLFont2Builder()
        {
            faceColor = MathCore::vec4f(1, 1, 1, 1);
            strokeColor = MathCore::vec4f(0, 0, 0, 1);

            drawFace = true;
            drawStroke = true;

            // strokeOffset = MathCore::vec3f(0, 0, EPSILON);
            strokeOffset = MathCore::vec3f(0, 0, 0.001f);

            horizontalAlign = GLFont2HorizontalAlign_left;
            verticalAlign = GLFont2VerticalAlign_baseline;

            lineHeight = 1.0;

            size = 32.0;

            wrapMode = GLFont2WrapMode::GLFont2WrapMode_Character;
            wordSeparatorChar = U' ';

            firstLineHeightMode = GLFont2FirstLineHeightMode_UseLineHeight;
        }

        void GLFont2Builder::load(const std::string &filename, bool force_srgb)
        {
            glFont2.loadFromBasof2(filename, force_srgb);
            size = glFont2.size;
        }

        enum class RichFontVarBitFlag : uint32_t
        {
            none = 0,

            faceColor = 1 << 0,
            strokeColor = 1 << 1,
            drawFace = 1 << 2,
            drawStroke = 1 << 3,
            lineHeight = 1 << 4,
            size = 1 << 5,
            push = 1 << 6,
            pop = 1 << 7,
            new_line_at_end = 1 << 8
        };

        inline RichFontVarBitFlag operator|(RichFontVarBitFlag a, RichFontVarBitFlag b) { return static_cast<RichFontVarBitFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b)); }
        inline RichFontVarBitFlag operator&(RichFontVarBitFlag a, RichFontVarBitFlag b) { return static_cast<RichFontVarBitFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b)); }
        inline RichFontVarBitFlag operator~(RichFontVarBitFlag a) { return static_cast<RichFontVarBitFlag>(~static_cast<uint32_t>(a)); }
        inline RichFontVarBitFlag &operator|=(RichFontVarBitFlag &a, RichFontVarBitFlag b)
        {
            a = a | b;
            return a;
        }
        inline RichFontVarBitFlag &operator&=(RichFontVarBitFlag &a, RichFontVarBitFlag b)
        {
            a = a & b;
            return a;
        }

        struct RichFontState
        {
            RichFontVarBitFlag flag;

            struct StyleData
            {
                MathCore::vec4f faceColor;   ///< current state of the face color
                MathCore::vec4f strokeColor; ///< current state of the stroke color
                bool drawFace;               ///< should draw face
                bool drawStroke;             ///< should draw stroke
                float lineHeight;            ///< current state of the line height
                float size;                  ///< current state of the line height

                float size_scaler;
                float lineHeight_scaled;
                float render_size_space_width;
            };

            StyleData style;
            StyleData saved_style;

            std::u32string str;

            // text string only attribute
            MathCore::vec2f bounds_with_size_applied;

            const GLFont2Builder *builder;

            static RichFontState Create(const GLFont2Builder *initial_state, bool use_srgb)
            {
                RichFontState rc;

                rc.builder = initial_state;

                rc.style.faceColor = initial_state->faceColor;
                rc.style.strokeColor = initial_state->strokeColor;

                if (use_srgb)
                {
                    rc.style.faceColor = MathCore::CVT<MathCore::vec4f>::sRGBToLinear(rc.style.faceColor);
                    rc.style.strokeColor = MathCore::CVT<MathCore::vec4f>::sRGBToLinear(rc.style.strokeColor);
                }

                rc.style.drawFace = initial_state->drawFace;
                rc.style.drawStroke = initial_state->drawStroke;

                rc.style.lineHeight = initial_state->lineHeight;
                rc.style.size = initial_state->size;

                rc.flag = RichFontVarBitFlag::none;

                rc.style.size_scaler = rc.style.size / initial_state->glFont2.size;
                rc.style.lineHeight_scaled = initial_state->glFont2.new_line_height * rc.style.size_scaler * rc.style.lineHeight;
                rc.style.render_size_space_width = rc.style.size_scaler * initial_state->glFont2.space_width;

                rc.saved_style = rc.style;

                return rc;
            }

            void push()
            {
                saved_style = style;
            }
            void pop()
            {
                style = saved_style;
            }

            void applyChange(const RichFontState &v)
            {

                if ((v.flag & RichFontVarBitFlag::faceColor) != RichFontVarBitFlag::none)
                    style.faceColor = v.style.faceColor;
                if ((v.flag & RichFontVarBitFlag::strokeColor) != RichFontVarBitFlag::none)
                    style.strokeColor = v.style.strokeColor;
                if ((v.flag & RichFontVarBitFlag::drawFace) != RichFontVarBitFlag::none)
                    style.drawFace = v.style.drawFace;
                if ((v.flag & RichFontVarBitFlag::drawStroke) != RichFontVarBitFlag::none)
                    style.drawStroke = v.style.drawStroke;
                if ((v.flag & RichFontVarBitFlag::lineHeight) != RichFontVarBitFlag::none)
                {
                    style.lineHeight = v.style.lineHeight;
                    style.lineHeight_scaled = v.style.lineHeight_scaled;
                }
                if ((v.flag & RichFontVarBitFlag::size) != RichFontVarBitFlag::none)
                {
                    style.size = v.style.size;
                    style.size_scaler = v.style.size_scaler;
                    style.lineHeight_scaled = v.style.lineHeight_scaled;
                    style.render_size_space_width = v.style.render_size_space_width;
                }

                // not used
                // RichFontVarBitFlag flag;
                // std::u32string str;
                // MathCore::vec2f bounds_with_size_applied;
            }

            void setText(const std::u32string &str, bool new_line_at_end, const GLFont2 &glFont2)
            {
                this->str = str;
                if (new_line_at_end)
                    flag |= RichFontVarBitFlag::new_line_at_end;
                else
                    flag &= ~RichFontVarBitFlag::new_line_at_end;
                computeLength(this->str, glFont2);
            }

            void computeLength(const std::u32string &strp, const GLFont2 &glFont2)
            {
                float length = 0;
                float max_height = 0;
                for (auto c : strp)
                {
                    if (c == U' ')
                        length += glFont2.space_width;
                    else if (auto glyph = glFont2.getGlyph((uint32_t)c))
                    {
                        length += glyph->advancex.x;
                        max_height = MathCore::OP<float>::maximum(max_height, glyph->face.height);
                    }
                }
                // bounds_with_size_applied = MathCore::vec2f(length, style.lineHeight) * style.size_scaler;
                bounds_with_size_applied = MathCore::vec2f(length, max_height) * style.size_scaler;
            }

            void ReadStyleCodeString(const std::u32string &json, bool use_srgb)
            {
                // clear flags
                const RichFontVarBitFlag clear_this_section_mask = ~(
                    RichFontVarBitFlag::push |
                    RichFontVarBitFlag::pop |
                    RichFontVarBitFlag::faceColor |
                    RichFontVarBitFlag::strokeColor |
                    RichFontVarBitFlag::drawFace |
                    RichFontVarBitFlag::drawStroke |
                    RichFontVarBitFlag::lineHeight |
                    RichFontVarBitFlag::size);
                flag &= clear_this_section_mask;

                auto atributes = ITKCommon::StringUtil::tokenizer(ITKCommon::StringUtil::utf32_to_utf8(json), ";");
                for (const auto &atribute : atributes)
                {
                    auto var_set = ITKCommon::StringUtil::tokenizer(atribute, ":");
                    if (var_set.size() == 1)
                    {
                        auto cmd = ITKCommon::StringUtil::trim(var_set[0]);
                        // push/pop
                        if (cmd.compare("push") == 0)
                        {
                            push();
                            flag |= RichFontVarBitFlag::push;
                        }
                        else if (cmd.compare("pop") == 0)
                        {
                            pop();
                            flag |= RichFontVarBitFlag::pop;
                        }
                    }
                    else if (var_set.size() == 2)
                    {
                        auto cmd = ITKCommon::StringUtil::trim(var_set[0]);
                        // auto value = ITKCommon::StringUtil::toLower( ITKCommon::StringUtil::trim(var_set[1]) );
                        std::string value = ITKCommon::StringUtil::trim(var_set[1]);
                        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

                        if (cmd.compare("faceColor") == 0)
                        {
                            MathCore::vec4u8 v;
                            if (sscanf(value.c_str(), "%x", (uint32_t *)v.array) == 1)
                            {
                                style.faceColor = (MathCore::vec4f)v / 255.0f;
                                if (use_srgb)
                                    style.faceColor = MathCore::CVT<MathCore::vec4f>::sRGBToLinear(style.faceColor);
                                flag |= RichFontVarBitFlag::faceColor;
                            }
                        }
                        else if (cmd.compare("strokeColor") == 0)
                        {
                            MathCore::vec4u8 v;
                            if (sscanf(value.c_str(), "%x", (uint32_t *)v.array) == 1)
                            {
                                style.strokeColor = (MathCore::vec4f)v / 255.0f;
                                if (use_srgb)
                                    style.strokeColor = MathCore::CVT<MathCore::vec4f>::sRGBToLinear(style.strokeColor);
                                flag |= RichFontVarBitFlag::strokeColor;
                            }
                        }
                        else if (cmd.compare("face") == 0)
                        {
                            style.drawFace = value.compare("1") == 0 || value.compare("true") == 0 || value.compare("on") == 0;
                            flag |= RichFontVarBitFlag::drawFace;
                        }
                        else if (cmd.compare("stroke") == 0)
                        {
                            style.drawStroke = value.compare("1") == 0 || value.compare("true") == 0 || value.compare("on") == 0;
                            flag |= RichFontVarBitFlag::drawStroke;
                        }
                        else if (cmd.compare("lineHeight") == 0)
                        {
                            if (sscanf(value.c_str(), "%f", &style.lineHeight) == 1)
                            {
                                flag |= RichFontVarBitFlag::lineHeight;
                                style.lineHeight_scaled = builder->glFont2.new_line_height * style.size_scaler * style.lineHeight;
                            }
                        }
                        else if (cmd.compare("size") == 0)
                        {
                            if (sscanf(value.c_str(), "%f", &style.size) == 1)
                            {
                                flag |= RichFontVarBitFlag::size;

                                style.size_scaler = style.size / builder->glFont2.size;
                                style.lineHeight_scaled = builder->glFont2.new_line_height * style.size_scaler * style.lineHeight;
                                style.render_size_space_width = style.size_scaler * builder->glFont2.space_width;
                            }
                        }
                    }
                }
            }
        };

        class RichTokenizer
        {
            int brace_count;
            const char32_t *current;
            const char32_t *start;

            bool latest_insert_new_line;
            RichFontState fontState;

            const GLFont2Builder *builder;

            bool use_srgb;

            float current_line_width;

            std::vector<RichFontState> fontStateBuffer;
            int fontStateBuffer_read_idx;
            int latest_inserted_word_index;

            void word_wrap_add_current_glyph(float max_width, char32_t wordSeparatorChar, const GLFont2 &glFont2, const GLFont2WrapMode &wrapMode, RichFontState *&to_return)
            {
                float char_size_x_scaled = 0;
                if (*current == U' ')
                    char_size_x_scaled = glFont2.space_width * fontState.style.size_scaler;
                else if (auto glyph = glFont2.getGlyph((uint32_t)*current))
                    char_size_x_scaled = glyph->advancex.x * fontState.style.size_scaler;

                // test max width with one character mode
                if (char_size_x_scaled > 0)
                {
                    float current_line_width_before_insert = this->current_line_width;

                    bool guarantee_more_than_one_char = (this->current_line_width > 0);
                    this->current_line_width += char_size_x_scaled;

                    // wordSeparatorChar
                    if (wrapMode == GLFont2WrapMode::GLFont2WrapMode_Character)
                    {
                        if (guarantee_more_than_one_char && this->current_line_width > max_width)
                        {
                            this->current_line_width = 0;
                            // force -> new line break
                            latest_insert_new_line = true;
                            // new line in the raw text content
                            fontState.setText(std::u32string(start, current), true, glFont2);
                            to_return = &fontState;

                            // back one char to reinsert it in the next iteration
                            current--;
                            start = current + 1;
                        }
                    }
                    else if (wrapMode == GLFont2WrapMode::GLFont2WrapMode_Word)
                    {
                        float width_to_check = (*current == wordSeparatorChar) ? current_line_width_before_insert : this->current_line_width;
                        // float width_to_check = this->current_line_width;

                        if (latest_inserted_word_index >= 0 && width_to_check > max_width)
                        {
                            // mark new_line in the latest word already breaked
                            auto &element = fontStateBuffer[latest_inserted_word_index];
                            element.setText(std::u32string(element.str.begin(), element.str.end() - 1), true, builder->glFont2);

                            // recompute the current_line_width
                            int new_latest_idx = -1;
                            this->current_line_width = 0;
                            for (int j = latest_inserted_word_index + 1; j < (int)fontStateBuffer.size(); j++)
                            {
                                auto &item = fontStateBuffer[j];
                                this->current_line_width += item.bounds_with_size_applied.x;
                                if (item.str.length() > 0 && item.str[item.str.length() - 1] == wordSeparatorChar)
                                    new_latest_idx = j;
                            }
                            auto aux = start;
                            while (aux <= current)
                            {
                                if (*aux == U' ')
                                    this->current_line_width += glFont2.space_width * fontState.style.size_scaler;
                                else if (auto glyph = glFont2.getGlyph((uint32_t)*aux))
                                    this->current_line_width += glyph->advancex.x * fontState.style.size_scaler;
                                aux++;
                            }
                            latest_inserted_word_index = new_latest_idx;
                        }

                        if (*current == wordSeparatorChar)
                        {
                            // break each word
                            // new line in the raw text content
                            latest_insert_new_line = false;
                            fontState.setText(std::u32string(start, current + 1), false, glFont2);
                            to_return = &fontState;
                            start = current + 1;

                            latest_inserted_word_index = (int)fontStateBuffer.size();
                        }
                    }
                }
            }

            RichFontState *raw_next(float max_width = -1)
            {
                // force reset state on after new line case
                // if ((fontState.flag & RichFontVarBitFlag::new_line_at_end) != RichFontVarBitFlag::none)
                fontState.ReadStyleCodeString(std::u32string(U""), use_srgb);

                RichFontState *to_return = nullptr;

                const GLFont2 &glFont2 = builder->glFont2;
                const GLFont2WrapMode &wrapMode = builder->wrapMode;
                char32_t wordSeparatorChar = builder->wordSeparatorChar;
                while (*current && to_return == nullptr)
                {
                    if (brace_count == 0 &&
                        ((*current == U'{' && *(current + 1) == U'{') ||
                         (*current == U'}' && *(current + 1) == U'}')))
                    {

                        if (max_width >= 0)
                        {
                            if (wrapMode == GLFont2WrapMode::GLFont2WrapMode_Word)
                            {
                                word_wrap_add_current_glyph(max_width, wordSeparatorChar, glFont2, wrapMode, to_return);
                            }
                            else if (wrapMode == GLFont2WrapMode::GLFont2WrapMode_Character)
                            {
                                // to check if a character was back_inserted...
                                //  in this case, we need to back the pointer twice.
                                auto current_before = current;
                                word_wrap_add_current_glyph(max_width, wordSeparatorChar, glFont2, wrapMode, to_return);
                                if (current < current_before)
                                    current--;
                            }
                        }

                        // skip this char
                        current++;
                        // two consecutives braces... not a style modifier
                        if (current > start)
                        {
                            if (to_return == nullptr)
                            {
                                latest_insert_new_line = false;
                                fontState.setText(std::u32string(start, current), false, glFont2);
                                to_return = &fontState;
                            }
                            start = current + 1;
                        }
                    }
                    else if (*current == U'{')
                    {
                        if (brace_count == 0 && current > start)
                        {
                            latest_insert_new_line = false;
                            // raw text content
                            fontState.setText(std::u32string(start, current), false, glFont2);
                            to_return = &fontState;
                        }
                        if (brace_count == 0)
                            start = current + 1;
                        brace_count++;
                    }
                    else if (*current == U'}')
                    {
                        brace_count--;
                        if (brace_count == 0)
                        {
                            // inside curly braces text content
                            fontState.ReadStyleCodeString(std::u32string(start, current), use_srgb);
                            start = current + 1;
                        }
                    }
                    else if (brace_count == 0)
                    {
                        // raw string processing
                        if (*current == U'\n')
                        {
                            this->current_line_width = 0;
                            latest_insert_new_line = true;
                            // new line in the raw text content
                            fontState.setText(std::u32string(start, current), true, glFont2);
                            to_return = &fontState;
                            start = current + 1;
                        }
                        else if ((max_width >= 0) && (wrapMode == GLFont2WrapMode::GLFont2WrapMode_Character ||
                                                      wrapMode == GLFont2WrapMode::GLFont2WrapMode_Word))
                        {
                            word_wrap_add_current_glyph(max_width, wordSeparatorChar, glFont2, wrapMode, to_return);
                        }
                    }
                    current++;
                }

                // special case:
                //   empty return after a new line character
                //   *current == U'\0'
                //   current == start
                if ((to_return == nullptr) && (*current == U'\0') && (current == start) && latest_insert_new_line)
                {
                    latest_insert_new_line = false;
                    // new line in the raw text content
                    fontState.setText(std::u32string(start, current), true, glFont2);
                    to_return = &fontState;
                }

                // put last part of the string
                if (!to_return && current > start)
                {
                    // raw text content
                    fontState.setText(std::u32string(start, current), false, glFont2);
                    to_return = &fontState;
                    // force while to eof
                    start = current;
                }

                return to_return;
            }

        public:
            RichTokenizer(const char32_t *str, const GLFont2Builder *initial_state, bool use_srgb) : builder(initial_state)
            {
                this->fontState = RichFontState::Create(initial_state, use_srgb);
                this->current = str;
                this->start = current;
                this->brace_count = 0;
                this->use_srgb = use_srgb;
                this->latest_insert_new_line = false;
                this->current_line_width = 0;
                latest_inserted_word_index = -1;
                fontStateBuffer_read_idx = 0;
            }

            // need this for the m_initial_state.wrapMode == GLFont2WrapMode_Word
            RichFontState *next(float max_width = -1)
            {
                if (max_width >= 0 && builder->wrapMode == GLFont2WrapMode_Word)
                {
                    if (fontStateBuffer_read_idx >= fontStateBuffer.size())
                    {
                        fontStateBuffer_read_idx = 0;
                        fontStateBuffer.clear();
                    }

                    if (fontStateBuffer.size() > 0)
                    {
                        auto result = &fontStateBuffer[fontStateBuffer_read_idx];
                        fontStateBuffer_read_idx++;
                        return result;
                    }
                    else
                    {
                        latest_inserted_word_index = -1;
                        // read entire line
                        while (auto next_token = raw_next(max_width))
                        {
                            fontStateBuffer.push_back(*next_token);
                            if ((next_token->flag & RichFontVarBitFlag::new_line_at_end) != RichFontVarBitFlag::none)
                                break;
                        }
                        // return the first token
                        if (fontStateBuffer.size() > 0)
                        {
                            auto result = &fontStateBuffer[fontStateBuffer_read_idx];
                            fontStateBuffer_read_idx++;
                            return result;
                        }
                        else
                            return nullptr;
                    }
                }
                else
                    return raw_next(max_width);
            }
        };

        CollisionCore::AABB<MathCore::vec3f> GLFont2Builder::u32RichComputeBox(const char32_t *str, float max_width)
        {
            CollisionCore::AABB<MathCore::vec3f> result;

            // std::vector<std::vector<RichFontState>> lines;
            float max_height_1stline_scaled = 0;
            float max_line_height_1stline_scaled = 0;
            float max_line_height_others_scaled = 0;
            bool _1st_line_processing = true;
            {
                RichTokenizer tokenizer(str, this, false);
                // std::vector<RichFontState> line;
                float max_lineHeight_scaled = 0;
                bool token_readed_but_not_used = false;

                float total_length = 0;
                while (auto token = tokenizer.next(max_width))
                {
                    if (_1st_line_processing)
                    {
                        max_height_1stline_scaled = MathCore::OP<float>::maximum(max_height_1stline_scaled, token->bounds_with_size_applied.y);
                        max_line_height_1stline_scaled = MathCore::OP<float>::maximum(max_line_height_1stline_scaled, token->style.lineHeight_scaled);
                    }
                    else
                        max_lineHeight_scaled = MathCore::OP<float>::maximum(max_lineHeight_scaled, token->style.lineHeight_scaled);
                    // line.push_back(*token);
                    token_readed_but_not_used = true;
                    total_length += token->bounds_with_size_applied.x;

                    if ((token->flag & RichFontVarBitFlag::new_line_at_end) != RichFontVarBitFlag::none)
                    {
                        max_line_height_others_scaled += max_lineHeight_scaled;
                        max_lineHeight_scaled = 0;
                        _1st_line_processing = false;
                        // lines.push_back(std::move(line));
                        // line.clear();
                        token_readed_but_not_used = false;

                        if (max_width >= 0)
                            total_length = max_width;

                        // make x axis processing
                        if (horizontalAlign == GLFont2HorizontalAlign_left)
                        {
                            result.min_box.x = 0;
                            result.max_box.x = MathCore::OP<float>::maximum(result.max_box.x, total_length);
                        }
                        else if (horizontalAlign == GLFont2HorizontalAlign_center)
                        {
                            float half_total_length = total_length * 0.5f;
                            result.min_box.x = MathCore::OP<float>::minimum(result.min_box.x, -half_total_length);
                            result.max_box.x = MathCore::OP<float>::maximum(result.max_box.x, half_total_length);
                        }
                        else if (horizontalAlign == GLFont2HorizontalAlign_right)
                        {
                            result.min_box.x = MathCore::OP<float>::minimum(result.min_box.x, -total_length);
                            result.max_box.x = 0;
                        }

                        total_length = 0;
                    }
                }
                // if (line.size() > 0)
                if (token_readed_but_not_used)
                {
                    max_line_height_others_scaled += max_lineHeight_scaled;
                    max_lineHeight_scaled = 0;
                    // lines.push_back(std::move(line));

                    if (max_width >= 0)
                        total_length = max_width;

                    // make x axis processing
                    if (horizontalAlign == GLFont2HorizontalAlign_left)
                    {
                        result.min_box.x = 0;
                        result.max_box.x = MathCore::OP<float>::maximum(result.max_box.x, total_length);
                    }
                    else if (horizontalAlign == GLFont2HorizontalAlign_center)
                    {
                        float half_total_length = total_length * 0.5f;
                        result.min_box.x = MathCore::OP<float>::minimum(result.min_box.x, -half_total_length);
                        result.max_box.x = MathCore::OP<float>::maximum(result.max_box.x, half_total_length);
                    }
                    else if (horizontalAlign == GLFont2HorizontalAlign_right)
                    {
                        result.min_box.x = MathCore::OP<float>::minimum(result.min_box.x, -total_length);
                        result.max_box.x = 0;
                    }
                }
            }

            if (firstLineHeightMode == GLFont2FirstLineHeightMode_UseCharacterMaxHeight)
                max_line_height_1stline_scaled = max_height_1stline_scaled;

            if (verticalAlign == GLFont2VerticalAlign_baseline)
            {
                result.max_box.y = max_line_height_1stline_scaled;
                result.min_box.y = -max_line_height_others_scaled;
            }
            else if (verticalAlign == GLFont2VerticalAlign_top)
            {
                result.max_box.y = 0;
                result.min_box.y = -max_line_height_others_scaled - max_line_height_1stline_scaled;
            }
            else if (verticalAlign == GLFont2VerticalAlign_middle)
            {
                float size = (max_line_height_others_scaled + max_line_height_1stline_scaled) * 0.5f;
                result.max_box.y = +size;
                result.min_box.y = -size;
            }
            else if (verticalAlign == GLFont2VerticalAlign_bottom)
            {
                result.max_box.y = max_line_height_others_scaled + max_line_height_1stline_scaled;
                result.min_box.y = 0;
            }

            return result;
        }
        CollisionCore::AABB<MathCore::vec3f> GLFont2Builder::richComputeBox(const char *str, float max_width)
        {
            return u32RichComputeBox(ITKCommon::StringUtil::utf8_to_utf32(str).c_str(), max_width);
        }

        GLFont2Builder *GLFont2Builder::u32richBuild(const char32_t *str, bool use_srgb, float max_width, std::shared_ptr<GLFont2PolygonCache> useThisPolygons)
        {
            vertexAttrib.clear();
            triangles.clear();
            if (str == nullptr || *str == U'\0')
                return this;

            // organize lines
            std::vector<std::vector<RichFontState>> lines;
            float max_height_1stline_scaled = 0;
            float max_line_height_1stline_scaled = 0;
            float max_line_height_others_scaled = 0;
            bool _1st_line_processing = true;
            {
                RichTokenizer tokenizer(str, this, use_srgb);
                std::vector<RichFontState> line;
                float max_lineHeight_scaled = 0;
                while (auto token = tokenizer.next(max_width))
                {
                    if (_1st_line_processing)
                    {
                        max_height_1stline_scaled = MathCore::OP<float>::maximum(max_height_1stline_scaled, token->bounds_with_size_applied.y);
                        max_line_height_1stline_scaled = MathCore::OP<float>::maximum(max_line_height_1stline_scaled, token->style.lineHeight_scaled);
                    }
                    else
                        max_lineHeight_scaled = MathCore::OP<float>::maximum(max_lineHeight_scaled, token->style.lineHeight_scaled);
                    line.push_back(*token);
                    if ((token->flag & RichFontVarBitFlag::new_line_at_end) != RichFontVarBitFlag::none)
                    {
                        max_line_height_others_scaled += max_lineHeight_scaled;
                        max_lineHeight_scaled = 0;
                        _1st_line_processing = false;
                        lines.push_back(std::move(line));
                        line.clear();
                    }
                }
                if (line.size() > 0)
                {
                    max_line_height_others_scaled += max_lineHeight_scaled;
                    max_lineHeight_scaled = 0;
                    lines.push_back(std::move(line));
                }
            }

            // each line processing
            MathCore::vec3f position = MathCore::vec3f(0);
            RichFontState state = RichFontState::Create(this, use_srgb);

            if (firstLineHeightMode == GLFont2FirstLineHeightMode_UseCharacterMaxHeight)
                max_line_height_1stline_scaled = max_height_1stline_scaled;

            if (verticalAlign == GLFont2VerticalAlign_top)
                position.y = -max_line_height_1stline_scaled; // position.y = -max_height_1stline_scaled;
            else if (verticalAlign == GLFont2VerticalAlign_middle)
                position.y = (-max_line_height_1stline_scaled + max_line_height_others_scaled) * 0.5f;
            else if (verticalAlign == GLFont2VerticalAlign_bottom)
                position.y = max_line_height_others_scaled;

            bool first_line = true;

            for (const auto &line : lines)
            {
                float total_length = 0;
                float max_lineHeight_scaled = 0;
                for (const auto &block : line)
                {
                    total_length += block.bounds_with_size_applied.x;
                    max_lineHeight_scaled = MathCore::OP<float>::maximum(max_lineHeight_scaled, block.style.lineHeight_scaled);
                }

                // increment y according the latest line max height
                if (!first_line)
                    position.y -= max_lineHeight_scaled;

                first_line = false;

                // x alignment
                if (horizontalAlign == GLFont2HorizontalAlign_left)
                    position.x = 0;
                else if (horizontalAlign == GLFont2HorizontalAlign_center)
                    position.x = -total_length * 0.5f;
                else if (horizontalAlign == GLFont2HorizontalAlign_right)
                    position.x = -total_length;

                for (const auto &block : line)
                {
                    // apply block modifications to the current state
                    if ((block.flag & RichFontVarBitFlag::push) != RichFontVarBitFlag::none)
                        state.push();
                    if ((block.flag & RichFontVarBitFlag::pop) != RichFontVarBitFlag::none)
                        state.pop();
                    state.applyChange(block);

                    // each character in the block text
                    for (const auto &c : block.str)
                    {
                        if (c == U' ')
                            position.x += state.style.render_size_space_width;
                        else if (useThisPolygons != nullptr)
                        {
                            if (auto glyph = useThisPolygons->getGlyph((uint32_t)c))
                            {
                                float scaler = state.style.size / useThisPolygons->size;
                                if (glyph->triangles.size() >= 3 && glyph->vertices.size() >= 3)
                                {
                                    uint32_t base_offset = (uint32_t)vertexAttrib.size();
                                    for (const auto &v : glyph->vertices)
                                        vertexAttrib.push_back(GLFont2Builder_VertexAttrib(
                                            v * scaler + position,
                                            MathCore::vec2f(0),
                                            state.style.faceColor));
                                    for (const auto &t : glyph->triangles)
                                        triangles.push_back(t + base_offset);
                                }
                                position.x += scaler * glyph->advancex.x;
                            }
                        }
                        else if (auto glyph = glFont2.getGlyph((uint32_t)c))
                        {
                            if (state.style.drawFace)
                                GLFont2BitmapRef_to_VertexAttrib(position, state.style.faceColor, glyph->face, state.style.size_scaler);
                            if (state.style.drawStroke)
                                GLFont2BitmapRef_to_VertexAttrib(position + strokeOffset, state.style.strokeColor, glyph->stroke, state.style.size_scaler);
                            position.x += state.style.size_scaler * glyph->advancex.x;
                        }
                    }
                }
            }

            return this;
        }

        GLFont2Builder *GLFont2Builder::richBuild(const char *utf8_str, bool use_srgb, float max_width, std::shared_ptr<GLFont2PolygonCache> useThisPolygons)
        {
            u32richBuild(ITKCommon::StringUtil::utf8_to_utf32(utf8_str).c_str(), use_srgb, max_width, useThisPolygons);
            return this;
        }

        std::shared_ptr<GLFont2PolygonCache> GLFont2Builder::createPolygonCache(float size, float max_distance_tolerance, Platform::ThreadPool *threadPool) const
        {
            auto polygonCache = std::make_shared<GLFont2PolygonCache>();
            polygonCache->setFromGLFont2(glFont2, size, max_distance_tolerance, threadPool);
            return polygonCache;
        }

        bool GLFont2Builder::isConstructedFromPolygonCache() const
        {
            return triangles.size() > 0 && vertexAttrib.size() > 0;
        }

    }
}
