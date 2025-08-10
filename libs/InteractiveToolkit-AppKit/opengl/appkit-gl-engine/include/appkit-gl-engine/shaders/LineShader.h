#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        class LineShader : public DefaultEngineShader
        {
        private:
            int u_screenSizePx;
            int u_screenSizePx_inv;
            int u_mvp;
            int u_color;
            int u_antialias;

            MathCore::vec2f uScreenSizePx;
            MathCore::vec2f uScreenSizePx_inv;
            MathCore::mat4f uMVP;
            MathCore::vec4f uColor;
            float uAntialias;

        public:
            static const AppKit::OpenGL::ShaderType Type;
            
            LineShader();

            void setMVP(const MathCore::mat4f &mvp);
            void setColor(const MathCore::vec4f &color);
            void setScreenSizePx(const MathCore::vec2f &screenSizePx);
            void setAntialias(float antialias);

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
