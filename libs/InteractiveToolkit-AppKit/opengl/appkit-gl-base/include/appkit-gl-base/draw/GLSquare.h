#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/shaders/GLShaderColor.h>
#include <appkit-gl-base/shaders/GLShaderTextureColor.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Draw a square using the base shaders
        ///
        /// This class uses the direct draw (draw triangles from main memory buffer).
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLSquare
        {

        public:
            GLSquare();

            /// \brief Draw a square
            ///
            /// \author Alessandro Ribeiro
            /// \param shader instance of #GLShaderColor
            ///
            void draw(GLShaderColor *shader);

            /// \brief Draw a square
            ///
            /// \author Alessandro Ribeiro
            /// \param shader instance of #GLShaderTextureColor
            ///
            void draw(GLShaderTextureColor *shader);
        };

    }
}