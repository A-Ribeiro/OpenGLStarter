#include "GLFont2Builder.h"

using namespace aRibeiro;

namespace openglWrapper {

    GLFont2Builder_VertexAttrib::GLFont2Builder_VertexAttrib() {}
    GLFont2Builder_VertexAttrib::GLFont2Builder_VertexAttrib(
        const vec3& pos,
        const vec2& uv,
        const vec4& color) {

        this->uv = uv;
        this->pos = pos;
        this->color = color;
    }


    //private copy constructores, to avoid copy...
    GLFont2Builder::GLFont2Builder(const GLFont2Builder& v) {}
    void GLFont2Builder::operator=(const GLFont2Builder&v) {}

    void GLFont2Builder::GLFont2BitmapRef_to_VertexAttrib(const vec3 &pos, const vec4 &color, const GLFont2BitmapRef &bitmapRef) {
        vec3 pos_new = pos + bitmapRef.move_before_draw;
        for (int i = 0; i < 6; i++) {
            vertexAttrib.push_back(
                GLFont2Builder_VertexAttrib(
                    bitmapRef.triangles.position[i] + pos_new,
                    bitmapRef.triangles.uv[i],
                    color
                )
            );
        }
    }

    void GLFont2Builder::countNewLines_1stlineHeight_1stlineLength(const wchar_t* str, int size, int *_newLineCount, float *_1stLineMaxHeight, float *_1stLinelength) {
        float length = 0;
        float height = 0;
        int new_line_count = 0;
        wchar_t c;
        int i;
        for (i = 0; i < size; i++) {
            c = str[i];
            if (c == L' ')
                length += glFont2.space_width;
            else if (c == L'\n')
                break;
            else {
                const GLFont2Glyph *glyph = glFont2.getGlyph(str[i]);
                if (glyph != NULL) {
                    length += glyph->advancex.x;
                    if (glyph->face.height > height)
                        height = glyph->face.height;
                }
            }
        }
        *_1stLineMaxHeight = height;
        *_1stLinelength = length;

        for (; i < size; i++) {
            c = str[i];
            if (c == L'\n')
                new_line_count++;
        }

        *_newLineCount = new_line_count;
    }

    float GLFont2Builder::computeStringLengthUntilNewLine(const wchar_t* str, int size, int offset) {
        float length = 0;
        wchar_t c;
        for (int i = offset; i < size; i++) {
            c = str[i];
            if (c == L' ')
                length += glFont2.space_width;
            else if (c == L'\n')
                break;
            else {
                const GLFont2Glyph *glyph = glFont2.getGlyph(str[i]);
                if (glyph != NULL)
                    length += glyph->advancex.x;
            }
        }
        return length;
    }

    GLFont2Builder::GLFont2Builder() {
        faceColor = vec4(1, 1, 1, 1);
        strokeColor = vec4(0, 0, 0, 1);

        drawFace = true;
        drawStroke = true;

        //strokeOffset = vec3(0, 0, EPSILON);
        strokeOffset = vec3(0, 0, 0.001f);

        horizontalAlign = GLFont2HorizontalAlign_left;
        verticalAlign = GLFont2VerticalAlign_none;
    }

    void GLFont2Builder::load(const std::string &filename) {
        glFont2.loadFromBasof2(filename);
    }
    
