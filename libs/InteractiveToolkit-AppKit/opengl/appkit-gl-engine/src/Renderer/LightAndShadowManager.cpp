#include <appkit-gl-engine/Renderer/LightAndShadowManager.h>

#include <appkit-gl-engine/Renderer/RenderPipeline.h>


namespace AppKit
{
    namespace GLEngine
    {

        ComponentType ShadowCache::Type = "ShadowCache";

        ShadowCache::ShadowCache() : Components::ComponentCamera(ShadowCache::Type),
                                     frustum(MathCore::CONSTANT<MathCore::mat4f>::Identity())
        {
        }

        ShadowCache::~ShadowCache()
        {
        }

        void ShadowCache::setFromLight(
            std::shared_ptr<Components::ComponentLight> _light,
            const CollisionCore::Sphere<MathCore::vec3f> &scene_sphere,
            const CollisionCore::Sphere<MathCore::vec3f> &camera_sphere)
        {

            light = _light;

            switch (_light->type)
            {
            case Components::LightSun:
            {
                // sun light case
                shadowCacheShape = ShadowCacheOBB;

                auto light_transform = light->getTransform();

                MathCore::quatf sunDirection = light_transform->getRotation(true);
                MathCore::vec3f front = sunDirection * MathCore::vec3f(0, 0, 1);

                float scene_center_projected = MathCore::OP<MathCore::vec3f>::dot(scene_sphere.center, front);
                float camera_center_projected = MathCore::OP<MathCore::vec3f>::dot(camera_sphere.center, front);

                float camera_to_scene_offset = scene_center_projected - camera_center_projected;

                MathCore::vec3f center = camera_sphere.center + camera_to_scene_offset * front;
                MathCore::vec3f dimension;
                dimension.x = dimension.y = camera_sphere.radius * 2.0f;
                dimension.z = scene_sphere.radius * 2.0f;

                // obb to filter the scene...
                obb = CollisionCore::OBB<MathCore::vec3f>(
                    center,
                    dimension,
                    sunDirection);

                // mvp to use inside the shader...
                projection = MathCore::GEN<MathCore::mat4f>::projection_ortho_lh_negative_one(
                    -obb.dimension_2.x, obb.dimension_2.x,
                    -obb.dimension_2.y, obb.dimension_2.y,
                    -obb.dimension_2.z, obb.dimension_2.z);
                // the view is the inverse of the desired transform...
                view = MathCore::GEN<MathCore::mat4f>::fromQuat(MathCore::OP<MathCore::quatf>::inverse(sunDirection)) * MathCore::GEN<MathCore::mat4f>::translateHomogeneous(-center);
                viewInv = MathCore::GEN<MathCore::mat4f>::translateHomogeneous(center) * MathCore::GEN<MathCore::mat4f>::fromQuat(sunDirection);
                viewIT = MathCore::OP<MathCore::mat4f>::transpose(viewInv);

                viewProjection = projection * view;
                // this bias matrix is used to convert the depth values to the [0,1] range
                // in order to be able to store them in a texture and sample it later in the shader
                const MathCore::mat4f biasMatrix(
                    0.5f, 0.0f, 0.0f, 0.5f,
                    0.0f, 0.5f, 0.0f, 0.5f,
                    0.0f, 0.0f, 0.5f, 0.5f,
                    0.0f, 0.0f, 0.0f, 1.0f);
                castFromTextureViewProjection = biasMatrix * viewProjection;
            }
            break;
            default:
                break;
            }
        }

        LightAndShadowManager::LightAndShadowManager() : camera_frustum(MathCore::CONSTANT<MathCore::mat4f>::Identity())
        {
            pointShadowResolution = 512;
            spotShadowResolution = 512;
            sunShadowResolution = 2048;

            dummyColorTextureSun.setSize(sunShadowResolution, sunShadowResolution, GL_RGB);
            dynamicFBOSun.setSize(sunShadowResolution, sunShadowResolution);

            dynamicFBOSun.enable();
            dynamicFBOSun.setColorAttachment(&dummyColorTextureSun, 0);
            dynamicFBOSun.disable();
        }

        void LightAndShadowManager::setSceneAABB(const CollisionCore::AABB<MathCore::vec3f> &_aabb)
        {

            scene_aabb = _aabb;
            scene_sphere = CollisionCore::Sphere<MathCore::vec3f>::fromAABB(scene_aabb);
        }

        void LightAndShadowManager::setMainCameraFrustum(const CollisionCore::Frustum<MathCore::vec3f> &_frustum)
        {

            camera_frustum = _frustum;
            camera_sphere = CollisionCore::Sphere<MathCore::vec3f>::fromOBB(camera_frustum.obb);
        }

        void LightAndShadowManager::setSceneRoot(Transform *root)
        {
            scene_root = root;
        }

        void LightAndShadowManager::setVisibleObjectsFromSceneTraverse(SceneTraverseHelper *_visibleObjects)
        {
            visibleObjects = _visibleObjects;
        }

        void LightAndShadowManager::setRenderPipeline(RenderPipeline *_renderPipeline)
        {
            renderPipeline = _renderPipeline;
        }

        void LightAndShadowManager::clearCache()
        {
            std::unordered_map<Components::ComponentLight *, ShadowCache *>::iterator it = shadowCacheDic.begin();
            while (it != shadowCacheDic.end())
            {
                shadowCachePool.release(it->second);
                it++;
            }
            shadowCacheDic.clear();
        }

