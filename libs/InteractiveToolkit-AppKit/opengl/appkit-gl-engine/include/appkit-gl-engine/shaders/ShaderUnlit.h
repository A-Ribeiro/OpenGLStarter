#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        class ShaderUnlit : public DefaultEngineShader
        {
            int u_mvp;
            int u_color;

            MathCore::mat4f uMVP;
            MathCore::vec4f uColor;

        public:
            ShaderUnlit();
            void setMVP(const MathCore::mat4f &mvp);
            void setColor(const MathCore::vec4f &color);

            Utils::ShaderPropertyBag createDefaultBag() const override;

        };

    }
}
