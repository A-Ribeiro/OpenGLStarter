#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Components/ComponentLight.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
#include <appkit-gl-engine/Components/ComponentParticleSystem.h>

#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/Renderer/ObjectPlaces.h>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLDynamicFBO.h>

#include <appkit-gl-engine/Components/ComponentCamera.h>
#include <appkit-gl-engine/shaders/FrankenShader.h>

#include <InteractiveToolkit/Platform/Core/ObjectPool.h>

#include <InteractiveToolkit/AlgorithmCore/Sorting/SortIndex.h>

namespace AppKit
{
    namespace GLEngine
    {

        class RenderPipeline;

        enum ShadowCacheShape
        {
            ShadowCacheFrustum,
            ShadowCacheOBB
        };

        class ShadowCache : public Components::ComponentCamera
        {
        public:
            static ComponentType Type;

            Components::ComponentLight *light;
            AppKit::OpenGL::GLTexture depthTexture;

            // mat4 mvp;

            ShadowCacheShape shadowCacheShape;

            // union {
            CollisionCore::Frustum<MathCore::vec3f> frustum; // spot light
            CollisionCore::OBB<MathCore::vec3f> obb;         // sun light
            //};

            ShadowCache();
            ~ShadowCache();

            void setFromLight(
                Components::ComponentLight *_light,
                const CollisionCore::Sphere<MathCore::vec3f> &scene_sphere,
                const CollisionCore::Sphere<MathCore::vec3f> &camera_sphere);
        };

        class LightAndShadowManager
        {
        public:
            int pointShadowResolution; // 256
            int spotShadowResolution;  // 256
            int sunShadowResolution;   // 512

            AppKit::OpenGL::GLDynamicFBO dynamicFBOSun;
            AppKit::OpenGL::GLTexture dummyColorTextureSun;

            std::map<Components::ComponentLight *, ShadowCache *> shadowCacheDic;
            Platform::ObjectPool<ShadowCache> shadowCachePool;

            std::vector<AlgorithmCore::Sorting::SortIndexi> sortResult;
            std::vector<AlgorithmCore::Sorting::SortIndexi> sortTmpBuffer;

            CollisionCore::AABB<MathCore::vec3f> scene_aabb;
            CollisionCore::Sphere<MathCore::vec3f> scene_sphere;
            CollisionCore::Frustum<MathCore::vec3f> camera_frustum;
            CollisionCore::Sphere<MathCore::vec3f> camera_sphere;
            Transform *scene_root;

            ObjectPlaces *visibleObjects;

            RenderPipeline *renderPipeline;

            ObjectPlaces auxObjPlaces;

            LightAndShadowManager();

            void setSceneAABB(const CollisionCore::AABB<MathCore::vec3f> &_aabb);
            void setMainCameraFrustum(const CollisionCore::Frustum<MathCore::vec3f> &_frustum);
            void setSceneRoot(Transform *root);
            void setVisibleObjects(ObjectPlaces *_visibleObjects);
            void setRenderPipeline(RenderPipeline *_renderPipeline);

            void clearCache();

            void createLightAssociations(
                Transform *root,
                ObjectPlaces *visibleObjects,
                int maxLightPerObject);

            // output light information after processing...
            std::vector<Components::ComponentLight *> noShadowlightList; // filled with all near lights
            std::vector<ShadowCache *> shadowLightList;                  // can be null...
            void computeShadowParametersForMesh(Components::ComponentMeshWrapper *meshWrapper,
                                                bool use_shadow,
                                                ShaderShadowAlgorithmEnum shaderShadowAlgorithm);
        };

    }

}