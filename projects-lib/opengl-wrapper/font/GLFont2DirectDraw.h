#ifndef GLFont2_DirectDraw__H
#define GLFont2_DirectDraw__H

#include <opengl-wrapper/GLFont2Builder.h>

namespace openglWrapper {

    /// \brief Direct draw the font vertex information
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE GLFont2DirectDraw {
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

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
