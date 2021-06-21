#ifndef detail__h
#define detail__h

/*
technique OpenGLDefault {
technique Bump_mapping {
technique Cone_step_mapping_original {
*/

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace aRibeiro;
using namespace openglWrapper;

class ShaderOpenGLDefault : public GLShader {
public:

};

class ShaderBumpMapping: public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DDiffuse;
    int uSampler2DBumpMap;

    int uVec3LightPosition;
    int uVec3LightAmbient;
    int uVec3LightDiffuse;
    int uVec3LightSpecular;
    int uFloatLightShine;
    int uFloatStrength;

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
    static const int aVec3Tangent = 3;
    static const int aVec3Binormal = 4;

    ShaderBumpMapping():GLShader() {

        const char vertexShaderCode[] = {

        #if defined(ARIBEIRO_RPI)
        "mat4 transpose(in mat4 inMatrix) { "
            "vec4 i0 = inMatrix[0]; "
            "vec4 i1 = inMatrix[1]; "
            "vec4 i2 = inMatrix[2]; "
            "vec4 i3 = inMatrix[3]; "
            "return mat4( "
                "vec4(i0.x, i1.x, i2.x, i3.x), "
                "vec4(i0.y, i1.y, i2.y, i3.y), "
                "vec4(i0.z, i1.z, i2.z, i3.z), "
                "vec4(i0.w, i1.w, i2.w, i3.w) "
            "); "
        "}"
        #else
            "#version 120\n"//opengl 2.1 - transpose function
        #endif

            //attributes
            "attribute vec3 aVec3Position;"
            "attribute vec2 aVec2UV;"
            "attribute vec3 aVec3Normal;"
            "attribute vec3 aVec3Tangent;"
            "attribute vec3 aVec3Binormal;"
            //uniforms
            "uniform mat4 uMat4ModelViewProjection;"
            "uniform mat4 uMat4ModelView;"
            "uniform mat4 uMat4ModelViewInverseTranspose;"

            "uniform vec3 uVec3LightPosition;"// {0.0f, 0.0f, 100.0f};
            //varying
            "varying vec4 vVec4Hpos;"
            "varying vec3 vVec3Eye;"
            "varying vec3 vVec3Light;"
            "varying vec2 vVec2UV;"
            //vertex program
            "void main() {"
                //calcula a coordenada homogenea da projecao configurada
                "vVec4Hpos = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
                //pega as coordenadas de textura vindas da aplicacao
                "vVec2UV = aVec2UV;"
                //rotacionando a tangente binormal e normal do modelo para coordenadas de mundo
                "mat4 tangentspace = mat4(vec4(aVec3Tangent.xyz,0),"
                                         "vec4(aVec3Binormal.xyz,0),"
                                         "vec4(aVec3Normal.xyz,0),"
                                         "vec4(0 ,0 ,0 ,1));"
                "tangentspace = uMat4ModelView * tangentspace;"
                "vec4 vpos = uMat4ModelView * vec4(aVec3Position,1.0);"
                "vec4 light = vec4(uVec3LightPosition,1);"
                //neste ponto pos, light e tangentspace estao no mesmo sistema de coordenadas
                //para transformar pos e light no sistema de coords tangenteMundo basta multiplicar pela inversa da matriz tangenteMundo
                "tangentspace = transpose(tangentspace);" //inversa de matriz ortogonal
                //aqui as coordenadas de eye representa os componentes de vpos em relacao aos eixos definidos pela matriz de rotacao da tangente
                //    o mesmo para a coordenada light
                "vVec3Eye = (tangentspace * vpos).xyz;"
                "vVec3Light = (tangentspace * (light - vpos) ).xyz;"//light-pos é o vetor que vai da posicao em questao para a luz
                "gl_Position = vVec4Hpos;"
            "}"
        };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            //uniforms
            "uniform sampler2D uSampler2DDiffuse;"
            "uniform sampler2D uSampler2DBumpMap;"

            "uniform vec3 uVec3LightAmbient;"// = {0.3,0.3,0.3};
            "uniform vec3 uVec3LightDiffuse;"// = {0.7,0.7,0.7};
            "uniform vec3 uVec3LightSpecular;"// = {0.4,0.4,0.4};
            "uniform float uFloatLightShine;"// = 32.0;

            "uniform float uFloatStrength;"

            //varying
            "varying vec4 vVec4Hpos;"
            "varying vec3 vVec3Eye;"
            "varying vec3 vVec3Light;"
            "varying vec2 vVec2UV;"

            //noemal map function
            // do normal mapping using given texture coordinate
            // tangent space phong lighting with optional border clamp
            // normal X and Y stored in red and green channels
            "vec4 normal_mapping(vec2 uv)"
            "{"
                // color map
                "vec4 color = texture2D(uSampler2DDiffuse, uv);"

                // normal map
                "vec3 normal = texture2D(uSampler2DBumpMap, uv).rgb;"
                "normal.xy = 2.0 * normal.xy - 1.0;"

                "normal.z = sqrt(1.0 - dot(normal.xy,normal.xy));"

                "normal = normalize( mix(vec3(0,0,1), normal, uFloatStrength) );"

                // light and view in tangent space
                "vec3 l = normalize(vVec3Light);"
                "vec3 v = normalize(vVec3Eye);"

                // compute diffuse and specular terms
                "float diff = clamp(dot(l,normal), 0.0, 1.0);"
                "float spec = clamp(dot(normalize(l - v),normal), 0.0, 1.0);"

                // attenuation factor
                //float att = 1.0 - max(0,l.z);
                //att = 1.0 - att*att;

                // compute final color
                "vec4 finalcolor;"
                "finalcolor.xyz = uVec3LightAmbient * color.xyz +"
                        //att * (color.xyz * uVec3LightDiffuse * diff +
                        "(color.xyz * uVec3LightDiffuse * diff +"
                        "uVec3LightSpecular * pow(spec, uFloatLightShine));"

                "finalcolor.w = 1.0;"
                "return finalcolor;"
            "}"

            //fragment program
            "void main() {"
            "  vec4 result = normal_mapping(vVec2UV);"
            "  gl_FragColor = result;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");
        //aVec3Normal = getAttribLocation("aVec3Normal");
        //aVec3Tangent = getAttribLocation("aVec3Tangent");
        //aVec3Binormal = getAttribLocation("aVec3Binormal");

        uSampler2DDiffuse = getUniformLocation("uSampler2DDiffuse");
        uSampler2DBumpMap = getUniformLocation("uSampler2DBumpMap");

        uVec3LightPosition = getUniformLocation("uVec3LightPosition");
        uVec3LightAmbient = getUniformLocation("uVec3LightAmbient");
        uVec3LightDiffuse = getUniformLocation("uVec3LightDiffuse");
        uVec3LightSpecular = getUniformLocation("uVec3LightSpecular");
        uFloatLightShine = getUniformLocation("uFloatLightShine");

        uFloatStrength = getUniformLocation("uFloatStrength");

        uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
        uMat4ModelView = getUniformLocation("uMat4ModelView");
        uMat4ModelViewInverseTranspose = getUniformLocation("uMat4ModelViewInverseTranspose");


    }


    void setDiffuse(int textureUnit) {
        setUniform(uSampler2DDiffuse, textureUnit);
    }

    void setBumpMap(int textureUnit) {
        setUniform(uSampler2DBumpMap, textureUnit);
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

    void setStrength(float v) {
        setUniform(uFloatStrength, v);
    }


protected:

    void setupAttribLocation() {
        bindAttribLocation(ShaderBumpMapping::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderBumpMapping::aVec2UV, "aVec2UV");
        bindAttribLocation(ShaderBumpMapping::aVec3Normal, "aVec3Normal");
        bindAttribLocation(ShaderBumpMapping::aVec3Tangent, "aVec3Tangent");
        bindAttribLocation(ShaderBumpMapping::aVec3Binormal, "aVec3Binormal");
    }

};


