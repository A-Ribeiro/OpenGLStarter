#include <appkit-gl-base/shaders/GLShaderColor.h>
#include <appkit-gl-base/platform/PlatformGL.h>


namespace AppKit
{

    namespace OpenGL
    {
        const AppKit::OpenGL::ShaderType GLShaderColor::Type = "GLShaderColor";

        GLShaderColor::GLShaderColor() : GLShader(GLShaderColor::Type)
        {
            const char vertexShaderCode[] = {
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "attribute vec4 vPosition;"
                "uniform mat4 matrix;"
                "void main() {"
                "  gl_Position = matrix * vPosition;"
                //"  gl_Position = gl_ModelViewProjectionMatrix * vPosition;"
                "}"
            };

            const char fragmentShaderCode[] = {
            //"precision mediump float;"
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "uniform vec4 color;"
                "void main() {"
                "  vec4 result = color;"
                "  gl_FragColor = result;"
                "}"
            };

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            bindAttribLocation(GLShaderColor::vPosition, "vPosition");
            link(__FILE__, __LINE__);

            // vPosition = getAttribLocation("vPosition");

            color = getUniformLocation("color");
            matrix = getUniformLocation("matrix");
        }

        void GLShaderColor::setColor(const MathCore::vec4f &c)
        {
            setUniform(color, c);
        }

        void GLShaderColor::setMatrix(const MathCore::mat4f &m)
        {
            setUniform(matrix, m);
        }

    }
}
