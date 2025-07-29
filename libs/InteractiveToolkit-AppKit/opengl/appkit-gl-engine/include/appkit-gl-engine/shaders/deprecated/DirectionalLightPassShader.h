#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        class DirectionalLightPassShader : public DefaultEngineShader
        {
            int u_mvp;

            int u_localToWorld;
            int u_localToWorld_it;

            int u_texture;
            int u_textureNormal;

            int u_lightRadiance;
            int u_lightDir;
            int u_cameraPosWorld;

            int u_materialAlbedoColor;
            int u_materialRoughness;
            int u_materialMetallic;

        public:
            DirectionalLightPassShader();

            void setMVP(const MathCore::mat4f &mvp)
            {
                setUniform(u_mvp, mvp);
            }
            void setLocalToWorld(const MathCore::mat4f &m)
            {
                setUniform(u_localToWorld, m);
            }
            void setLocalToWorld_it(const MathCore::mat4f &m)
            {
                setUniform(u_localToWorld_it, m);
            }
            void setTexture(int texunit)
            {
                setUniform(u_texture, texunit);
            }
            void setTextureNormal(int texunit)
            {
                setUniform(u_textureNormal, texunit);
            }
            void setMaterialAlbedoColor(const MathCore::vec3f &color)
            {
                setUniform(u_materialAlbedoColor, color);
            }
            void setMaterialRoughness(float roughness)
            {
                setUniform(u_materialRoughness, roughness);
            }
            void setMaterialMetallic(float metallic)
            {
                setUniform(u_materialMetallic, metallic);
            }
            void setLightRadiance(const MathCore::vec3f &radiance)
            {
                setUniform(u_lightRadiance, radiance);
            }
            void setLightDirWorld(const MathCore::vec3f &dir)
            {
                setUniform(u_lightDir, dir);
            }
            void setCameraPosWorld(const MathCore::vec3f &pos)
            {
                setUniform(u_cameraPosWorld, pos);
            }
        };

    }

}
