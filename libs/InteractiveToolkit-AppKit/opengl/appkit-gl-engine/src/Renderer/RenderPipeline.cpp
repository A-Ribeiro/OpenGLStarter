#include <appkit-gl-engine/Renderer/RenderPipeline.h>

#include <appkit-gl-engine/util/ResourceHelper.h>

#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

#include <algorithm> // std::sort

namespace AppKit
{
    namespace GLEngine
    {

        void RenderPipeline::traverse_singlepass_render(Transform *element, Components::ComponentCamera *camera, ResourceMap *resourceMap)
        {
            if (element->skip_traversing)
                return;

            Components::ComponentMaterial *material = nullptr;
            Components::ComponentMesh *mesh = nullptr;
            // int start_index = 0;
            // int end_index = 0;
            bool camera_set = false;
            GLRenderState *state = GLRenderState::Instance();

            MathCore::mat4f *mvp;
            MathCore::mat4f *mv;
            MathCore::mat4f *mvIT;
            MathCore::mat4f *mvInv;

            DefaultEngineShader *shader_uniform_matrices_set = nullptr;

            for (auto &component : element->getComponents())
            {
                if (component->compareType(Components::ComponentMaterial::Type))
                {
                    material = (Components::ComponentMaterial *)component.get();
                    if (material->shader == nullptr)
                    {
                        material = nullptr;
                        // in case the object has no material, or a material without a shader, skip it completely
                        return;
                    }
                    setCurrentMaterial(material, resourceMap, camera);
                }
                else if (material != nullptr && component->compareType(Components::ComponentMesh::Type))
                {
                    mesh = (Components::ComponentMesh *)component.get();

                    if (!mesh->usesVBO() && mesh->indices.size() <= agregateMesh_ConcatenateLowerThanIndexCount)
                    {
                        // flush if more than the triangles limit
                        if (meshAgregator->indices.size() > agregateMesh_FlushMoreThanIndexCount)
                            renderMeshAgregatorAndClear(resourceMap, camera);

                        meshAgregatorMaterial = material;
                        meshAgregator->concatenate(element, mesh, material->shader.get());
                        setCurrentMesh(meshAgregator, resourceMap, camera);
                        continue;
                    }

                    setCurrentMesh(mesh, resourceMap, camera);
                    if (!camera_set)
                    {
                        camera_set = true;
                        element->computeRenderMatrix(camera->viewProjection, camera->view, camera->viewIT, camera->viewInv,
                                                     &mvp, &mv, &mvIT, &mvInv);
                    }

                    auto shader = material->shader.get();
                    if (shader_uniform_matrices_set != shader)
                    {
                        shader_uniform_matrices_set = shader;
                        shader->setUniformsFromMatrices(
                            state, resourceMap, this,
                            material, element, camera,
                            mvp, mv, mvIT, mvInv);
                    }

                    mesh->draw();

                    // break; // Only render the first mesh found in the element
                }
            }

            for (auto &child : element->getChildren())
                traverse_singlepass_render(child.get(), camera, resourceMap);
        }

