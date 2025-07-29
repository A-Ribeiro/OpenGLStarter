#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        class ShaderUnlitTextureAlpha : public DefaultEngineShader
        {
            int u_mvp;
            int u_texture;
            int u_color;

            MathCore::mat4f uMVP;
            MathCore::vec4f uColor;
            int uTexture;

        public:
            ShaderUnlitTextureAlpha();

            void setMVP(const MathCore::mat4f &mvp);
            void setTexture(int texunit);
            void setColor(const MathCore::vec4f &color);

            Utils::ShaderPropertyBag createDefaultBag() const override;

            void ActiveShader_And_SetUniformsFromMaterial(
                GLRenderState *state,
                ResourceMap *resourceMap,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material) override;
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
                const MathCore::mat4f *mvInv) override;
        };

    }
}