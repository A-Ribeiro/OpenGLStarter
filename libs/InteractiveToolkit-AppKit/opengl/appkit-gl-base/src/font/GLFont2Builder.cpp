#include <appkit-gl-base/font/GLFont2Builder.h>

// using namespace aRibeiro;

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

        void GLFont2Builder::GLFont2BitmapRef_to_VertexAttrib(const MathCore::vec3f &pos, const MathCore::vec4f &color, const GLFont2BitmapRef &bitmapRef)
        {
            if (bitmapRef.bitmapBounds.w == 0 || bitmapRef.bitmapBounds.h == 0)
                return;
            MathCore::vec3f pos_new = pos + bitmapRef.move_before_draw;
            for (int i = 0; i < 6; i++)
            {
                vertexAttrib.push_back(
                    GLFont2Builder_VertexAttrib(
                        bitmapRef.triangles.position[i] + pos_new,
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
            verticalAlign = GLFont2VerticalAlign_none;

            lineHeight = 1.0;
        }

        void GLFont2Builder::load(const std::string &filename, bool force_srgb)
        {
            glFont2.loadFromBasof2(filename, force_srgb);
        }

        void GLFont2Builder::u32ComputeBox(const char32_t *str, float *xmin, float *xmax, float *ymin, float *ymax)
        {
            //size_t count = wcslen(str);

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
                ITKCommon::StringUtil::utf8_to_utf32(str).c_str()
                , xmin, xmax, ymin, ymax);
        }

        GLFont2Builder *GLFont2Builder::u32build(const char32_t *str)
        {
            //size_t count = wcslen(str);
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
                ITKCommon::StringUtil::utf8_to_utf32(utf8_str).c_str()
            );
            return this;
        }

    }
}
