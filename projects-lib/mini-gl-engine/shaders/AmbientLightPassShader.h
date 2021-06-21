#ifndef __ambient_light_pass_shader__h_
#define __ambient_light_pass_shader__h_

#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {
    class AmbientLightPassShader : public DefaultEngineShader{
    public:
        virtual void setAlbedo(int texunit){
        }
        virtual void setAmbientColor(const aRibeiro::vec4 &color){
        }
    };


    class AmbientLight_tex_cube_PassShader : public DefaultEngineShader {
        int u_mvp;
        int u_localToWorld;
        int u_localToWorld_it;
        int u_texture;
        int u_textureNormal;
        int u_textureCubeEnvironment;
        int u_color;
    public:
        AmbientLight_tex_cube_PassShader();

        void setMVP(const aRibeiro::mat4 &mvp) {
            setUniform(u_mvp, mvp);
        }
        void setLocalToWorld(const aRibeiro::mat4 & m) {
            setUniform(u_localToWorld, m);
        }
        void setLocalToWorld_it(const aRibeiro::mat4 & m) {
            setUniform(u_localToWorld_it, m);
        }
        void setTexture(int texunit) {
            setUniform(u_texture, texunit);
        }
        void setTextureNormal(int texunit) {
            setUniform(u_textureNormal, texunit);
        }
        void setTextureCubeEnvironment(int texunit) {
            setUniform(u_textureCubeEnvironment, texunit);
        }
        void setColor(const aRibeiro::vec4 &color) {
            setUniform(u_color, color);
        }
    };



}

#endif