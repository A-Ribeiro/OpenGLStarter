#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        class ShaderUnlitTextureVertexColorAlpha : public DefaultEngineShader
        {
            int u_mvp;
            int u_texture;
            int u_color;

            MathCore::mat4f uMVP;
            MathCore::vec4f uColor;
            int uTexture;

        public:
            ShaderUnlitTextureVertexColorAlpha();

            void setMVP(const MathCore::mat4f &mvp);
            void setTexture(int texunit);
            void setColor(const MathCore::vec4f &color);

            Utils::ShaderPropertyBag createDefaultBag() const override;

        };
        
    }
}