#ifdef _WIN32

#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)

#endif

#include "Font.h"
#include "GLFont.h"
#include "GLStripText.h"
#include <opengl-wrapper/PlatformGL.h>
#include <aribeiro/SetNullAndDelete.h>

#include <stdlib.h>
#include <string.h>
#include <string>

using namespace aRibeiro;

namespace openglWrapper {

    GLShaderFont *GLFont::globalShader = NULL;

    //
    // private constructors, avoid copy construct
    //
    GLFont::GLFont(const GLFont& v) {}
    void GLFont::operator=(const GLFont&v) {}
    //
    //
    //
    void GLFont::releaseSharedResources() {
        setNullAndDelete(globalShader);
    }
    //
    //
    //
    GLFont::GLFont(const FontHeader& aHeader,
        std::map<UTF32, FontGlyphInfo>& aGlyphs,
        const char* luminancePointer) {

        if (globalShader == NULL)
            globalShader = new GLShaderFont();

        mHeader = aHeader;
        //mGlyphs = aGlyphs;
        float w = (float)aHeader.mTexW;
        float h = (float)aHeader.mTexH;

        std::map<UTF32, FontGlyphInfo>::iterator it = aGlyphs.begin();

        while (it != aGlyphs.end()) {
            FontGlyphInfo glyph = it->second;
            GLFontGlyphInfo glGlyphInfo;
            glGlyphInfo.beforeStart = vec2(glyph.x, -glyph.h + glyph.y);
            glGlyphInfo.advance = vec2(glyph.advX, glyph.advY);//- glGlyphInfo.beforeStart;
            glGlyphInfo.dimension = vec2(glyph.w, glyph.h);
            glGlyphInfo.texBegin = vec2(((float)glyph.texX) / w, 1.0f - ((float)glyph.texY) / h);
            glGlyphInfo.texEnd = glGlyphInfo.texBegin + glGlyphInfo.dimension / vec2(w, -h);

            //compute the
            vec2 buffer[6 * 2] = {
                            vec2(glGlyphInfo.texBegin.x, glGlyphInfo.texBegin.y), vec2(0                      , glGlyphInfo.dimension.y) + glGlyphInfo.beforeStart,
                            vec2(glGlyphInfo.texBegin.x, glGlyphInfo.texEnd.y), vec2(0                      , 0) + glGlyphInfo.beforeStart,
                            vec2(glGlyphInfo.texEnd.x  , glGlyphInfo.texEnd.y), vec2(glGlyphInfo.dimension.x, 0) + glGlyphInfo.beforeStart,


                            vec2(glGlyphInfo.texBegin.x, glGlyphInfo.texBegin.y), vec2(0                      , glGlyphInfo.dimension.y) + glGlyphInfo.beforeStart,
                            vec2(glGlyphInfo.texEnd.x  , glGlyphInfo.texEnd.y), vec2(glGlyphInfo.dimension.x, 0) + glGlyphInfo.beforeStart,
                            vec2(glGlyphInfo.texEnd.x  , glGlyphInfo.texBegin.y), vec2(glGlyphInfo.dimension.x, glGlyphInfo.dimension.y) + glGlyphInfo.beforeStart
            };
            for (int i = 0; i < 6 * 2; i++)
                glGlyphInfo.TexcoordVertex2fStrip[i] = buffer[i];
            mGlyphs[it->first] = glGlyphInfo;
            it++;
        }

        texture.uploadBufferAlpha8(luminancePointer, mHeader.mTexW, mHeader.mTexH);
    }

    GLFont::~GLFont() {
    }

    GLTexture *GLFont::getTexture() {
        return &texture;
    }

    GLShaderFont *GLFont::getShader() {
        return globalShader;
    }


    //void OpenGLFont::printf(const char* format,...){
      //va_list args;
      //char buffer[BUFSIZ];
      //va_start(args,format);
      //int charCount = vsnprintf (buffer, BUFSIZ, format, args );
      //for (int i=0;i<charCount;i++)
        //putc(buffer[i]);
    //  va_end(args);

