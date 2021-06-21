#ifndef __directional_light_shader__h_
#define __directional_light_shader__h_

#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {
    
    class DirectionalLightPassShader : public DefaultEngineShader {
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
        void setMaterialAlbedoColor(const aRibeiro::vec4 &color) {
            setUniform(u_materialAlbedoColor, color);
        }
        void setMaterialRoughness(float roughness) {
            setUniform(u_materialRoughness, roughness);
        }
        void setMaterialMetallic(float metallic) {
            setUniform(u_materialMetallic, metallic);
        }
        void setLightRadiance(const aRibeiro::vec3 &radiance) {
            setUniform(u_lightRadiance, radiance);
        }
        void setLightDirWorld(const aRibeiro::vec3 &dir) {
            setUniform(u_lightDir, dir);
        }
        void setCameraPosWorld(const aRibeiro::vec3 &pos) {
            setUniform(u_cameraPosWorld, pos);
        }
    };
    
    
}

#endif
