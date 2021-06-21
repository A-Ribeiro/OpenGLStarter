
#ifndef GLPostProcessPowerOfTwo__h
#define GLPostProcessPowerOfTwo__h

#include <opengl-wrapper/opengl-wrapper.h>
using namespace openglWrapper;
#include <vector>


namespace aRibeiro {

    
    class GLPostProcessingPowerOfTwoShader : public GLShader {
    public:
        static const int aVec2Position = 0;
        static const int aVec2UV = 1;
        
        virtual void setTexelNeighbor(const vec2 &texelNeighbor) = 0;
        virtual void setTexelMaxUVAccess(const vec2 &texelMaxUVAccess) = 0;
    };
    
    class GLPostProcessPowerOfTwo;
    
    class iPostProcessPowerOfTwo{
    public:
        virtual void render(GLPostProcessPowerOfTwo *postProcess, GLTexture *colorSrc, GLTexture *depthSrc, GLFramebufferObject *dst) = 0;
    };
    
    class GLPostProcessPowerOfTwo{
        
    private:
        
        GLFramebufferObject* firstRender;
        GLFramebufferObject* fbo[2];
        int current;
        
        //private copy constructores, to avoid copy...
        GLPostProcessPowerOfTwo(const GLPostProcessPowerOfTwo& v);
        void operator=(const GLPostProcessPowerOfTwo& v);
        
    public:
        std::vector<iPostProcessPowerOfTwo*> pipeline;
        int width,height;
        
        int potWidth, potHeight;// will hold the power of two size
        vec2 texelNeighbor;
        vec2 texelMaxUVAccess;
        vec2 drawQuadMaxUV;
        
        GLPostProcessPowerOfTwo();
        virtual ~GLPostProcessPowerOfTwo();
        
        void resize(int w, int h);
        void beginDraw();
        void endDraw();
        
        static void drawQuad(GLPostProcessingPowerOfTwoShader *shader,
                             const vec2 &texelNeighbor,
                             const vec2 &texelMaxUVAccess,
                             const vec2 &drawQuadMaxUV);

        SSE2_CLASS_NEW_OPERATOR
    };

}

#endif

