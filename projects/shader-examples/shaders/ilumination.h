#ifndef ilumination__h
#define ilumination__h


#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace aRibeiro;
using namespace openglWrapper;

/*
technique RenderJustColor {
technique BlinPhongVert {
technique BlinPhongPixel {
technique TextureSimplePink {
technique TextureSimplePulga {
technique TexturePinkPulga {
technique TextureProcedural {
technique TextureLight {
*/


class ShaderBlinPhongVertex : public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DDiffuse;
    int uVec3ObjectColor;

    int uVec3LightPosition;
    int uVec3LightAmbient;
    int uVec3LightDiffuse;
    int uVec3LightSpecular;
    int uFloatLightShine;

    int uMat4ModelViewProjection;
    int uMat4ModelView;
    int uMat4ModelViewInverseTranspose;

public:
    //
    // vertex attrib
    //
    static const int aVec3Position = 0;
    static const int aVec2UV = 1;
    static const int aVec3Normal = 2;

    ShaderBlinPhongVertex() :GLShader() {

        const char vertexShaderCode[] = {
            //attributes
            "attribute vec3 aVec3Position;"
            "attribute vec2 aVec2UV;"
            "attribute vec3 aVec3Normal;"
            //uniforms
            "uniform mat4 uMat4ModelViewProjection;"
            "uniform mat4 uMat4ModelView;"
            "uniform mat4 uMat4ModelViewInverseTranspose;"

            "uniform vec3 uVec3LightPosition;"// {0.0f, 0.0f, 100.0f};
            "uniform vec3 uVec3LightAmbient;"// = {0.3,0.3,0.3};
            "uniform vec3 uVec3LightDiffuse;"// = {0.7,0.7,0.7};
            "uniform vec3 uVec3LightSpecular;"// = {0.4,0.4,0.4};
            "uniform float uFloatLightShine;"// = 32.0; 256
            
            "uniform vec3 uVec3ObjectColor;"

            //varying
            "varying vec2 vVec2UV;"
            "varying vec3 vVec3ColorMultiply;"
            "varying vec3 vVec3ColorSum;"
            //vertex program
            "void main() {"

                "vec3 pos = ( uMat4ModelView * vec4(aVec3Position,1.0) ).xyz;"
                "vec3 light = normalize( uVec3LightPosition - pos );"
                "vec3 normal = normalize( ( uMat4ModelViewInverseTranspose * vec4(aVec3Normal,1.0) ).xyz);"
                "vec3 eyeVec = vec3(0,0,1);"
                "vec3 halfV = normalize(eyeVec + light);"
                "float kd = clamp(dot(normal, light), 0.0, 1.0);"
                "float ke = clamp(dot(normal, halfV), 0.0, 1.0);"
                "vVec3ColorMultiply = uVec3ObjectColor * ( uVec3LightAmbient + kd * uVec3LightDiffuse );"
                "vVec3ColorSum = uVec3LightSpecular * pow(ke, uFloatLightShine);"
                "vVec2UV = aVec2UV;"

                "gl_Position = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
            "}"
        };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            //uniforms
            "uniform sampler2D uSampler2DDiffuse;"

            //varying
            "varying vec2 vVec2UV;"
            "varying vec3 vVec3ColorMultiply;"
            "varying vec3 vVec3ColorSum;"

            //fragment program
            "void main() {"
                "vec4 color = texture2D(uSampler2DDiffuse, vVec2UV);"
                "color.rgb = vVec3ColorMultiply*color.rgb + vVec3ColorSum;"
                "gl_FragColor = color;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");
        //aVec3Normal = getAttribLocation("aVec3Normal");

        uSampler2DDiffuse = getUniformLocation("uSampler2DDiffuse");
        uVec3ObjectColor = getUniformLocation("uVec3ObjectColor");

        uVec3LightPosition = getUniformLocation("uVec3LightPosition");
        uVec3LightAmbient = getUniformLocation("uVec3LightAmbient");
        uVec3LightDiffuse = getUniformLocation("uVec3LightDiffuse");
        uVec3LightSpecular = getUniformLocation("uVec3LightSpecular");
        uFloatLightShine = getUniformLocation("uFloatLightShine");

        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
        uMat4ModelView = getUniformLocation("uMat4ModelView");
        uMat4ModelViewInverseTranspose = getUniformLocation("uMat4ModelViewInverseTranspose");

    }


    void setDiffuse(int textureUnit) {
        setUniform(uSampler2DDiffuse, textureUnit);
    }

    void setObjectColor(const vec3 &color) {
        setUniform(uVec3ObjectColor, color);
    }

    void setModelViewProjection(const mat4 &matrix) {
        setUniform(uMat4ModelViewProjection, matrix);
    }
    void setModelView(const mat4 &matrix) {
        setUniform(uMat4ModelView, matrix);
    }
    void setModelViewInverseTranspose(const mat4 &matrix) {
        setUniform(uMat4ModelViewInverseTranspose, matrix);
    }

    void setLightPosition(const vec3 &pos) {
        setUniform(uVec3LightPosition, pos);
    }

    void setLightAmbient(const vec3 &color) {
        setUniform(uVec3LightAmbient, color);
    }

    void setLightDiffuse(const vec3 &color) {
        setUniform(uVec3LightDiffuse, color);
    }

    void setLightSpecular(const vec3 &color) {
        setUniform(uVec3LightSpecular, color);
    }

    void setLightShine(float v) {
        setUniform(uFloatLightShine, v);
    }
    
