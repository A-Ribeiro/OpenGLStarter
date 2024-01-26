#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/font/GLFont2Builder.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Direct draw the font vertex information
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLFont2DirectDraw
        {
        public:
            /// \brief Direct draw the font vertex information
            ///
            /// \author Alessandro Ribeiro
            /// \param fontBuilder the current build to draw
            /// \param aPosition the shader vertex attribute layout for position
            /// \param aUV the shader vertex attribute layout for uv
            /// \param aColor the shader vertex attribute layout for color
            ///
            static void draw(const GLFont2Builder *fontBuilder, int aPosition, int aUV, int aColor);
        };

    }

}
