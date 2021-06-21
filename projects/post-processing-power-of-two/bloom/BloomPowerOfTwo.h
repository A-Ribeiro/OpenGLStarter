#ifndef BloomPowerOfTwo__h
#define BloomPowerOfTwo__h

#include <aribeiro/all_math.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace openglWrapper;
#include "BloomPowerOfTwoThresholdShader.h"
#include "BloomPowerOfTwoBlurShader.h"
#include "BloomPowerOfTwoCompositeShader.h"

#include "../GLPostProcessPowerOfTwo.h"

namespace aRibeiro {
    
    class BloomPowerOfTwo: public iPostProcessPowerOfTwo {
        
        BloomPowerOfTwoThresholdShader *thresholdShader;
        BloomPowerOfTwoBlurShader *blurShader;
        BloomPowerOfTwoCompositeShader *compositeShader;
        
        //private copy constructores, to avoid copy...
        BloomPowerOfTwo(const BloomPowerOfTwo& v);
        void operator=(const BloomPowerOfTwo& v);
        
        GLFramebufferObject *fbo1, *fbo2;
        int width,height;
        int potWidth, potHeight;// will hold the power of two size
        vec2 texelNeighbor;
        vec2 texelMaxUVAccess;
        vec2 drawQuadMaxUV;
        
        //vec2 neighborhood;
        
        void setSize(int w, int h);
        
    public:
        float threshold;
        float intensity;
        
        BloomPowerOfTwo();
        virtual ~BloomPowerOfTwo();
        
        virtual void render(GLPostProcessPowerOfTwo *postProcess, GLTexture *colorSrc, GLTexture *depthSrc, GLFramebufferObject *dst);

        SSE2_CLASS_NEW_OPERATOR

    };

}

#endif
