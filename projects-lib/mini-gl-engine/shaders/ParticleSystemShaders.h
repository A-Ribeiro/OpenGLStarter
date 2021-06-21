#ifndef __particle_system_shaders__h_
#define __particle_system_shaders__h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {

    class NormalParticleShader : public DefaultEngineShader {

        int u_mvp;
        int u_v_inv;
        int u_texture;
        int u_color;
    public:
        NormalParticleShader();

        virtual int queryAttribLocation(const char* aname);

        virtual void setupAttribLocation() ;
        
        void setMVP(const aRibeiro::mat4 &mvp);
        void setV_inv(const aRibeiro::mat4 &v_inv);
        void setTexture(int texunit);
        void setColor(const aRibeiro::vec4 &color);
    };


    class SoftParticleShader : public DefaultEngineShader {

        int u_mvp;
        int u_v_inv;
        int u_texture;
        int u_DepthTextureComponent24;
        int u_color;
        int u_uScreenSize;
        int u_uCameraNearFar;
    public:
        SoftParticleShader();

        virtual int queryAttribLocation(const char* aname);

        virtual void setupAttribLocation();
        
        void setMVP(const aRibeiro::mat4 &mvp);
        void setV_inv(const aRibeiro::mat4 &v_inv);
        void setTexture(int texunit);
        void setDepthTextureComponent24(int texunit);
        void setColor(const aRibeiro::vec4 &color);
        void setScreenSize(const aRibeiro::vec2 &ss) ;
        void setCameraNearFar(const aRibeiro::vec2 &nf);
    };
}

#endif