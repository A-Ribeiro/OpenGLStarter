#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/GLShader.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Direct draw YUV buffer
        ///
        /// This shader does not project or transform the input vertex by a matrix.
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLShaderYUV2RGB : public GLShader
        {

        public:
            static const AppKit::OpenGL::ShaderType Type;
            //
            // attrib
            //
            static const int vPosition = 0; ///< vertex atribute layout position
            static const int vUV = 1;       ///< vertex atribute layout uv

            //
            // uniform
            //
            int scale;
            int textureY;
            int textureU;
            int textureV;
            int fade;

            /// \brief Constructor
            ///
            /// \author Alessandro Ribeiro
            /// \param useBlur if true then use blur inside the shader
            ///
            GLShaderYUV2RGB(bool useBlur);

            /// \brief Set the internal scale that is applyed to the input positions
            ///
            /// \author Alessandro Ribeiro
            /// \param x scale x
            /// \param y scale y
            ///
            void setScale(float x, float y);

            void setTextureY(int textureUnit); ///< set the OpenGL texture unit to be used by the texture sampler Y.
            void setTextureU(int textureUnit); ///< set the OpenGL texture unit to be used by the texture sampler U.
            void setTextureV(int textureUnit); ///< set the OpenGL texture unit to be used by the texture sampler V.

            void setFade(float v); ///< set the black to saturated color output
        };

    }

}