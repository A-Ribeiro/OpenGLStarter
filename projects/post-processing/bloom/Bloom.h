#ifndef Bloom__h
#define Bloom__h

#include <aribeiro/all_math.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace openglWrapper;
#include "BloomThresholdShader.h"
#include "BloomBlurShader.h"
#include "BloomCompositeShader.h"

namespace aRibeiro {
    
    class Bloom: public iPostProcess {
        BloomThresholdShader *thresholdShader;
        BloomBlurShader *blurShader;
        BloomCompositeShader *compositeShader;
        
        //private copy constructores, to avoid copy...
        Bloom(const Bloom& v);
        void operator=(const Bloom& v);
        
        GLFramebufferObject *fbo1, *fbo2;
        vec2 neighborhood;
        
        void setSize(int w, int h);
        
    public:
        float threshold;
        float intensity;
        
        Bloom();
        virtual ~Bloom();
        
        virtual void render(GLPostProcess *postProcess, GLTexture *colorSrc, GLTexture *depthSrc, GLFramebufferObject *dst);

        SSE2_CLASS_NEW_OPERATOR

    };

}

#endif
