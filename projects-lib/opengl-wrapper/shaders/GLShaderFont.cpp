#include "GLShaderFont.h"
#include "opengl-wrapper/PlatformGL.h"

using namespace aRibeiro;

namespace openglWrapper {

    GLShaderFont::GLShaderFont() :GLShader() {
        const char vertexShaderCode[] = {
            "attribute vec2 vPosition;"
            "attribute vec4 vColor;"
            "attribute vec2 vUV;"
            "uniform mat4 matrix;"
            "varying vec2 uv;"
            "varying vec4 color;"
            "void main() {"
            "  uv = vUV;"
            "  color = vColor;"
            "  vec4 p;"
            "  p.xy = vPosition.xy;"
            "  p.zw = vec2(0.0,1.0);"
            "  gl_Position = matrix * p;"
            //"  gl_Position = gl_ModelViewProjectionMatrix * p;"
            "}" };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            "varying vec2 uv;"
            "varying vec4 color;"
            "uniform sampler2D texture;"
            "void main() {"
            "  float alpha = texture2D(texture, uv).a;"
            "  vec4 result = vec4(color.r,color.g,color.b,alpha*color.a);"
            "  gl_FragColor = result;"
            "}" };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //vPosition = getAttribLocation("vPosition");
        //vColor = getAttribLocation("vColor");
        //vUV = getAttribLocation("vUV");

        texture = getUniformLocation("texture");
        matrix = getUniformLocation("matrix");
    }

    // Set the texture unit, in OpenGL you can use 8 textures units at same time (from 0 to 7)
    void GLShaderFont::setTexture(int activeTextureUnit) {
        setUniform(texture, activeTextureUnit);
    }

    void GLShaderFont::setMatrix(const mat4 & m) {
        setUniform(matrix, m);
    }
    
    void GLShaderFont::setupAttribLocation() {
        bindAttribLocation(GLShaderFont::vPosition, "vPosition");
        bindAttribLocation(GLShaderFont::vColor, "vColor");
        bindAttribLocation(GLShaderFont::vUV, "vUV");
    }

}
