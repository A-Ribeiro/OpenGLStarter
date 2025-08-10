#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/GLShader.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Shader that draws a solid color for an object
        ///
        /// This shader fill all triangles with the same color.
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLShaderColor : public GLShader
        {

        public:
            static const AppKit::OpenGL::ShaderType Type;
            //
            // attrib locations
            //
            static const int vPosition = 0; ///< vertex atribute layout position

            //
            // uniform locations
            //
            int color;
            int matrix;

            GLShaderColor();

            void setColor(const MathCore::vec4f &c);  ///< set the color to fill the triangles
            void setMatrix(const MathCore::mat4f &m); ///< set the transform matrix
        };

    }
}