#include "GLShaderFont2.h"
#include "opengl-wrapper/PlatformGL.h"

using namespace aRibeiro;

namespace openglWrapper {

    GLShaderFont2::GLShaderFont2() :GLShader() {
        const char vertexShaderCode[] = {
            "attribute vec4 vPosition;"
            "attribute vec2 vUV;"
            "attribute vec4 vColor;"
            "uniform mat4 matrix;"
            "varying vec2 uv;"
            "varying vec4 color;"
            "void main() {"
            "  uv = vUV;"
            "  color = vColor;"
            "  gl_Position = matrix * vPosition;"
            "}" };

        const char fragmentShaderCode[] = {
            "varying vec2 uv;"
            "varying vec4 color;"
            "uniform sampler2D texture;"
            "void main() {"
            "  float alpha = texture2D(texture, uv).a;"
            "  vec4 result = vec4(color.r,color.g,color.b,alpha*color.a);"
            //"  result = result*0.0 + vec4(alpha,0,1,1);"//debug
            "  gl_FragColor = result;"
            "}" };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        texture = getUniformLocation("texture");
        matrix = getUniformLocation("matrix");
    }

    // Set the texture unit, in OpenGL you can use 8 textures units at same time (from 0 to 7)
    void GLShaderFont2::setTexture(int activeTextureUnit) {
        setUniform(texture, activeTextureUnit);
    }

    void GLShaderFont2::setMatrix(const mat4 & m) {
        setUniform(matrix, m);
    }
    
    void GLShaderFont2::setupAttribLocation() {
        bindAttribLocation(GLShaderFont2::vPosition, "vPosition");
        bindAttribLocation(GLShaderFont2::vColor, "vColor");
        bindAttribLocation(GLShaderFont2::vUV, "vUV");
    }

}
