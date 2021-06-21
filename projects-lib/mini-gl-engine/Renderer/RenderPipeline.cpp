#include "RenderPipeline.h"

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <data-model/data-model.h>
#include <mini-gl-engine/mini-gl-engine.h>

#include <algorithm> // std::sort

namespace GLEngine {


    bool RenderPipeline::traverse_search_elements(Transform *element, void* userData) {
        Components::ComponentLight* light = (Components::ComponentLight*)element->findComponent(Components::ComponentLight::Type);
        if (light != NULL && light->type == Components::LightSun)
            sceneSunLights.push_back(light);

        Components::ComponentParticleSystem* particleSystem = (Components::ComponentParticleSystem*)element->findComponent(Components::ComponentParticleSystem::Type);
        if (particleSystem != NULL)
            sceneParticleSystem.push_back(particleSystem);
        
        return true;
    }

    void RenderPipeline::SearchSpecialObjects(Transform *root) {
        sceneSunLights.clear();
        sceneParticleSystem.clear();
        root->traversePreOrder_DepthFirst(
            TransformTraverseMethod(this, &RenderPipeline::traverse_search_elements),
            NULL);
    }
    
    void RenderPipeline::allMeshRender(Transform *element, const DefaultEngineShader *shader)const {
        for (int i = 0; i < element->getComponentCount(); i++) {
            Component *component = element->getComponentAt(i);
            if (component->compareType(Components::ComponentMesh::Type)) {
                Components::ComponentMesh *mesh = (Components::ComponentMesh*)component;
                mesh->setLayoutPointers(shader);
                mesh->draw();
                mesh->unsetLayoutPointers(shader);
            }
        }
    }

    void RenderPipeline::allMeshRender_Range(Transform *element, const DefaultEngineShader *shader, int start_index, int end_index)const {
        for (int i = start_index; i < end_index; i++) {
            Component *component = element->getComponentAt(i);
            if (component->compareType(Components::ComponentMesh::Type)) {
                Components::ComponentMesh *mesh = (Components::ComponentMesh*)component;
                mesh->setLayoutPointers(shader);
                mesh->draw();
                mesh->unsetLayoutPointers(shader);
            }
        }
    }

