#include <appkit-gl-engine/shaders/ParticleSystemShaders.h>

namespace AppKit
{
    namespace GLEngine
    {

        NormalParticleShader::NormalParticleShader()
        {
            format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec4 aPosition;\n"
                "attribute vec2 aUV0;\n"
                "attribute vec3 aColor0;\n"
                "attribute float aSize;\n"
                "attribute float aAlpha;\n"

                "uniform mat4 uMVP;\n"
                "uniform mat4 uV_inv;\n"

                "varying vec2 uv;\n"
                "varying vec4 color;\n"

                "void main() {\n"
                "  uv = aUV0.xy;\n"
                "  color = vec4(aColor0.rgb,aAlpha);\n"
                "  vec2 newUV = (aUV0 - vec2(0.5)) * aSize;\n"
                "  vec3 offset = newUV.x * uV_inv[0].xyz + newUV.y * uV_inv[1].xyz;\n"
                "  gl_Position = uMVP * ( aPosition + vec4(offset , 0.0) );\n"
                "}"};

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "varying vec2 uv;\n"
                "varying vec4 color;\n"

                "uniform vec4 uColor;\n"
                "uniform sampler2D uTexture;\n"

                "void main() {\n"
                "  vec4 texel = texture2D(uTexture, uv);\n"
                "  vec4 result = texel * uColor * color;\n"
                //"  result.rgb = result.rgb*vec3(0) + vec3(1);\n"
                //"  result.a = result.a*0 + 1;\n"
                "  gl_FragColor = result;\n"
                "}"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            int userAttribute = getUserAttribLocationStart();
            bindAttribLocation(userAttribute++, "aSize");
            bindAttribLocation(userAttribute++, "aAlpha");
            link(__FILE__, __LINE__);

            u_mvp = getUniformLocation("uMVP");
            u_v_inv = getUniformLocation("uV_inv");
            u_texture = getUniformLocation("uTexture");
            u_color = getUniformLocation("uColor");
        }

        int NormalParticleShader::queryAttribLocation(const char *aname)
        {
            int result = DefaultEngineShader::queryAttribLocation(aname);
            if (result == -1)
            {
                int userAttribute = getUserAttribLocationStart();

                if (strcmp(aname, "aSize") == 0)
                    return userAttribute;
                userAttribute++;

                if (strcmp(aname, "aAlpha") == 0)
                    return userAttribute;
                userAttribute++;
            }
            return result;
        }

        void NormalParticleShader::setMVP(const MathCore::mat4f &mvp)
        {
            setUniform(u_mvp, mvp);
        }
        void NormalParticleShader::setV_inv(const MathCore::mat4f &v_inv)
        {
            setUniform(u_v_inv, v_inv);
        }
        void NormalParticleShader::setTexture(int texunit)
        {
            setUniform(u_texture, texunit);
        }
        void NormalParticleShader::setColor(const MathCore::vec4f &color)
        {
            setUniform(u_color, color);
        }

        SoftParticleShader::SoftParticleShader()
        {
            format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                // https://en.wikipedia.org/wiki/OpenGL_Shading_Language#Versions
                SHADER_HEADER_120

                "attribute vec4 aPosition;\n"
                "attribute vec2 aUV0;\n"
                "attribute vec3 aColor0;\n"
                "attribute float aSize;\n"
                "attribute float aAlpha;\n"

                "uniform mat4 uMVP;\n"
                "uniform mat4 uV_inv;\n"

                "varying vec2 uv;\n"
                "varying vec4 color;\n"

                "void main() {\n"
                "  uv = aUV0.xy;\n"
                "  color = vec4(aColor0.rgb,aAlpha);\n"
                "  vec2 newUV = (aUV0 - vec2(0.5)) * aSize;\n"
                "  vec3 offset = newUV.x * uV_inv[0].xyz + newUV.y * uV_inv[1].xyz;\n"
                "  gl_Position = uMVP * ( aPosition + vec4(offset , 0.0) );\n"
                "}"};

