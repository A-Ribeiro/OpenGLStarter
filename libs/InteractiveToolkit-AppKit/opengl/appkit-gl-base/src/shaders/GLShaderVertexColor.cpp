#include <appkit-gl-base/shaders/GLShaderVertexColor.h>
#include <appkit-gl-base/platform/PlatformGL.h>

// using namespace aRibeiro;

namespace AppKit
{

    namespace OpenGL
    {

        GLShaderVertexColor::GLShaderVertexColor() : GLShader()
        {
            const char vertexShaderCode[] = {
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "attribute vec4 vPosition;"
                "attribute vec4 vColor;"
                "uniform mat4 matrix;"
                "varying vec4 color;"
                "void main() {"
                "  color = vColor;"
                "  gl_Position = matrix * vPosition;"
                //"  gl_Position = gl_ModelViewProjectionMatrix * vPosition;"
                "}"
            };

            const char fragmentShaderCode[] = {
            //"precision mediump float;"
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "varying vec4 color;"
                "void main() {"
                "  vec4 result = color;"
                "  gl_FragColor = result;"
                "}"
            };

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            bindAttribLocation(GLShaderVertexColor::vPosition, "vPosition");
            bindAttribLocation(GLShaderVertexColor::vColor, "vColor");
            link(__FILE__, __LINE__);

            // vPosition = getAttribLocation("vPosition");
            // vColor = getAttribLocation("vColor");

            matrix = getUniformLocation("matrix");
        }

        void GLShaderVertexColor::setMatrix(const MathCore::mat4f &m)
        {
            setUniform(matrix, m);
        }

    }
}
