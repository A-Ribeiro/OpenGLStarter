#ifndef __depth_pass_shader__h_
#define __depth_pass_shader__h_

#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {
    class DepthPassShader : public DefaultEngineShader{
    public:
        DepthPassShader() {
            format = model::CONTAINS_POS;
            //format = model::CONTAINS_POS | model::CONTAINS_UV0;
        }
    };
}

#endif