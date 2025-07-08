#include <appkit-gl-engine/Renderer/RenderPipeline.h>

#include <appkit-gl-engine/util/ResourceHelper.h>

#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

// #include <aRibeiroCore/aRibeiroCore.h>
// #include <appkit-gl-base/opengl-wrapper.h>
// #include <aRibeiroData/aRibeiroData.h>
// #include <appkit-gl-engine/mini-gl-engine.h>

#include <algorithm> // std::sort
// #include <appkit-gl-engine/Frankenshader.h>

namespace AppKit
{
    namespace GLEngine
    {

        /*

        bool RenderPipeline::traverse_search_elements(Transform *element, void* userData) {
            Components::ComponentLight* light = (Components::ComponentLight*)element->findComponent(Components::ComponentLight::Type);
            if (light != nullptr && light->type == Components::LightSun)
                sceneSunLights.push_back(light);

            Components::ComponentParticleSystem* particleSystem = (Components::ComponentParticleSystem*)element->findComponent(Components::ComponentParticleSystem::Type);
            if (particleSystem != nullptr)
                sceneParticleSystem.push_back(particleSystem);

            Components::ComponentColorLine* colorLine = (Components::ComponentColorLine*)element->findComponent(Components::ComponentColorLine::Type);
            if (colorLine != nullptr)
                debugLines.push_back(colorLine);

            return true;
        }

        void RenderPipeline::SearchSpecialObjects(Transform *root) {
            sceneSunLights.clear();
            sceneParticleSystem.clear();
            debugLines.clear();
            root->traversePreOrder_DepthFirst(
                EventCore::CallbackWrapper(this, &RenderPipeline::traverse_search_elements),
                nullptr);
        }
        */

        void RenderPipeline::allMeshRender(Transform *element, const DefaultEngineShader *shader) const
        {
            for (int i = 0; i < element->getComponentCount(); i++)
            {
                auto component = element->getComponentAt(i);
                if (component->compareType(Components::ComponentMesh::Type))
                {
                    auto mesh = std::dynamic_pointer_cast<Components::ComponentMesh>(component);
                    mesh->setLayoutPointers(shader);
                    mesh->draw();
                    mesh->unsetLayoutPointers(shader);
                }
            }
        }

        void RenderPipeline::allMeshRender_Range(Transform *element, const DefaultEngineShader *shader, int start_index, int end_index) const
        {
            for (int i = start_index; i < end_index; i++)
            {
                auto component = element->getComponentAt(i);
                if (component->compareType(Components::ComponentMesh::Type))
                {
                    auto mesh = std::dynamic_pointer_cast<Components::ComponentMesh>(component);
                    mesh->setLayoutPointers(shader);
                    mesh->draw();
                    mesh->unsetLayoutPointers(shader);
                }
            }
        }

