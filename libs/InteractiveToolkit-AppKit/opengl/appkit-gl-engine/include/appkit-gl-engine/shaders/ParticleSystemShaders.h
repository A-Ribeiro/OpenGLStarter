#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

//#include <appkit-gl-base/GLTexture.h>

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        class NormalParticleShader : public DefaultEngineShader
        {

            int u_mvp;
            int u_v_inv;
            int u_texture;
            int u_color;

        public:
            NormalParticleShader();

            virtual int queryAttribLocation(const char *aname);

            void setMVP(const MathCore::mat4f &mvp);
            void setV_inv(const MathCore::mat4f &v_inv);
            void setTexture(int texunit);
            void setColor(const MathCore::vec4f &color);
        };

        class SoftParticleShader : public DefaultEngineShader
        {

            int u_mvp;
            int u_v_inv;
            int u_texture;
            int u_DepthTextureComponent24;
            int u_color;
            int u_uScreenSize;
            int u_FMinusN_FPlusN_FTimesNTimes2_N;

        public:
            SoftParticleShader();

            virtual int queryAttribLocation(const char *aname);

            void setMVP(const MathCore::mat4f &mvp);
            void setV_inv(const MathCore::mat4f &v_inv);
            void setTexture(int texunit);
            void setDepthTextureComponent24(int texunit);
            void setColor(const MathCore::vec4f &color);
            void setScreenSize(const MathCore::vec2f &ss);
            void setCamera_FMinusN_FPlusN_FTimesNTimes2_N(const MathCore::vec4f &v);
        };
    }
}