        void RenderPipeline::traverse_depth_render_only_mesh(Transform *element, Components::ComponentCamera *camera, ResourceMap *resourceMap)
        {
            if (element->skip_traversing)
                return;

            Components::ComponentMaterial *material = nullptr;
            Components::ComponentMesh *mesh = nullptr;
            // int start_index = 0;
            // int end_index = 0;
            bool camera_set = false;
            GLRenderState *state = GLRenderState::Instance();

            MathCore::mat4f *mvp;
            MathCore::mat4f *mv;
            MathCore::mat4f *mvIT;
            MathCore::mat4f *mvInv;

            bool matrices_set = false;

            for (auto &component : element->getComponents())
            {
                if (component->compareType(Components::ComponentMaterial::Type))
                {
                    material = (Components::ComponentMaterial *)component.get();
                    if (material->shader == nullptr)
                    {
                        material = nullptr;
                        continue;
                    }
                    setCurrentMaterial(resourceMap->renderOnlyDepthMaterial.get(), resourceMap, camera);
                }
                else if (material != nullptr && component->compareType(Components::ComponentMesh::Type))
                {
                    mesh = (Components::ComponentMesh *)component.get();

                    if (!mesh->usesVBO() && mesh->indices.size() <= agregateMesh_ConcatenateLowerThanIndexCount)
                    {
                        // flush if more than the triangles limit
                        if (meshAgregator->indices.size() > agregateMesh_FlushMoreThanIndexCount)
                            renderMeshAgregatorAndClear(resourceMap, camera);

                        meshAgregatorMaterial = material;
                        meshAgregator->concatenate(element, mesh, material->shader.get());
                        setCurrentMesh(meshAgregator, resourceMap, camera);
                        continue;
                    }

                    setCurrentMesh(mesh, resourceMap, camera);
                    if (!camera_set)
                    {
                        camera_set = true;
                        element->computeRenderMatrix(camera->viewProjection, camera->view, camera->viewIT, camera->viewInv,
                                                     &mvp, &mv, &mvIT, &mvInv);
                    }

                    // using the depth material
                    if (!matrices_set)
                    {
                        matrices_set = true;
                        this->currentMaterial->shader->setUniformsFromMatrices(
                            state, resourceMap, this,
                            this->currentMaterial, element, camera,
                            mvp, mv, mvIT, mvInv);
                    }

                    mesh->draw();

                    // break; // Only render the first mesh found in the element
                }
            }

            for (auto &child : element->getChildren())
                traverse_depth_render_only_mesh(child.get(), camera, resourceMap);
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

            currentMaterial = nullptr;
            currentMesh = nullptr;
            meshAgregatorMaterial = nullptr;
            meshAgregator = new Components::ComponentMesh();
            dummyTransform = new Transform();
            dummyTransform->setName("__dummy__");

            agregateMesh_ConcatenateLowerThanTriangleCount = 1024;
            agregateMesh_FlushMoreThanTriangleCount = 65536; // 64K
        }

        RenderPipeline::~RenderPipeline()
        {
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
            if (meshAgregator != nullptr)
            {
                delete meshAgregator;
                meshAgregator = nullptr;
            }

            if (dummyTransform != nullptr) 
            {
                delete dummyTransform;
                dummyTransform = nullptr;
            }

            meshAgregatorMaterial = nullptr;
        }

        void RenderPipeline::renderMeshAgregatorAndClear(ResourceMap *resourceMap, Components::ComponentCamera *camera)
        {
            if (meshAgregator->indices.size() == 0)
                return;

            MathCore::mat4f *mvp;
            MathCore::mat4f *mv;
            MathCore::mat4f *mvIT;
            MathCore::mat4f *mvInv;

            dummyTransform->computeRenderMatrix(camera->viewProjection, camera->view, camera->viewIT, camera->viewInv,
                                                &mvp, &mv, &mvIT, &mvInv);
            auto shader = meshAgregatorMaterial->shader.get();
            shader->setUniformsFromMatrices(
                GLRenderState::Instance(), resourceMap, this,
                meshAgregatorMaterial, dummyTransform, camera,
                mvp, mv, mvIT, mvInv);

            // the meshAgregator is dynamic, and can change its size...
            // so we need to set it immediately before draw
            meshAgregator->setLayoutPointers(shader);
            meshAgregator->draw();
            meshAgregator->clear();
        }

