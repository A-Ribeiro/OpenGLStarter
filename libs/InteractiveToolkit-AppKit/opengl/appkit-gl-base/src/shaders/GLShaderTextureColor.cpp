#include <appkit-gl-base/shaders/GLShaderTextureColor.h>
#include <appkit-gl-base/platform/PlatformGL.h>

// using namespace aRibeiro;

namespace AppKit
{

    namespace OpenGL
    {
        const AppKit::OpenGL::ShaderType GLShaderTextureColor::Type = "GLShaderTextureColor";

        GLShaderTextureColor::GLShaderTextureColor() : GLShader(GLShaderTextureColor::Type)
        {
            const char vertexShaderCode[] = {
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "attribute vec4 vPosition;"
                "attribute vec2 vUV;"
                "uniform mat4 matrix;"
                "varying vec2 uv;"
                "void main() {"
                "  uv = vUV;"
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
                "varying vec2 uv;"
                "uniform sampler2D texture;"
                "void main() {"
                "  vec4 texel = texture2D(texture, uv);"
                "  vec4 result = texel * color;"
                "  gl_FragColor = result;"
                "}"
            };

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            bindAttribLocation(GLShaderTextureColor::vPosition, "vPosition");
            bindAttribLocation(GLShaderTextureColor::vUV, "vUV");
            link(__FILE__, __LINE__);

            // vPosition = getAttribLocation("vPosition");
            // vUV = getAttribLocation("vUV");

            color = getUniformLocation("color");
            matrix = getUniformLocation("matrix");
            texture = getUniformLocation("texture");
        }

        void GLShaderTextureColor::setColor(const MathCore::vec4f &c)
        {
            setUniform(color, c);
        }

        void GLShaderTextureColor::setMatrix(const MathCore::mat4f &m)
        {
            setUniform(matrix, m);
        }

        void GLShaderTextureColor::setTexture(int activeTextureUnit)
        {
            setUniform(texture, activeTextureUnit);
        }

    }

}