            const char fragmentShaderCode[] = {
                // https://en.wikipedia.org/wiki/OpenGL_Shading_Language#Versions
                SHADER_HEADER_120

                "varying vec2 uv;\n"
                "varying vec4 color;\n"

                "uniform vec4 uColor;\n"
                "uniform sampler2D uTexture;\n"

                "uniform vec2 uScreenSize;\n"
                "uniform vec4 u_FMinusN_FPlusN_FTimesNTimes2_N;\n"

                "uniform sampler2D uDepthTextureComponent24;\n"

                "float DepthToWorld_optimized(float z)\n"
                "{\n"
                "    float z_aux = z * 2.0 - 1.0;\n"
                "    return u_FMinusN_FPlusN_FTimesNTimes2_N.z / (u_FMinusN_FPlusN_FTimesNTimes2_N.y - z_aux * u_FMinusN_FPlusN_FTimesNTimes2_N.x);\n"
                "}\n"

                "void main() {\n"
                "  vec4 texel = texture2D(uTexture, uv);\n"
                "  vec4 result = texel * uColor * color;\n"

                "  float framebuffer_depth = texture2D(uDepthTextureComponent24, (gl_FragCoord.xy / uScreenSize) ).x;\n"

                //"if (framebuffer_depth < 0.5){ result = vec4(1,0,0,1);  }else{\n"

                //"  framebuffer_depth = DepthToWorld(framebuffer_depth);\n"
                "  framebuffer_depth = DepthToWorld_optimized(framebuffer_depth);\n"

                //"  float particle_depth = DepthToWorld(gl_FragCoord.z);\n"
                "  float particle_depth = DepthToWorld_optimized(gl_FragCoord.z);\n"

                "  float distance_01 = framebuffer_depth - particle_depth;\n"

                /*
                // configure soft range (starts in 0 with alpha 0, and go to 1.0 with alpha 1)

                "  float soft_delta = 1.0;\n"
                "  distance_01 /= soft_delta;\n"

                "  float soft_start = 0.0;\n"
                "  distance_01 -= soft_start;\n"
                */

                "  distance_01 = clamp(distance_01,0.0,1.0);\n"

                // configure camera soft range (starts in 1 with alpha 0, and go to 2.0 - delta 1 -  with alpha 1)

                "  float cameraDistance_01 = particle_depth - u_FMinusN_FPlusN_FTimesNTimes2_N.w;\n"

                /*
                "  float soft_camera_delta = 1.0;\n"
                "  cameraDistance_01 /= soft_camera_delta;\n"
                */

                "  float soft_camera_start = 1.0;\n"
                "  cameraDistance_01 -= soft_camera_start;\n"

                "  cameraDistance_01 = clamp(cameraDistance_01,0.0,1.0);\n"

                "  result.a *= distance_01 * cameraDistance_01;\n"

                //"  result.rgb = result.rgb*vec3(0) + vec3(distance_01 * cameraDistance_01);\n"
                //"  result.rgb = result.rgb*vec3(0) + vec3(1);\n"
                //"  result.a = result.a*0 + 1;\n"

                "  gl_FragColor = result;\n"
                "}"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            int userAttribute = getUserAttribLocationStart();
            bindAttribLocation(userAttribute++, "aSize");
            bindAttribLocation(userAttribute++, "aAlpha");
            link(__FILE__, __LINE__);

            u_mvp = getUniformLocation("uMVP");
            u_v_inv = getUniformLocation("uV_inv");
            u_texture = getUniformLocation("uTexture");
            u_DepthTextureComponent24 = getUniformLocation("uDepthTextureComponent24");
            u_color = getUniformLocation("uColor");

            u_uScreenSize = getUniformLocation("uScreenSize");
            u_FMinusN_FPlusN_FTimesNTimes2_N = getUniformLocation("u_FMinusN_FPlusN_FTimesNTimes2_N");
        }

        int SoftParticleShader::queryAttribLocation(const char *aname)
        {
            int result = DefaultEngineShader::queryAttribLocation(aname);
            if (result == -1)
            {
                int userAttribute = getUserAttribLocationStart();

                if (strcmp(aname, "aSize") == 0)
                    return userAttribute;
                userAttribute++;

                if (strcmp(aname, "aAlpha") == 0)
                    return userAttribute;
                userAttribute++;
            }
            return result;
        }

        void SoftParticleShader::setMVP(const MathCore::mat4f &mvp)
        {
            setUniform(u_mvp, mvp);
        }
        void SoftParticleShader::setV_inv(const MathCore::mat4f &v_inv)
        {
            setUniform(u_v_inv, v_inv);
        }
        void SoftParticleShader::setTexture(int texunit)
        {
            setUniform(u_texture, texunit);
        }
        void SoftParticleShader::setDepthTextureComponent24(int texunit)
        {
            setUniform(u_DepthTextureComponent24, texunit);
        }
        void SoftParticleShader::setColor(const MathCore::vec4f &color)
        {
            setUniform(u_color, color);
        }

        void SoftParticleShader::setScreenSize(const MathCore::vec2f &ss)
        {
            setUniform(u_uScreenSize, ss);
        }

        void SoftParticleShader::setCamera_FMinusN_FPlusN_FTimesNTimes2_N(const MathCore::vec4f &v)
        {
            // void SoftParticleShader::setCameraNearFar(const MathCore::vec2f &nf) {
            setUniform(u_FMinusN_FPlusN_FTimesNTimes2_N, v);
        }

    }

}