protected:
    
    void setupAttribLocation() {
        
        bindAttribLocation(ShaderBlinPhongVertex::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderBlinPhongVertex::aVec2UV, "aVec2UV");
        bindAttribLocation(ShaderBlinPhongVertex::aVec3Normal, "aVec3Normal");
    }
    
};

class ShaderBlinPhongPixel : public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DDiffuse;
    int uVec3ObjectColor;

    int uVec3LightPosition;
    int uVec3LightAmbient;
    int uVec3LightDiffuse;
    int uVec3LightSpecular;
    int uFloatLightShine;

    int uMat4ModelViewProjection;
    int uMat4ModelView;
    int uMat4ModelViewInverseTranspose;

public:
    //
    // vertex attrib
    //
    static const int aVec3Position = 0;
    static const int aVec2UV = 1;
    static const int aVec3Normal = 2;

    ShaderBlinPhongPixel() :GLShader() {

        const char vertexShaderCode[] = {
            //attributes
            "attribute vec3 aVec3Position;"
            "attribute vec2 aVec2UV;"
            "attribute vec3 aVec3Normal;"
            //uniforms
            "uniform mat4 uMat4ModelViewProjection;"
            "uniform mat4 uMat4ModelView;"
            "uniform mat4 uMat4ModelViewInverseTranspose;"

            "uniform vec3 uVec3LightPosition;"// {0.0f, 0.0f, 100.0f};

            //varying
            "varying vec2 vVec2UV;"
            "varying vec3 vVec3Normal;"
            "varying vec3 vVec3Half;"
            "varying vec3 vVec3Light;"
            //vertex program
            "void main() {"

                "vec3 pos = (uMat4ModelView * vec4(aVec3Position,1.0)).xyz;"
                "vec3 light = normalize(uVec3LightPosition - pos);"
                "vec3 normalVec = (uMat4ModelViewInverseTranspose * vec4(aVec3Normal,1.0)).xyz;"  //will be normalized in fragment shader
                "vec3 eyeVec = vec3(0,0,1);"
                "vVec3Half = eyeVec + light;"  //will be normalized in fragment shader
                "vVec3Normal = normalVec;"
                "vVec3Light = light;"
                "vVec2UV = aVec2UV;"

                "gl_Position = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
            "}"
        };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            //uniforms
            "uniform sampler2D uSampler2DDiffuse;"

            "uniform vec3 uVec3LightAmbient;"// = {0.3,0.3,0.3};
            "uniform vec3 uVec3LightDiffuse;"// = {0.7,0.7,0.7};
            "uniform vec3 uVec3LightSpecular;"// = {0.4,0.4,0.4};
            "uniform float uFloatLightShine;"// = 32.0; 256

            "uniform vec3 uVec3ObjectColor;"

            //varying
            "varying vec2 vVec2UV;"
            "varying vec3 vVec3Normal;"
            "varying vec3 vVec3Half;"
            "varying vec3 vVec3Light;"

            //fragment program
            "void main() {"
                "vec4 color = texture2D(uSampler2DDiffuse, vVec2UV);"

                "vec3 normal = normalize(vVec3Normal);"
                "float kd = clamp(dot(normal, normalize(vVec3Light)), 0.0, 1.0);"
                "float ke = clamp(dot(normal, normalize(vVec3Half)), 0.0, 1.0);"

                "color.rgb = color.rgb * (uVec3LightAmbient + kd * uVec3LightDiffuse) + uVec3LightSpecular * pow(ke,uFloatLightShine);"

                "gl_FragColor = color;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");
        //aVec3Normal = getAttribLocation("aVec3Normal");

        uSampler2DDiffuse = getUniformLocation("uSampler2DDiffuse");
        uVec3ObjectColor = getUniformLocation("uVec3ObjectColor");

        uVec3LightPosition = getUniformLocation("uVec3LightPosition");
        uVec3LightAmbient = getUniformLocation("uVec3LightAmbient");
        uVec3LightDiffuse = getUniformLocation("uVec3LightDiffuse");
        uVec3LightSpecular = getUniformLocation("uVec3LightSpecular");
        uFloatLightShine = getUniformLocation("uFloatLightShine");

        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
        uMat4ModelView = getUniformLocation("uMat4ModelView");
        uMat4ModelViewInverseTranspose = getUniformLocation("uMat4ModelViewInverseTranspose");

    }


    void setDiffuse(int textureUnit) {
        setUniform(uSampler2DDiffuse, textureUnit);
    }

    void setObjectColor(const vec3 &color) {
        setUniform(uVec3ObjectColor, color);
    }

    void setModelViewProjection(const mat4 &matrix) {
        setUniform(uMat4ModelViewProjection, matrix);
    }
    void setModelView(const mat4 &matrix) {
        setUniform(uMat4ModelView, matrix);
    }
    void setModelViewInverseTranspose(const mat4 &matrix) {
        setUniform(uMat4ModelViewInverseTranspose, matrix);
    }

    void setLightPosition(const vec3 &pos) {
        setUniform(uVec3LightPosition, pos);
    }

    void setLightAmbient(const vec3 &color) {
        setUniform(uVec3LightAmbient, color);
    }

    void setLightDiffuse(const vec3 &color) {
        setUniform(uVec3LightDiffuse, color);
    }

    void setLightSpecular(const vec3 &color) {
        setUniform(uVec3LightSpecular, color);
    }

    void setLightShine(float v) {
        setUniform(uFloatLightShine, v);
    }
    
protected:
    
    void setupAttribLocation() {
        
        bindAttribLocation(ShaderBlinPhongPixel::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderBlinPhongPixel::aVec2UV, "aVec2UV");
        bindAttribLocation(ShaderBlinPhongPixel::aVec3Normal, "aVec3Normal");
    }
    
};

#endif