        void LightAndShadowManager::createLightAssociations(
            Transform *root,
            SceneTraverseHelper *visibleObjects,
            int maxLightPerObject)
        {
            // visibleObjects->sunLights.size()+
            // visibleObjects->pointLights.size()+
            // visibleObjects->spotLights.size();

            sortResult.resize(visibleObjects->sunLightList.size());
            sortTmpBuffer.resize(visibleObjects->sunLightList.size());

            MathCore::vec3f center;
            for (size_t i = 0; i < visibleObjects->meshWrapperList.size(); i++)
            {
                auto meshWrapper = visibleObjects->meshWrapperList[i];
                //...
            }
        }

        void LightAndShadowManager::computeShadowParametersForMesh(Components::ComponentMeshWrapper *meshWrapper,
                                                                   bool use_shadow,
                                                                   ShaderShadowAlgorithmEnum shaderShadowAlgorithm,
                                                                   ResourceMap *resourceMap)
        {

            std::unordered_map<Components::ComponentLight *, ShadowCache *>::iterator it;

            noShadowlightList.clear();
            shadowLightList.clear();

            // MathCore::vec3f center = meshWrapper->getCenter();

            // add all sun lights
            for (size_t j = 0; j < visibleObjects->sunLightList.size(); j++)
            {
                auto sunLight = visibleObjects->sunLightList[j];

                if (!sunLight->cast_shadow || !use_shadow)
                {
                    noShadowlightList.push_back(sunLight);
                    continue;
                }

                it = shadowCacheDic.find(sunLight);

                if (it != shadowCacheDic.end())
                {
                    shadowLightList.push_back(it->second);
                    continue;
                }

                ShadowCache *shadowCache = shadowCachePool.create(true);
                shadowCacheDic[sunLight] = shadowCache;
                shadowCache->setFromLight(sunLight->self<Components::ComponentLight>(), scene_sphere, camera_sphere);

                // render the depth map...
                shadowCache->viewport = iRect(sunShadowResolution, sunShadowResolution);
                shadowCache->depthTexture.setSize(sunShadowResolution, sunShadowResolution, GL_DEPTH_COMPONENT24);

                shadowCache->depthTexture.active(0);
                if (shaderShadowAlgorithm == ShaderShadowAlgorithm_Basic ||
                    shaderShadowAlgorithm == ShaderShadowAlgorithm_PCSS_PCF ||
                    shaderShadowAlgorithm == ShaderShadowAlgorithm_PCSS_DST_CENTER)
                {
                    OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                    OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                }
                else
                {
                    OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
                    OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
                }

                // OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
                // OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
                float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
                OPENGL_CMD(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

                shadowCache->depthTexture.deactive(0);

                // use HW shadow
                if (shaderShadowAlgorithm != ShaderShadowAlgorithm_PCSS_PCF &&
                    shaderShadowAlgorithm != ShaderShadowAlgorithm_PCSS_DST_CENTER)
                    shadowCache->depthTexture.setAsShadowMapFiltering(true, GL_LESS);
                else
                    shadowCache->depthTexture.setAsShadowMapFiltering(false);

                dynamicFBOSun.enable();
                dynamicFBOSun.setDepthTextureAttachment(&shadowCache->depthTexture);

                sceneTraverseHelper_aux.filterByOBB(scene_root, shadowCache->obb, FilterFlags_None);

                GLRenderState *state = GLRenderState::Instance();

                // state->CurrentShader = &renderPipeline->depthShader;

                state->ColorWrite = ColorWriteNone;
                state->DepthTest = DepthTestLess;
                state->BlendMode = BlendModeDisabled; // material->pbr.blendMode;
                iRect old_viewport = state->Viewport;
                state->Viewport = shadowCache->viewport;
                state->FrontFace = FrontFaceCW;

                // vec4 old_clear_color = state->ClearColor;
                // state->ClearColor = vec4(1);
                // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

                glClear(GL_DEPTH_BUFFER_BIT);

                // render the depth buffer
                for (auto &transform : sceneTraverseHelper_aux.transformList)
                    renderPipeline->traverse_depth_render_only_mesh(transform, shadowCache, resourceMap);

                state->ColorWrite = ColorWriteAll;
                state->Viewport = old_viewport;

                state->FrontFace = FrontFaceCCW;
                // state->ClearColor = old_clear_color;

                // read the data

                /*
                {
                    AppKit::OpenGL::TextureBuffer buffer = dynamicFBOSun.readPixels(0);
                    if (buffer.input_data_type == GL_UNSIGNED_BYTE)
                        ITKExtension::Image::PNG::writePNG("camera_depth.png", buffer.width, buffer.height, buffer.input_component_count, (char*)buffer.data);
                    buffer.dispose();
                }
                */

                dynamicFBOSun.disable();

                shadowLightList.push_back(shadowCache);
            }

            /*

            //compute distances
            for(size_t j=0;j<visibleObjects->sunLights.size();j++){
                MathCore::vec3f lightPos = visibleObjects->sunLights[j]->transform[0]->getPosition(true);
                float sqrDist = sqrDistance(center, lightPos);
                sortResult[j].index = j;
                // reverse order
                //sortResult[j].toSort = ~Sorting::sort_float_to_int32(sqrDist);
                // normal order
                sortResult[j].toSort = Sorting::sort_float_to_int32(sqrDist);
            }

            Sorting::radix_counting_sort_signed_index( &sortResult[0], (uint32_t)sortResult.size(), &sortTmpBuffer[0] );

            for(size_t j=0;j<sortResult.size();j++){
                uint32_t lightIndex = sortResult[j].index;
                Components::ComponentLight *sunLight = visibleObjects->sunLights[lightIndex];


            }
            */
        }

    }

}
