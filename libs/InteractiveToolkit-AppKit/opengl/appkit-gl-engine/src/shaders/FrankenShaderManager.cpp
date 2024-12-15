#include <appkit-gl-engine/shaders/FrankenShaderManager.h>

namespace AppKit
{
    namespace GLEngine
    {

        // #if defined(PBR_MODE_TEX_LOOKUP)

        float FrankenShaderManager::fresnelSchlick(float cosTheta, float F0)
        {
            return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
        }

        float FrankenShaderManager::DistributionGGX(float NdotH, float roughness)
        {
            // NdotH = clamp(NdotH,0.0f, 0.999f);
            roughness = MathCore::OP<float>::clamp(roughness, 0.085f, 1.0f);
            float a = roughness * roughness;
            float a2 = a * a;
            // float NdotH  = max(dot(N, H), 0.0);
            float NdotH2 = NdotH * NdotH;
            float num = a2;
            float denom = (NdotH2 * (a2 - 1.0) + 1.0);
            denom = 3.14159265358 * denom * denom; // PI = 3.14159265358
            return MathCore::OP<float>::clamp(num / denom, 0.0f, 1.0f);
        }

        float FrankenShaderManager::GeometrySchlickGGX(float NdotV, float roughness)
        {
            float r = (roughness + 1.0);
            float k = (r * r) / 8.0;
            float num = NdotV;
            float denom = NdotV * (1.0 - k) + k;
            return num / denom;
        }
        // https://www.khronos.org/opengl/wiki/Normalized_Integer
        //  this is the direct byte to float conversion
        float FrankenShaderManager::byte2float(uint8_t b)
        {
            return (float)b / 255.0f;
        }
        // the float to byte may vary
        uint8_t FrankenShaderManager::float2byte(float f)
        {
            // can use the direct inverse
            return (uint8_t)(f * 255.0f);
            // or can round the value
            // return (uint8_t)(f * 255.0f + 0.5f);
        }

        MathCore::vec2f FrankenShaderManager::encode_2el(float f)
        {
            float f_ge_1 = f >= 1.0;
            // if (f >= 1.0)
            // return MathCore::vec2f(1.0f,1.0f);
            MathCore::vec2f enc = MathCore::vec2f(f, f * 255.0f);
            // enc = fract(enc);
            enc.x = MathCore::OP<float>::fmod(enc.x, 1.0f);
            enc.y = MathCore::OP<float>::fmod(enc.y, 1.0f);
            // enc -= enc.yzww * vec4(1. / 255., 1. / 255., 1. / 255., 0.);
            // enc -= enc.yzww * vec4(3.9215686275e-03, 3.9215686275e-03, 3.9215686275e-03, 0.0);

            uint8_t byte = float2byte(enc.x);
            // enc.x -= enc.y * (1.0f / 255.0f);
            enc.x = ((float)byte + 0.2f) / 255.0f; // force byte RGB value

            if (byte % 2)
            {
                // enc.y = 1.0f - enc.y;
                enc.y = 1.0f - enc.y;
            }

            return MathCore::OP<MathCore::vec2f>::lerp(enc, MathCore::vec2f(1.0f, 1.0f), f_ge_1);
        }

        float FrankenShaderManager::decode_2el(const MathCore::vec2f &_v)
        {
            MathCore::vec2f v = _v;
            // return dot(v, vec4(1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 16581375.0));
            // return dot(v, vec4(1.0, 3.9215686275e-03, 1.5378700500e-05, 6.0308629411e-08));
            // return dot(v, MathCore::vec2f(1.0, 1.0f / 255.0f));
            uint8_t byte = float2byte(v.x);
            if (byte % 2)
            {
                v.y = 1.0 - v.y;
            }

            return v.x + v.y * 3.9215686275e-03;
        }

        // #endif
        FrankenShaderManager::FrankenShaderManager()
        {

            // #if defined(PBR_MODE_TEX_LOOKUP)
            // pbrOptimizationTexture = new AppKit::OpenGL::Texture();
            // generate the pbr texture lookup
            int size = 64;
            uint8_t *texture_rgb = (uint8_t *)ITKCommon::Memory::malloc(size * size * sizeof(uint8_t) * 3);
            for (int y = 0; y < size; y++)
            {
                float lrp_param2 = (float)y / (float)(size - 1);
                for (int x = 0; x < size; x++)
                {
                    float lrp_param1 = (float)x / (float)(size - 1);
                    int index = (x + y * size) * 3;
                    texture_rgb[index + 0] = float2byte(fresnelSchlick(lrp_param1, lrp_param2));
                    texture_rgb[index + 1] = float2byte(DistributionGGX(lrp_param1, lrp_param2));
                    texture_rgb[index + 2] = float2byte(GeometrySchlickGGX(lrp_param1, lrp_param2));
                }
            }

            pbrOptimizationTexture.uploadBufferRGB_888(texture_rgb, size, size, false);
            pbrOptimizationTexture.active(0);
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            pbrOptimizationTexture.deactive(0);
            // ITKExtension::Image::PNG::writePNG("texture_rgb.png", size, size, 3, (char*)texture_rgb);
            ITKCommon::Memory::free(texture_rgb);
            // #endif
        }

        FrankenShaderManager::~FrankenShaderManager()
        {

            // #if defined(PBR_MODE_TEX_LOOKUP)
            // delete pbrOptimizationTexture);
            // #endif

            std::unordered_map<ShaderAlgorithmsBitMask, FrankenShader *>::iterator it;

            for (it = shaderMap.begin(); it != shaderMap.end(); it++)
            {
                FrankenShader *shader = it->second;
                delete shader;
            }

            shaderMap.clear();
        }

        FrankenShader *FrankenShaderManager::getShader(ShaderAlgorithmsBitMask frankenFormat,
                                                       ShaderPBRAlgorithmEnum _shaderPBRAlgorithm,
                                                       ShaderShadowAlgorithmEnum _shaderShadowAlgorithm)
        {
            std::unordered_map<ShaderAlgorithmsBitMask, FrankenShader *>::iterator it = shaderMap.find(frankenFormat);
            if (it == shaderMap.end())
            {
                FrankenShader *result = new FrankenShader(frankenFormat, _shaderPBRAlgorithm, _shaderShadowAlgorithm);
                shaderMap[frankenFormat] = result;
                return result;
            }
            return it->second;
        }

        /*
        void FrankenShaderManager::enable_PBR_OptimizationTexUnit(int texUnit) {
    #if defined(PBR_MODE_TEX_LOOKUP)
            pbrOptimizationTexture.active(texUnit);
    #endif
        }

        void FrankenShaderManager::disable_PBR_OptimizationTexUnit(int texUnit) {
    #if defined(PBR_MODE_TEX_LOOKUP)
            pbrOptimizationTexture.deactive(texUnit);
    #endif
        }
        */

    }
}