        void RenderPipeline::materialSetupAndRender(
            Components::ComponentMaterial *material,
            Transform *element,
            Components::ComponentCamera *camera,
            int start_index, int end_index,
            const MathCore::mat4f *mvp,
            const MathCore::mat4f *mv,
            const MathCore::mat4f *mvIT,
            const MathCore::mat4f *mvInv)
        {

            GLRenderState *state = GLRenderState::Instance();
            /*
            Components::ComponentCamera *camera = (Components::ComponentCamera*)userData;

            MathCore::mat4f *mvp;
            MathCore::mat4f *mv;
            MathCore::mat4f *mvIT;
            MathCore::mat4f *mvInv;
            element->computeRenderMatrix(camera->viewProjection,
                camera->view,
                camera->viewIT,
                camera->viewInv,
                &mvp,
                &mv,
                &mvIT,
                &mvInv);
            */

            ShaderAlgorithmsBitMask frankenFormat;
            FrankenShader *frankenShader;
            bool use_sRGBConvertion = !AppKit::GLEngine::Engine::Instance()->sRGBCapable;
            int sunCount;
            int sunShadowCount;
            // int sunCount;
            switch (material->type)
            {
            case Components::MaterialUnlit:
                state->BlendMode = material->unlit.blendMode;
                state->CurrentShader = &unlitShader;

                unlitShader.setColor(material->unlit.color);
                unlitShader.setMVP(*mvp);

                allMeshRender_Range(element, &unlitShader, start_index, end_index);
                break;

            case Components::MaterialUnlitVertexColor:
                state->BlendMode = material->unlit.blendMode;
                state->CurrentShader = &unlit_vertcolor_Shader;

                unlit_vertcolor_Shader.setColor(material->unlit.color);
                unlit_vertcolor_Shader.setMVP(*mvp);

                allMeshRender_Range(element, &unlit_vertcolor_Shader, start_index, end_index);
                break;
                break;

            case Components::MaterialUnlitTexture:
                state->BlendMode = material->unlit.blendMode;
                state->CurrentShader = &unlit_tex_Shader;

                unlit_tex_Shader.setColor(material->unlit.color);
                unlit_tex_Shader.setTexture(0);
                unlit_tex_Shader.setMVP(*mvp);

                material->unlit.tex->active(0);
                allMeshRender_Range(element, &unlit_tex_Shader, start_index, end_index);
                material->unlit.tex->deactive(0);

                break;

            case Components::MaterialUnlitTextureVertexColorFont:

                state->BlendMode = material->unlit.blendMode;
                state->CurrentShader = &unlit_tex_vertcolor_font_Shader;

                unlit_tex_vertcolor_font_Shader.setColor(material->unlit.color);
                unlit_tex_vertcolor_font_Shader.setTexture(0);
                unlit_tex_vertcolor_font_Shader.setMVP(*mvp);

                material->unlit.tex->active(0);
                allMeshRender_Range(element, &unlit_tex_vertcolor_font_Shader, start_index, end_index);
                material->unlit.tex->deactive(0);

                break;

                // PBR setup
            case Components::MaterialPBR:

                state->BlendMode = BlendModeDisabled; // material->pbr.blendMode;
                state->DepthTest = DepthTestLess;

                // sunCount = 0;

                frankenFormat = 0;

                if (material->pbr.texAlbedo != nullptr)
                    frankenFormat |= ShaderAlgorithms_TextureAlbedo;
                if (material->pbr.texSpecular != nullptr)
                    frankenFormat |= ShaderAlgorithms_TextureSpecular;
                if (material->pbr.texNormal != nullptr)
                    frankenFormat |= ShaderAlgorithms_NormalMap;
                if (material->pbr.texEmission != nullptr && material->pbr.emissionColor != MathCore::vec3f(0.0f))
                    frankenFormat |= ShaderAlgorithms_TextureEmission;

                if (cubeAmbientLight_1x1 != nullptr && ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture)
                    frankenFormat |= ShaderAlgorithms_AmbientLightSkybox;
                if (sphereAmbientLight != nullptr && ambientLight.lightMode == AmbientLightMode_SphereMapTexture)
                    frankenFormat |= ShaderAlgorithms_AmbientLightSpheremap;

                else if (ambientLight.lightMode != AmbientLightMode_None)
                    frankenFormat |= ShaderAlgorithms_AmbientLightColor;

                // sunCount = sceneSunLights.size();
                // sunCount = (int)objectPlaces.sunLights.size();

                if (use_sRGBConvertion)
                    frankenFormat |= ShaderAlgorithms_sRGB;

                // mesh skinning bit flag
                frankenFormat |= material->skin_shader_matrix_size_bitflag;

                // setup sun light and shadow sun light
                sunCount = 0;
                // no shadow processing
                for (size_t i = 0; i < lightAndShadowManager.noShadowlightList.size(); i++)
                {
                    auto light = lightAndShadowManager.noShadowlightList[i];
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

                sunShadowCount = 0;
                // shadow processing...
                for (size_t i = 0; i < lightAndShadowManager.shadowLightList.size(); i++)
                {
                    ShadowCache *shadow = lightAndShadowManager.shadowLightList[i];
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
                frankenFormat &= ShaderAlgorithmsEnum_allowedFlags;

                frankenShader = frankenShaderManager.getShader(frankenFormat, shaderPBRAlgorithm, shaderShadowAlgorithm);

                frankenShader->frankenUniformManager.activateShader();

                frankenShader->frankenUniformManager.setMVP(mvp);
                frankenShader->frankenUniformManager.setNormalTransform(element);
                frankenShader->frankenUniformManager.setCameraReference(camera);
                frankenShader->frankenUniformManager.setPBRMaterial(material);
                frankenShader->frankenUniformManager.setAmbientLightColor(ambientLightColorVec3);

                // texture setup... from global scene
                frankenShader->frankenUniformManager.setEnvironmentCubeTexture(cubeAmbientLight_1x1.get());
                frankenShader->frankenUniformManager.setEnvironmentSphereTexture(sphereAmbientLight.get());

                // #if defined(PBR_MODE_TEX_LOOKUP)
                frankenShader->frankenUniformManager.setPBROptimizationTexture(&frankenShaderManager.pbrOptimizationTexture);
                // #endif

                // light parameters upload
                sunCount = 0;
                // no shadow processing
                for (size_t i = 0; i < lightAndShadowManager.noShadowlightList.size(); i++)
                {
                    auto light = lightAndShadowManager.noShadowlightList[i];
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
                for (size_t i = 0; i < lightAndShadowManager.shadowLightList.size(); i++)
                {
                    ShadowCache *shadow = lightAndShadowManager.shadowLightList[i];
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

                frankenShader->frankenUniformManager.activeTexUnit();

                /*
                for (size_t i = 0; i < objectPlaces.sunLights.size(); i++)
                {
                    Components::ComponentLight *sunLight = objectPlaces.sunLights[i];
                    frankenShader->sendSunLightConfig(i, sunLight->sun.finalIntensity, sunLight->sun.worldDirection);
                }
                */

                // mesh skinning upload matrix
                if (material->skin_shader_matrix_size_bitflag != 0)
                {
                    if (material->skin_gradient_matrix_dirty)
                    {
                        material->skin_gradient_matrix_dirty = false;
                        frankenShader->sendSkinMatrix(material->skin_gradient_matrix);
                    }
                }

                allMeshRender_Range(element, frankenShader, start_index, end_index);

                /*
                //#if defined(PBR_MODE_TEX_LOOKUP)
                    frankenShaderManager.disable_PBR_OptimizationTexUnit(5);
                //#endif

                if (cubeAmbientLight_1x1 != nullptr && ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture)
                    cubeAmbientLight_1x1->deactive(4);
                if (sphereAmbientLight != nullptr && ambientLight.lightMode == AmbientLightMode_SphereMapTexture)
                    sphereAmbientLight->deactive(4);


                if ((frankenFormat & ShaderAlgorithms_TextureEmission) != 0) {
                    material->pbr.texEmission->deactive(3);
                }
                if (material->pbr.texNormal != nullptr)
                    material->pbr.texNormal->deactive(2);
                if (material->pbr.texSpecular != nullptr)
                    material->pbr.texSpecular->deactive(1);
                if (material->pbr.texAlbedo != nullptr)
                    material->pbr.texAlbedo->deactive(0);

                */

                frankenShader->frankenUniformManager.deactiveTexUnit();

                break;
            case Components::MaterialCustomShader:

                if (material->custom_shader == nullptr)
                    break;

                material->custom_shader->activateShaderAndSetPropertiesFromBag(
                    camera, mvp,
                    element, state,
                    material->custom_shader_property_bag);
                allMeshRender_Range(element, material->custom_shader.get(), start_index, end_index);
                material->custom_shader->deactivateShader(state);

                break;
            default:
                break;
            }
        }

        // bool RenderPipeline::traverse_multipass_render(Transform *element, void *userData)
        // {
        //     Components::ComponentMaterial *material = (Components::ComponentMaterial *)element->findComponent(Components::ComponentMaterial::Type);
        //     if (material == nullptr)
        //         return true;

        //     GLRenderState *state = GLRenderState::Instance();
        //     Components::ComponentCamera *camera = (Components::ComponentCamera *)userData;

        //     MathCore::mat4f *mvp;
        //     MathCore::mat4f *mv;
        //     MathCore::mat4f *mvIT;
        //     MathCore::mat4f *mvInv;
        //     element->computeRenderMatrix(camera->viewProjection,
        //                                  camera->view,
        //                                  camera->viewIT,
        //                                  camera->viewInv,
        //                                  &mvp,
        //                                  &mv,
        //                                  &mvIT,
        //                                  &mvInv);

        //     switch (material->type)
        //     {
        //     case Components::MaterialUnlit:
        //         state->BlendMode = material->unlit.blendMode;
        //         state->CurrentShader = &unlitShader;

        //         unlitShader.setColor(material->unlit.color);
        //         unlitShader.setMVP(*mvp);

        //         allMeshRender(element, &unlitShader);
        //         break;

        //     case Components::MaterialUnlitTexture:
        //         state->BlendMode = material->unlit.blendMode;
        //         state->CurrentShader = &unlit_tex_Shader;

        //         unlit_tex_Shader.setColor(material->unlit.color);
        //         unlit_tex_Shader.setTexture(0);
        //         unlit_tex_Shader.setMVP(*mvp);

        //         material->unlit.tex->active(0);
        //         allMeshRender(element, &unlit_tex_Shader);
        //         material->unlit.tex->deactive(0);

        //         break;

        //     case Components::MaterialUnlitTextureVertexColorFont:

        //         state->BlendMode = material->unlit.blendMode;
        //         state->CurrentShader = &unlit_tex_vertcolor_font_Shader;

        //         unlit_tex_vertcolor_font_Shader.setColor(material->unlit.color);
        //         unlit_tex_vertcolor_font_Shader.setTexture(0);
        //         unlit_tex_vertcolor_font_Shader.setMVP(*mvp);

        //         material->unlit.tex->active(0);
        //         allMeshRender(element, &unlit_tex_vertcolor_font_Shader);
        //         material->unlit.tex->deactive(0);

        //         break;

        //         // PBR setup
        //     case Components::MaterialPBR:

        //         state->BlendMode = BlendModeDisabled; // material->pbr.blendMode;
        //         state->DepthTest = DepthTestLessEqual;

        //         material->pbr.texAlbedo->active(0);
        //         material->pbr.texNormal->active(1);

        //         // 1st pass: ambient light
        //         if (ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor ||
        //             ambientLight.lightMode == AmbientLightMode_Color)
        //         {
        //             state->CurrentShader = &unlit_tex_Shader;

        //             unlit_tex_Shader.setColor(MathCore::vec4f(material->pbr.albedoColor * ambientLightColorVec3, 1.0f));
        //             unlit_tex_Shader.setTexture(0);
        //             unlit_tex_Shader.setMVP(*mvp);

        //             allMeshRender(element, &unlit_tex_Shader);
        //         }
        //         else if (ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture)
        //         {

        //             cubeAmbientLight_1x1->active(2);

        //             state->CurrentShader = &ambientLight_tex_cube_PassShader;

        //             ambientLight_tex_cube_PassShader.setColor(MathCore::vec4f(material->pbr.albedoColor, 1.0f));

        //             // texture setup...
        //             ambientLight_tex_cube_PassShader.setTexture(0);
        //             ambientLight_tex_cube_PassShader.setTextureNormal(1);
        //             ambientLight_tex_cube_PassShader.setTextureCubeEnvironment(2);

        //             ambientLight_tex_cube_PassShader.setMVP(*mvp);
        //             ambientLight_tex_cube_PassShader.setLocalToWorld(element->getMatrix(true));
        //             ambientLight_tex_cube_PassShader.setLocalToWorld_it(element->getMatrixInverseTranspose(true));

        //             allMeshRender(element, &ambientLight_tex_cube_PassShader);

        //             cubeAmbientLight_1x1->deactive(2);
        //         }

        //         // 2nd pass: sun light
        //         state->BlendMode = BlendModeAdd; // material->pbr.blendMode;
        //         state->DepthWrite = false;
        //         state->CurrentShader = &directionalLightPassShader;

        //         directionalLightPassShader.setTexture(0);
        //         directionalLightPassShader.setTextureNormal(1);

        //         directionalLightPassShader.setMVP(*mvp);
        //         directionalLightPassShader.setLocalToWorld(element->getMatrix(true));
        //         directionalLightPassShader.setLocalToWorld_it(element->getMatrixInverseTranspose(true));

        //         directionalLightPassShader.setMaterialAlbedoColor(material->pbr.albedoColor);
        //         directionalLightPassShader.setMaterialRoughness(material->pbr.roughness);
        //         directionalLightPassShader.setMaterialMetallic(material->pbr.metallic);

        //         directionalLightPassShader.setCameraPosWorld(camera->transform[0]->getPosition(true));

        //         for (size_t i = 0; i < objectPlaces.sunLights.size(); i++)
        //         {
        //             Components::ComponentLight *sunLight = objectPlaces.sunLights[i];

        //             directionalLightPassShader.setLightDirWorld(sunLight->sun.worldDirection);
        //             directionalLightPassShader.setLightRadiance(sunLight->sun.finalIntensity);

        //             allMeshRender(element, &directionalLightPassShader);
        //         }

        //         material->pbr.texNormal->deactive(1);
        //         material->pbr.texAlbedo->deactive(0);

        //         state->DepthWrite = true;

        //         break;
        //     default:
        //         break;
        //     }

        //     return true;
        // }

        bool RenderPipeline::traverse_singlepass_render(std::shared_ptr<Transform> element, void *userData)
        {

            Components::ComponentMaterial *material = nullptr;
            int start_index = 0;
            int end_index = 0;
            bool camera_set = false;

            Components::ComponentCamera *camera = (Components::ComponentCamera *)userData;

            MathCore::mat4f *mvp;
            MathCore::mat4f *mv;
            MathCore::mat4f *mvIT;
            MathCore::mat4f *mvInv;

            for (int i = 0; i < element->getComponentCount(); i++)
            {
                auto component = element->getComponentAt(i);
                if (component->compareType(Components::ComponentMaterial::Type))
                {
                    end_index = i;
                    if (material != nullptr && start_index != end_index)
                    {
                        // camera matrix computation
                        if (!camera_set)
                        {
                            camera_set = true;
                            element->computeRenderMatrix(camera->viewProjection,
                                                         camera->view,
                                                         camera->viewIT,
                                                         camera->viewInv,
                                                         &mvp,
                                                         &mv,
                                                         &mvIT,
                                                         &mvInv);
                        }
                        // setup material and render
                        materialSetupAndRender(
                            material,
                            element.get(),
                            camera,
                            start_index, end_index,
                            mvp,
                            mv,
                            mvIT,
                            mvInv);
                    }
                    material = (Components::ComponentMaterial *)component.get();
                    start_index = i + 1;
                }
            }

            end_index = element->getComponentCount();
            if (material != nullptr && start_index != end_index)
            {
                // camera matrix computation
                if (!camera_set)
                {
                    camera_set = true;
                    element->computeRenderMatrix(camera->viewProjection,
                                                 camera->view,
                                                 camera->viewIT,
                                                 camera->viewInv,
                                                 &mvp,
                                                 &mv,
                                                 &mvIT,
                                                 &mvInv);
                }
                // setup material and render
                materialSetupAndRender(
                    material,
                    element.get(),
                    camera,
                    start_index, end_index,
                    mvp,
                    mv,
                    mvIT,
                    mvInv);
            }

            /*

            Components::ComponentMaterial* material = (Components::ComponentMaterial*)element->findComponent(Components::ComponentMaterial::Type);
            if (material == nullptr)
                return true;

            GLRenderState *state = GLRenderState::Instance();
            Components::ComponentCamera *camera = (Components::ComponentCamera*)userData;

            MathCore::mat4f *mvp;
            MathCore::mat4f *mv;
            MathCore::mat4f *mvIT;
            MathCore::mat4f *mvInv;
            element->computeRenderMatrix(camera->viewProjection,
                                         camera->view,
                                         camera->viewIT,
                                         camera->viewInv,
                                         &mvp,
                                         &mv,
                                         &mvIT,
                                         &mvInv);

            ShaderAlgorithmsBitMask frankenFormat;
            FrankenShader *frankenShader;
            bool use_sRGBConvertion = !AppKit::GLEngine::Engine::Instance()->sRGBCapable;
            int sunCount;
            switch (material->type) {
                case Components::MaterialUnlit:
                    state->BlendMode = material->unlit.blendMode;
                    state->CurrentShader = &unlitShader;

                    unlitShader.setColor(material->unlit.color);
                    unlitShader.setMVP(*mvp);

                    allMeshRender(element, &unlitShader);
                    break;

                case Components::MaterialUnlitTexture:
                    state->BlendMode = material->unlit.blendMode;
                    state->CurrentShader = &unlit_tex_Shader;

                    unlit_tex_Shader.setColor(material->unlit.color);
                    unlit_tex_Shader.setTexture(0);
                    unlit_tex_Shader.setMVP(*mvp);

                    material->unlit.tex->active(0);
                    allMeshRender(element, &unlit_tex_Shader);
                    material->unlit.tex->deactive(0);

                    break;

                case Components::MaterialUnlitTextureVertexColorFont:

                    state->BlendMode = material->unlit.blendMode;
                    state->CurrentShader = &unlit_tex_vertcolor_font_Shader;

                    unlit_tex_vertcolor_font_Shader.setColor(material->unlit.color);
                    unlit_tex_vertcolor_font_Shader.setTexture(0);
                    unlit_tex_vertcolor_font_Shader.setMVP(*mvp);

                    material->unlit.tex->active(0);
                    allMeshRender(element, &unlit_tex_vertcolor_font_Shader);
                    material->unlit.tex->deactive(0);

                    break;

                    //PBR setup
                case Components::MaterialPBR:

                    state->BlendMode = BlendModeDisabled;//material->pbr.blendMode;
                    state->DepthTest = DepthTestLessEqual;

                    sunCount = 0;

                    frankenFormat = 0;

                    if (material->pbr.texAlbedo != nullptr)
                        frankenFormat |= ShaderAlgorithms_TextureAlbedo;
                    if (material->pbr.texNormal != nullptr)
                        frankenFormat |= ShaderAlgorithms_NormalMap;


                    if ( cubeAmbientLight_1x1 != nullptr && ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture)
                        frankenFormat |= ShaderAlgorithms_AmbientLightSkybox;
                    else if (ambientLight.lightMode != AmbientLightMode_None)
                        frankenFormat |= ShaderAlgorithms_AmbientLightColor;

                    sunCount = sceneSunLights.size();

                    if (use_sRGBConvertion)
                        frankenFormat |= ShaderAlgorithms_sRGB;

                    frankenFormat = FrankenShader::MountFormat(frankenFormat,sunCount);

                    frankenFormat &= ShaderAlgorithmsEnum_allowedFlags;

                    frankenShader = frankenShaderManager.getShader(frankenFormat);
                    state->CurrentShader = frankenShader;

                    if (material->pbr.texAlbedo != nullptr)
                        material->pbr.texAlbedo->active(0);
                    if (material->pbr.texNormal != nullptr)
                        material->pbr.texNormal->active(1);
                    if (cubeAmbientLight_1x1 != nullptr)
                        cubeAmbientLight_1x1->active(2);

                    frankenShader->sendParamaters(mvp, element, camera, material,
                                                  ambientLightColorVec3, 0, 1, 2);

                    for (size_t i=0;i<sceneSunLights.size();i++)
                    {
                        Components::ComponentLight *sunLight = sceneSunLights[i];
                        frankenShader->sendSunLightConfig(i,sunLight->sun.finalIntensity,sunLight->sun.worldDirection);
                    }

                    allMeshRender(element, frankenShader);

                    if (cubeAmbientLight_1x1 != nullptr)
                        cubeAmbientLight_1x1->deactive(2);
                    if (material->pbr.texNormal != nullptr)
                        material->pbr.texNormal->deactive(1);
                    if (material->pbr.texAlbedo != nullptr)
                        material->pbr.texAlbedo->deactive(0);

                    break;
                default:
                    break;
            }

            */

            return true;
        }

        RenderPipeline::RenderPipeline()
        {
            cubeSkyBox = nullptr;
            cubeAmbientLight_1x1 = nullptr;
            sphereAmbientLight = nullptr;
            depthRenderer = nullptr;

            ShaderAlgorithmsEnum_allowedFlags = (uint64_t)0xffffffffffffffff;

            // use_shadow = true;
            shaderShadowAlgorithm = ShaderShadowAlgorithm_PCSS_DST_CENTER;
            // shaderShadowAlgorithm = ShaderShadowAlgorithm_4x4PCF;
            shaderPBRAlgorithm = ShaderPBRAlgorithm_Normal;

            AppKit::GLEngine::Components::setSphere(
                1.0f, 32, 32, // radius, sectorCount, stackCount,
                &sunIndex,    //&result->indices,
                &sunVertex,   //&result->pos,
                nullptr,      //&result->uv[0],
                nullptr,      //&result->normals,
                nullptr,      //&result->tangent,
                nullptr       //&result->binormal
            );
        }

        RenderPipeline::~RenderPipeline()
        {
            // ReferenceCounter<void*> *refCounter = &Engine::Instance()->referenceCounter;
            // refCounter->remove(cubeSkyBox);
            // cubeSkyBox = nullptr;
            if (cubeSkyBox != nullptr)
                cubeSkyBox = nullptr;
            if (cubeAmbientLight_1x1 != nullptr)
                cubeAmbientLight_1x1 = nullptr;
            if (sphereAmbientLight != nullptr)
                sphereAmbientLight = nullptr;
            if (depthRenderer != nullptr)
            {
                delete depthRenderer;
                depthRenderer = nullptr;
            }
        }

        // void RenderPipeline::runMultiPassPipeline(Transform *root, Components::ComponentCamera *camera, bool clear)
        // {

        //     ITK_ABORT(!AppKit::GLEngine::Engine::Instance()->sRGBCapable, "Error: Cannot run this pipeline on non-sRGB capable device.\n");

        //     root->resetVisited();
        //     root->preComputeTransforms();

        //     camera->precomputeViewProjection(true);

        //     if (clear)
        //     {
        //         if (cubeSkyBox == nullptr)
        //         {
        //             glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        //         }
        //         else
        //         {
        //             glClear(GL_DEPTH_BUFFER_BIT);
        //             cubeSkyBox->draw(camera->view, camera->projection);
        //             // cubeSkyBox->drawAnotherCube(camera->view, camera->projection, cubeAmbientLight_1x1);
        //         }
        //     }
        //     // light setup
        //     if (
        //         (ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor ||
        //          ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture) &&
        //         cubeSkyBox != nullptr)
        //     {
        //         // ambientLightColor = MathCore::vec4f(cubeSkyBox->cubeMap.colorEstimation,1.0f);
        //         ambientLightColorVec3 = cubeSkyBox->cubeMap.colorEstimation;
        //         ambientLightColorVec3 = ResourceHelper::vec3ColorGammaToLinear(ambientLightColorVec3);
        //     }
        //     else
        //         ambientLightColorVec3 = ambientLight.color;
        //     // ambientLightColor = MathCore::vec4f(ambientLight.color, 1.0f);

        //     // ambientLightColorVec3 = MathCore::CVT<MathCore::vec4f>::toVec3(ambientLightColor);

        //     // ambientLightColorVec3 =

        //     objectPlaces.searchObjects(root);

        //     // SearchSpecialObjects(root);

        //     for (size_t i = 0; i < objectPlaces.sunLights.size(); i++)
        //         objectPlaces.sunLights[i]->postProcessing_computeLightParameters();

        //     root->traversePreOrder_DepthFirst(
        //         EventCore::CallbackWrapper(&RenderPipeline::traverse_multipass_render, this),
        //         camera);
        // }

        bool __compare__particle__system__reverse__(const std::shared_ptr<Components::ComponentParticleSystem> a, const std::shared_ptr<Components::ComponentParticleSystem> b)
        {
            return (b->distance_to_camera < a->distance_to_camera);
        }

        void RenderPipeline::runSinglePassPipeline(std::shared_ptr<Transform> root, std::shared_ptr<Components::ComponentCamera> camera, bool clear)
        {

            // bool srgb = AppKit::GLEngine::Engine::Instance()->sRGBCapable;
            // ITK_ABORT(!AppKit::GLEngine::Engine::Instance()->sRGBCapable, "Error: Cannot run this pipeline on non-sRGB capable device.\n");

            /*
            root->resetVisited();
            root->preComputeTransforms();

            camera->precomputeViewProjection(true);
            */

            if (clear)
            {
                if (cubeSkyBox == nullptr)
                {
                    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                }
                else
                {
                    glClear(GL_DEPTH_BUFFER_BIT);
                    cubeSkyBox->draw(camera->view, camera->projection);
                    // cubeSkyBox->drawAnotherCube(camera->view, camera->projection, cubeAmbientLight_1x1);
                }
            }
            // light setup
            if ((ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor ||
                 ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture) &&
                cubeSkyBox != nullptr)
            {

                ambientLightColorVec3 = cubeSkyBox->cubeMap.colorEstimation;
                ambientLightColorVec3 = ResourceHelper::vec3ColorGammaToLinear(ambientLightColorVec3);

                // ambientLightColor = MathCore::vec4f(cubeSkyBox->cubeMap.colorEstimation,1.0f);
            }
            else
                ambientLightColorVec3 = ambientLight.color;
            // ambientLightColor = MathCore::vec4f(ambientLight.color, 1.0f);

            // ambientLightColorVec3 = MathCore::CVT<MathCore::vec4f>::toVec3(ambientLightColor);
            objectPlaces.searchObjects(root);
            // SearchSpecialObjects(root);

            for (size_t i = 0; i < objectPlaces.sunLights.size(); i++)
                objectPlaces.sunLights[i]->postProcessing_computeLightParameters();

            bool perspective = false;
            if (camera->compareType(Components::ComponentCameraPerspective::Type))
            {

                CollisionCore::AABB<MathCore::vec3f> scene_aabb;
                for (int i = 0; i < objectPlaces.filteredMeshWrappers.size(); i++)
                {
                    auto meshWrapper = objectPlaces.filteredMeshWrappers[i];
                    if (i == 0)
                        scene_aabb = meshWrapper->getAABB();
                    else
                        scene_aabb = CollisionCore::AABB<MathCore::vec3f>::joinAABB(scene_aabb, meshWrapper->getAABB());
                }

                perspective = true;

                CollisionCore::Frustum<MathCore::vec3f> frustum = CollisionCore::Frustum<MathCore::vec3f>(camera->projection, camera->view);
                // filter only objects visible to camera...
                objectPlaces.filterObjectsFrustum(root, frustum);
                // printf("Visible objects: %i\n", (int)objectPlaces.filteredMeshWrappers.size());

                lightAndShadowManager.setSceneAABB(scene_aabb);
                lightAndShadowManager.setMainCameraFrustum(frustum);
                lightAndShadowManager.setSceneRoot(root);
                lightAndShadowManager.setVisibleObjects(&objectPlaces);
                lightAndShadowManager.setRenderPipeline(this);
            }
            else if (camera->compareType(Components::ComponentCameraOrthographic::Type))
            {
                auto ortho = std::dynamic_pointer_cast<Components::ComponentCameraOrthographic>(camera);

                // auto projection_offset = MathCore::CVT<MathCore::vec4f>::toVec3(ortho->projection[3]);
                // auto projection_offset = MathCore::vec3f(
                //     0, 
                //     0, 
                //     (ortho->nearPlane + ortho->farPlane) * 0.5f);

                MathCore::vec3f size = 1.0f / MathCore::vec3f(ortho->projection.a1, ortho->projection.b2, ortho->projection.c3);

                // MathCore::quatf rotation = MathCore::OP<MathCore::quatf>::conjugate( ortho->getTransform()->getRotation(true) );
                MathCore::quatf rotation = ortho->getTransform()->getRotation(true);

                auto projection_offset = 
                rotation * (MathCore::vec3f(
                    ortho->projection.d1, 
                    ortho->projection.d2, 
                    ortho->projection.d3) * size);
                MathCore::vec3f center = -ortho->getTransform()->getPosition(true) + projection_offset;

                

                MathCore::vec3f right = rotation * MathCore::vec3f(size.x, 0, 0);
                MathCore::vec3f top = rotation * MathCore::vec3f(0, size.y, 0);
                MathCore::vec3f depth = rotation * MathCore::vec3f(0, 0, size.z);

                MathCore::vec3f vertices[8] = {
                    center - right - top - depth, // 000
                    center - right - top + depth, // 001
                    center - right + top - depth, // 010
                    center - right + top + depth, // 011
                    center + right - top - depth, // 100
                    center + right - top + depth, // 101
                    center + right + top - depth, // 110
                    center + right + top + depth  // 111
                };

                auto aabb = CollisionCore::AABB<MathCore::vec3f>(vertices[0], vertices[1]);
                for (int i = 2; i < 8; i++)
                {
                    aabb.min_box = MathCore::OP<MathCore::vec3f>::minimum(aabb.min_box, vertices[i]);
                    aabb.max_box = MathCore::OP<MathCore::vec3f>::maximum(aabb.max_box, vertices[i]);
                }

                // size = MathCore::OP<MathCore::quatf>::conjugate( rotation ) * size; // apply rotation to size

                // translation *= size * 2.0f;
                // OOBB basis not implemented yet...
                // camera->view;
                // CollisionCore::AABB<MathCore::vec3f> aabb =
                //     CollisionCore::AABB<MathCore::vec3f>(
                //         translation - size,
                //         translation + size);

                // printf("aabb\n");
                // printf("  near: %f\n", ortho->nearPlane.c_val());
                // printf("  far: %f\n", ortho->farPlane.c_val());
                // printf("  translation: %f %f %f\n", translation.x,translation.y,translation.z);
                // printf("  size: %f %f %f\n", size.x, size.y, size.z);
                // printf("  projection_offset: %f %f %f\n", projection_offset.x, projection_offset.y, projection_offset.z);

                // printf("  min: %f %f %f\n", aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);
                // printf("  max: %f %f %f\n", aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
                // printf("projection\n");
                // printf("  %f %f %f %f\n", ortho->projection.a1, ortho->projection.b1, ortho->projection.c1, ortho->projection.d1);
                // printf("  %f %f %f %f\n", ortho->projection.a2, ortho->projection.b2, ortho->projection.c2, ortho->projection.d2);
                // printf("  %f %f %f %f\n", ortho->projection.a3, ortho->projection.b3, ortho->projection.c3, ortho->projection.d3);
                // printf("  %f %f %f %f\n", ortho->projection.a4, ortho->projection.b4, ortho->projection.c4, ortho->projection.d4);

                // printf("  screen: %i %i\n", camera->viewport.w, camera->viewport.h);

                // filter only objects visible to camera...
                objectPlaces.filterObjectsAABB(root, aabb);

                // printf("Objects count %zu\n", objectPlaces.filteredMeshWrappers.size());
            }

            // draw all sphere suns
            if (perspective && clear && objectPlaces.sunLights.size() > 0)
            {

                AppKit::GLEngine::GLRenderState *renderstate = AppKit::GLEngine::GLRenderState::Instance();

                // AppKit::OpenGL::GLShader* oldShader = renderstate->CurrentShader;
                AppKit::GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
                bool oldDepthTestEnabled = renderstate->DepthWrite;
                AppKit::GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;

                renderstate->CurrentShader = &unlitShader;
                renderstate->DepthTest = AppKit::GLEngine::DepthTestDisabled;
                renderstate->DepthWrite = false;
                renderstate->BlendMode = AppKit::GLEngine::BlendModeAlpha;

                int posAttribLocation = unlitShader.queryAttribLocation("aPosition");

                // direct draw commands
                OPENGL_CMD(glEnableVertexAttribArray(posAttribLocation));
                OPENGL_CMD(glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &sunVertex[0]));

                auto perspCamera = std::dynamic_pointer_cast<Components::ComponentCameraPerspective>(camera);

                MathCore::mat4f projection = MathCore::GEN<MathCore::mat4f>::projection_perspective_lh_negative_one(
                    perspCamera->fovDegrees,                                         // fov
                    (float)perspCamera->viewport.w / (float)perspCamera->viewport.h, // aspect
                    1.0f,                                                            // near
                    1000.0f                                                          // far
                );

                MathCore::mat4f VP = projection * perspCamera->view;
                for (size_t j = 0; j < objectPlaces.sunLights.size(); j++)
                {
                    auto sunLight = objectPlaces.sunLights[j];

                    if (!sunLight->sun.render_after_skybox)
                        continue;
                    auto sunLight_transform = sunLight->getTransform();

                    MathCore::vec3f dir = sunLight_transform->getRotation(true) * MathCore::vec3f(0, 0, -1);
                    MathCore::vec3f sunPos = dir * sunLight->sun.distance;
                    MathCore::mat4f model =
                        MathCore::GEN<MathCore::mat4f>::translateHomogeneous(sunPos) *
                        MathCore::GEN<MathCore::mat4f>::scaleHomogeneous(sunLight->sun.radius, sunLight->sun.radius, sunLight->sun.radius);

                    unlitShader.setMVP(VP * model);
                    unlitShader.setColor(MathCore::CVT<MathCore::vec3f>::toPtn4(sunLight->sun.color));

                    OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)sunIndex.size(), GL_UNSIGNED_INT, &sunIndex[0]));
                }

                // OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

                OPENGL_CMD(glDisableVertexAttribArray(posAttribLocation));

                // renderstate->CurrentShader = oldShader;
                renderstate->DepthTest = oldDepthTest;
                renderstate->DepthWrite = oldDepthTestEnabled;
                renderstate->BlendMode = oldBlendMode;
            }

            for (int i = 0; i < objectPlaces.filteredMeshWrappers.size(); i++)
            {
                auto meshWrapper = objectPlaces.filteredMeshWrappers[i];

                if (perspective)
                {
                    lightAndShadowManager.computeShadowParametersForMesh(meshWrapper, shaderShadowAlgorithm != ShaderShadowAlgorithm_None, shaderShadowAlgorithm);
                }

                traverse_singlepass_render(meshWrapper->getTransform(), camera.get());
            }

            /*
            root->traversePreOrder_DepthFirst(
                                              EventCore::CallbackWrapper(this, &RenderPipeline::traverse_singlepass_render),
                                              camera);
            */

            // render debug lines
            if (objectPlaces.debugLines.size() > 0)
            {
                GLRenderState *state = GLRenderState::Instance();
                state->CurrentShader = &unlitShader;
                state->BlendMode = BlendModeDisabled; // material->pbr.blendMode;
                state->DepthTest = DepthTestDisabled;
                state->DepthWrite = false;

                unlitShader.setMVP(camera->viewProjection);

                /*
                MathCore::mat4f *mvp;
                MathCore::mat4f *mv;
                MathCore::mat4f *mvIT;
                MathCore::mat4f *mvInv;*/

                int attribPos = unlitShader.queryAttribLocation("aPosition");

                for (size_t i = 0; i < objectPlaces.debugLines.size(); i++)
                {

                    auto colorLine = objectPlaces.debugLines[i];

                    /*
                    debugLines[i]->transform[0]->computeRenderMatrix(camera->viewProjection,
                        camera->view,
                        camera->viewIT,
                        camera->viewInv,
                        &mvp,
                        &mv,
                        &mvIT,
                        &mvInv);
                        unlitShader.setMVP(*mvp);
                        */

                    // unlitShader.setMVP(*mvp);
                    unlitShader.setColor(colorLine->color);
                    state->LineWidth = colorLine->width;

                    colorLine->setLayoutPointers(attribPos);
                    colorLine->draw();
                    colorLine->unsetLayoutPointers(attribPos);
                }

                state->DepthWrite = true;
            }

            if (objectPlaces.sceneParticleSystem.size() > 0)
            {
                // render depth texture
                if (depthRenderer == nullptr)
                    depthRenderer = new DepthRenderer();
                depthRenderer->method1_copy_from_current_framebuffer();
                // depthRenderer->method2_render_just_depth( root, camera );

                auto camera_transform = camera->getTransform();

                // sort and render all particle system
                MathCore::vec3f cameraPosition = camera_transform->getPosition(true);
                MathCore::vec3f cameraDirection = camera_transform->getRotation(true) * MathCore::vec3f(0, 0, 1);

                for (int i = 0; i < objectPlaces.sceneParticleSystem.size(); i++)
                {
                    auto particleSystem = objectPlaces.sceneParticleSystem[i];
                    particleSystem->distance_to_camera = MathCore::OP<MathCore::vec3f>::sqrDistance(particleSystem->aabb_center, cameraPosition);
                }

                std::sort(objectPlaces.sceneParticleSystem.begin(), objectPlaces.sceneParticleSystem.end(), __compare__particle__system__reverse__);

                for (int i = 0; i < objectPlaces.sceneParticleSystem.size(); i++)
                {
                    auto particleSystem = objectPlaces.sceneParticleSystem[i];
                    particleSystem->sortPositions(cameraPosition, cameraDirection);
                }

                // printf("------------------PARTICLES SYSTEM DISTANCES------------------\n");
                // for(int i=0;i<sceneParticleSystem.size();i++)
                // printf("%f \n", sceneParticleSystem[i]->distance_to_camera);

                // draw particles...
                for (int i = 0; i < objectPlaces.sceneParticleSystem.size(); i++)
                {
                    auto particleSystem = objectPlaces.sceneParticleSystem[i];

                    if (particleSystem->soft)
                        particleSystemRenderer.drawSoftDepthComponent24(
                            camera, particleSystem,
                            &depthRenderer->depthTexture);
                    else
                        particleSystemRenderer.draw(camera, particleSystem);

                    // particleSystemRenderer.drawDebugPoints( camera, sceneParticleSystem[i], 0.25f );
                }
            }
        }