    //}


    float GLFont::getLineHeight() {
        return mHeader.mGlyphHeight;
    }

    void GLFont::print(const vec4& color, const vec2& pos, const wchar_t* string) {

        vec2 p = pos;
        size_t count = wcslen(string);
        //    glBegin(GL_TRIANGLES);
        mModelBuffer.clear();
        for (int i = 0; i < count; i++) {
            p += putc_(string[i], p, i != 0, i < count - 1, color, pos.x);
        }

        if (mModelBuffer.size() == 0)
            return;

        texture.active(0);

        globalShader->enable();
        globalShader->setTexture(0);

        OPENGL_CMD(glEnableVertexAttribArray(GLShaderFont::vPosition));
        OPENGL_CMD(glVertexAttribPointer(GLShaderFont::vPosition, 2, GL_FLOAT, false, sizeof(VertexAttrib), &mModelBuffer[0].pos));

        OPENGL_CMD(glEnableVertexAttribArray(GLShaderFont::vUV));
        OPENGL_CMD(glVertexAttribPointer(GLShaderFont::vUV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &mModelBuffer[0].uv));

        OPENGL_CMD(glEnableVertexAttribArray(GLShaderFont::vColor));
        OPENGL_CMD(glVertexAttribPointer(GLShaderFont::vColor, 4, GL_FLOAT, false, sizeof(VertexAttrib), &mModelBuffer[0].color));

        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, mModelBuffer.size()));

        OPENGL_CMD(glDisableVertexAttribArray(GLShaderFont::vPosition));
        OPENGL_CMD(glDisableVertexAttribArray(GLShaderFont::vUV));
        OPENGL_CMD(glDisableVertexAttribArray(GLShaderFont::vColor));

        texture.deactive(0);
        //globalShader->disable();
    }

    void GLFont::print(const vec4& color, const vec2& pos, const char* string) {

        std::string text(string);
        std::wstring wText(text.begin(), text.end());

        print(color, pos, wText.c_str());
    }

    void GLFont::print(const vec3& color, const wchar_t* string) {
        print(vec4(color, 1), string);
    }

    void GLFont::print(const vec4& color, const wchar_t* string) {
        print(color, vec2(0), string);
    }

