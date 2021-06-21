#include "GLShaderColor.h"
#include "opengl-wrapper/PlatformGL.h"

using namespace aRibeiro;

namespace openglWrapper {

    GLShaderColor::GLShaderColor() :GLShader() {
        const char vertexShaderCode[] = {
            "attribute vec4 vPosition;"
            "uniform mat4 matrix;"
            "void main() {"
            "  gl_Position = matrix * vPosition;"
            //"  gl_Position = gl_ModelViewProjectionMatrix * vPosition;"
            "}" };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            "uniform vec4 color;"
            "void main() {"
            "  vec4 result = color;"
            "  gl_FragColor = result;"
            "}" };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //vPosition = getAttribLocation("vPosition");

        color = getUniformLocation("color");
        matrix = getUniformLocation("matrix");
    }


    void GLShaderColor::setColor(const vec4 &c) {
        setUniform(color, c);
    }

    void GLShaderColor::setMatrix(const mat4 & m) {
        setUniform(matrix, m);
    }

    void GLShaderColor::setupAttribLocation() {
        bindAttribLocation(GLShaderColor::vPosition, "vPosition");
    }

}

