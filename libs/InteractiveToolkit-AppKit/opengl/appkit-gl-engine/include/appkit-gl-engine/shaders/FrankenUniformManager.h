#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

#include <appkit-gl-engine/Components/ComponentCamera.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>

namespace AppKit
{
    namespace GLEngine
    {

        class PBRSetup
        {
        public:
            MathCore::vec3f albedoColor;
            MathCore::vec3f emissionColor;

            std::shared_ptr<OpenGL::VirtualTexture> texAlbedo;
            std::shared_ptr<OpenGL::VirtualTexture> texNormal;
            std::shared_ptr<OpenGL::VirtualTexture> texSpecular;
            std::shared_ptr<OpenGL::VirtualTexture> texEmission;

            float roughness;
            float metallic;

            PBRSetup()
            {
                albedoColor = MathCore::vec3f(1.0f);
                emissionColor = MathCore::vec3f(0.0f); // [0.0f .. 8.0f]
                roughness = 1.0f;
                metallic = 0.0f;
            }
        };

        class FrankenShader;

        enum FrankenUniformTextureSlot
        {
            FrankenUniformTextureSlot_Albedo = 0,
            FrankenUniformTextureSlot_Normal,
            FrankenUniformTextureSlot_Emission,
            FrankenUniformTextureSlot_Specular,
            FrankenUniformTextureSlot_PBROptimization,
            FrankenUniformTextureSlot_EnvironmentCubeOrSphere,

            FrankenUniformTextureSlot_ShadowSunLight0,
            FrankenUniformTextureSlot_ShadowSunLight1,
            FrankenUniformTextureSlot_ShadowSunLight2,
            FrankenUniformTextureSlot_ShadowSunLight3,

            FrankenUniformTextureSlot_Count
        };

        class FrankenUniformManager
        {

            bool different_shader;

        public:
            FrankenShader *old_frankenShader;
            FrankenShader *frankenShader;

            // static const AppKit::OpenGL::VirtualTexture *last_tex_unit[FrankenUniformTextureSlot_Count];
            const AppKit::OpenGL::VirtualTexture *tex_unit[FrankenUniformTextureSlot_Count];

            // GLint tex_unit_uniform_location[FrankenUniformTextureSlot_Count];

            // mvp
            MathCore::mat4f mvp;

            // node
            MathCore::mat4f normal_LocalToWorld;
            MathCore::mat4f normal_LocalToWorld_it;

            // camera
            MathCore::vec3f camera_PosWorld;

            // material
            PBRSetup material_PBRSetup;

            // ambientColor
            MathCore::vec3f ambientColor;

            // sunLight
            MathCore::vec3f sunLight_Radiance[4];
            MathCore::vec3f sunLight_WorldDirection[4];

            // shadow sunLight
            MathCore::vec3f shadowSunLight_Radiance[4];
            MathCore::vec3f shadowSunLight_WorldDirection[4];
            MathCore::mat4f shadowSunLight_ProjMatrix[4];
            MathCore::vec3f shadowSunLight_Dimension[4];
            MathCore::vec3f shadowSunLight_Dimension_inv[4];
            MathCore::vec3f shadowSunLight_Cone[4];

            // float shadowSpreadCoefs[32][2];

            FrankenUniformManager(FrankenShader *_frankenShader);

            void clear();
            void setMVP(const MathCore::mat4f *mvp);
            void setNormalTransform(Transform *node);
            void setCameraReference(Components::ComponentCamera *camera);
            void setPBRMaterial(const PBRSetup &material);
            void setAmbientLightColor(const MathCore::vec3f &ambientLightColor);

            // texture setup... from material
            void setNormalTexture(const AppKit::OpenGL::VirtualTexture *tex);
            void setAlbedoTexture(const AppKit::OpenGL::VirtualTexture *tex);
            void setSpecularTexture(const AppKit::OpenGL::VirtualTexture *tex);
            void setEmissionTexture(const AppKit::OpenGL::VirtualTexture *tex);

            // texture setup... from global scene
            void setEnvironmentCubeTexture(const AppKit::OpenGL::GLCubeMap *tex);
            void setEnvironmentSphereTexture(const AppKit::OpenGL::GLTexture *tex);
            void setPBROptimizationTexture(const AppKit::OpenGL::GLTexture *tex);

            void setSunLightConfig(int index, const MathCore::vec3f &radiance, const MathCore::vec3f &worldDirection);
            void setShadowSunLightConfig(int index,
                                         const MathCore::vec3f &radiance, const MathCore::vec3f &worldDirection,
                                         const MathCore::vec3f &dimension, const MathCore::vec3f &cone_cos_sin_tan,
                                         const MathCore::mat4f &shadowProjMatrix,
                                         const AppKit::OpenGL::GLTexture *shadowTex);

            // uses the last material setup
            // set just the needed texture unit
            // upload the uniform parameters
            void activateShader();

            void activeTexUnit(GLRenderState *state);
            // static void deactiveTexUnit();

            void readUniformsFromShaderAndInitStatic();
        };

    }
}