        bool RenderPipeline::traverse_depth_render(std::shared_ptr<Transform> element, void *userData)
        {

            Components::ComponentMaterial *material = nullptr;
            int start_index = 0;
            int end_index = 0;
            bool camera_set = false;

            Components::ComponentCamera *camera = (Components::ComponentCamera *)userData;

            MathCore::mat4f *mvp;
            MathCore::mat4f *mv;
            MathCore::mat4f *mvIT;
            MathCore::mat4f *mvInv;

            for (int i = 0; i < element->getComponentCount(); i++)
            {
                auto component = element->getComponentAt(i);
                if (component->compareType(Components::ComponentMaterial::Type))
                {
                    end_index = i;
                    if (material != nullptr && start_index != end_index)
                    {
                        // camera matrix computation
                        if (!camera_set)
                        {
                            camera_set = true;
                            element->computeRenderMatrix(camera->viewProjection,
                                                         camera->view,
                                                         camera->viewIT,
                                                         camera->viewInv,
                                                         &mvp,
                                                         &mv,
                                                         &mvIT,
                                                         &mvInv);

                            depthShader.setMVP(*mvp);
                        }
                        // setup material and render
                        materialSetupAndRender_depth(
                            material,
                            element.get(),
                            camera,
                            start_index, end_index,
                            mvp,
                            mv,
                            mvIT,
                            mvInv);
                    }
                    material = (Components::ComponentMaterial *)component.get();
                    start_index = i + 1;
                }
            }

            end_index = element->getComponentCount();
            if (material != nullptr && start_index != end_index)
            {
                // camera matrix computation
                if (!camera_set)
                {
                    camera_set = true;
                    element->computeRenderMatrix(camera->viewProjection,
                                                 camera->view,
                                                 camera->viewIT,
                                                 camera->viewInv,
                                                 &mvp,
                                                 &mv,
                                                 &mvIT,
                                                 &mvInv);

                    depthShader.setMVP(*mvp);
                }
                // setup material and render
                materialSetupAndRender_depth(
                    material,
                    element.get(),
                    camera,
                    start_index, end_index,
                    mvp,
                    mv,
                    mvIT,
                    mvInv);
            }

            return true;
        }