    void GLFont2Builder::computeBoxW(const wchar_t* str, float*xmin, float*xmax, float*ymin, float*ymax) {
        size_t count = wcslen(str);
        wchar_t c;
        vec3 position = vec3(0);
        
        int _newLineCount = 0;
        float _1stLineMaxHeight = 0;
        float _1stLinelength = 0;
        
        countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
        
        if (verticalAlign == GLFont2VerticalAlign_top) {
            //countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
            position.y = -_1stLineMaxHeight;
        }
        else if (verticalAlign == GLFont2VerticalAlign_middle) {
            //countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
            float totalSize = (float)_newLineCount * glFont2.new_line_height;
            position.y = -_1stLineMaxHeight * 0.5f + totalSize * 0.5f;
        }
        else if (verticalAlign == GLFont2VerticalAlign_bottom) {
            //countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
            float totalSize = (float)_newLineCount * glFont2.new_line_height;
            position.y = totalSize;
        }
        
        if (horizontalAlign == GLFont2HorizontalAlign_center) {
            position.x = (_1stLinelength != 0) ? (-_1stLinelength * 0.5f) : (-computeStringLengthUntilNewLine(str, count, 0) * 0.5f);
        }
        else if (horizontalAlign == GLFont2HorizontalAlign_right) {
            position.x = (_1stLinelength != 0) ? (-_1stLinelength) : (-computeStringLengthUntilNewLine(str, count, 0));
        }
        
        *xmin = position.x;
        *xmax = position.x;
        *ymin = position.y;
        *ymax = position.y + _1stLineMaxHeight;
        
        for (size_t i = 0; i < count; i++) {
            c = str[i];
            
            if (c == L' ') {
                position.x += glFont2.space_width;
            }
            else if (c == L'\n') {
                if (horizontalAlign == GLFont2HorizontalAlign_left) {
                    position.x = 0;
                }
                else if (horizontalAlign == GLFont2HorizontalAlign_center) {
                    position.x = -computeStringLengthUntilNewLine(str, count, i + 1) * 0.5f;
                    *xmin = minimum( *xmin, position.x );
                    //*xmax = maximum( *xmax, position.x );
                }
                else if (horizontalAlign == GLFont2HorizontalAlign_right) {
                    position.x = -computeStringLengthUntilNewLine(str, count, i + 1);
                    *xmin = minimum( *xmin, position.x );
                    //*xmax = maximum( *xmax, position.x );
                }
                position.y -= glFont2.new_line_height;
                
                *ymin = minimum( *ymin, position.y );
                //*ymax = maximum( *ymin, position.y );
                
            }
            else {
                const GLFont2Glyph *glyph = glFont2.getGlyph(c);
                if (glyph != NULL) {
                    position.x += glyph->advancex.x;
                    
                    //*xmin = minimum( *xmin, position.x );
                    *xmax = maximum( *xmax, position.x );
                    
                    
                }
            }
        }
    }
    
    void GLFont2Builder::computeBox(const char* str, float*xmin, float*xmax, float*ymin, float*ymax) {
        computeBoxW(StringUtil::toWString(str).c_str(), xmin, xmax, ymin, ymax);
    }

    GLFont2Builder* GLFont2Builder::wbuild(const wchar_t* str) {
        size_t count = wcslen(str);
        vertexAttrib.clear();

        wchar_t c;
        vec3 position = vec3(0);

        int _newLineCount = 0;
        float _1stLineMaxHeight = 0;
        float _1stLinelength = 0;

        if (verticalAlign == GLFont2VerticalAlign_top) {
            countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
            position.y = -_1stLineMaxHeight;
        }
        else if (verticalAlign == GLFont2VerticalAlign_middle) {
            countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
            float totalSize = (float)_newLineCount * glFont2.new_line_height;
            position.y = -_1stLineMaxHeight * 0.5f + totalSize * 0.5f;
        }
        else if (verticalAlign == GLFont2VerticalAlign_bottom) {
            countNewLines_1stlineHeight_1stlineLength(str, count, &_newLineCount, &_1stLineMaxHeight, &_1stLinelength);
            float totalSize = (float)_newLineCount * glFont2.new_line_height;
            position.y = totalSize;
        }

        if (horizontalAlign == GLFont2HorizontalAlign_center) {
            position.x = (_1stLinelength != 0) ? (-_1stLinelength * 0.5f) : (-computeStringLengthUntilNewLine(str, count, 0) * 0.5f);
        }
        else if (horizontalAlign == GLFont2HorizontalAlign_right) {
            position.x = (_1stLinelength != 0) ? (-_1stLinelength) : (-computeStringLengthUntilNewLine(str, count, 0));
        }

        for (size_t i = 0; i < count; i++) {
            c = str[i];

            if (c == L' ') {
                position.x += glFont2.space_width;
            }
            else if (c == L'\n') {
                if (horizontalAlign == GLFont2HorizontalAlign_left) {
                    position.x = 0;
                }
                else if (horizontalAlign == GLFont2HorizontalAlign_center) {
                    position.x = -computeStringLengthUntilNewLine(str, count, i + 1) * 0.5f;
                }
                else if (horizontalAlign == GLFont2HorizontalAlign_right) {
                    position.x = -computeStringLengthUntilNewLine(str, count, i + 1);
                }
                position.y -= glFont2.new_line_height;
            }
            else {
                const GLFont2Glyph *glyph = glFont2.getGlyph(c);
                if (glyph != NULL) {

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

    GLFont2Builder* GLFont2Builder::build(const char* str) {
        wbuild(StringUtil::toWString(str).c_str());
        return this;
    }

}

