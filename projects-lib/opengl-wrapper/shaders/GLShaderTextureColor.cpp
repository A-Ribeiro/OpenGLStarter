#include "GLShaderTextureColor.h"
#include "opengl-wrapper/PlatformGL.h"

using namespace aRibeiro;

namespace openglWrapper {

    GLShaderTextureColor::GLShaderTextureColor() :GLShader() {
        const char vertexShaderCode[] = {
            "attribute vec4 vPosition;"
            "attribute vec2 vUV;"
            "uniform mat4 matrix;"
            "varying vec2 uv;"
            "void main() {"
            "  uv = vUV;"
            "  gl_Position = matrix * vPosition;"
            //"  gl_Position = gl_ModelViewProjectionMatrix * vPosition;"
            "}" };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            "uniform vec4 color;"
            "varying vec2 uv;"
            "uniform sampler2D texture;"
            "void main() {"
            "  vec4 texel = texture2D(texture, uv);"
            "  vec4 result = texel * color;"
            "  gl_FragColor = result;"
            "}" };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //vPosition = getAttribLocation("vPosition");
        //vUV = getAttribLocation("vUV");
        
        color = getUniformLocation("color");
        matrix = getUniformLocation("matrix");
        texture = getUniformLocation("texture");
    }


    void GLShaderTextureColor::setColor(const vec4 &c) {
        setUniform(color, c);
    }

    void GLShaderTextureColor::setMatrix(const mat4 & m) {
        setUniform(matrix, m);
    }

    void GLShaderTextureColor::setTexture(int activeTextureUnit) {
        setUniform(texture, activeTextureUnit);
    }
    
    void GLShaderTextureColor::setupAttribLocation() {
        bindAttribLocation(GLShaderTextureColor::vPosition, "vPosition");
        bindAttribLocation(GLShaderTextureColor::vUV, "vUV");
    }

}

