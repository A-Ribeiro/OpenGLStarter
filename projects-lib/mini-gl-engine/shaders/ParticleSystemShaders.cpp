#include "ParticleSystemShaders.h"

namespace GLEngine {

    NormalParticleShader::NormalParticleShader() {
        format = model::CONTAINS_POS | model::CONTAINS_UV0 | model::CONTAINS_COLOR0;
        
        const char vertexShaderCode[] = {
            "attribute vec4 aPosition;"
            "attribute vec2 aUV0;"
            "attribute vec3 aColor0;"
            "attribute float aSize;"
            "attribute float aAlpha;"

            "uniform mat4 uMVP;"
            "uniform mat4 uV_inv;"

            "varying vec2 uv;"
            "varying vec4 color;"

            "void main() {"
            "  uv = aUV0.xy;"
            "  color = vec4(aColor0.rgb,aAlpha);"
            "  vec2 newUV = (aUV0 - vec2(0.5)) * aSize;"
            "  vec3 offset = newUV.x * uV_inv[0].xyz + newUV.y * uV_inv[1].xyz;"
            "  gl_Position = uMVP * ( aPosition + vec4(offset , 0.0) );"
            "}" };
        
        const char fragmentShaderCode[] = {
            
            "varying vec2 uv;"
            "varying vec4 color;"

            "uniform vec4 uColor;"
            "uniform sampler2D uTexture;"

            "void main() {"
            "  vec4 texel = texture2D(uTexture, uv);"
            "  vec4 result = texel * uColor * color;"
            //"  result.rgb = result.rgb*vec3(0) + vec3(1);"
            //"  result.a = result.a*0 + 1;"
            "  gl_FragColor = result;"
            "}" };
        
        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
        
        u_mvp = getUniformLocation("uMVP");
        u_v_inv = getUniformLocation("uV_inv");
        u_texture = getUniformLocation("uTexture");
        u_color = getUniformLocation("uColor");
    }

    int NormalParticleShader::queryAttribLocation(const char* aname) {
        int result = DefaultEngineShader::queryAttribLocation(aname);
        if (result == -1){
            int userAttribute = getUserAttribLocationStart();

            if (strcmp(aname,"aSize") == 0)
                return userAttribute;
            userAttribute++;

            if (strcmp(aname,"aAlpha") == 0)
                return userAttribute;
            userAttribute++;
        }
        return result;
    }

    void NormalParticleShader::setupAttribLocation() {
        DefaultEngineShader::setupAttribLocation();

        int userAttribute = getUserAttribLocationStart();
        bindAttribLocation(userAttribute++, "aSize");
        bindAttribLocation(userAttribute++, "aAlpha");
    }
    
    void NormalParticleShader::setMVP(const aRibeiro::mat4 &mvp){
        setUniform(u_mvp, mvp);
    }
    void NormalParticleShader::setV_inv(const aRibeiro::mat4 &v_inv){
        setUniform(u_v_inv, v_inv);
    }
    void NormalParticleShader::setTexture(int texunit){
        setUniform(u_texture, texunit);
    }
    void NormalParticleShader::setColor(const aRibeiro::vec4 &color){
        setUniform(u_color, color);
    }


