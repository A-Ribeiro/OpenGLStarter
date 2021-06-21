
#ifndef BloomBlurShader__h
#define BloomBlurShader__h

#include <opengl-wrapper/GLPostProcessingShader.h>

namespace aRibeiro {
    
#define BloomBlurShader_USE_GAUSSIAN 1
    
    class BloomBlurShader : public GLPostProcessingShader {
        
        
        int uSampler2DTexture;
        int uVec2TextureNeighbor;
        int uVec2Direction;
        
    public:
        
        BloomBlurShader() {
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
                "uniform vec2 uVec2TextureNeighbor;"
                "uniform vec2 uVec2Direction;"
#if BloomBlurShader_USE_GAUSSIAN == 1
                "uniform float gaussCoefs_15[15];"
#else
#endif
                
                //varying
                "varying vec2 vVec2UV;"
                
                "void main() {"
                    "vec3 texel = vec3(0.0,0.0,0.0);"
#if BloomBlurShader_USE_GAUSSIAN == 1
                    "for (int i=-7;i<=7;i++)"
                        "texel += texture2D(uSampler2DTexture, vVec2UV + uVec2Direction * uVec2TextureNeighbor * float(i) ).rgb * gaussCoefs_15[i+7];"
                    "texel = clamp(texel * 1.5,vec3(0.0),vec3(1.0));"//make blur stronger
#else
                    "for (int i=-7;i<=7;i++)"
                        "texel += texture2D(uSampler2DTexture, vVec2UV + uVec2Direction * uVec2TextureNeighbor * float(i) ).rgb;"
                    "texel = clamp( (texel / 15.0) * 1.5,vec3(0.0),vec3(1.0));"//make blur stronger
#endif
                
                    "vec4 result = vec4(texel, 1.0);"
                    "gl_FragColor = result;"
                "}"
            };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
#if BloomBlurShader_USE_GAUSSIAN == 1
            // load blur coefs
            float gaussCoefs_15[] = { 0.0229490642f, 0.0344506279f, 0.0485831723f, 0.06436224f, 0.0801001f, 0.09364651f, 0.102850571f, 0.1061154f, 0.102850571f, 0.09364651f, 0.0801001f, 0.06436224f, 0.0485831723f, 0.0344506279f, 0.0229490642f };
            enable();
            glUniform1fv(getUniformLocation("gaussCoefs_15"),15,gaussCoefs_15);
#else
#endif
            
            //aVec2Position = getAttribLocation("aVec2Position");
            //aVec2UV = getAttribLocation("aVec2UV");
            
            uSampler2DTexture = getUniformLocation("uSampler2DTexture");
            uVec2TextureNeighbor = getUniformLocation("uVec2TextureNeighbor");
            uVec2Direction = getUniformLocation("uVec2Direction");
            
        }
        
        void setTexture(int texUnit){
            setUniform(uSampler2DTexture, texUnit);
        }
        void setTextureNeighbor(const vec2 &neighbor){
            setUniform(uVec2TextureNeighbor, neighbor);
        }
        void setHorizontal() {
            setUniform(uVec2Direction, vec2(1.0,0.0));
        }
        void setVertical() {
            setUniform(uVec2Direction, vec2(0.0,1.0));
        }
        
    protected:
        
        void setupAttribLocation() {
            bindAttribLocation(GLPostProcessingShader::aVec2Position, "aVec2Position");
            bindAttribLocation(GLPostProcessingShader::aVec2UV, "aVec2UV");
        }
        
    };
    
}

#endif

