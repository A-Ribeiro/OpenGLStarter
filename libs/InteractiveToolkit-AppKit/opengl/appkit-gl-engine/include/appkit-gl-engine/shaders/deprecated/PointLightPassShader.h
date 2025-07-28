#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        class PhongPointLightPass : public DefaultEngineShader
        {
        public:
            virtual void setAlbedo(int texunit)
            {
            }
            virtual void setNormal(int texunit)
            {
            }
            virtual void setLightWorldPosition(const MathCore::vec4f &color)
            {
            }
            virtual void setLightDiffuse(const float &diffuse)
            {
            }
            virtual void setLightSpecular(const float &specular)
            {
            }
            virtual void setLightSpecularShine(const float &shine)
            {
            }
        };

    }

}