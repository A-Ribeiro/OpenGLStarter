#ifndef filters__h
#define filters__h

/*

full screen shaders

uses framebuffer texture copy to process each pass

technique laplace {
technique _blur1 {
technique _blur5 {
*/

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace aRibeiro;
using namespace openglWrapper;

#define ShaderBlur_USE_15_SAMPLES 1

class ShaderLaplace : public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DFramebuffer;
    //
    // vec2(1.0/w,1.0/h)
    //
    int uVec2FramebufferTexelNeighbor;

    int uMat4ModelViewProjection;

    float uFloatIntensity;
    float uFloatBlend;

public:
    //
    // vertex attrib
    //
    static const int aVec3Position = 0;
    static const int aVec2UV = 1;

    ShaderLaplace() :GLShader() {

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
            "uniform sampler2D uSampler2DFramebuffer;"
            "uniform vec2 uVec2FramebufferTexelNeighbor;"
            "uniform float uFloatIntensity;"
            "uniform float uFloatBlend;"

            //varying
            "varying vec2 vVec2UV;"

            "void main() {"
                "vec3 centerTexelColor = texture2D(uSampler2DFramebuffer, vVec2UV ).rgb;"
                //    -1
                //  -1 4-1
                //    -1
            
                "vec3 texel = abs("
                    "centerTexelColor * 4.0"
                    "-texture2D(uSampler2DFramebuffer, vVec2UV + uVec2FramebufferTexelNeighbor * vec2(-1, 0)).rgb"
                    "-texture2D(uSampler2DFramebuffer, vVec2UV + uVec2FramebufferTexelNeighbor * vec2( 1, 0)).rgb"
                    "-texture2D(uSampler2DFramebuffer, vVec2UV + uVec2FramebufferTexelNeighbor * vec2( 0, 1)).rgb"
                    "-texture2D(uSampler2DFramebuffer, vVec2UV + uVec2FramebufferTexelNeighbor * vec2( 0,-1)).rgb"
                ") * ( uFloatIntensity * 1.1 );"

                "texel = vec3(1.0 - clamp(texel.r + texel.g + texel.b, 0.0, 1.0));"
                "texel = mix(centerTexelColor, texel, uFloatBlend);"
                "vec4 result = vec4(texel, 1.0);"
                "gl_FragColor = result;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");

        uSampler2DFramebuffer = getUniformLocation("uSampler2DFramebuffer");
        uVec2FramebufferTexelNeighbor = getUniformLocation("uVec2FramebufferTexelNeighbor");
        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
        uFloatIntensity = getUniformLocation("uFloatIntensity");
        uFloatBlend = getUniformLocation("uFloatBlend");
    }


    void setFramebuffer(int textureUnit) {
        setUniform(uSampler2DFramebuffer, textureUnit);
    }

    void setFramebufferTexelNeighbor(const vec2 &v) {
        setUniform(uVec2FramebufferTexelNeighbor, v);
    }

    void setModelViewProjection(const mat4 &matrix) {
        setUniform(uMat4ModelViewProjection, matrix);
    }

    void setIntensity(float v) {
        setUniform(uFloatIntensity, v);
    }

    void setBlend(float v) {
        setUniform(uFloatBlend, v);
    }

    
protected:
    
    void setupAttribLocation() {
        bindAttribLocation(ShaderLaplace::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderLaplace::aVec2UV, "aVec2UV");
    }
    
};

class ShaderBlur : public GLShader {


protected:

    //
    // uniform
    //
    int uSampler2DFramebuffer;
    //
    // vec2(1.0/w,1.0/h)
    //
    int uVec2FramebufferTexelNeighbor;

    int uMat4ModelViewProjection;

    int uVec2HorizontalVertical;

public:
    //
    // vertex attrib
    //
    static const int aVec3Position = 0;
    static const int aVec2UV = 1;

    ShaderBlur() :GLShader() {

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
            "uniform sampler2D uSampler2DFramebuffer;"
            "uniform vec2 uVec2FramebufferTexelNeighbor;"
            "uniform vec2 uVec2HorizontalVertical;"
            //varying
            "varying vec2 vVec2UV;"

#if ShaderBlur_USE_15_SAMPLES == 1
            "uniform float coef[15];"
#else
            "uniform float coef[7];"
#endif
            "void main() {"
            
                "vec3 centerTexelColor = texture2D(uSampler2DFramebuffer, vVec2UV ).rgb;"
                "vec3 texel = vec3(0.0);"
#if ShaderBlur_USE_15_SAMPLES == 1
                "for (int i = -7; i <= 7; i++) {"
                    "float iF = float (i);"
                    "vec2 uv = vVec2UV + uVec2FramebufferTexelNeighbor * uVec2HorizontalVertical * iF;"
                    "texel += texture2D(uSampler2DFramebuffer, uv ).rgb * coef[i + 7];"
                "}"
                
#else
                "for (int i = -3; i <= 3; i++) {"
                    "float iF = float (i);"
                    "vec2 uv = vVec2UV + uVec2FramebufferTexelNeighbor * uVec2HorizontalVertical * iF;"
                    "texel += texture2D(uSampler2DFramebuffer, uv ).rgb * coef[i + 3];"
                "}"
            
#endif
             
                "vec4 result = vec4(texel, 1.0);"
                "gl_FragColor = result;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
        
#if ShaderBlur_USE_15_SAMPLES == 1
        enable();
        float gaussCoefs_15[] = {0.0229490642f, 0.0344506279f, 0.0485831723f, 0.06436224f, 0.0801001f, 0.09364651f, 0.102850571f, 0.1061154f, 0.102850571f, 0.09364651f, 0.0801001f, 0.06436224f, 0.0485831723f, 0.0344506279f, 0.0229490642f};
        glUniform1fv(getUniformLocation("coef"),15,gaussCoefs_15);
#else
        enable();
        float coef[] = {0.015625f, 0.093750f, 0.234375f, 0.312500f, 0.234375f, 0.093750f, 0.015625f};
        glUniform1fv(getUniformLocation("coef"),7,coef);
#endif

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");

        uSampler2DFramebuffer = getUniformLocation("uSampler2DFramebuffer");
        uVec2FramebufferTexelNeighbor = getUniformLocation("uVec2FramebufferTexelNeighbor");

        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");

        uVec2HorizontalVertical = getUniformLocation("uVec2HorizontalVertical");
    }


    void setFramebuffer(int textureUnit) {
        setUniform(uSampler2DFramebuffer, textureUnit);
    }

    void setFramebufferTexelNeighbor(const vec2 &v) {
        setUniform(uVec2FramebufferTexelNeighbor, v);
    }

    void setModelViewProjection(const mat4 &matrix) {
        setUniform(uMat4ModelViewProjection, matrix);
    }

    void setHorizontal() {
        setUniform(uVec2HorizontalVertical, vec2(1.0f,0.0f));
    }

    void setVertical() {
        setUniform(uVec2HorizontalVertical, vec2(0.0f, 1.0f));
    }
    
protected:
    
    void setupAttribLocation() {
        bindAttribLocation(ShaderBlur::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderBlur::aVec2UV, "aVec2UV");
    }
    

};


#endif