    SoftParticleShader::SoftParticleShader() {
        format = model::CONTAINS_POS | model::CONTAINS_UV0 | model::CONTAINS_COLOR0;
        
        const char vertexShaderCode[] = {
            //https://en.wikipedia.org/wiki/OpenGL_Shading_Language#Versions
            #ifndef ARIBEIRO_RPI
            "#version 120\n"
            #endif

            "attribute vec4 aPosition;"
            "attribute vec2 aUV0;"
            "attribute vec3 aColor0;"
            "attribute float aSize;"
            "attribute float aAlpha;"

            "uniform mat4 uMVP;"
            "uniform mat4 uV_inv;"

            "varying vec2 uv;"
            "varying vec4 color;"

            "void main() {"
            "  uv = aUV0.xy;"
            "  color = vec4(aColor0.rgb,aAlpha);"
            "  vec2 newUV = (aUV0 - vec2(0.5)) * aSize;"
            "  vec3 offset = newUV.x * uV_inv[0].xyz + newUV.y * uV_inv[1].xyz;"
            "  gl_Position = uMVP * ( aPosition + vec4(offset , 0.0) );"
            "}" };
        
        const char fragmentShaderCode[] = {
            //https://en.wikipedia.org/wiki/OpenGL_Shading_Language#Versions
            #ifndef ARIBEIRO_RPI
            "#version 120\n"
            #endif

            "varying vec2 uv;"
            "varying vec4 color;"

            "uniform vec4 uColor;"
            "uniform sampler2D uTexture;"

            "uniform vec2 uScreenSize;"
            "uniform vec2 uCameraNearFar;"

            "uniform sampler2D uDepthTextureComponent24;"

            /*
            "float LinearizeDepth(float depth)"
            "{"
            "    float n = uCameraNearFar.x;"
            "    float f = uCameraNearFar.y;"
            "    float z = depth;"
            "    return (2.0 * n) / (f + n - z * (f - n));"
            "}"

            "float DepthToWorld(float depth)"
            "{"
            "    float n = uCameraNearFar.x;"
            "    float f = uCameraNearFar.y;"
            "    float f_minus_n = f - n;"
            "    float z = depth;"
            "    float result = (2.0 * n) / (f + n - z * f_minus_n);"
            "    return result * f_minus_n / 2.0 + n;"
            "}"
            */

            "float DepthToWorld_optimized(float z, float f_minus_n, float f_plus_n)"
            "{"
            "    return (f_minus_n * uCameraNearFar.x) / (f_plus_n - z * f_minus_n) + uCameraNearFar.x;"
            "}"

            "void main() {"
            "  vec4 texel = texture2D(uTexture, uv);"
            "  vec4 result = texel * uColor * color;"

            //pre compute near/far
            "  float f_minus_n = uCameraNearFar.y - uCameraNearFar.x;"
            "  float f_plus_n = uCameraNearFar.y + uCameraNearFar.x;"
            
            "  float framebuffer_depth = texture2D(uDepthTextureComponent24, (gl_FragCoord.xy / uScreenSize) ).x;"
            //"  framebuffer_depth = DepthToWorld(framebuffer_depth);"
            "  framebuffer_depth = DepthToWorld_optimized(framebuffer_depth, f_minus_n, f_plus_n);"

            //"  float particle_depth = DepthToWorld(gl_FragCoord.z);"
            "  float particle_depth = DepthToWorld_optimized(gl_FragCoord.z, f_minus_n, f_plus_n);"

            "  float distance_01 = framebuffer_depth - particle_depth;"
            
            /*
            // configure soft range (starts in 0 with alpha 0, and go to 1.0 with alpha 1)

            "  float soft_delta = 1.0;"
            "  distance_01 /= soft_delta;"
            
            "  float soft_start = 0.0;"
            "  distance_01 -= soft_start;"
            */

           "  distance_01 = clamp(distance_01,0.0,1.0);"

           // configure camera soft range (starts in 1 with alpha 0, and go to 2.0 - delta 1 -  with alpha 1)

            "  float cameraDistance_01 = particle_depth - uCameraNearFar.x;"

            /*
            "  float soft_camera_delta = 1.0;"
            "  cameraDistance_01 /= soft_camera_delta;"
            */
            
            "  float soft_camera_start = 1.0;"
            "  cameraDistance_01 -= soft_camera_start;"

            "  cameraDistance_01 = clamp(cameraDistance_01,0.0,1.0);"


            "  result.a *= distance_01 * cameraDistance_01;"

            //"  result.rgb = result.rgb*vec3(0) + vec3(distance_01 * cameraDistance_01);"
            //"  result.rgb = result.rgb*vec3(0) + vec3(1);"
            //"  result.a = result.a*0 + 1;"

            "  gl_FragColor = result;"
            "}" };
        
        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
        
        u_mvp = getUniformLocation("uMVP");
        u_v_inv = getUniformLocation("uV_inv");
        u_texture = getUniformLocation("uTexture");
        u_DepthTextureComponent24 = getUniformLocation("uDepthTextureComponent24");
        u_color = getUniformLocation("uColor");

        u_uScreenSize = getUniformLocation("uScreenSize");
        u_uCameraNearFar = getUniformLocation("uCameraNearFar");
    }

    int SoftParticleShader::queryAttribLocation(const char* aname) {
        int result = DefaultEngineShader::queryAttribLocation(aname);
        if (result == -1){
            int userAttribute = getUserAttribLocationStart();

            if (strcmp(aname,"aSize") == 0)
                return userAttribute;
            userAttribute++;

            if (strcmp(aname,"aAlpha") == 0)
                return userAttribute;
            userAttribute++;
        }
        return result;
    }

    void SoftParticleShader::setupAttribLocation() {
        DefaultEngineShader::setupAttribLocation();

        int userAttribute = getUserAttribLocationStart();
        bindAttribLocation(userAttribute++, "aSize");
        bindAttribLocation(userAttribute++, "aAlpha");
    }
    
    void SoftParticleShader::setMVP(const aRibeiro::mat4 &mvp){
        setUniform(u_mvp, mvp);
    }
    void SoftParticleShader::setV_inv(const aRibeiro::mat4 &v_inv){
        setUniform(u_v_inv, v_inv);
    }
    void SoftParticleShader::setTexture(int texunit){
        setUniform(u_texture, texunit);
    }
    void SoftParticleShader::setDepthTextureComponent24(int texunit){
        setUniform(u_DepthTextureComponent24, texunit);
    }
    void SoftParticleShader::setColor(const aRibeiro::vec4 &color){
        setUniform(u_color, color);
    }

    void SoftParticleShader::setScreenSize(const aRibeiro::vec2 &ss) {
        setUniform(u_uScreenSize, ss);
    }

    void SoftParticleShader::setCameraNearFar(const aRibeiro::vec2 &nf) {
        setUniform(u_uCameraNearFar, nf);
    }

}
