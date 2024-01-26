#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {
        class AmbientLightPassShader : public DefaultEngineShader
        {
        public:
            virtual void setAlbedo(int texunit)
            {
            }
            virtual void setAmbientColor(const MathCore::vec4f &color)
            {
            }
        };

        class AmbientLight_tex_cube_PassShader : public DefaultEngineShader
        {
            int u_mvp;
            int u_localToWorld;
            int u_localToWorld_it;
            int u_texture;
            int u_textureNormal;
            int u_textureCubeEnvironment;
            int u_color;

        public:
            AmbientLight_tex_cube_PassShader();

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
            void setTextureCubeEnvironment(int texunit)
            {
                setUniform(u_textureCubeEnvironment, texunit);
            }
            void setColor(const MathCore::vec4f &color)
            {
                setUniform(u_color, color);
            }
        };

    }
}