    void RenderPipeline::materialSetupAndRender(
        Components::ComponentMaterial* material, 
        Transform *element, 
        Components::ComponentCamera *camera,
        int start_index, int end_index,
        const aRibeiro::mat4 *mvp,
        const aRibeiro::mat4 *mv,
        const aRibeiro::mat4 *mvIT,
        const aRibeiro::mat4 *mvInv) {

        GLRenderState *state = GLRenderState::Instance();
        /*
        Components::ComponentCamera *camera = (Components::ComponentCamera*)userData;

        aRibeiro::mat4 *mvp;
        aRibeiro::mat4 *mv;
        aRibeiro::mat4 *mvIT;
        aRibeiro::mat4 *mvInv;
        element->computeRenderMatrix(camera->viewProjection,
            camera->view,
            camera->viewIT,
            camera->viewInv,
            &mvp,
            &mv,
            &mvIT,
            &mvInv);
        */

        uint32_t frankenFormat;
        FrankenShader *frankenShader;
        bool use_sRGBConvertion = !GLEngine::Engine::Instance()->sRGBCapable;
        int sunCount;
        switch (material->type) {
        case Components::MaterialUnlit:
            state->BlendMode = material->unlit.blendMode;
            state->CurrentShader = &unlitShader;

            unlitShader.setColor(material->unlit.color);
            unlitShader.setMVP(*mvp);

            allMeshRender_Range(element, &unlitShader, start_index, end_index);
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

            //PBR setup
        case Components::MaterialPBR:

            state->BlendMode = BlendModeDisabled;//material->pbr.blendMode;
            state->DepthTest = DepthTestLess;

            sunCount = 0;

            frankenFormat = 0;

            if (material->pbr.texAlbedo != NULL)
                frankenFormat |= ShaderAlgorithms_TextureAlbedo;
            if (material->pbr.texSpecular != NULL)
                frankenFormat |= ShaderAlgorithms_TextureSpecular;
            if (material->pbr.texNormal != NULL)
                frankenFormat |= ShaderAlgorithms_NormalMap;

            if (cubeAmbientLight_1x1 != NULL && ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture)
                frankenFormat |= ShaderAlgorithms_AmbientLightSkybox;
            else if (ambientLight.lightMode != AmbientLightMode_None)
                frankenFormat |= ShaderAlgorithms_AmbientLightColor;

            sunCount = sceneSunLights.size();

            if (use_sRGBConvertion)
                frankenFormat |= ShaderAlgorithms_sRGB;

            //mesh skinning bit flag
            frankenFormat |= material->skin_shader_matrix_size_bitflag;

            frankenFormat = FrankenShader::MountFormat(frankenFormat, sunCount);


            frankenFormat &= ShaderAlgorithmsEnum_allowedFlags;

            frankenShader = frankenShaderManager.getShader(frankenFormat);
            state->CurrentShader = frankenShader;

            if (material->pbr.texAlbedo != NULL)
                material->pbr.texAlbedo->active(0);
            if (material->pbr.texSpecular != NULL)
                material->pbr.texSpecular->active(1);
            if (material->pbr.texNormal != NULL)
                material->pbr.texNormal->active(2);
            if (cubeAmbientLight_1x1 != NULL)
                cubeAmbientLight_1x1->active(3);

            frankenShader->sendParamaters(mvp, element, camera, material,
                ambientLightColorVec3, 0, 1, 2, 3);

            for (size_t i = 0; i < sceneSunLights.size(); i++)
            {
                Components::ComponentLight *sunLight = sceneSunLights[i];
                frankenShader->sendSunLightConfig(i, sunLight->sun.finalIntensity, sunLight->sun.worldDirection);
            }

            //mesh skinning upload matrix
            if (material->skin_shader_matrix_size_bitflag != 0) {
                if (material->skin_gradient_matrix_dirty) {
                    material->skin_gradient_matrix_dirty = false;
                    frankenShader->sendSkinMatrix(material->skin_gradient_matrix);
                }
            }


            allMeshRender_Range(element, frankenShader, start_index, end_index);

            if (cubeAmbientLight_1x1 != NULL)
                cubeAmbientLight_1x1->deactive(3);
            if (material->pbr.texNormal != NULL)
                material->pbr.texNormal->deactive(2);
            if (material->pbr.texSpecular != NULL)
                material->pbr.texSpecular->deactive(1);
            if (material->pbr.texAlbedo != NULL)
                material->pbr.texAlbedo->deactive(0);

            break;
        default:
            break;
        }
    }
    
    
    bool RenderPipeline::traverse_multipass_render(Transform *element, void* userData) {
        Components::ComponentMaterial* material = (Components::ComponentMaterial*)element->findComponent(Components::ComponentMaterial::Type);
        if (material == NULL)
            return true;
        
        GLRenderState *state = GLRenderState::Instance();
        Components::ComponentCamera *camera = (Components::ComponentCamera*)userData;
        
        aRibeiro::mat4 *mvp;
        aRibeiro::mat4 *mv;
        aRibeiro::mat4 *mvIT;
        aRibeiro::mat4 *mvInv;
        element->computeRenderMatrix(camera->viewProjection,
                                     camera->view,
                                     camera->viewIT,
                                     camera->viewInv,
                                     &mvp,
                                     &mv,
                                     &mvIT,
                                     &mvInv);
        
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
                
                material->pbr.texAlbedo->active(0);
                material->pbr.texNormal->active(1);
                
                
                //1st pass: ambient light
                if (ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor ||
                    ambientLight.lightMode == AmbientLightMode_Color) {
                    state->CurrentShader = &unlit_tex_Shader;
                    
                    unlit_tex_Shader.setColor(material->pbr.albedoColor * ambientLightColor);
                    unlit_tex_Shader.setTexture(0);
                    unlit_tex_Shader.setMVP(*mvp);
                    
                    allMeshRender(element, &unlit_tex_Shader);
                }
                else if (ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture) {
                    
                    cubeAmbientLight_1x1->active(2);
                    
                    state->CurrentShader = &ambientLight_tex_cube_PassShader;
                    
                    ambientLight_tex_cube_PassShader.setColor(material->pbr.albedoColor);
                    
                    //texture setup...
                    ambientLight_tex_cube_PassShader.setTexture(0);
                    ambientLight_tex_cube_PassShader.setTextureNormal(1);
                    ambientLight_tex_cube_PassShader.setTextureCubeEnvironment(2);
                    
                    ambientLight_tex_cube_PassShader.setMVP(*mvp);
                    ambientLight_tex_cube_PassShader.setLocalToWorld(element->getMatrix(true));
                    ambientLight_tex_cube_PassShader.setLocalToWorld_it(element->getMatrixInverseTranspose(true));
                    
                    allMeshRender(element, &ambientLight_tex_cube_PassShader);
                    
                    cubeAmbientLight_1x1->deactive(2);
                }
                
                //2nd pass: sun light
                state->BlendMode = BlendModeAdd;//material->pbr.blendMode;
                state->DepthWrite = false;
                state->CurrentShader = &directionalLightPassShader;
                
                directionalLightPassShader.setTexture(0);
                directionalLightPassShader.setTextureNormal(1);
                
                directionalLightPassShader.setMVP(*mvp);
                directionalLightPassShader.setLocalToWorld(element->getMatrix(true));
                directionalLightPassShader.setLocalToWorld_it(element->getMatrixInverseTranspose(true));
                
                directionalLightPassShader.setMaterialAlbedoColor(material->pbr.albedoColor);
                directionalLightPassShader.setMaterialRoughness(material->pbr.roughness);
                directionalLightPassShader.setMaterialMetallic(material->pbr.metallic);
                
                directionalLightPassShader.setCameraPosWorld( camera->transform[0]->getPosition(true) );
                
                for (size_t i=0;i<sceneSunLights.size();i++)
                {
                    Components::ComponentLight *sunLight = sceneSunLights[i];
                    
                    directionalLightPassShader.setLightDirWorld(sunLight->sun.worldDirection);
                    directionalLightPassShader.setLightRadiance(sunLight->sun.finalIntensity);
                    
                    allMeshRender(element, &directionalLightPassShader);
                    
                }
                
                material->pbr.texNormal->deactive(1);
                material->pbr.texAlbedo->deactive(0);
                
                state->DepthWrite = true;
                
                break;
            default:
                break;
        }
        
