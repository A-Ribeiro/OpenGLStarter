#ifndef BloomCompositeShader__h
#define BloomCompositeShader__h

#include <opengl-wrapper/GLPostProcessingShader.h>

namespace aRibeiro {
    
    class BloomCompositeShader : public GLPostProcessingShader {
        
        int uSampler2DSourceTexture;
        int uSampler2DBlurTexture;
        int uFloatIntensity;
        
    public:
        
        BloomCompositeShader() {
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
                "uniform sampler2D uSampler2DSourceTexture;"
                "uniform sampler2D uSampler2DBlurTexture;"
                "uniform float uFloatIntensity;"
                
                //varying
                "varying vec2 vVec2UV;"
                
                "void main() {"
                    "vec3 source = texture2D(uSampler2DSourceTexture, vVec2UV).rgb;"
                    "vec3 blur = texture2D(uSampler2DBlurTexture, vVec2UV).rgb;"
                    "vec3 color = clamp( source + blur * uFloatIntensity * 2.0, vec3(0.0),vec3(1.0));"
                    "vec4 result = vec4(color, 1.0);"
                    "gl_FragColor = result;"
                "}"
            };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            
            //aVec2Position = getAttribLocation("aVec2Position");
            //aVec2UV = getAttribLocation("aVec2UV");
            
            uSampler2DSourceTexture = getUniformLocation("uSampler2DSourceTexture");
            uSampler2DBlurTexture = getUniformLocation("uSampler2DBlurTexture");
            uFloatIntensity = getUniformLocation("uFloatIntensity");
            
        }
        
        void setSourceTexture(int texUnit){
            setUniform(uSampler2DSourceTexture, texUnit);
        }
        void setBlurTexture(int texUnit){
            setUniform(uSampler2DBlurTexture, texUnit);
        }
        void setIntensity(float v){
            setUniform(uFloatIntensity, v);
        }
        
        void setupAttribLocation() {
            bindAttribLocation(GLPostProcessingShader::aVec2Position, "aVec2Position");
            bindAttribLocation(GLPostProcessingShader::aVec2UV, "aVec2UV");
        }
        
    };

}

#endif
