#pragma once

#include <appkit-gl-engine/shaders/PBRShaderSelector.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/ResourceMap.h>
#include <appkit-gl-engine/shaders/FrankenShaderManager.h>
#include <appkit-gl-engine/Renderer/RenderPipeline.h>


namespace AppKit
{
    namespace GLEngine
    {

        Utils::ShaderPropertyBag PBRShaderSelector::createDefaultBag() const
        {
            Utils::ShaderPropertyBag bag;

            bag.addProperty("albedoColor", MathCore::vec3f(1.0f));
            bag.addProperty("emissionColor", MathCore::vec3f(0.0f));
            bag.addProperty("roughness", 1.0f);
            bag.addProperty("metallic", 0.0f);

            bag.addProperty("texAlbedo", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            bag.addProperty("texNormal", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            bag.addProperty("texSpecular", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            bag.addProperty("texEmission", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));

            return bag;
        }

        void PBRShaderSelector::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material)
        {
            const auto &materialBag = material->property_bag;

            PBRSetup pbrSetup;

            pbrSetup.albedoColor = materialBag.getProperty<MathCore::vec3f>("albedoColor");
            pbrSetup.emissionColor = materialBag.getProperty<MathCore::vec3f>("emissionColor");
            pbrSetup.roughness = materialBag.getProperty<float>("roughness");
            pbrSetup.metallic = materialBag.getProperty<float>("metallic");

            pbrSetup.texAlbedo = materialBag.getProperty<std::shared_ptr<OpenGL::VirtualTexture>>("texAlbedo");
            pbrSetup.texNormal = materialBag.getProperty<std::shared_ptr<OpenGL::VirtualTexture>>("texNormal");
            pbrSetup.texSpecular = materialBag.getProperty<std::shared_ptr<OpenGL::VirtualTexture>>("texSpecular");
            pbrSetup.texEmission = materialBag.getProperty<std::shared_ptr<OpenGL::VirtualTexture>>("texEmission");

            if (pbrSetup.texAlbedo == nullptr)
                pbrSetup.texAlbedo = resourceMap->defaultAlbedoTexture;
            if (pbrSetup.texNormal == nullptr)
                pbrSetup.texNormal = resourceMap->defaultNormalTexture;

            state->BlendMode = BlendModeDisabled; // material->pbr.blendMode;
            state->DepthTest = DepthTestLess;

            ShaderAlgorithmsBitMask frankenFormat = 0;

            if (pbrSetup.texAlbedo != nullptr)
                frankenFormat |= ShaderAlgorithms_TextureAlbedo;
            if (pbrSetup.texSpecular != nullptr)
                frankenFormat |= ShaderAlgorithms_TextureSpecular;
            if (pbrSetup.texNormal != nullptr)
                frankenFormat |= ShaderAlgorithms_NormalMap;
            if (pbrSetup.texEmission != nullptr && pbrSetup.emissionColor != MathCore::vec3f(0.0f))
                frankenFormat |= ShaderAlgorithms_TextureEmission;

            if (renderPipeline->cubeAmbientLight_1x1 != nullptr && renderPipeline->ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture)
                frankenFormat |= ShaderAlgorithms_AmbientLightSkybox;
            if (renderPipeline->sphereAmbientLight != nullptr && renderPipeline->ambientLight.lightMode == AmbientLightMode_SphereMapTexture)
                frankenFormat |= ShaderAlgorithms_AmbientLightSpheremap;

            else if (renderPipeline->ambientLight.lightMode != AmbientLightMode_None)
                frankenFormat |= ShaderAlgorithms_AmbientLightColor;

            bool use_sRGBConvertion = !AppKit::GLEngine::Engine::Instance()->sRGBCapable;

            if (use_sRGBConvertion)
                frankenFormat |= ShaderAlgorithms_sRGB;

            // mesh skinning bit flag
            frankenFormat |= material->skin_shader_matrix_size_bitflag;

            // setup sun light and shadow sun light
            int sunCount = 0;
            // no shadow processing
            for (size_t i = 0; i < renderPipeline->lightAndShadowManager.noShadowlightList.size(); i++)
            {
                auto light = renderPipeline->lightAndShadowManager.noShadowlightList[i];
                switch (light->type)
                {
                case Components::LightSun:
                    if (sunCount < 4)
                        frankenFormat |= ShaderAlgorithms_SunLight0 << (sunCount++);
                    break;
                default:
                    break;
                }
            }

            int sunShadowCount = 0;
            // shadow processing...
            for (size_t i = 0; i < renderPipeline->lightAndShadowManager.shadowLightList.size(); i++)
            {
                ShadowCache *shadow = renderPipeline->lightAndShadowManager.shadowLightList[i];
                switch (shadow->light->type)
                {
                case Components::LightSun:
                    if (sunShadowCount < 4)
                        frankenFormat |= ShaderAlgorithms_ShadowSunLight0 << (sunShadowCount++);
                    break;
                default:
                    break;
                }
            }

            frankenFormat = FrankenShader::ValidateFormat(frankenFormat);
            frankenFormat &= renderPipeline->ShaderAlgorithmsEnum_allowedFlags;

            frankenShader = renderPipeline->frankenShaderManager.getShader(frankenFormat, renderPipeline->shaderPBRAlgorithm, renderPipeline->shaderShadowAlgorithm);

            // frankenShader->frankenUniformManager.activateShader();
            state->CurrentShader = frankenShader;

            frankenShader->frankenUniformManager.setPBRMaterial(pbrSetup);
            frankenShader->frankenUniformManager.setAmbientLightColor(renderPipeline->ambientLightColorVec3);

            // texture setup... from global scene
            frankenShader->frankenUniformManager.setEnvironmentCubeTexture(renderPipeline->cubeAmbientLight_1x1.get());
            frankenShader->frankenUniformManager.setEnvironmentSphereTexture(renderPipeline->sphereAmbientLight.get());

            // #if defined(PBR_MODE_TEX_LOOKUP)
            frankenShader->frankenUniformManager.setPBROptimizationTexture(&renderPipeline->frankenShaderManager.pbrOptimizationTexture);
            // #endif

            // light parameters upload
            sunCount = 0;
            // no shadow processing
            for (size_t i = 0; i < renderPipeline->lightAndShadowManager.noShadowlightList.size(); i++)
            {
                auto light = renderPipeline->lightAndShadowManager.noShadowlightList[i];
                switch (light->type)
                {
                case Components::LightSun:
                    if (sunCount < 4)
                    {
                        // frankenShader->sendSunLightConfig(sunCount, light->sun.finalIntensity, light->sun.worldDirection);
                        frankenShader->frankenUniformManager.setSunLightConfig(sunCount, light->sun.finalIntensity, light->sun.worldDirection);
                        sunCount++;
                    }
                    break;
                default:
                    break;
                }
            }

            sunShadowCount = 0;
            // shadow processing...
            for (size_t i = 0; i < renderPipeline->lightAndShadowManager.shadowLightList.size(); i++)
            {
                ShadowCache *shadow = renderPipeline->lightAndShadowManager.shadowLightList[i];
                switch (shadow->light->type)
                {
                case Components::LightSun:
                    if (sunShadowCount < 4)
                    {
                        frankenShader->frankenUniformManager.setShadowSunLightConfig(
                            sunShadowCount,
                            shadow->light->sun.finalIntensity, shadow->light->sun.worldDirection,
                            shadow->obb.dimension_2 * 2.0f, MathCore::vec3f(shadow->light->sun.cone_cos, shadow->light->sun.cone_sin, shadow->light->sun.cone_tangent),
                            shadow->viewProjection, &shadow->depthTexture);
                        sunShadowCount++;
                    }
                    break;
                default:
                    break;
                }
            }

            frankenShader->frankenUniformManager.activeTexUnit(state);

        }
        void PBRShaderSelector::setUniformsFromMatrices(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material,
            Transform *element,
            Components::ComponentCamera *camera,
            const MathCore::mat4f *mvp,
            const MathCore::mat4f *mv,
            const MathCore::mat4f *mvIT,
            const MathCore::mat4f *mvInv)
        {
            frankenShader->frankenUniformManager.setMVP(mvp);
            frankenShader->frankenUniformManager.setNormalTransform(element);
            frankenShader->frankenUniformManager.setCameraReference(camera);

            // mesh skinning upload matrix
            if (material->skin_shader_matrix_size_bitflag != 0)
            {
                if (material->skin_gradient_matrix_dirty)
                {
                    material->skin_gradient_matrix_dirty = false;
                    frankenShader->sendSkinMatrix(material->skin_gradient_matrix);
                }
            }
        }

    }
}