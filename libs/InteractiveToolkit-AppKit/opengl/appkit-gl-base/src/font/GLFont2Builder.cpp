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

        void GLFont2Builder::countNewLines_1stlineHeight_1stlineLength(const char32_t *str, int size, int *_newLineCount, float *_1stLineMaxHeight, float *_1stLinelength)
        {
            float length = 0;
            float height = 0;
            int new_line_count = 0;
            char32_t c;
            int i;
            for (i = 0; i < size; i++)
            {
                c = str[i];
                if (c == U' ')
                    length += glFont2.space_width;
                else if (c == U'\n')
                    break;
                else
                {
                    const GLFont2Glyph *glyph = glFont2.getGlyph(str[i]);
                    if (glyph != nullptr)
                    {
                        length += glyph->advancex.x;
                        if (glyph->face.height > height)
                            height = glyph->face.height;
                    }
                }
            }
            *_1stLineMaxHeight = height;
            *_1stLinelength = length;

            for (; i < size; i++)
            {
                c = str[i];
                if (c == U'\n')
                    new_line_count++;
            }

            *_newLineCount = new_line_count;
        }

        float GLFont2Builder::computeStringLengthUntilNewLine(const char32_t *str, int size, int offset)
        {
            float length = 0;
            char32_t c;
            for (int i = offset; i < size; i++)
            {
                c = str[i];
                if (c == U' ')
                    length += glFont2.space_width;
                else if (c == U'\n')
                    break;
                else
                {
                    const GLFont2Glyph *glyph = glFont2.getGlyph(str[i]);
                    if (glyph != nullptr)
                        length += glyph->advancex.x;
                }
            }
            return length;
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
        }

        void GLFont2Builder::load(const std::string &filename, bool force_srgb)
        {
            glFont2.loadFromBasof2(filename, force_srgb);
            size = glFont2.size;
        }

        void GLFont2Builder::u32ComputeBox(const char32_t *str, float *xmin, float *xmax, float *ymin, float *ymax)
        {
            // size_t count = wcslen(str);

            int count = (int)std::char_traits<char32_t>::length(str);

            char32_t c;
            MathCore::vec3f position = MathCore::vec3f(0);

            int _newLineCount = 0;
            float _1stLineMaxHeight = 0;
            float _1stLinelength = 0;

            countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);

            if (verticalAlign == GLFont2VerticalAlign_top)
            {
                // countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
                position.y = -_1stLineMaxHeight;
            }
            else if (verticalAlign == GLFont2VerticalAlign_middle)
            {
                // countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
                float totalSize = (float)_newLineCount * glFont2.new_line_height * lineHeight;
                position.y = -_1stLineMaxHeight * 0.5f + totalSize * 0.5f;
            }
            else if (verticalAlign == GLFont2VerticalAlign_bottom)
            {
                // countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
                float totalSize = (float)_newLineCount * glFont2.new_line_height * lineHeight;
                position.y = totalSize;
            }

            if (horizontalAlign == GLFont2HorizontalAlign_center)
            {
                position.x = (_1stLinelength != 0) ? (-_1stLinelength * 0.5f) : (-computeStringLengthUntilNewLine(str, count, 0) * 0.5f);
            }
            else if (horizontalAlign == GLFont2HorizontalAlign_right)
            {
                position.x = (_1stLinelength != 0) ? (-_1stLinelength) : (-computeStringLengthUntilNewLine(str, count, 0));
            }

            *xmin = position.x;
            *xmax = position.x;
            *ymin = position.y;
            *ymax = position.y + _1stLineMaxHeight;

            for (int i = 0; i < count; i++)
            {
                c = str[i];

                if (c == U' ')
                {
                    position.x += glFont2.space_width;

                    *xmax = MathCore::OP<float>::maximum(*xmax, position.x);
                }
                else if (c == U'\n')
                {
                    if (horizontalAlign == GLFont2HorizontalAlign_left)
                    {
                        position.x = 0;
                    }
                    else if (horizontalAlign == GLFont2HorizontalAlign_center)
                    {
                        position.x = -computeStringLengthUntilNewLine(str, count, i + 1) * 0.5f;
                        *xmin = MathCore::OP<float>::minimum(*xmin, position.x);
                        //*xmax = maximum( *xmax, position.x );
                    }
                    else if (horizontalAlign == GLFont2HorizontalAlign_right)
                    {
                        position.x = -computeStringLengthUntilNewLine(str, count, i + 1);
                        *xmin = MathCore::OP<float>::minimum(*xmin, position.x);
                        //*xmax = maximum( *xmax, position.x );
                    }
                    position.y -= glFont2.new_line_height * lineHeight;

                    *ymin = MathCore::OP<float>::minimum(*ymin, position.y);
                    //*ymax = maximum( *ymin, position.y );
                }
                else
                {
                    const GLFont2Glyph *glyph = glFont2.getGlyph(c);
                    if (glyph != nullptr)
                    {
                        position.x += glyph->advancex.x;

                        //*xmin = minimum( *xmin, position.x );
                        *xmax = MathCore::OP<float>::maximum(*xmax, position.x);
                    }
                }
            }
        }

        void GLFont2Builder::computeBox(const char *str, float *xmin, float *xmax, float *ymin, float *ymax)
        {
            u32ComputeBox(
                ITKCommon::StringUtil::utf8_to_utf32(str).c_str(), xmin, xmax, ymin, ymax);
        }

        GLFont2Builder *GLFont2Builder::u32build(const char32_t *str)
        {
            // size_t count = wcslen(str);
            int count = (int)std::char_traits<char32_t>::length(str);

            vertexAttrib.clear();

            char32_t c;
            MathCore::vec3f position = MathCore::vec3f(0);

            int _newLineCount = 0;
            float _1stLineMaxHeight = 0;
            float _1stLinelength = 0;

            if (verticalAlign == GLFont2VerticalAlign_top)
            {
                countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
                position.y = -_1stLineMaxHeight;
            }
            else if (verticalAlign == GLFont2VerticalAlign_middle)
            {
                countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
                float totalSize = (float)_newLineCount * glFont2.new_line_height * lineHeight;
                position.y = -_1stLineMaxHeight * 0.5f + totalSize * 0.5f;
            }
            else if (verticalAlign == GLFont2VerticalAlign_bottom)
            {
                countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
                float totalSize = (float)_newLineCount * glFont2.new_line_height * lineHeight;
                position.y = totalSize;
            }

            if (horizontalAlign == GLFont2HorizontalAlign_center)
            {
                position.x = (_1stLinelength != 0) ? (-_1stLinelength * 0.5f) : (-computeStringLengthUntilNewLine(str, count, 0) * 0.5f);
            }
            else if (horizontalAlign == GLFont2HorizontalAlign_right)
            {
                position.x = (_1stLinelength != 0) ? (-_1stLinelength) : (-computeStringLengthUntilNewLine(str, count, 0));
            }

            for (int i = 0; i < count; i++)
            {
                c = str[i];

                if (c == U' ')
                {
                    position.x += glFont2.space_width;
                }
                else if (c == U'\n')
                {
                    if (horizontalAlign == GLFont2HorizontalAlign_left)
                    {
                        position.x = 0;
                    }
                    else if (horizontalAlign == GLFont2HorizontalAlign_center)
                    {
                        position.x = -computeStringLengthUntilNewLine(str, count, i + 1) * 0.5f;
                    }
                    else if (horizontalAlign == GLFont2HorizontalAlign_right)
                    {
                        position.x = -computeStringLengthUntilNewLine(str, count, i + 1);
                    }
                    position.y -= glFont2.new_line_height * lineHeight;
                }
                else
                {
                    const GLFont2Glyph *glyph = glFont2.getGlyph(c);
                    if (glyph != nullptr)
                    {

                        if (drawFace)
                            GLFont2BitmapRef_to_VertexAttrib(position, faceColor, glyph->face);
                        if (drawStroke)
                            GLFont2BitmapRef_to_VertexAttrib(position + strokeOffset, strokeColor, glyph->stroke);

                        position.x += glyph->advancex.x;
                    }
                }
            }

            return this;
        }

        GLFont2Builder *GLFont2Builder::build(const char *utf8_str)
        {
            u32build(
                ITKCommon::StringUtil::utf8_to_utf32(utf8_str).c_str());
            return this;
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

            const GLFont2Builder &builder;

            RichFontState(const GLFont2Builder &initial_state, bool use_srgb) : builder(initial_state)
            {
                style.faceColor = initial_state.faceColor;
                style.strokeColor = initial_state.strokeColor;

                if (use_srgb)
                {
                    style.faceColor = MathCore::CVT<MathCore::vec4f>::sRGBToLinear(style.faceColor);
                    style.strokeColor = MathCore::CVT<MathCore::vec4f>::sRGBToLinear(style.strokeColor);
                }

                style.drawFace = initial_state.drawFace;
                style.drawStroke = initial_state.drawStroke;

                style.lineHeight = initial_state.lineHeight;
                style.size = initial_state.size;

                flag = RichFontVarBitFlag::none;

                style.size_scaler = style.size / initial_state.glFont2.size;
                style.lineHeight_scaled = initial_state.glFont2.new_line_height * style.size_scaler * style.lineHeight;
                style.render_size_space_width = style.size_scaler * initial_state.glFont2.space_width;
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
                computeLength(glFont2);
            }

            void computeLength(const GLFont2 &glFont2)
            {
                float length = 0;
                float max_height = 0;
                for (auto c : str)
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
                        std::transform(value.begin(), value.end(), value.begin(), std::tolower);

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
                                style.lineHeight_scaled = builder.glFont2.new_line_height * style.size_scaler * style.lineHeight;
                            }
                        }
                        else if (cmd.compare("size") == 0)
                        {
                            if (sscanf(value.c_str(), "%f", &style.size) == 1)
                            {
                                flag |= RichFontVarBitFlag::size;

                                style.size_scaler = style.size / builder.glFont2.size;
                                style.lineHeight_scaled = builder.glFont2.new_line_height * style.size_scaler * style.lineHeight;
                                style.render_size_space_width = style.size_scaler * builder.glFont2.space_width;
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

            const GLFont2Builder &m_initial_state;

            bool use_srgb;

        public:
            RichTokenizer(const char32_t *str, const GLFont2Builder &initial_state, bool use_srgb) : fontState(initial_state, use_srgb),
                                                                                                     m_initial_state(initial_state)
            {
                this->current = str;
                this->start = current;
                this->brace_count = 0;
                this->use_srgb = use_srgb;
                this->latest_insert_new_line = false;
            }

            RichFontState *next()
            {

                // force reset state on after new line case
                // if ((fontState.flag & RichFontVarBitFlag::new_line_at_end) != RichFontVarBitFlag::none)
                fontState.ReadStyleCodeString(std::u32string(U""), use_srgb);

                RichFontState *to_return = nullptr;

                const GLFont2 &glFont2 = m_initial_state.glFont2;
                while (*current && to_return == nullptr)
                {
                    if (brace_count == 0 &&
                        ((*current == U'{' && *(current + 1) == U'{') ||
                         (*current == U'}' && *(current + 1) == U'}')))
                    {
                        // skip this char
                        current++;
                        // two consecutives braces... not a style modifier
                        if (current > start)
                        {
                            latest_insert_new_line = false;
                            fontState.setText(std::u32string(start, current), false, glFont2);
                            to_return = &fontState;
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
                            latest_insert_new_line = true;
                            // new line in the raw text content
                            fontState.setText(std::u32string(start, current), true, glFont2);
                            to_return = &fontState;
                            start = current + 1;
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
                if (current > start)
                {
                    // raw text content
                    fontState.setText(std::u32string(start, current), false, glFont2);
                    to_return = &fontState;
                    // force while to eof
                    start = current;
                }

                return to_return;
            }
        };

        CollisionCore::AABB<MathCore::vec3f> GLFont2Builder::u32RichComputeBox(const char32_t *str)
        {
            CollisionCore::AABB<MathCore::vec3f> result;

            // std::vector<std::vector<RichFontState>> lines;
            float max_height_1stline_scaled = 0;
            float max_line_height_1stline_scaled = 0;
            float max_line_height_others_scaled = 0;
            bool _1st_line_processing = true;
            {
                RichTokenizer tokenizer(str, *this, false);
                // std::vector<RichFontState> line;
                float max_lineHeight_scaled = 0;
                bool token_readed_but_not_used = false;

                float total_length = 0;
                while (auto token = tokenizer.next())
                {
                    if (_1st_line_processing)
                    {
                        max_height_1stline_scaled = MathCore::OP<float>::maximum(max_height_1stline_scaled, token->bounds_with_size_applied.y);
                        max_line_height_1stline_scaled = MathCore::OP<float>::maximum(max_line_height_1stline_scaled, token->style.lineHeight_scaled);
                    }
                    else
                    {
                        max_lineHeight_scaled = MathCore::OP<float>::maximum(max_lineHeight_scaled, token->style.lineHeight_scaled);
                    }
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
        CollisionCore::AABB<MathCore::vec3f> GLFont2Builder::richComputeBox(const char *str)
        {
            return u32RichComputeBox(ITKCommon::StringUtil::utf8_to_utf32(str).c_str());
        }

        GLFont2Builder *GLFont2Builder::u32richBuild(const char32_t *str, bool use_srgb)
        {
            vertexAttrib.clear();

            // organize lines
            std::vector<std::vector<RichFontState>> lines;
            float max_height_1stline_scaled = 0;
            float max_line_height_1stline_scaled = 0;
            float max_line_height_others_scaled = 0;
            bool _1st_line_processing = true;
            {
                RichTokenizer tokenizer(str, *this, use_srgb);
                std::vector<RichFontState> line;
                float max_lineHeight_scaled = 0;
                while (auto token = tokenizer.next())
                {
                    if (_1st_line_processing)
                    {
                        max_height_1stline_scaled = MathCore::OP<float>::maximum(max_height_1stline_scaled, token->bounds_with_size_applied.y);
                        max_line_height_1stline_scaled = MathCore::OP<float>::maximum(max_line_height_1stline_scaled, token->style.lineHeight_scaled);
                    }
                    else
                    {
                        max_lineHeight_scaled = MathCore::OP<float>::maximum(max_lineHeight_scaled, token->style.lineHeight_scaled);
                    }
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
            RichFontState state(*this, use_srgb);

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

        GLFont2Builder *GLFont2Builder::richBuild(const char *utf8_str, bool use_srgb)
        {
            u32richBuild(ITKCommon::StringUtil::utf8_to_utf32(utf8_str).c_str(), use_srgb);
            return this;
        }

    }
}
