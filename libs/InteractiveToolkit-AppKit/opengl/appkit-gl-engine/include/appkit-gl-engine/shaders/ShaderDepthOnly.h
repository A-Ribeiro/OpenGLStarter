#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        class ShaderDepthOnly : public DefaultEngineShader
        {
            int u_mvp;
            MathCore::mat4f uMVP;
        public:
            ShaderDepthOnly();
            void setMVP(const MathCore::mat4f &mvp);

            void ActiveShader_And_SetUniformsFromMaterial(
                GLRenderState *state,
                ResourceMap *resourceMap,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material)override;
            void setUniformsFromMatrices(
                GLRenderState *state,
                ResourceMap *resourceMap,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material,
                Transform *element,
                Components::ComponentCamera *camera,
                const MathCore::mat4f *mvp,
                const MathCore::mat4f *mv,
                const MathCore::mat4f *mvIT,
                const MathCore::mat4f *mvInv)override;
        };

    }
}
