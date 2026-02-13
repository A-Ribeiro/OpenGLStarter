#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLCubeMap.h>

#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/GL/GLCubeMapSkyBox.h>
#include <appkit-gl-engine/Components/Core/ComponentCamera.h>

//#include <appkit-gl-engine/shaders/UnlitPassShader.h>
//#include <appkit-gl-engine/shaders/DepthPassShader.h>

//#include <appkit-gl-engine/shaders/AmbientLightPassShader.h>
//#include <appkit-gl-engine/shaders/DirectionalLightPassShader.h>
#include <appkit-gl-engine/shaders/FrankenShaderManager.h>

#include <appkit-gl-engine/Components/Core/ComponentLight.h>
#include <appkit-gl-engine/Components/Ext/ComponentParticleSystem.h>

#include <appkit-gl-engine/Renderer/ParticleSystemRenderer.h>
#include <appkit-gl-engine/Renderer/DepthRenderer.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>

#include <appkit-gl-engine/Renderer/SceneTraverseHelper.h>
#include <appkit-gl-engine/Renderer/SortingHelper.h>

#include <appkit-gl-engine/Renderer/LightAndShadowManager.h>

#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        class PBRShaderSelector;

        enum OrthographicFilterEnum
        {
            OrthographicFilter_UsingAABB, /// AABB filter, faster, when using orthographic camera aligned with axes
            OrthographicFilter_UsingOBB /// OBB filter, slower, when using orthographic camera not aligned with axes, with any rotation
        };

        enum AmbientLightModeEnum
        {
            AmbientLightMode_None,
            AmbientLightMode_SphereMapTexture,
            AmbientLightMode_SkyBoxSolidColor,
            AmbientLightMode_SkyBoxCubeTexture,
            AmbientLightMode_Color
        };

        struct AmbientLight
        {
            AmbientLightModeEnum lightMode;
            MathCore::vec3f color;

            AmbientLight()
            {
                lightMode = AmbientLightMode_Color;
                color = MathCore::vec3f(1.0f);
            }
        };

        // the render pipeline binds the material to shader
        //  And have some auxiliary methods to handle the tree
        class RenderPipeline: public EventCore::HandleCallback
        {

            std::vector<uint32_t> sunIndex;
            std::vector<MathCore::vec3f> sunVertex;

            DepthRenderer *depthRenderer;
            ParticleSystemRenderer particleSystemRenderer;
            MathCore::vec3f ambientLightColorVec3;

            SceneTraverseHelper sceneTraverseHelper;
            SortingHelper sortingHelper;


            Components::ComponentMaterial *currentMaterial;
            Components::ComponentMesh *currentMesh;
            const DefaultEngineShader *shaderMeshLastSet;

            Components::ComponentMaterial *meshAgregatorMaterial;
            Components::ComponentMesh *meshAgregator;
            Transform *dummyTransform;
            size_t agregateMesh_ConcatenateLowerThanIndexCount;
            size_t agregateMesh_FlushMoreThanIndexCount;

            Platform::ThreadPool *threadPool;

            struct concatenation_parallel
            {
                Transform *toApply;
                const Components::ComponentMesh *other;
                const DefaultEngineShader *shader;

                uint32_t pos_offset;
                uint32_t indices_offset;

                uint32_t pos_count;
                uint32_t indices_count;
            };
            Platform::ObjectQueue<concatenation_parallel> concatenation_parallel_queue;
            Platform::Semaphore concatenation_parallel_semaphore;
            // int concatenation_parallel_total_tasks;


            void renderMeshAgregatorAndClear(ResourceMap *resourceMap, Components::ComponentCamera *camera);

            // supported shaders in this Render Pipeline
            void setCurrentMaterial(Components::ComponentMaterial *material, ResourceMap *resourceMap, Components::ComponentCamera *camera);
            void setCurrentMesh(Components::ComponentMesh *mesh, ResourceMap *resourceMap, Components::ComponentCamera *camera);
        public:
            // PBR single pass frankenshader
            FrankenShaderManager frankenShaderManager;

            size_t agregateMesh_ConcatenateLowerThanTriangleCount;
            size_t agregateMesh_FlushMoreThanTriangleCount;
        private:

            void traverse_singlepass_render(Transform *element, Components::ComponentCamera *camera, ResourceMap *resourceMap);

        public:
            // lightManager calls this from outside
            void traverse_depth_render_only_mesh(Transform *element, Components::ComponentCamera *camera, ResourceMap *resourceMap);

        public:
            // public skybox setup variables...
            std::shared_ptr<AppKit::GLEngine::GLCubeMapSkyBox> cubeSkyBox;
            std::shared_ptr<AppKit::OpenGL::GLCubeMap> cubeAmbientLight_1x1;
            std::shared_ptr<AppKit::OpenGL::GLTexture> sphereAmbientLight;

            // public parameters
            // light setup
            AmbientLight ambientLight;

            // uses as flags ShaderAlgorithmsEnum
            ShaderAlgorithmsBitMask ShaderAlgorithmsEnum_allowedFlags;

            // new parameters...
            // bool use_shadow;
            ShaderShadowAlgorithmEnum shaderShadowAlgorithm;
            ShaderPBRAlgorithmEnum shaderPBRAlgorithm;

            LightAndShadowManager lightAndShadowManager;

            RenderPipeline();

            ~RenderPipeline();

            void runSinglePassPipeline(
                ResourceMap *resourceMap,
                std::shared_ptr<Transform> root, 
                std::shared_ptr<Components::ComponentCamera> camera, 
                bool clear = true,
                OrthographicFilterEnum orthoFilter = OrthographicFilter_UsingAABB,
                Platform::ThreadPool *threadPool = nullptr);

            friend class PBRShaderSelector;
        };

    }

}
