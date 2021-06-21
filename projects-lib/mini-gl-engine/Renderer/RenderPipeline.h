#ifndef __render_pipeline__h_
#define __render_pipeline__h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Transform.h>
#include <data-model/data-model.h>
#include <mini-gl-engine/GLCubeMapSkyBox.h>
#include <mini-gl-engine/ComponentCamera.h>
#include <mini-gl-engine/UnlitPassShader.h>
#include <mini-gl-engine/AmbientLightPassShader.h>
#include <mini-gl-engine/DirectionalLightPassShader.h>
#include <mini-gl-engine/FrankenShaderManager.h>

#include <mini-gl-engine/ComponentLight.h>
#include <mini-gl-engine/ComponentParticleSystem.h>

#include <mini-gl-engine/ParticleSystemRenderer.h>
#include <mini-gl-engine/DepthRenderer.h>

namespace GLEngine {

    enum AmbientLightModeEnum {
        AmbientLightMode_None,
        AmbientLightMode_SkyBoxSolidColor,
        AmbientLightMode_SkyBoxCubeTexture,
        AmbientLightMode_Color
    };

    struct _SSE2_ALIGN_PRE AmbientLight {
        AmbientLightModeEnum lightMode;
        aRibeiro::vec3 color;

        AmbientLight() {
            lightMode = AmbientLightMode_Color;
            color = aRibeiro::vec3(1.0f);
        }

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    //the render pipeline binds the material to shader
    // And have some auxiliary methods to handle the tree
    class _SSE2_ALIGN_PRE RenderPipeline {

        DepthRenderer *depthRenderer;

        std::vector<Components::ComponentParticleSystem *> sceneParticleSystem;
        ParticleSystemRenderer particleSystemRenderer;

        std::vector<Components::ComponentLight *> sceneSunLights;
        aRibeiro::vec4 ambientLightColor;
        aRibeiro::vec3 ambientLightColorVec3;
        
        // supported shaders in this Render Pipeline
        UnlitPassShader unlitShader;
        Unlit_tex_vertcolor_font_PassShader unlit_tex_vertcolor_font_Shader;
        Unlit_tex_PassShader unlit_tex_Shader;
        
        // PBR multipass shaders
        AmbientLight_tex_cube_PassShader ambientLight_tex_cube_PassShader;
        DirectionalLightPassShader directionalLightPassShader;
        
        // PBR single pass frankenshader
        FrankenShaderManager frankenShaderManager;

        //search lights and particle systems
        bool traverse_search_elements(Transform *element, void* userData);
        void SearchSpecialObjects(Transform *root);


        void allMeshRender(Transform *element, const DefaultEngineShader *shader)const;
        void allMeshRender_Range(Transform *element, const DefaultEngineShader *shader, int start_index, int end_index)const;
        void materialSetupAndRender(
            Components::ComponentMaterial* material,
            Transform *element,
            Components::ComponentCamera *camera,
            int start_index, int end_index,
            const aRibeiro::mat4 *mvp,
            const aRibeiro::mat4 *mv,
            const aRibeiro::mat4 *mvIT,
            const aRibeiro::mat4 *mvInv);
        
        
        
        bool traverse_multipass_render(Transform *element, void* userData);
        
        bool traverse_singlepass_render(Transform *element, void* userData);
        
        
    public:

        // public skybox setup variables...
        GLEngine::GLCubeMapSkyBox *cubeSkyBox;
        openglWrapper::GLCubeMap *cubeAmbientLight_1x1;

        // public parameters
        //light setup
        AmbientLight ambientLight;

        // uses as flags ShaderAlgorithmsEnum
        uint32_t ShaderAlgorithmsEnum_allowedFlags;

        RenderPipeline();

        virtual ~RenderPipeline();

        void runMultiPassPipeline(Transform*root, Components::ComponentCamera *camera, bool clear = true);
        
        void runSinglePassPipeline(Transform*root, Components::ComponentCamera *camera, bool clear = true);

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
