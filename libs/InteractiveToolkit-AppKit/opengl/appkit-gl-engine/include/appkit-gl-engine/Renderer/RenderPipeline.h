#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLCubeMap.h>

#include <appkit-gl-engine/Transform.h>

//#include <aRibeiroData/aRibeiroData.h>

#include <appkit-gl-engine/GL/GLCubeMapSkyBox.h>
#include <appkit-gl-engine/Components/ComponentCamera.h>

#include <appkit-gl-engine/shaders/UnlitPassShader.h>
#include <appkit-gl-engine/shaders/DepthPassShader.h>

#include <appkit-gl-engine/shaders/AmbientLightPassShader.h>
#include <appkit-gl-engine/shaders/DirectionalLightPassShader.h>
#include <appkit-gl-engine/shaders/FrankenShaderManager.h>

#include <appkit-gl-engine/Components/ComponentLight.h>
#include <appkit-gl-engine/Components/ComponentParticleSystem.h>

#include <appkit-gl-engine/Renderer/ParticleSystemRenderer.h>
#include <appkit-gl-engine/Renderer/DepthRenderer.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>

#include <appkit-gl-engine/Renderer/ObjectPlaces.h>

#include <appkit-gl-engine/Renderer/LightAndShadowManager.h>

namespace AppKit
{
    namespace GLEngine
    {

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

            ObjectPlaces objectPlaces;

            // std::vector<Components::ComponentColorLine *> debugLines;

            DepthRenderer *depthRenderer;

            // std::vector<Components::ComponentParticleSystem *> sceneParticleSystem;
            ParticleSystemRenderer particleSystemRenderer;

            // std::vector<Components::ComponentLight *> sceneSunLights;
            // MathCore::vec4f ambientLightColor;
            MathCore::vec3f ambientLightColorVec3;

            // supported shaders in this Render Pipeline
        public:
            DepthPassShader depthShader;
            UnlitPassShader unlitShader;
            Unlit_tex_vertcolor_font_PassShader unlit_tex_vertcolor_font_Shader;
            Unlit_tex_PassShader unlit_tex_Shader;

        private:
            // PBR multipass shaders
            AmbientLight_tex_cube_PassShader ambientLight_tex_cube_PassShader;
            DirectionalLightPassShader directionalLightPassShader;

            // PBR single pass frankenshader
            FrankenShaderManager frankenShaderManager;

            // search lights and particle systems
            // bool traverse_search_elements(Transform *element, void* userData);
            // void SearchSpecialObjects(Transform *root);

            void allMeshRender(Transform *element, const DefaultEngineShader *shader) const;
            void allMeshRender_Range(Transform *element, const DefaultEngineShader *shader, int start_index, int end_index) const;
            void materialSetupAndRender(
                Components::ComponentMaterial *material,
                Transform *element,
                Components::ComponentCamera *camera,
                int start_index, int end_index,
                const MathCore::mat4f *mvp,
                const MathCore::mat4f *mv,
                const MathCore::mat4f *mvIT,
                const MathCore::mat4f *mvInv);

            void materialSetupAndRender_depth(
                Components::ComponentMaterial *material,
                Transform *element,
                Components::ComponentCamera *camera,
                int start_index, int end_index,
                const MathCore::mat4f *mvp,
                const MathCore::mat4f *mv,
                const MathCore::mat4f *mvIT,
                const MathCore::mat4f *mvInv);

            bool traverse_multipass_render(Transform *element, void *userData);

            bool traverse_singlepass_render(Transform *element, void *userData);

        public:
            bool traverse_depth_render(Transform *element, void *userData);

        public:
            // public skybox setup variables...
            AppKit::GLEngine::GLCubeMapSkyBox *cubeSkyBox;
            AppKit::OpenGL::GLCubeMap *cubeAmbientLight_1x1;
            AppKit::OpenGL::GLTexture *sphereAmbientLight;

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

            void runMultiPassPipeline(Transform *root, Components::ComponentCamera *camera, bool clear = true);

            void runSinglePassPipeline(Transform *root, Components::ComponentCamera *camera, bool clear = true);

            void renderDepth(Transform *root, Components::ComponentCamera *camera);
        };

    }

}