        void RenderPipeline::renderDepth(std::shared_ptr<Transform> root, std::shared_ptr<Components::ComponentCamera> camera)
        {

            GLRenderState *state = GLRenderState::Instance();

            state->ColorWrite = ColorWriteNone;
            state->DepthTest = DepthTestLess;
            state->BlendMode = BlendModeDisabled; // material->pbr.blendMode;

            root->traversePreOrder_DepthFirst(
                EventCore::CallbackWrapper(&RenderPipeline::traverse_depth_render, this),
                camera.get());
        }

        void RenderPipeline::materialSetupAndRender_depth(
            Components::ComponentMaterial *material,
            Transform *element,
            Components::ComponentCamera *camera,
            int start_index, int end_index,
            const MathCore::mat4f *mvp,
            const MathCore::mat4f *mv,
            const MathCore::mat4f *mvIT,
            const MathCore::mat4f *mvInv)
        {

            /*
            GLRenderState *state = GLRenderState::Instance();

            switch (material->type) {
            case Components::MaterialPBR:
                //mesh skinning upload matrix
                if (material->skin_shader_matrix_size_bitflag != 0) {
                    if (material->skin_gradient_matrix_dirty) {
                        material->skin_gradient_matrix_dirty = false;
                        frankenShader->sendSkinMatrix(material->skin_gradient_matrix);
                    }
                }
                break;
            default:
                break;
            }
            */

            allMeshRender_Range(element, &depthShader, start_index, end_index);
        }

    }

}
