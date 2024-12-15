#pragma once

#include <appkit-gl-engine/shaders/FrankenShader.h>
// #if defined(PBR_MODE_TEX_LOOKUP)
#include <appkit-gl-base/GLTexture.h>
// #endif

namespace AppKit
{
    namespace GLEngine
    {

        class FrankenShaderManager : public DefaultEngineShader
        {

            std::unordered_map<ShaderAlgorithmsBitMask, FrankenShader *> shaderMap;

            // #if defined(PBR_MODE_TEX_LOOKUP)
            // AppKit::OpenGL::GLTexture pbrOptimizationTexture;

            float fresnelSchlick(float cosTheta, float F0);
            float DistributionGGX(float NdotH, float roughness);
            float GeometrySchlickGGX(float NdotV, float roughness);
            float byte2float(uint8_t b);
            uint8_t float2byte(float f);

            MathCore::vec2f encode_2el(float f);
            float decode_2el(const MathCore::vec2f &v);
            // #endif

        public:
            // #if defined(PBR_MODE_TEX_LOOKUP)
            AppKit::OpenGL::GLTexture pbrOptimizationTexture;
            // #endif
            FrankenShaderManager();
            ~FrankenShaderManager();
            FrankenShader *getShader(ShaderAlgorithmsBitMask frankenFormat,
                                     ShaderPBRAlgorithmEnum _shaderPBRAlgorithm,
                                     ShaderShadowAlgorithmEnum _shaderShadowAlgorithm);

            // void enable_PBR_OptimizationTexUnit(int texUnit);
            // void disable_PBR_OptimizationTexUnit(int texUnit);
        };

    }

}