#define checkMinMax(position) \
if (first) { \
    first = false; \
    min = position; \
    max = position; \
} \
else { \
    min = aRibeiro::minimum(position, min); \
    max = aRibeiro::maximum(position, max); \
}


    collision::AABB GLFont::computeBounds(const wchar_t* string) {
        std::map<UTF32, GLFontGlyphInfo>::iterator it;

        bool first = true;
        vec2 p;
        vec2 min, max;

        //check the top part of the font metrics
        checkMinMax(p);
        p.x = 0;
        p.y += mHeader.mGlyphHeight;
        checkMinMax(p);
        p.y = 0;

        size_t count = wcslen(string);
        for (int i = 0; i < count; i++) {

            if (string[i] == L'\n') {

                checkMinMax(p);

                p.x = 0;
                p.y -= mHeader.mGlyphHeight;

                checkMinMax(p);

                continue;
            }

            it = mGlyphs.find(string[i]);
            if (it != mGlyphs.end()) {
                const GLFontGlyphInfo& glyph = it->second;
                for (int j = 0; j < 6 * 2; j += 2) {
                    vec2 position = glyph.TexcoordVertex2fStrip[j + 1] + p;

                    checkMinMax(position)
                    /*
                    //if (i == 0 && j == 0) {
                    if (first) {
                        first = false;
                        min = position;
                        max = position;
                    }
                    else {
                        min = aRibeiro::minimum(position, min);
                        max = aRibeiro::maximum(position, max);
                    }
                    */
                }
                p += glyph.advance;

                checkMinMax(p)
            }
        }
        return collision::AABB(min, max);
    }

    collision::AABB GLFont::computeBounds(const char* string) {
        std::string text(string);
        std::wstring wText(text.begin(), text.end());

        return computeBounds(wText.c_str());
    }

    collision::AABB GLFont::computeBoundsJustBox(const wchar_t* string) {
        std::map<UTF32, GLFontGlyphInfo>::iterator it;

        bool first = true;
        vec2 p;
        vec2 min, max;

        //check the top part of the font metrics
        checkMinMax(p);
        p.x = 0;
        p.y += mHeader.mGlyphHeight;
        checkMinMax(p);
        p.y = 0;

        size_t count = wcslen(string);
        for (int i = 0; i < count; i++) {

            if (string[i] == L'\n') {

                checkMinMax(p);

                p.x = 0;
                p.y -= mHeader.mGlyphHeight;

                checkMinMax(p);

                continue;
            }

            it = mGlyphs.find(string[i]);
            if (it != mGlyphs.end()) {
                const GLFontGlyphInfo& glyph = it->second;
                p += glyph.advance;
                checkMinMax(p)
            }
        }
        return collision::AABB(min, max);
    }

    collision::AABB GLFont::computeBoundsJustBox(const char* string) {
        std::string text(string);
        std::wstring wText(text.begin(), text.end());

        return computeBoundsJustBox(wText.c_str());
    }




    vec2 GLFont::putc_(wchar_t inputChar, const vec2& p, bool repeatFirstPtn, bool repeatLastPtn, const vec4& color, float basex) {
        UTF32 c = inputChar;

        if (inputChar == L'\n') {
            //        p.x = 0;
            //        p.y -= mHeader.mGlyphHeight;
            //return vec2(-p.x, -mHeader.mGlyphHeight);
            return vec2(-p.x + basex, -mHeader.mGlyphHeight);
        }

        //consult the glyphTable
        std::map<UTF32, GLFontGlyphInfo>::iterator it = mGlyphs.find(c);
        if (it != mGlyphs.end()) {
            const GLFontGlyphInfo& glyph = it->second;
            //glTranslatef(glyph.beforeStart.x,glyph.beforeStart.y,0);
            for (int i = 0; i < 6 * 2; i += 2) {
                mModelBuffer.push_back(VertexAttrib(
                    (glyph.TexcoordVertex2fStrip[i]),//uv
                    color,//color
                    (glyph.TexcoordVertex2fStrip[i + 1] + p)));//pos
//            glTexCoord2fv((glyph.TexcoordVertex2fStrip[i]).array);
//            glVertex2fv((glyph.TexcoordVertex2fStrip[i+1]+p).array);
            }
            return glyph.advance;
        }
        return vec2(0);
    }

    GLStripText* GLFont::createGLStripText(const wchar_t* string) {
        vec4 color(1, 0, 1, 1);
        vec2 p;
        size_t count = wcslen(string);
        GLStripText* result = new GLStripText(count * 6, &texture, globalShader);

        int pos = 0;
        for (int i = 0; i < count; i++) {
            if (string[i] == L'\n') {
                p.x = 0;
                p.y -= mHeader.mGlyphHeight;
                continue;
            }
            std::map<UTF32, GLFontGlyphInfo>::iterator it = mGlyphs.find(string[i]);
            if (it != mGlyphs.end()) {
                const GLFontGlyphInfo& glyph = it->second;
                for (int j = 0; j < 6 * 2; j += 2) {
                    StripInfoVertexAttrib& v = result->getVertex(pos++);
                    v.mColor = color;
                    v.mCoord = glyph.TexcoordVertex2fStrip[j + 0];
                    v.mPos = glyph.TexcoordVertex2fStrip[j + 1] + p;
                }
                p += glyph.advance;
            }
        }
        result->setSize(pos);

        return result;
    }

}
