#include "GLShaderVertexColor.h"
#include <opengl-wrapper/PlatformGL.h>

using namespace aRibeiro;

namespace openglWrapper {
    
    GLShaderVertexColor::GLShaderVertexColor() :GLShader() {
        const char vertexShaderCode[] = {
            "attribute vec4 vPosition;"
            "attribute vec4 vColor;"
            "uniform mat4 matrix;"
            "varying vec4 color;"
            "void main() {"
            "  color = vColor;"
            "  gl_Position = matrix * vPosition;"
            //"  gl_Position = gl_ModelViewProjectionMatrix * vPosition;"
            "}" };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            "varying vec4 color;"
            "void main() {"
            "  vec4 result = color;"
            "  gl_FragColor = result;"
            "}" };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //vPosition = getAttribLocation("vPosition");
        //vColor = getAttribLocation("vColor");

        matrix = getUniformLocation("matrix");
    }


    void GLShaderVertexColor::setMatrix(const mat4 & m) {
        setUniform(matrix, m);
    }
    
    void GLShaderVertexColor::setupAttribLocation() {
        bindAttribLocation(GLShaderVertexColor::vPosition, "vPosition");
        bindAttribLocation(GLShaderVertexColor::vColor, "vColor");
    }

}

