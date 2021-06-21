#ifndef radiometric__h
#define radiometric__h

/*
technique BrilhoContrasteX {
technique NivelCinza_internet {
technique NivelCinza_SVH {
technique NivelCinza_SisCorHSV {
technique BrilhoContrasteXPink {
technique NivelCinza_internetPink {
technique NivelCinza_SVHPink {
technique NivelCinza_SisCorHSVPink {
*/

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace aRibeiro;
using namespace openglWrapper;

class ShaderBrightnessContrast : public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DTexture;

    int uMat4ModelViewProjection;

    float uFloatBrightness;
    float uFloatContrast;

public:
    //
    // vertex attrib
    //
    static const int aVec3Position = 0;
    static const int aVec2UV = 1;

    ShaderBrightnessContrast() :GLShader() {

        const char vertexShaderCode[] = {
            //attributes
            "attribute vec3 aVec3Position;"
            "attribute vec2 aVec2UV;"
            //uniforms
            "uniform mat4 uMat4ModelViewProjection;"
            //varying
            "varying vec2 vVec2UV;"
            //vertex program
            "void main() {"
                //pega as coordenadas de textura vindas da aplicacao
                "vVec2UV = aVec2UV;"
                //calcula a coordenada homogenea da projecao configurada
                "gl_Position = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
            "}"
        };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            //uniforms
            "uniform sampler2D uSampler2DTexture;"
            "uniform float uFloatBrightness;"
            "uniform float uFloatContrast;"

            //varying
            "varying vec2 vVec2UV;"

            "void main() {"
                "vec3 textel = texture2D(uSampler2DTexture, vVec2UV).rgb;"
                "float brilho = (uFloatBrightness - 0.5) * 2.0;"
                "float contraste = (uFloatContrast - 0.5) * 4.0 + 1.0;"
                "textel = (textel - 0.5)*contraste + 0.5 + brilho;"
                "vec4 result = vec4(textel, 1.0);"
                "gl_FragColor = result;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");

        uSampler2DTexture = getUniformLocation("uSampler2DTexture");
        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
        uFloatBrightness = getUniformLocation("uFloatBrightness");
        uFloatContrast = getUniformLocation("uFloatContrast");
    }


    void setTexture(int textureUnit) {
        setUniform(uSampler2DTexture, textureUnit);
    }

    void setModelViewProjection(const mat4 &matrix) {
        setUniform(uMat4ModelViewProjection, matrix);
    }

    void setBrightness(float v) {
        setUniform(uFloatBrightness, v);
    }

    void setContrast(float v) {
        setUniform(uFloatContrast, v);
    }
    
protected:
    
    void setupAttribLocation() {
        
        bindAttribLocation(ShaderBrightnessContrast::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderBrightnessContrast::aVec2UV, "aVec2UV");
    }
    
};

class ShaderGrayScaleInternet : public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DTexture;

    int uMat4ModelViewProjection;

public:
    //
    // vertex attrib
    //
    static const int aVec3Position = 0;
    static const int aVec2UV = 1;

    ShaderGrayScaleInternet() :GLShader() {

        const char vertexShaderCode[] = {
            //attributes
            "attribute vec3 aVec3Position;"
            "attribute vec2 aVec2UV;"
            //uniforms
            "uniform mat4 uMat4ModelViewProjection;"
            //varying
            "varying vec2 vVec2UV;"
            //vertex program
            "void main() {"
                //pega as coordenadas de textura vindas da aplicacao
                "vVec2UV = aVec2UV;"
                //calcula a coordenada homogenea da projecao configurada
                "gl_Position = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
            "}"
        };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            //uniforms
            "uniform sampler2D uSampler2DTexture;"

            //varying
            "varying vec2 vVec2UV;"

            "void main() {"
                "vec3 textel = texture2D(uSampler2DTexture, vVec2UV).rgb;"
                "textel = vec3(max(max(textel.r,textel.g),textel.b));"
                "vec4 result = vec4(textel, 1.0);"
                "gl_FragColor = result;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");

        uSampler2DTexture = getUniformLocation("uSampler2DTexture");
        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
    }


    void setTexture(int textureUnit) {
        setUniform(uSampler2DTexture, textureUnit);
    }

    void setModelViewProjection(const mat4 &matrix) {
        setUniform(uMat4ModelViewProjection, matrix);
    }
    
