#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {
        class FrankenShader;
        
        class PBRShaderSelector : public DefaultEngineShader
        {
            FrankenShader *frankenShader;
        public:
            Utils::ShaderPropertyBag PBRShaderSelector::createDefaultBag() const override;

            void PBRShaderSelector::ActiveShader_And_SetUniformsFromMaterial(
                GLRenderState *state,
                ResourceMap *resourceMap,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material) override;
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
                const MathCore::mat4f *mvInv) override;
        };
    }
}