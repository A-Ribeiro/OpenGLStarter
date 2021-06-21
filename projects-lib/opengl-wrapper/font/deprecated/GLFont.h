#ifndef GLFont___H
#define GLFont___H

#include <glew/glew.h>
#include <map>
#include <vector>
#include <convertutf/ConvertUTF.h>

#include <aribeiro/all_math.h>
//#include <aribeiro/GLStripText.h>
#include <opengl-wrapper/Font.h>
#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLShaderFont.h>

#include <wchar.h> // to define swprintf(...);

namespace openglWrapper {

    class GLStripText;

    struct GLFontGlyphInfo {
        //base calculations
        aRibeiro::vec2 texBegin, texEnd;
        aRibeiro::vec2 dimension;
        aRibeiro::vec2 beforeStart, advance;
        //used for render
        aRibeiro::vec2 TexcoordVertex2fStrip[6 * 2];
    };

    class GLFont {
        FontHeader mHeader;
        std::map<UTF32, GLFontGlyphInfo> mGlyphs;
        //GLuint mTextureID;

        GLTexture texture;
        static GLShaderFont *globalShader;

    private:

        //private copy constructores, to avoid copy...
        GLFont(const GLFont& v);
        void operator=(const GLFont&v);

    public:

        static void releaseSharedResources();
        
        GLFont(const FontHeader& aHeader,
            std::map<UTF32, FontGlyphInfo>& aGlyphs,
            const char* luminancePointer);
        virtual ~GLFont();

        GLTexture *getTexture();
        GLShaderFont *getShader();

        void print(const aRibeiro::vec4& color, const aRibeiro::vec2& pos, const wchar_t* string);

        void print(const aRibeiro::vec4& color, const aRibeiro::vec2& pos, const char* string);

        //void print(const wchar_t* v,...);
        void print(const aRibeiro::vec4& color, const wchar_t* string);
        void print(const aRibeiro::vec3& color, const wchar_t* string);

        aRibeiro::collision::AABB computeBounds(const wchar_t* string);
        aRibeiro::collision::AABB computeBounds(const char* string);

        aRibeiro::collision::AABB computeBoundsJustBox(const wchar_t* string);
        aRibeiro::collision::AABB computeBoundsJustBox(const char* string);

        GLStripText* createGLStripText(const wchar_t* string);

        float getLineHeight();
    private:
        struct VertexAttrib {
            aRibeiro::vec2 uv;
            aRibeiro::vec4 color;
            aRibeiro::vec2 pos;
            VertexAttrib() {}
            VertexAttrib(const aRibeiro::vec2& uv,
                const aRibeiro::vec4& color,
                const aRibeiro::vec2& pos) {
                this->uv = uv;
                this->color = color;
                this->pos = pos;
            }
        };
        aRibeiro::aligned_vector<VertexAttrib> mModelBuffer;
        aRibeiro::vec2 putc_(wchar_t c, const aRibeiro::vec2& p, bool repeatFirstPtn, bool repeatLastPtn, const aRibeiro::vec4& color, float basex);
    };

}

#endif
