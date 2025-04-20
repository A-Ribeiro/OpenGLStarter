#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <map>
#include <vector>
// #include <glew/glew.h>

// #include <aRibeiroCore/all_math.h>

// #include <aRibeiroCore/all_math.h>
#include <appkit-gl-base/font/deprecated/Font.h>
#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/shaders/GLShaderFont.h>
// #include <aRibeiroCore/SSE2.h>

namespace AppKit
{

    namespace OpenGL
    {

        class StripInfoVertexAttrib
        {
        public:
            MathCore::vec4f mColor;
            MathCore::vec2f mCoord;
            MathCore::vec2f mPos;
        };

        class GLStripText
        {
            StripInfoVertexAttrib *mStripInfo;
            int mSize;

            GLTexture *texture;
            GLShaderFont *shader;

        public:

            //deleted copy constructor and assign operator, to avoid copy...
            GLStripText(const GLStripText &v) = delete;
            GLStripText& operator=(const GLStripText &v) = delete;

            void setSize(int s);

            GLStripText(int buffSize, GLTexture *texture, GLShaderFont *shader);
            ~GLStripText();
            StripInfoVertexAttrib &getVertex(int index);
            StripInfoVertexAttrib &getCharVertex(int CharIndex);

            void draw();

            void resetColor(const MathCore::vec4f &c);

            int getCharacterCount();

            void saturateFromBegin(float increment);
        };

    }

}
