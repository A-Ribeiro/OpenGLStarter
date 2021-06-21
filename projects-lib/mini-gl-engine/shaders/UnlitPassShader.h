#ifndef __unlit_pass_shader__h_
#define __unlit_pass_shader__h_

#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {
    
    class UnlitPassShader : public DefaultEngineShader{
        int u_mvp;
        int u_color;
    public:
        UnlitPassShader() {
            format = model::CONTAINS_POS;
            
            const char vertexShaderCode[] = {
                "attribute vec4 aPosition;"
                "uniform mat4 uMVP;"
                "void main() {"
                "  gl_Position = uMVP * aPosition;"
                "}" };
            
            const char fragmentShaderCode[] = {
                "uniform vec4 uColor;"
                "void main() {"
                "  gl_FragColor = uColor;"
                "}" };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            
            u_mvp = getUniformLocation("uMVP");
            u_color = getUniformLocation("uColor");
        }
        
        void setMVP(const aRibeiro::mat4 &mvp){
            setUniform(u_mvp, mvp);
        }
        
        void setColor(const aRibeiro::vec4 &color){
            setUniform(u_color, color);
        }
        
    };
    
    class Unlit_tex_PassShader : public DefaultEngineShader{
        int u_mvp;
        int u_texture;
        int u_color;
    public:
        Unlit_tex_PassShader() {
            format = model::CONTAINS_POS | model::CONTAINS_UV0;
            
            const char vertexShaderCode[] = {
                "attribute vec4 aPosition;"
                "attribute vec3 aUV0;"
                "uniform mat4 uMVP;"
                "varying vec2 uv;"
                "void main() {"
                "  uv = aUV0.xy;"
                "  gl_Position = uMVP * aPosition;"
                "}" };
            
            const char fragmentShaderCode[] = {
                "varying vec2 uv;"
                "uniform vec4 uColor;"
                "uniform sampler2D uTexture;"
                "void main() {"
                "  vec4 texel = texture2D(uTexture, uv);"
                "  vec4 result = texel * uColor;"
                "  gl_FragColor = result;"
                "}" };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            
            u_mvp = getUniformLocation("uMVP");
            u_texture = getUniformLocation("uTexture");
            u_color = getUniformLocation("uColor");
        }
        
        void setMVP(const aRibeiro::mat4 &mvp){
            setUniform(u_mvp, mvp);
        }
        void setTexture(int texunit){
            setUniform(u_texture, texunit);
        }
        void setColor(const aRibeiro::vec4 &color){
            setUniform(u_color, color);
        }
    };
    
    class Unlit_tex_vertcolor_PassShader : public DefaultEngineShader{
        int u_mvp;
        int u_texture;
        int u_color;
    public:
        Unlit_tex_vertcolor_PassShader() {
            format = model::CONTAINS_POS | model::CONTAINS_UV0 | model::CONTAINS_COLOR0;
            
            const char vertexShaderCode[] = {
                "attribute vec4 aPosition;"
                "attribute vec3 aUV0;"
                "attribute vec4 aColor0;"
                "uniform mat4 uMVP;"
                "varying vec2 uv;"
                "varying vec4 color;"
                "void main() {"
                "  uv = aUV0.xy;"
                "  color = aColor0;"
                "  gl_Position = uMVP * aPosition;"
                "}" };
            
            const char fragmentShaderCode[] = {
                "varying vec2 uv;"
                "varying vec4 color;"
                "uniform vec4 uColor;"
                "uniform sampler2D uTexture;"
                "void main() {"
                "  vec4 texel = texture2D(uTexture, uv);"
                "  vec4 result = texel * color * uColor;"
                "  gl_FragColor = result;"
                "}" };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            
            u_mvp = getUniformLocation("uMVP");
            u_texture = getUniformLocation("uTexture");
            u_color = getUniformLocation("uColor");
        }
        
        void setMVP(const aRibeiro::mat4 &mvp){
            setUniform(u_mvp, mvp);
        }
        void setTexture(int texunit){
            setUniform(u_texture, texunit);
        }
        void setColor(const aRibeiro::vec4 &color){
            setUniform(u_color, color);
        }
    };
    
    
    class Unlit_tex_vertcolor_font_PassShader : public DefaultEngineShader{
        int u_mvp;
        int u_texture;
        int u_color;
    public:
        Unlit_tex_vertcolor_font_PassShader() {
            format = model::CONTAINS_POS | model::CONTAINS_UV0 | model::CONTAINS_COLOR0;
            
            const char vertexShaderCode[] = {
                "attribute vec4 aPosition;"
                "attribute vec3 aUV0;"
                "attribute vec4 aColor0;"
                "uniform mat4 uMVP;"
                "varying vec2 uv;"
                "varying vec4 color;"
                "void main() {"
                "  uv = aUV0.xy;"
                "  color = aColor0;"
                "  gl_Position = uMVP * aPosition;"
                "}" };
            
            const char fragmentShaderCode[] = {
                "varying vec2 uv;"
                "varying vec4 color;"
                "uniform vec4 uColor;"
                "uniform sampler2D uTexture;"
                "void main() {"
                "  vec4 texel = vec4( 1.0,1.0,1.0, texture2D(uTexture, uv).a);"
                "  vec4 result = texel * color * uColor;"
                "  gl_FragColor = result;"
                "}" };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            
            u_mvp = getUniformLocation("uMVP");
            u_texture = getUniformLocation("uTexture");
            u_color = getUniformLocation("uColor");
        }
        
        void setMVP(const aRibeiro::mat4 &mvp){
            setUniform(u_mvp, mvp);
        }
        void setTexture(int texunit){
            setUniform(u_texture, texunit);
        }
        void setColor(const aRibeiro::vec4 &color){
            setUniform(u_color, color);
        }
        
    };
}

#endif
