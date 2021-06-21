#ifndef fade_h_
#define fade_h_

//#include <mini-gl-engine/mini-gl-engine.h>
#include <opengl-wrapper/GLShaderColor.h>
#include <aribeiro/aribeiro.h>

namespace GLEngine {

    DefineMethodPointer(FadeMethodPtr, void) VoidMethodCall();

    class _SSE2_ALIGN_PRE Fade{
        aRibeiro::aligned_vector<aRibeiro::vec3> vertex;
        openglWrapper::GLShaderColor shaderColor;

        aRibeiro::PlatformTime *time;
        FadeMethodPtr OnEndCall;


        float lrp;
        float sec;
        aRibeiro::vec4 colorSrc;
        aRibeiro::vec4 colorTarget;
        aRibeiro::vec4 color;
        bool reset_draw_visible;
        bool draw_visible;
        float oldTimeScale;
        bool completeOnNextFrame;

        void setColor(aRibeiro::PlatformTime *time);

        void createScreenVertex();

    public:

        bool isFading;

        Fade(aRibeiro::PlatformTime *_time);
        virtual ~Fade();

        void fadeIn(float _sec, const FadeMethodPtr &_OnEndCall);

        void fadeOut(float _sec, const FadeMethodPtr &_OnEndCall);

        void draw();

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}


#endif
