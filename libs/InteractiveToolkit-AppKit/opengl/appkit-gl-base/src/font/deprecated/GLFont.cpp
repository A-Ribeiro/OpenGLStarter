#ifdef _WIN32

#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4309)
#pragma warning(disable : 4018)

#endif

#include <appkit-gl-base/font/deprecated/Font.h>
#include <appkit-gl-base/font/deprecated/GLFont.h>
#include <appkit-gl-base/font/deprecated/GLStripText.h>
#include <appkit-gl-base/platform/PlatformGL.h>
// #include <aRibeiroCore/SetNullAndDelete.h>

#include <stdlib.h>
#include <string.h>
#include <string>

// using namespace aRibeiro;

namespace AppKit
{

    namespace OpenGL
    {

        GLShaderFont *GLFont::globalShader = NULL;

        //
        // private constructors, avoid copy construct
        //
        GLFont::GLFont(const GLFont &v) {}
        void GLFont::operator=(const GLFont &v) {}
        //
        //
        //
        void GLFont::releaseSharedResources()
        {
            if (globalShader != NULL){
                delete globalShader;
                globalShader = NULL;
            }
        }
        //
        //
        //
        GLFont::GLFont(const FontHeader &aHeader,
                       std::map<uint32_t, FontGlyphInfo> &aGlyphs,
                       const char *luminancePointer)
        {

            if (globalShader == NULL)
                globalShader = new GLShaderFont();

            mHeader = aHeader;
            // mGlyphs = aGlyphs;
            float w = (float)aHeader.mTexW;
            float h = (float)aHeader.mTexH;

            std::map<uint32_t, FontGlyphInfo>::iterator it = aGlyphs.begin();

            while (it != aGlyphs.end())
            {
                FontGlyphInfo glyph = it->second;
                GLFontGlyphInfo glGlyphInfo;
                glGlyphInfo.beforeStart = MathCore::vec2f(glyph.x, -glyph.h + glyph.y);
                glGlyphInfo.advance = MathCore::vec2f(glyph.advX, glyph.advY); //- glGlyphInfo.beforeStart;
                glGlyphInfo.dimension = MathCore::vec2f(glyph.w, glyph.h);
                glGlyphInfo.texBegin = MathCore::vec2f(((float)glyph.texX) / w, 1.0f - ((float)glyph.texY) / h);
                glGlyphInfo.texEnd = glGlyphInfo.texBegin + glGlyphInfo.dimension / MathCore::vec2f(w, -h);

                // compute the
                MathCore::vec2f buffer[6 * 2] = {
                    MathCore::vec2f(glGlyphInfo.texBegin.x, glGlyphInfo.texBegin.y), MathCore::vec2f(0, glGlyphInfo.dimension.y) + glGlyphInfo.beforeStart,
                    MathCore::vec2f(glGlyphInfo.texBegin.x, glGlyphInfo.texEnd.y), MathCore::vec2f(0, 0) + glGlyphInfo.beforeStart,
                    MathCore::vec2f(glGlyphInfo.texEnd.x, glGlyphInfo.texEnd.y), MathCore::vec2f(glGlyphInfo.dimension.x, 0) + glGlyphInfo.beforeStart,

                    MathCore::vec2f(glGlyphInfo.texBegin.x, glGlyphInfo.texBegin.y), MathCore::vec2f(0, glGlyphInfo.dimension.y) + glGlyphInfo.beforeStart,
                    MathCore::vec2f(glGlyphInfo.texEnd.x, glGlyphInfo.texEnd.y), MathCore::vec2f(glGlyphInfo.dimension.x, 0) + glGlyphInfo.beforeStart,
                    MathCore::vec2f(glGlyphInfo.texEnd.x, glGlyphInfo.texBegin.y), MathCore::vec2f(glGlyphInfo.dimension.x, glGlyphInfo.dimension.y) + glGlyphInfo.beforeStart};
                for (int i = 0; i < 6 * 2; i++)
                    glGlyphInfo.TexcoordVertex2fStrip[i] = buffer[i];
                mGlyphs[it->first] = glGlyphInfo;
                it++;
            }

            texture.uploadBufferAlpha8(luminancePointer, mHeader.mTexW, mHeader.mTexH);
        }

