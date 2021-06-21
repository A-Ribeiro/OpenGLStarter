#ifndef BloomThresholdShader__h
#define BloomThresholdShader__h

#include <opengl-wrapper/GLPostProcessingShader.h>

namespace aRibeiro {
    
    class BloomThresholdShader : public GLPostProcessingShader {
        
        int uSampler2DTexture;
        int uFloatThreshold;
        
    public:
        
        BloomThresholdShader() {
            const char vertexShaderCode[] = {
                //attributes
                "attribute vec2 aVec2Position;"
                "attribute vec2 aVec2UV;"
                //varying
                "varying vec2 vVec2UV;"
                //vertex program
                "void main() {"
                    //pega as coordenadas de textura vindas da aplicacao
                    "vVec2UV = aVec2UV;"
                    //calcula a coordenada homogenea da projecao configurada
                    "gl_Position = vec4(aVec2Position, 0.0, 1.0);"
                "}"
            };
            
            const char fragmentShaderCode[] = {
                //"precision mediump float;"
                //uniforms
                "uniform sampler2D uSampler2DTexture;"
                "uniform float uFloatThreshold;"
                
                //varying
                "varying vec2 vVec2UV;"
                
                "void main() {"
                    "vec3 texel = texture2D(uSampler2DTexture, vVec2UV ).rgb;"
                    //"texel = step(vec3(uFloatThreshold), texel);"//faz cor ir de 0 a 1
                    "texel = texel * step(vec3(uFloatThreshold), texel);"
                    /*"texel.r = texel.r * step(uFloatThreshold, texel.r);"
                    "texel.g = texel.g * step(uFloatThreshold, texel.g);"
                    "texel.b = texel.b * step(uFloatThreshold, texel.b);"*/
                    "vec4 result = vec4(texel, 1.0);"
                    "gl_FragColor = result;"
                "}"
            };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            
            //aVec2Position = getAttribLocation("aVec2Position");
            //aVec2UV = getAttribLocation("aVec2UV");
            
            uSampler2DTexture = getUniformLocation("uSampler2DTexture");
            uFloatThreshold = getUniformLocation("uFloatThreshold");
            
        }
        
        void setTexture(int texUnit){
            setUniform(uSampler2DTexture, texUnit);
        }
        void setThreshold(float v){
            setUniform(uFloatThreshold, v);
        }
        
        void setupAttribLocation() {
            bindAttribLocation(GLPostProcessingShader::aVec2Position, "aVec2Position");
            bindAttribLocation(GLPostProcessingShader::aVec2UV, "aVec2UV");
        }
        
    };
    
}

#endif