protected:
    
    void setupAttribLocation() {
        
        bindAttribLocation(ShaderGrayScaleInternet::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderGrayScaleInternet::aVec2UV, "aVec2UV");
    }
};

class ShaderGrayScaleHumanVisualSystem : public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DTexture;

    int uMat4ModelViewProjection;

public:
    //
    // vertex attrib
    //
    static const int aVec3Position = 0;
    static const int aVec2UV = 1;

    ShaderGrayScaleHumanVisualSystem() :GLShader() {

        const char vertexShaderCode[] = {
            //attributes
            "attribute vec3 aVec3Position;"
            "attribute vec2 aVec2UV;"
            //uniforms
            "uniform mat4 uMat4ModelViewProjection;"
            //varying
            "varying vec2 vVec2UV;"
            //vertex program
            "void main() {"
                //pega as coordenadas de textura vindas da aplicacao
                "vVec2UV = aVec2UV;"
                //calcula a coordenada homogenea da projecao configurada
                "gl_Position = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
            "}"
        };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            //uniforms
            "uniform sampler2D uSampler2DTexture;"

            //varying
            "varying vec2 vVec2UV;"

            "void main() {"
                "vec3 textel = texture2D(uSampler2DTexture, vVec2UV).rgb;"
                "textel *= vec3(0.299, 0.587,0.114);"
                "textel = vec3(textel.r + textel.g + textel.b);"
                "vec4 result = vec4(textel, 1.0);"
                "gl_FragColor = result;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");

        uSampler2DTexture = getUniformLocation("uSampler2DTexture");
        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
    }


    void setTexture(int textureUnit) {
        setUniform(uSampler2DTexture, textureUnit);
    }

    void setModelViewProjection(const mat4 &matrix) {
        setUniform(uMat4ModelViewProjection, matrix);
    }
    
protected:
    
    void setupAttribLocation() {
        
        bindAttribLocation(ShaderGrayScaleHumanVisualSystem::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderGrayScaleHumanVisualSystem::aVec2UV, "aVec2UV");
    }
    
};

class ShaderGrayScaleHueSaturationValue : public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DTexture;

    int uMat4ModelViewProjection;

public:
    //
    // vertex attrib
    //
    static const int aVec3Position = 0;
    static const int aVec2UV = 1;

    ShaderGrayScaleHueSaturationValue() :GLShader() {

        const char vertexShaderCode[] = {
            //attributes
            "attribute vec3 aVec3Position;"
            "attribute vec2 aVec2UV;"
            //uniforms
            "uniform mat4 uMat4ModelViewProjection;"
            //varying
            "varying vec2 vVec2UV;"
            //vertex program
            "void main() {"
            //pega as coordenadas de textura vindas da aplicacao
            "vVec2UV = aVec2UV;"
            //calcula a coordenada homogenea da projecao configurada
            "gl_Position = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
        "}"
        };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            //uniforms
            "uniform sampler2D uSampler2DTexture;"

            //varying
            "varying vec2 vVec2UV;"

            "void main() {"
                "vec3 textel = texture2D(uSampler2DTexture, vVec2UV).rgb;"
                "textel = vec3((textel.r+textel.g+textel.b)*0.33333);"
                "vec4 result = vec4(textel, 1.0);"
                "gl_FragColor = result;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");

        uSampler2DTexture = getUniformLocation("uSampler2DTexture");
        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
    }


    void setTexture(int textureUnit) {
        setUniform(uSampler2DTexture, textureUnit);
    }

    void setModelViewProjection(const mat4 &matrix) {
        setUniform(uMat4ModelViewProjection, matrix);
    }
    
protected:
    
    void setupAttribLocation() {
        
        bindAttribLocation(ShaderGrayScaleHumanVisualSystem::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderGrayScaleHumanVisualSystem::aVec2UV, "aVec2UV");
    }
};

#endif