        GLFont::~GLFont()
        {
        }

        GLTexture *GLFont::getTexture()
        {
            return &texture;
        }

        GLShaderFont *GLFont::getShader()
        {
            return globalShader;
        }

        // void OpenGLFont::printf(const char* format,...){
        // va_list args;
        // char buffer[BUFSIZ];
        // va_start(args,format);
        // int charCount = vsnprintf (buffer, BUFSIZ, format, args );
        // for (int i=0;i<charCount;i++)
        // putc(buffer[i]);
        //  va_end(args);

        //}

        float GLFont::getLineHeight()
        {
            return mHeader.mGlyphHeight;
        }

        void GLFont::print(const MathCore::vec4f &color, const MathCore::vec2f &pos, const wchar_t *string)
        {

            MathCore::vec2f p = pos;
            size_t count = wcslen(string);
            //    glBegin(GL_TRIANGLES);
            mModelBuffer.clear();
            for (int i = 0; i < count; i++)
            {
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

            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mModelBuffer.size()));

            OPENGL_CMD(glDisableVertexAttribArray(GLShaderFont::vPosition));
            OPENGL_CMD(glDisableVertexAttribArray(GLShaderFont::vUV));
            OPENGL_CMD(glDisableVertexAttribArray(GLShaderFont::vColor));

            texture.deactive(0);
            // globalShader->disable();
        }

        void GLFont::print(const MathCore::vec4f &color, const MathCore::vec2f &pos, const char *string)
        {

            std::string text(string);
            std::wstring wText(text.begin(), text.end());

            print(color, pos, wText.c_str());
        }

        void GLFont::print(const MathCore::vec3f &color, const wchar_t *string)
        {
            print(MathCore::vec4f(color, 1), string);
        }

        void GLFont::print(const MathCore::vec4f &color, const wchar_t *string)
        {
            print(color, MathCore::vec2f(0), string);
        }

#define checkMinMax(position)                                        \
    if (first)                                                       \
    {                                                                \
        first = false;                                               \
        min = position;                                              \
        max = position;                                              \
    }                                                                \
    else                                                             \
    {                                                                \
        min = MathCore::OP<MathCore::vec2f>::minimum(position, min); \
        max = MathCore::OP<MathCore::vec2f>::maximum(position, max); \
    }

