#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/ResourceMap.h>

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
        };

    }
}