        void RenderPipeline::setCurrentMaterial(Components::ComponentMaterial *material, ResourceMap *resourceMap, Components::ComponentCamera *camera)
        {
            if (currentMaterial == material)
                return;
            if (currentMesh == meshAgregator && currentMesh->indices.size() > 0)
                renderMeshAgregatorAndClear(resourceMap, camera);
            currentMaterial = material;
            if (currentMaterial != nullptr)
            {
                currentMaterial->shader->ActiveShader_And_SetUniformsFromMaterial(
                    GLRenderState::Instance(), resourceMap,
                    this,
                    currentMaterial);
            }
        }
        void RenderPipeline::setCurrentMesh(Components::ComponentMesh *mesh, ResourceMap *resourceMap, Components::ComponentCamera *camera)
        {
            if (currentMesh == mesh)
                return;
            if (currentMesh != nullptr && (mesh == nullptr || currentMaterial == nullptr ||
                                           // here mesh and currentMaterial are not null, so we can safely access them
                                           mesh->usesVBO() != currentMesh->usesVBO() ||
                                           // here usesVBO are equal, we check if they are not using VBOs
                                           (!mesh->usesVBO() && currentMaterial->shader.get() != shaderMeshLastSet)))
            {
                if (currentMesh == meshAgregator && currentMesh->indices.size() > 0)
                    renderMeshAgregatorAndClear(resourceMap, camera);
                currentMesh->unsetLayoutPointers(shaderMeshLastSet);
            }

            if (mesh != nullptr && currentMaterial != nullptr)
            {
                currentMesh = mesh;
                shaderMeshLastSet = currentMaterial->shader.get();

                // the meshAgregator is dynamic, and can change its size...
                // so we need to set it immediately before draw
                if (currentMesh != meshAgregator)
                    currentMesh->setLayoutPointers(shaderMeshLastSet);
            }
            else
            {
                currentMesh = nullptr;
                shaderMeshLastSet = nullptr;
            }
        }

        // bool __compare__particle__system__reverse__(const Components::ComponentParticleSystem *a, const Components::ComponentParticleSystem *b)
        // {
        //     return (b->distance_to_camera < a->distance_to_camera);
        // }

