
#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>
//#include <appkit-gl-engine/ResourceMap.h>
#include "./MaskCommon.h"


namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {
            class ComponentRectangle;
        }

        class SpriteShaderWithMask : public AddShaderRectangleMask
        {
        private:
            int u_mvp;
            int u_color;
            int u_texture;

            MathCore::mat4f uMVP;
            MathCore::vec4f uColor;
            int uTexture;

        public:
            SpriteShaderWithMask();

            void setMVP(const MathCore::mat4f &mvp);
            void setColor(const MathCore::vec4f &color);
            void setTexture(int texunit);

            Utils::ShaderPropertyBag createDefaultBag() const override;

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