class ShaderConeStepMappingOriginal : public GLShader {

protected:

    //
    // uniform
    //
    int uSampler2DDiffuse;
    int uSampler2DConeMap;
    int uSampler2DBumpMap;

    int uFloatConeDepth;

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
    static const int aVec3Tangent = 3;
    static const int aVec3Binormal = 4;

    ShaderConeStepMappingOriginal() :GLShader() {

        const char vertexShaderCode[] = {
        #if defined(ARIBEIRO_RPI)
            "mat4 transpose(in mat4 inMatrix) { "
                "vec4 i0 = inMatrix[0]; "
                "vec4 i1 = inMatrix[1]; "
                "vec4 i2 = inMatrix[2]; "
                "vec4 i3 = inMatrix[3]; "
                "return mat4( "
                    "vec4(i0.x, i1.x, i2.x, i3.x), "
                    "vec4(i0.y, i1.y, i2.y, i3.y), "
                    "vec4(i0.z, i1.z, i2.z, i3.z), "
                    "vec4(i0.w, i1.w, i2.w, i3.w) "
                "); "
            "}"
        #else
            "#version 120\n"//opengl 2.1 - transpose function
        #endif

            //attributes
            "attribute vec3 aVec3Position;"
            "attribute vec2 aVec2UV;"
            "attribute vec3 aVec3Normal;"
            "attribute vec3 aVec3Tangent;"
            "attribute vec3 aVec3Binormal;"
            //uniforms
            "uniform mat4 uMat4ModelViewProjection;"
            "uniform mat4 uMat4ModelView;"
            "uniform mat4 uMat4ModelViewInverseTranspose;"

            "uniform vec3 uVec3LightPosition;"// {0.0f, 0.0f, 100.0f};
            //varying
            "varying vec4 vVec4Hpos;"
            "varying vec3 vVec3Eye;"
            "varying vec3 vVec3Light;"
            "varying vec2 vVec2UV;"
            //vertex program
            "void main() {"
                //calcula a coordenada homogenea da projecao configurada
                "vVec4Hpos = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
                //pega as coordenadas de textura vindas da aplicacao
                "vVec2UV = aVec2UV;"
                //rotacionando a tangente binormal e normal do modelo para coordenadas de mundo
                "mat4 tangentspace = mat4(vec4(aVec3Tangent.xyz,0),"
                                         "vec4(aVec3Binormal.xyz,0),"
                                         "vec4(aVec3Normal.xyz,0),"
                                         "vec4(0 ,0 ,0 ,1));"
                "tangentspace = uMat4ModelView * tangentspace;"
                "vec4 vpos = uMat4ModelView * vec4(aVec3Position,1.0);"
                "vec4 light = vec4(uVec3LightPosition,1);"
                //neste ponto pos, light e tangentspace estao no mesmo sistema de coordenadas
                //para transformar pos e light no sistema de coords tangenteMundo basta multiplicar pela inversa da matriz tangenteMundo
                "tangentspace = transpose(tangentspace);" //inversa de matriz ortogonal
                //aqui as coordenadas de eye representa os componentes de vpos em relacao aos eixos definidos pela matriz de rotacao da tangente
                //    o mesmo para a coordenada light
                "vVec3Eye = (tangentspace * vpos).xyz;"
                "vVec3Light = (tangentspace * (light - vpos) ).xyz;"//light-pos é o vetor que vai da posicao em questao para a luz
                "gl_Position = vVec4Hpos;"
            "}"
        };

        const char fragmentShaderCode[] = {
            //"precision mediump float;"
            //uniforms
            "uniform sampler2D uSampler2DDiffuse;"
            "uniform sampler2D uSampler2DConeMap;"
            "uniform sampler2D uSampler2DBumpMap;"
            "uniform float uFloatConeDepth;"// = 0.1

            "uniform vec3 uVec3LightAmbient;"// = {0.3,0.3,0.3};
            "uniform vec3 uVec3LightDiffuse;"// = {0.7,0.7,0.7};
            "uniform vec3 uVec3LightSpecular;"// = {0.4,0.4,0.4};
            "uniform float uFloatLightShine;"// = 32.0;

            //varying
            "varying vec4 vVec4Hpos;"
            "varying vec3 vVec3Eye;"
            "varying vec3 vVec3Light;"
            "varying vec2 vVec2UV;"

            //noemal map function
            // do normal mapping using given texture coordinate
            // tangent space phong lighting with optional border clamp
            // normal X and Y stored in red and green channels
            "vec4 normal_mapping(vec2 uv)"
            "{"
                // color map
                "vec4 color = texture2D(uSampler2DDiffuse, uv);"

                // normal map
                "vec3 normal = texture2D(uSampler2DBumpMap, uv).rgb;"
                "normal.xy = 2.0 * normal.xy - 1.0;"

                "normal.z = sqrt(1.0 - dot(normal.xy,normal.xy));"

                "normal = normalize( mix(vec3(0,0,1), normal, uFloatConeDepth) );"

                // light and view in tangent space
                "vec3 l = normalize(vVec3Light);"
                "vec3 v = normalize(vVec3Eye);"

                // compute diffuse and specular terms
                "float diff = clamp(dot(l,normal), 0.0, 1.0);"
                "float spec = clamp(dot(normalize(l - v),normal), 0.0, 1.0);"

                // attenuation factor
                //float att = 1.0 - max(0,l.z);
                //att = 1.0 - att*att;

                // compute final color
                "vec4 finalcolor;"
                "finalcolor.xyz = uVec3LightAmbient * color.xyz +"
                //att * (color.xyz * uVec3LightDiffuse * diff +
                "(color.xyz * uVec3LightDiffuse * diff +"
                "uVec3LightSpecular * pow(spec, uFloatLightShine));"

                "finalcolor.w = 1.0;"
                "return finalcolor;"
            "}"

            //cone step mapping
            //fragment program
            "\n#define NUM_ITERATIONS  32\n"//5000 //16
            "void main() {"
                //configurando a posicao
                "vec3 O = vec3(vVec2UV,0);"
                "vec3 v = vec3((vVec3Eye * (-(uFloatConeDepth * 0.2) / vVec3Eye.z)).xy, 1.0);"
                "float iz = max(abs(v.x),abs(v.y));"
                "const float w = 1.2;"
                "vec4 info;"
                "for (int i = 0; i < NUM_ITERATIONS; i++) {"
                    "info = texture2D(uSampler2DConeMap, O.xy);" //z = raio / w = height
                    "float scale = (info.w - O.z) / (iz / (info.z*info.z) + 1.0);"
                    "if (scale < 0.001)"
                        "break;"
                    "O += v * w*scale * vec3(1,-1,1);"//a texture foi gerada com a coordenada Y invertida...
                "}"
                "if (O.x > 1.0 || O.x < 0.0 ||"
                    "O.y > 1.0 || O.y < 0.0) discard;"
                "vec4 result = normal_mapping(O.xy);"
                "gl_FragColor = result;"
            "}"
        };

        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

        //aVec3Position = getAttribLocation("aVec3Position");
        //aVec2UV = getAttribLocation("aVec2UV");
        //aVec3Normal = getAttribLocation("aVec3Normal");
        //aVec3Tangent = getAttribLocation("aVec3Tangent");
        //aVec3Binormal = getAttribLocation("aVec3Binormal");

        uSampler2DDiffuse = getUniformLocation("uSampler2DDiffuse");
        uSampler2DConeMap = getUniformLocation("uSampler2DConeMap");
        uSampler2DBumpMap = getUniformLocation("uSampler2DBumpMap");
        uFloatConeDepth = getUniformLocation("uFloatConeDepth");

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
    void setConeMap(int textureUnit) {
        setUniform(uSampler2DConeMap, textureUnit);
    }
    void setBumpMap(int textureUnit) {
        setUniform(uSampler2DBumpMap, textureUnit);
    }
    void setConeDepth(float depth) {
        setUniform(uFloatConeDepth, depth);
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
        bindAttribLocation(ShaderConeStepMappingOriginal::aVec3Position, "aVec3Position");
        bindAttribLocation(ShaderConeStepMappingOriginal::aVec2UV, "aVec2UV");
        bindAttribLocation(ShaderConeStepMappingOriginal::aVec3Normal, "aVec3Normal");
        bindAttribLocation(ShaderConeStepMappingOriginal::aVec3Tangent, "aVec3Tangent");
        bindAttribLocation(ShaderConeStepMappingOriginal::aVec3Binormal, "aVec3Binormal");
    }

};

#endif
