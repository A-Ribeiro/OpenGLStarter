#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/GLShader.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Shader that draws colored triangles (vertex color)
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLShaderVertexColor : public GLShader
        {

        public:
            static const AppKit::OpenGL::ShaderType Type;
            //
            // attrib locations
            //
            static const int vPosition = 0; ///< vertex atribute layout position
            static const int vColor = 1;    ///< vertex atribute layout color

            //
            // uniform locations
            //
            int matrix;

            GLShaderVertexColor();

            void setMatrix(const MathCore::mat4f &m); ///< set the transform matrix
        };

    }

}