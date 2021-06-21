#ifndef __point_light_shader__h_
#define __point_light_shader__h_

#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {
    
    class PhongPointLightPass : public DefaultEngineShader{
    public:
        virtual void setAlbedo(int texunit){
        }
        virtual void setNormal(int texunit){
        }
        virtual void setLightWorldPosition(const aRibeiro::vec4 &color){
        }
        virtual void setLightDiffuse(const float &diffuse){
        }
        virtual void setLightSpecular(const float &specular){
        }
        virtual void setLightSpecularShine(const float &shine){
        }
    };

}

#endif