        void RenderPipeline::runSinglePassPipeline(
            ResourceMap *resourceMap,
            std::shared_ptr<Transform> rootp,
            std::shared_ptr<Components::ComponentCamera> camerap,
            bool clear,
            OrthographicFilterEnum orthoFilter)
        {
            agregateMesh_ConcatenateLowerThanIndexCount = agregateMesh_ConcatenateLowerThanTriangleCount * 3;
            agregateMesh_FlushMoreThanIndexCount = agregateMesh_FlushMoreThanTriangleCount * 3;

            Transform *root = rootp.get();
            Components::ComponentCamera *camera = camerap.get();

            AppKit::GLEngine::GLRenderState *renderstate = AppKit::GLEngine::GLRenderState::Instance();

            if (clear)
            {
                if (cubeSkyBox != nullptr)
                {
                    glClear(GL_DEPTH_BUFFER_BIT);
                    cubeSkyBox->draw(camera->view, camera->projection);
                    // cubeSkyBox->drawAnotherCube(camera->view, camera->projection, cubeAmbientLight_1x1);
                }
                else
                    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            }

            // light setup
            if ((ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor ||
                 ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture) &&
                cubeSkyBox != nullptr)
            {
                ambientLightColorVec3 = cubeSkyBox->cubeMap.colorEstimation;
                ambientLightColorVec3 = ResourceHelper::vec3ColorGammaToLinear(ambientLightColorVec3);
            }
            else
                ambientLightColorVec3 = ambientLight.color;

            bool perspective = false;
            if (camera->compareType(Components::ComponentCameraPerspective::Type))
            {

                perspective = true;

                CollisionCore::Frustum<MathCore::vec3f> frustum = CollisionCore::Frustum<MathCore::vec3f>(camera->projection, camera->view);
                // filter only objects visible to camera...
                sceneTraverseHelper.filterByFrustum(root, frustum, FilterFlags_All);
                // printf("Visible objects: %i\n", (int)objectPlaces.filteredMeshWrappers.size());

                lightAndShadowManager.setSceneAABB(sceneTraverseHelper.scene_aabb);
                lightAndShadowManager.setMainCameraFrustum(frustum);
                lightAndShadowManager.setSceneRoot(root);
                lightAndShadowManager.setVisibleObjectsFromSceneTraverse(&sceneTraverseHelper);
                lightAndShadowManager.setRenderPipeline(this);
            }
            else if (camera->compareType(Components::ComponentCameraOrthographic::Type))
            {
                auto ortho = (Components::ComponentCameraOrthographic *)camera;

                MathCore::vec3f size = 1.0f / MathCore::vec3f(ortho->projection.a1, ortho->projection.b2, ortho->projection.c3);
                MathCore::quatf rotation = ortho->getTransform()->getRotation(true);
                auto projection_offset =
                    rotation * (MathCore::vec3f(
                                    ortho->projection.d1,
                                    ortho->projection.d2,
                                    ortho->projection.d3) *
                                size);
                MathCore::vec3f center = ortho->getTransform()->getPosition(true) - projection_offset;

                if (orthoFilter == OrthographicFilter_UsingAABB)
                {
                    // filtering using aabb
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

                    sceneTraverseHelper.filterByAABB(root, aabb, FilterFlags_All);
                }
                else if (orthoFilter == OrthographicFilter_UsingOBB)
                {
                    // filtering using obb
                    auto obb = CollisionCore::OBB<MathCore::vec3f>(center, size * 2.0f, rotation);
                    sceneTraverseHelper.filterByOBB(root, obb, FilterFlags_All);
                }

                auto z_axis_lh = rotation * MathCore::vec3f(0, 0, 1);

                if (MathCore::OP<MathCore::vec3f>::angleBetween(z_axis_lh, MathCore::vec3f(0, 0, 1)) < MathCore::OP<float>::deg_2_rad(5.0f))
                    sortingHelper.sort_by_z(sceneTraverseHelper.transformList, SortingMode_Desc, true);
                else
                    sortingHelper.sort_by_direction(sceneTraverseHelper.transformList, z_axis_lh, SortingMode_Desc, true);
            }
            else
            {
                ITK_ABORT(true, "Needs at least one camera in the scene to render.");
            }

            for (auto &sunLight : sceneTraverseHelper.sunLightList)
                sunLight->postProcessing_computeLightParameters();

            // // draw all sphere suns
            // if (perspective && clear && sceneTraverseHelper.sunLightList.size() > 0)
            // {

            //     // AppKit::GLEngine::GLRenderState *renderstate = AppKit::GLEngine::GLRenderState::Instance();

            //     // AppKit::OpenGL::GLShader* oldShader = renderstate->CurrentShader;
            //     AppKit::GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
            //     bool oldDepthTestEnabled = renderstate->DepthWrite;
            //     AppKit::GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;

            //     renderstate->CurrentShader = &unlitShader;
            //     renderstate->DepthTest = AppKit::GLEngine::DepthTestDisabled;
            //     renderstate->DepthWrite = false;
            //     renderstate->BlendMode = AppKit::GLEngine::BlendModeAlpha;

            //     int posAttribLocation = unlitShader.queryAttribLocation("aPosition");

            //     // direct draw commands
            //     OPENGL_CMD(glEnableVertexAttribArray(posAttribLocation));
            //     OPENGL_CMD(glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &sunVertex[0]));

            //     auto perspCamera = (Components::ComponentCameraPerspective *)camera;

            //     MathCore::mat4f projection = MathCore::GEN<MathCore::mat4f>::projection_perspective_lh_negative_one(
            //         perspCamera->fovDegrees,                                         // fov
            //         (float)perspCamera->viewport.w / (float)perspCamera->viewport.h, // aspect
            //         1.0f,                                                            // near
            //         1000.0f                                                          // far
            //     );

            //     MathCore::mat4f VP = projection * perspCamera->view;
            //     for (size_t j = 0; j < sceneTraverseHelper.sunLightList.size(); j++)
            //     {
            //         auto sunLight = sceneTraverseHelper.sunLightList[j];

            //         if (!sunLight->sun.render_after_skybox)
            //             continue;
            //         auto sunLight_transform = sunLight->getTransform();

            //         MathCore::vec3f dir = sunLight_transform->getRotation(true) * MathCore::vec3f(0, 0, -1);
            //         MathCore::vec3f sunPos = dir * sunLight->sun.distance;
            //         MathCore::mat4f model =
            //             MathCore::GEN<MathCore::mat4f>::translateHomogeneous(sunPos) *
            //             MathCore::GEN<MathCore::mat4f>::scaleHomogeneous(sunLight->sun.radius, sunLight->sun.radius, sunLight->sun.radius);

            //         unlitShader.setMVP(VP * model);
            //         unlitShader.setColor(MathCore::CVT<MathCore::vec3f>::toPtn4(sunLight->sun.color));

            //         OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)sunIndex.size(), GL_UNSIGNED_INT, &sunIndex[0]));
            //     }

            //     // OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

            //     OPENGL_CMD(glDisableVertexAttribArray(posAttribLocation));

            //     // renderstate->CurrentShader = oldShader;
            //     renderstate->DepthTest = oldDepthTest;
            //     renderstate->DepthWrite = oldDepthTestEnabled;
            //     renderstate->BlendMode = oldBlendMode;
            // }

            if (perspective)
            {
                // setCurrentMaterial(resourceMap->renderOnlyDepthMaterial.get());
                lightAndShadowManager.computeShadowParametersForMesh(nullptr, shaderShadowAlgorithm != ShaderShadowAlgorithm_None, shaderShadowAlgorithm, resourceMap);
            }

            for (auto &transform : sceneTraverseHelper.transformList)
            {
                traverse_singlepass_render(transform, camera, resourceMap);
            }

            setCurrentMesh(nullptr, resourceMap, camera);
            setCurrentMaterial(nullptr, resourceMap, camera);
            renderstate->clearTextureUnitActivationArray();

            // render debug lines
            // if (objectPlaces.debugLines.size() > 0)
            // {
            //     GLRenderState *state = GLRenderState::Instance();
            //     state->CurrentShader = &unlitShader;
            //     state->BlendMode = BlendModeDisabled; // material->pbr.blendMode;
            //     state->DepthTest = DepthTestDisabled;
            //     state->DepthWrite = false;

            //     unlitShader.setMVP(camera->viewProjection);

            //     /*
            //     MathCore::mat4f *mvp;
            //     MathCore::mat4f *mv;
            //     MathCore::mat4f *mvIT;
            //     MathCore::mat4f *mvInv;*/

            //     int attribPos = unlitShader.queryAttribLocation("aPosition");

            //     for (size_t i = 0; i < objectPlaces.debugLines.size(); i++)
            //     {

            //         auto colorLine = objectPlaces.debugLines[i];

            //         /*
            //         debugLines[i]->transform[0]->computeRenderMatrix(camera->viewProjection,
            //             camera->view,
            //             camera->viewIT,
            //             camera->viewInv,
            //             &mvp,
            //             &mv,
            //             &mvIT,
            //             &mvInv);
            //             unlitShader.setMVP(*mvp);
            //             */

            //         // unlitShader.setMVP(*mvp);
            //         unlitShader.setColor(colorLine->color);
            //         state->LineWidth = colorLine->width;

            //         colorLine->setLayoutPointers(attribPos);
            //         colorLine->draw();
            //         colorLine->unsetLayoutPointers(attribPos);
            //     }

            //     state->DepthWrite = true;
            // }

            if (sceneTraverseHelper.particleSystemList.size() > 0)
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

                sortingHelper.sort_by_direction(sceneTraverseHelper.particleSystemList, cameraDirection, SortingMode_Desc);

                // draw particles...
                for (int i = 0; i < sceneTraverseHelper.particleSystemList.size(); i++)
                {
                    auto particleSystem = sceneTraverseHelper.particleSystemList[i];

                    if (particleSystem->soft)
                        particleSystemRenderer.drawSoftDepthComponent24(
                            camera, cameraDirection, particleSystem,
                            &depthRenderer->depthTexture, &sortingHelper);
                    else
                        particleSystemRenderer.draw(camera, cameraDirection, particleSystem, &sortingHelper);

                    // particleSystemRenderer.drawDebugPoints( camera, sceneParticleSystem[i], 0.25f );
                }
            }
        }

    }

}
