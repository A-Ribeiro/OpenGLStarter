#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

// #include <glew/glew.h>
#include <map>
#include <vector>
// #include <convertutf/ConvertUTF.h>

// #include <aRibeiroCore/all_math.h>
// #include <aribeiro/GLStripText.h>
#include <appkit-gl-base/font/deprecated/Font.h>
#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/shaders/GLShaderFont.h>

#include <wchar.h> // to define swprintf(...);

namespace AppKit
{

    namespace OpenGL
    {

        class GLStripText;

        struct GLFontGlyphInfo
        {
            // base calculations
            MathCore::vec2f texBegin, texEnd;
            MathCore::vec2f dimension;
            MathCore::vec2f beforeStart, advance;
            // used for render
            MathCore::vec2f TexcoordVertex2fStrip[6 * 2];
        };

        class GLFont
        {
            FontHeader mHeader;
            std::unordered_map<uint32_t, GLFontGlyphInfo> mGlyphs;
            // GLuint mTextureID;

            GLTexture texture;
            static GLShaderFont *globalShader;

        private:
            // private copy constructores, to avoid copy...
            GLFont(const GLFont &v);
            void operator=(const GLFont &v);

        public:
            static void releaseSharedResources();

            GLFont(const FontHeader &aHeader,
                   std::unordered_map<uint32_t, FontGlyphInfo> &aGlyphs,
                   const char *luminancePointer);
            ~GLFont();

            GLTexture *getTexture();
            GLShaderFont *getShader();

            void print(const MathCore::vec4f &color, const MathCore::vec2f &pos, const wchar_t *string);

            void print(const MathCore::vec4f &color, const MathCore::vec2f &pos, const char *string);

            // void print(const wchar_t* v,...);
            void print(const MathCore::vec4f &color, const wchar_t *string);
            void print(const MathCore::vec3f &color, const wchar_t *string);

            CollisionCore::AABB<MathCore::vec3f> computeBounds(const wchar_t *string);
            CollisionCore::AABB<MathCore::vec3f> computeBounds(const char *string);

            CollisionCore::AABB<MathCore::vec3f> computeBoundsJustBox(const wchar_t *string);
            CollisionCore::AABB<MathCore::vec3f> computeBoundsJustBox(const char *string);

            GLStripText *createGLStripText(const wchar_t *string);

            float getLineHeight();

        private:
            struct VertexAttrib
            {
                MathCore::vec2f uv;
                MathCore::vec4f color;
                MathCore::vec2f pos;
                VertexAttrib() {}
                VertexAttrib(const MathCore::vec2f &uv,
                             const MathCore::vec4f &color,
                             const MathCore::vec2f &pos)
                {
                    this->uv = uv;
                    this->color = color;
                    this->pos = pos;
                }
            };
            std::vector<VertexAttrib> mModelBuffer;
            MathCore::vec2f putc_(wchar_t c, const MathCore::vec2f &p, bool repeatFirstPtn, bool repeatLastPtn, const MathCore::vec4f &color, float basex);
        };

    }

}
