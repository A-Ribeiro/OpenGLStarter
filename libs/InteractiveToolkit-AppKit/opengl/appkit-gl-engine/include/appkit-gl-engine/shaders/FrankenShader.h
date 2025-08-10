#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

#include <appkit-gl-engine/Components/Core/ComponentCamera.h>
#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>

#include <appkit-gl-engine/shaders/FrankenUniformManager.h>

namespace AppKit
{
    namespace GLEngine
    {

        enum ShaderShadowAlgorithmEnum
        {
            ShaderShadowAlgorithm_None,
            ShaderShadowAlgorithm_Basic,
            ShaderShadowAlgorithm_2x2PCF,
            ShaderShadowAlgorithm_4x4PCF,
            ShaderShadowAlgorithm_PCSS_PCF,
            ShaderShadowAlgorithm_PCSS_DST_CENTER,
        };

        enum ShaderPBRAlgorithmEnum
        {
            ShaderPBRAlgorithm_Normal,
            ShaderPBRAlgorithm_TexLookUp
        };

        enum ShaderAlgorithmsEnum
        {

            // generate texture albedo access
            ShaderAlgorithms_TextureAlbedo = ((uint64_t)1 << 0),

            // generate texture specular access
            ShaderAlgorithms_TextureSpecular = ((uint64_t)1 << 1),

            // generate emission map access
            ShaderAlgorithms_TextureEmission = ((uint64_t)1 << 2),

            // generate normal
            ShaderAlgorithms_NormalMap = ((uint64_t)1 << 3), // need aTangent

            // generate output color
            ShaderAlgorithms_AmbientLightColor = ((uint64_t)1 << 6),     // needs Albedo
            ShaderAlgorithms_AmbientLightSkybox = ((uint64_t)1 << 7),    // needs textureCube could use normalMap
            ShaderAlgorithms_AmbientLightSpheremap = ((uint64_t)1 << 8), // needs uTextureSphereAmbient could use normalMap

            // generate output color
            ShaderAlgorithms_SunLight0 = ((uint64_t)1 << 10), // needs camera pos world
            ShaderAlgorithms_SunLight1 = ((uint64_t)1 << 11), // needs camera pos world
            ShaderAlgorithms_SunLight2 = ((uint64_t)1 << 12), // needs camera pos world
            ShaderAlgorithms_SunLight3 = ((uint64_t)1 << 13), // needs camera pos world

            ShaderAlgorithms_ShadowSunLight0 = ((uint64_t)1 << 14), // needs camera pos world
            ShaderAlgorithms_ShadowSunLight1 = ((uint64_t)1 << 15), // needs camera pos world
            ShaderAlgorithms_ShadowSunLight2 = ((uint64_t)1 << 16), // needs camera pos world
            ShaderAlgorithms_ShadowSunLight3 = ((uint64_t)1 << 17), // needs camera pos world

            // mesh skinning
            ShaderAlgorithms_skinGradientMatrix_16 = ((uint64_t)1 << 20),  // can upload max of 16 matrix
            ShaderAlgorithms_skinGradientMatrix_32 = ((uint64_t)1 << 21),  // can upload max of 32 matrix
            ShaderAlgorithms_skinGradientMatrix_64 = ((uint64_t)1 << 22),  // can upload max of 64 matrix
            ShaderAlgorithms_skinGradientMatrix_96 = ((uint64_t)1 << 23),  // can upload max of 96 matrix
            ShaderAlgorithms_skinGradientMatrix_128 = ((uint64_t)1 << 24), // can upload max of 128 matrix

            // used to render on non-capable sRGB devices
            // ShaderAlgorithms_sRGB = (1 << 31)
            //(1 << 63)
            ShaderAlgorithms_sRGB = ((uint64_t)1 << 63) // 0x8000000000000000ui64
        };

        typedef uint64_t ShaderAlgorithmsBitMask;

        class FrankenShader : public DefaultEngineShader
        {

            int u_mvp;
            int u_cameraPosWorld;

            // material setup
            struct PBRUniform
            {
                int u_textureAlbedo;
                int u_textureSpecular;
                int u_textureEmission;

                // optimization to run on Raspberry PI
                int u_TexturePBR_F_NDF_G_GHigh; // new

                int u_materialAlbedoColor;
                int u_materialEmissionColor;
                int u_materialRoughness;
                int u_materialMetallic;
            } PBR;

            struct normalMapUniform
            {
                int u_localToWorld;
                int u_localToWorld_it;
                int u_textureNormal;
            } normalMap;

            struct AmbientLight
            {
                int u_textureCubeAmbient;
                int u_textureSphereAmbient;
                int u_ambientColor;
            } ambientLight;

            struct SunLightUniform
            {
                int u_lightRadiance;
                int u_lightDir;
            } sunLight[4];

            struct ShadowSunLightUniform
            {
                int u_lightRadiance;
                int u_lightDir;
                int u_projMatrix;
                int u_textureDepth;

                // new
                int u_shadowDimension;
                int u_shadowDimension_inv;
                int u_shadowCone;

            } shadowSunLight[4];

            int u_shadow_spread_coefs;

            // Mesh Skinning uniform
            int u_gradient_matrix_array;

            void compileShader();
            void queryShaderUniforms();

        public:
            ShaderAlgorithmsBitMask frankenFormat;
            ShaderPBRAlgorithmEnum shaderPBRAlgorithm;
            ShaderShadowAlgorithmEnum shaderShadowAlgorithm;

            FrankenShader(
                ShaderAlgorithmsBitMask shaderAlgorithms,
                ShaderPBRAlgorithmEnum _shaderPBRAlgorithm,
                ShaderShadowAlgorithmEnum _shaderShadowAlgorithm);

            void sendSkinMatrix(std::vector<MathCore::mat4f> *gradient_matrix);

            /*
            void sendParamaters(const MathCore::mat4f *mvp, Transform *node, Components::ComponentCamera *camera,
                const Components::ComponentMaterial* material, const MathCore::vec3f &ambientLightColor,
                                int albedoTexUnit, int specularTexUnit, int normalTexUnit, int emissionTexUnit, int ambientCubeOrSphereTexUnit,
                                int pbrOptimizationTexUnit);
            */

            // void sendSunLightConfig(int index, const MathCore::vec3f &radiance, const MathCore::vec3f &worldDirection);

            static ShaderAlgorithmsBitMask ValidateFormat(ShaderAlgorithmsBitMask flags);

            FrankenUniformManager frankenUniformManager;

            friend class FrankenUniformManager;
        };

    }

}
