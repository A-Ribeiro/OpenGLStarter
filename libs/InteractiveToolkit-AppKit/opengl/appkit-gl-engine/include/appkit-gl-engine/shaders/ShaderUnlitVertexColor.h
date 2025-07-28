#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

/*
#include <appkit-gl-engine/shaders/ShaderUnlit.h> // UnlitPassShader
#include <appkit-gl-engine/shaders/ShaderUnlitVertexColor.h> // Unlit_vertcolor_Shader
#include <appkit-gl-engine/shaders/ShaderUnlitTexture.h> // Unlit_tex_PassShader
// #include <appkit-gl-engine/shaders/ShaderUnlitTextureVertexColor.h> //
#include <appkit-gl-engine/shaders/ShaderUnlitTextureVertexColor.h> // Unlit_tex_vertcolor_font_PassShader
#include <appkit-gl-engine/shaders/PBRShaderSelector.h> //
*/

namespace AppKit
{
    namespace GLEngine
    {

        class ShaderUnlitVertexColor : public DefaultEngineShader
        {
            int u_mvp;
            int u_color;

            MathCore::mat4f uMVP;
            MathCore::vec4f uColor;

        public:
            ShaderUnlitVertexColor();

            void setMVP(const MathCore::mat4f &mvp);
            void setColor(const MathCore::vec4f &color);

            Utils::ShaderPropertyBag createDefaultBag() const override;

        };

    }
}