        CollisionCore::AABB<MathCore::vec3f> GLFont::computeBounds(const wchar_t *string)
        {
            std::map<uint32_t, GLFontGlyphInfo>::iterator it;

            bool first = true;
            MathCore::vec2f p;
            MathCore::vec2f min, max;

            // check the top part of the font metrics
            checkMinMax(p);
            p.x = 0;
            p.y += mHeader.mGlyphHeight;
            checkMinMax(p);
            p.y = 0;

            size_t count = wcslen(string);
            for (int i = 0; i < count; i++)
            {

                if (string[i] == L'\n')
                {

                    checkMinMax(p);

                    p.x = 0;
                    p.y -= mHeader.mGlyphHeight;

                    checkMinMax(p);

                    continue;
                }

                it = mGlyphs.find(string[i]);
                if (it != mGlyphs.end())
                {
                    const GLFontGlyphInfo &glyph = it->second;
                    for (int j = 0; j < 6 * 2; j += 2)
                    {
                        MathCore::vec2f position = glyph.TexcoordVertex2fStrip[j + 1] + p;

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
                            max = MathCore::OP<float>::maximum(position, max);
                        }
                        */
                    }
                    p += glyph.advance;

                    checkMinMax(p)
                }
            }
            return CollisionCore::AABB<MathCore::vec3f>(min, max);
        }

        CollisionCore::AABB<MathCore::vec3f> GLFont::computeBounds(const char *string)
        {
            std::string text(string);
            std::wstring wText(text.begin(), text.end());

            return computeBounds(wText.c_str());
        }

        CollisionCore::AABB<MathCore::vec3f> GLFont::computeBoundsJustBox(const wchar_t *string)
        {
            std::map<uint32_t, GLFontGlyphInfo>::iterator it;

            bool first = true;
            MathCore::vec2f p;
            MathCore::vec2f min, max;

            // check the top part of the font metrics
            checkMinMax(p);
            p.x = 0;
            p.y += mHeader.mGlyphHeight;
            checkMinMax(p);
            p.y = 0;

            size_t count = wcslen(string);
            for (int i = 0; i < count; i++)
            {

                if (string[i] == L'\n')
                {

                    checkMinMax(p);

                    p.x = 0;
                    p.y -= mHeader.mGlyphHeight;

                    checkMinMax(p);

                    continue;
                }

                it = mGlyphs.find(string[i]);
                if (it != mGlyphs.end())
                {
                    const GLFontGlyphInfo &glyph = it->second;
                    p += glyph.advance;
                    checkMinMax(p)
                }
            }
            return CollisionCore::AABB<MathCore::vec3f>(min, max);
        }

        CollisionCore::AABB<MathCore::vec3f> GLFont::computeBoundsJustBox(const char *string)
        {
            std::string text(string);
            std::wstring wText(text.begin(), text.end());

            return computeBoundsJustBox(wText.c_str());
        }

        MathCore::vec2f GLFont::putc_(wchar_t inputChar, const MathCore::vec2f &p, bool repeatFirstPtn, bool repeatLastPtn, const MathCore::vec4f &color, float basex)
        {
            uint32_t c = inputChar;

            if (inputChar == L'\n')
            {
                //        p.x = 0;
                //        p.y -= mHeader.mGlyphHeight;
                // return MathCore::vec2f(-p.x, -mHeader.mGlyphHeight);
                return MathCore::vec2f(-p.x + basex, -mHeader.mGlyphHeight);
            }

            // consult the glyphTable
            std::map<uint32_t, GLFontGlyphInfo>::iterator it = mGlyphs.find(c);
            if (it != mGlyphs.end())
            {
                const GLFontGlyphInfo &glyph = it->second;
                // glTranslatef(glyph.beforeStart.x,glyph.beforeStart.y,0);
                for (int i = 0; i < 6 * 2; i += 2)
                {
                    mModelBuffer.push_back(VertexAttrib(
                        (glyph.TexcoordVertex2fStrip[i]),           // uv
                        color,                                      // color
                        (glyph.TexcoordVertex2fStrip[i + 1] + p))); // pos
                    //            glTexCoord2fv((glyph.TexcoordVertex2fStrip[i]).array);
                    //            glVertex2fv((glyph.TexcoordVertex2fStrip[i+1]+p).array);
                }
                return glyph.advance;
            }
            return MathCore::vec2f(0);
        }

        GLStripText *GLFont::createGLStripText(const wchar_t *string)
        {
            MathCore::vec4f color(1, 0, 1, 1);
            MathCore::vec2f p;
            int count = (int)wcslen(string);
            GLStripText *result = new GLStripText(count * 6, &texture, globalShader);

            int pos = 0;
            for (int i = 0; i < count; i++)
            {
                if (string[i] == L'\n')
                {
                    p.x = 0;
                    p.y -= mHeader.mGlyphHeight;
                    continue;
                }
                std::map<uint32_t, GLFontGlyphInfo>::iterator it = mGlyphs.find(string[i]);
                if (it != mGlyphs.end())
                {
                    const GLFontGlyphInfo &glyph = it->second;
                    for (int j = 0; j < 6 * 2; j += 2)
                    {
                        StripInfoVertexAttrib &v = result->getVertex(pos++);
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
}
