#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/GLShader.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Shader that draws 2D fonts
        ///
        /// This shader is intended to be used by #GLFont and #GLStripText classes.
        ///
        /// It uses the vertex color GPU algorithm (Gouraud)
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLShaderFont : public GLShader
        {

        public:
            static const AppKit::OpenGL::ShaderType Type;
            //
            // attrib locations
            //
            static const int vPosition = 0; ///< vertex atribute layout position
            static const int vColor = 1;    ///< vertex atribute layout color
            static const int vUV = 2;       ///< vertex atribute layout uv

            //
            // uniform locations
            //
            int texture;
            int matrix;

            GLShaderFont();

            void setTexture(int activeTextureUnit);   ///< set the OpenGL texture unit to be used by the texture sampler.
            void setMatrix(const MathCore::mat4f &m); ///< set the transform matrix
        };

    }

}
