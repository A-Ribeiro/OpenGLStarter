#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/GLShader.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Shader to draw a world aligned sky box using a cubemap as input
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLShaderCubeMapSkyBox : public GLShader
        {

        public:
            //
            // attrib locations
            //
            static const int vPosition = 0; ///< vertex atribute layout position

            //
            // uniform locations
            //
            int matrix;
            int cubeTexture;

            GLShaderCubeMapSkyBox() : GLShader()
            {
                const char vertexShaderCode[] = {
#if !defined(GLAD_GLES2)
                    "#version 120\n"
#endif
                    "attribute vec4 vPosition;"
                    "varying vec3 cubeAccess;"
                    "uniform mat4 matrix;"
                    "void main() {"
                    "  cubeAccess = vPosition.rgb;"
                    "  gl_Position = matrix * vPosition;"
                    "}"
                };

                const char fragmentShaderCode[] = {
                //"precision mediump float;"
#if !defined(GLAD_GLES2)
                    "#version 120\n"
#endif
                    "varying vec3 cubeAccess;"
                    "uniform samplerCube cubeTexture;"
                    "void main() {"
                    "  vec4 texel = textureCube(cubeTexture, cubeAccess);"
                    "  gl_FragColor = texel;"
                    "}"
                };

                compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
                bindAttribLocation(GLShaderCubeMapSkyBox::vPosition, "vPosition");
                link(__FILE__, __LINE__);

                matrix = getUniformLocation("matrix");
                cubeTexture = getUniformLocation("cubeTexture");
            }

            void setMatrix(const MathCore::mat4f &m)
            {
                setUniform(matrix, m);
            }

            void setCubeTexture(int texunit)
            {
                setUniform(cubeTexture, texunit);
            }

            ~GLShaderCubeMapSkyBox()
            {
            }
        };

    }

}