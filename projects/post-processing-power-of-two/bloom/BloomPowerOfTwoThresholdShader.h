#ifndef BloomPowerOfTwoThresholdShader__h
#define BloomPowerOfTwoThresholdShader__h

#include <opengl-wrapper/opengl-wrapper.h>
using namespace openglWrapper;
#include "../GLPostProcessPowerOfTwo.h"

namespace aRibeiro {
    
    class BloomPowerOfTwoThresholdShader : public GLPostProcessingPowerOfTwoShader {
        
        int uSampler2DTexture;
        int uFloatThreshold;
        int uVec2TexelMaxUVAccess;
        
    public:
        
        BloomPowerOfTwoThresholdShader() {
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
                
                //post process uv access
                "uniform vec2 uVec2TexelMaxUVAccess;"
                "vec4 pp_tex2d(sampler2D tex, vec2 uv){"
                "    return texture2D(tex, clamp(uv, vec2(0.0), uVec2TexelMaxUVAccess ));"
                "}"
                
                //varying
                "varying vec2 vVec2UV;"
                
                "void main() {"
                    "vec3 texel = pp_tex2d(uSampler2DTexture, vVec2UV ).rgb;"
                    "texel = texel * step(vec3(uFloatThreshold), texel);"
                    "vec4 result = vec4(texel, 1.0);"
                    "gl_FragColor = result;"
                "}"
            };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            
            //aVec2Position = getAttribLocation("aVec2Position");
            //aVec2UV = getAttribLocation("aVec2UV");
            
            uSampler2DTexture = getUniformLocation("uSampler2DTexture");
            uFloatThreshold = getUniformLocation("uFloatThreshold");
            
            uVec2TexelMaxUVAccess = getUniformLocation("uVec2TexelMaxUVAccess");
            
        }
        
        void setTexture(int texUnit){
            setUniform(uSampler2DTexture, texUnit);
        }
        void setThreshold(float v){
            setUniform(uFloatThreshold, v);
        }
        
        void setTexelNeighbor(const vec2 &texelNeighbor){
            
        }
        
        void setTexelMaxUVAccess(const vec2 &texelMaxUVAccess){
            setUniform(uVec2TexelMaxUVAccess, texelMaxUVAccess);
        }
        
        void setupAttribLocation() {
            bindAttribLocation(GLPostProcessingShader::aVec2Position, "aVec2Position");
            bindAttribLocation(GLPostProcessingShader::aVec2UV, "aVec2UV");
        }
        
    };
    
}

#endif

