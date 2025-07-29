#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {
        class DepthPassShader : public DefaultEngineShader
        {
            int u_mvp;

        public:
            DepthPassShader();
            void setMVP(const MathCore::mat4f &mvp);
        };
    }

}