        return true;
    }
    
    
    
    bool RenderPipeline::traverse_singlepass_render(Transform *element, void* userData) {

        Components::ComponentMaterial* material = NULL;
        int start_index = 0;
        int end_index = 0;
        bool camera_set = false;

        Components::ComponentCamera *camera = (Components::ComponentCamera*)userData;

        aRibeiro::mat4 *mvp;
        aRibeiro::mat4 *mv;
        aRibeiro::mat4 *mvIT;
        aRibeiro::mat4 *mvInv;

        for (int i = 0; i < element->getComponentCount(); i++) {
            Component* component = element->getComponentAt(i);
            if (component->compareType(Components::ComponentMaterial::Type)) {
                end_index = i;
                if (material != NULL && start_index != end_index) {
                    // camera matrix computation
                    if (!camera_set) {
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
                    //setup material and render
                    materialSetupAndRender(
                        material,
                        element,
                        camera,
                        start_index, end_index,
                        mvp,
                        mv,
                        mvIT,
                        mvInv);
                }
                material = (Components::ComponentMaterial*)component;
                start_index = i + 1;
            }
        }

        end_index = element->getComponentCount();
        if (material != NULL && start_index != end_index) {
            // camera matrix computation
            if (!camera_set) {
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
            //setup material and render
            materialSetupAndRender(
                material,
                element,
                camera,
                start_index, end_index,
                mvp,
                mv,
                mvIT,
                mvInv);
        }

        /*

        Components::ComponentMaterial* material = (Components::ComponentMaterial*)element->findComponent(Components::ComponentMaterial::Type);
        if (material == NULL)
            return true;
        
        GLRenderState *state = GLRenderState::Instance();
        Components::ComponentCamera *camera = (Components::ComponentCamera*)userData;
        
        aRibeiro::mat4 *mvp;
        aRibeiro::mat4 *mv;
        aRibeiro::mat4 *mvIT;
        aRibeiro::mat4 *mvInv;
        element->computeRenderMatrix(camera->viewProjection,
                                     camera->view,
                                     camera->viewIT,
                                     camera->viewInv,
                                     &mvp,
                                     &mv,
                                     &mvIT,
                                     &mvInv);
        
        uint32_t frankenFormat;
        FrankenShader *frankenShader;
        bool use_sRGBConvertion = !GLEngine::Engine::Instance()->sRGBCapable;
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
                
                if (material->pbr.texAlbedo != NULL)
                    frankenFormat |= ShaderAlgorithms_TextureAlbedo;
                if (material->pbr.texNormal != NULL)
                    frankenFormat |= ShaderAlgorithms_NormalMap;


                if ( cubeAmbientLight_1x1 != NULL && ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture)
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
                
                if (material->pbr.texAlbedo != NULL)
                    material->pbr.texAlbedo->active(0);
                if (material->pbr.texNormal != NULL)
                    material->pbr.texNormal->active(1);
                if (cubeAmbientLight_1x1 != NULL)
                    cubeAmbientLight_1x1->active(2);
                
                frankenShader->sendParamaters(mvp, element, camera, material,
                                              ambientLightColorVec3, 0, 1, 2);
                
                for (size_t i=0;i<sceneSunLights.size();i++)
                {
                    Components::ComponentLight *sunLight = sceneSunLights[i];
                    frankenShader->sendSunLightConfig(i,sunLight->sun.finalIntensity,sunLight->sun.worldDirection);
                }
                
                allMeshRender(element, frankenShader);
                
                if (cubeAmbientLight_1x1 != NULL)
                    cubeAmbientLight_1x1->deactive(2);
                if (material->pbr.texNormal != NULL)
                    material->pbr.texNormal->deactive(1);
                if (material->pbr.texAlbedo != NULL)
                    material->pbr.texAlbedo->deactive(0);
                
                break;
            default:
                break;
        }

        */
        
        return true;
    }
    
    RenderPipeline::RenderPipeline() {
        cubeSkyBox = NULL;
        cubeAmbientLight_1x1 = NULL;
        depthRenderer = NULL;

        ShaderAlgorithmsEnum_allowedFlags = 0xffffffff;
    }

    RenderPipeline::~RenderPipeline() {
        //ReferenceCounter<void*> *refCounter = &Engine::Instance()->referenceCounter;
        //refCounter->remove(cubeSkyBox);
        //cubeSkyBox = NULL;
        aRibeiro::setNullAndDelete(cubeSkyBox);
        aRibeiro::setNullAndDelete(cubeAmbientLight_1x1);
        aRibeiro::setNullAndDelete(depthRenderer);
    }

    void RenderPipeline::runMultiPassPipeline(Transform*root, Components::ComponentCamera *camera, bool clear) {


        ARIBEIRO_ABORT(!GLEngine::Engine::Instance()->sRGBCapable, "Error: Cannot run this pipeline on non-sRGB capable device.\n");

        root->resetVisited();
        root->preComputeTransforms();

        camera->precomputeViewProjection(true);

        if (clear) {
            if (cubeSkyBox == NULL) {
                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            }
            else {
                glClear(GL_DEPTH_BUFFER_BIT);
                cubeSkyBox->draw(camera->view, camera->projection);
                //cubeSkyBox->drawAnotherCube(camera->view, camera->projection, cubeAmbientLight_1x1);
            }
        }
        //light setup
        if (
            (ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor
            ||
            ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture )
            &&
            cubeSkyBox != NULL)
            ambientLightColor = aRibeiro::vec4(cubeSkyBox->cubeMap.colorEstimation,1.0f);
        else
            ambientLightColor = aRibeiro::vec4(ambientLight.color, 1.0f);
        
        ambientLightColorVec3 = aRibeiro::toVec3(ambientLightColor);
        
        SearchSpecialObjects(root);
        
        for(size_t i=0;i<sceneSunLights.size();i++)
            sceneSunLights[i]->postProcessing_computeLightParameters();
        
        
        root->traversePreOrder_DepthFirst(
              TransformTraverseMethod(this, &RenderPipeline::traverse_multipass_render),
              camera);
    }

    bool __compare__particle__system__reverse__( const Components::ComponentParticleSystem* a, const Components::ComponentParticleSystem* b ){
        return (a->distance_to_camera > b->distance_to_camera);
    }
    
    void RenderPipeline::runSinglePassPipeline(Transform*root, Components::ComponentCamera *camera, bool clear) {
        
        //bool srgb = GLEngine::Engine::Instance()->sRGBCapable;
        //ARIBEIRO_ABORT(!GLEngine::Engine::Instance()->sRGBCapable, "Error: Cannot run this pipeline on non-sRGB capable device.\n");
        
        /*
        root->resetVisited();
        root->preComputeTransforms();
        
        camera->precomputeViewProjection(true);
        */
        
        if (clear) {
            if (cubeSkyBox == NULL) {
                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            }
            else {
                glClear(GL_DEPTH_BUFFER_BIT);
                cubeSkyBox->draw(camera->view, camera->projection);
                //cubeSkyBox->drawAnotherCube(camera->view, camera->projection, cubeAmbientLight_1x1);
            }
        }
        //light setup
        if ((ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor
             ||
             ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture ) &&
            cubeSkyBox != NULL)
            ambientLightColor = aRibeiro::vec4(cubeSkyBox->cubeMap.colorEstimation,1.0f);
        else
            ambientLightColor = aRibeiro::vec4(ambientLight.color, 1.0f);
        
        ambientLightColorVec3 = aRibeiro::toVec3(ambientLightColor);
        
        SearchSpecialObjects(root);
        
        for(size_t i=0;i<sceneSunLights.size();i++)
            sceneSunLights[i]->postProcessing_computeLightParameters();
        
        
        root->traversePreOrder_DepthFirst(
                                          TransformTraverseMethod(this, &RenderPipeline::traverse_singlepass_render),
                                          camera);

        if (sceneParticleSystem.size() > 0) {
            //render depth texture
            if (depthRenderer == NULL)
                depthRenderer = new DepthRenderer();
            depthRenderer->method1_copy_from_current_framebuffer();
            //depthRenderer->method2_render_just_depth( root, camera );

            //sort and render all particle system
            aRibeiro::vec3 cameraPosition = camera->transform[0]->getPosition(true);
            aRibeiro::vec3 cameraDirection = camera->transform[0]->getRotation(true) * aRibeiro::vec3(0,0,1);

            for(int i=0;i<sceneParticleSystem.size();i++)
                sceneParticleSystem[i]->distance_to_camera = aRibeiro::sqrDistance(sceneParticleSystem[i]->aabb_center,cameraPosition);

            std::sort(sceneParticleSystem.begin(),sceneParticleSystem.end(),__compare__particle__system__reverse__);
            
            for(int i=0;i<sceneParticleSystem.size();i++)
                sceneParticleSystem[i]->sortPositions(cameraPosition, cameraDirection);
            
            //printf("------------------PARTICLES SYSTEM DISTANCES------------------\n");
            //for(int i=0;i<sceneParticleSystem.size();i++)
                //printf("%f \n", sceneParticleSystem[i]->distance_to_camera);

            //draw particles...
            for(int i=0;i<sceneParticleSystem.size();i++) {

                if (sceneParticleSystem[i]->soft)
                    particleSystemRenderer.drawSoftDepthComponent24( 
                        camera, sceneParticleSystem[i],
                        &depthRenderer->depthTexture
                    ); 
                else
                    particleSystemRenderer.draw( camera, sceneParticleSystem[i] );

                //particleSystemRenderer.drawDebugPoints( camera, sceneParticleSystem[i], 0.25f );
            }
        }

    }

}

