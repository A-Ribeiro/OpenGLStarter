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
            LineShader();

            void setMVP(const MathCore::mat4f &mvp);
            void setColor(const MathCore::vec4f &color);
            void setScreenSizePx(const MathCore::vec2f &screenSizePx);
            void setAntialias(float antialias);

            void activateShaderAndSetPropertiesFromBag(
                Components::ComponentCamera *camera,
                const MathCore::mat4f *mvp,
                const Transform *element, // for localToWorld, localToWorld_IT, worldToLocal, 
                GLRenderState *state,
                const Utils::ShaderPropertyBag &bag
            ) override;

            void deactivateShader(GLRenderState *state) override;

            Utils::ShaderPropertyBag createDefaultBag() const override;
        };
    }
}
