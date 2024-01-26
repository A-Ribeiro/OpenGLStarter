#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/GLShader.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Shader that draws textured triangles with a color multiplier
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLShaderTextureColor : public GLShader
        {

        public:
            //
            // attrib locations
            //
            static const int vPosition = 0; ///< vertex atribute layout position
            static const int vUV = 1;       ///< vertex atribute layout uv

            //
            // uniform locations
            //
            int color;
            int matrix;
            int texture;

            GLShaderTextureColor();

            void setColor(const MathCore::vec4f &c);  ///< set the color to fill the triangles
            void setMatrix(const MathCore::mat4f &m); ///< set the transform matrix
            void setTexture(int activeTextureUnit);  ///< set the OpenGL texture unit to be used by the texture sampler.
        };

    }

}