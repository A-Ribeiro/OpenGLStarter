#ifndef Render_state_h
#define Render_state_h

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/iRect.h>

namespace GLEngine {

    enum CullFaceType {
        CullFaceNone,
        CullFaceFront,
        CullFaceBack
    };

    enum FrontFaceType {
        FrontFaceCCW,
        FrontFaceCW
    };

    enum DepthTestType {
        DepthTestDisabled,
        DepthTestEqual,
        DepthTestLess,
        DepthTestLessEqual,
        DepthTestGreater,
        DepthTestGreaterEqual
    };

    enum BlendModeType {
        BlendModeDisabled,
        BlendModeAlpha,
        BlendModeAdd,
        BlendModeAddAlpha,
        BlendModeSubtract,
        BlendModeSubtractAlpha
    };

    enum AlphaTestType {
        AlphaTestDisabled,
        AlphaTestEqual,
        AlphaTestLess,
        AlphaTestLessEqual,
        AlphaTestGreater,
        AlphaTestGreaterEqual,
    };

    enum HintType {
        HintDisabled,
        HintFastest,
        HintNicest
    };

    enum WireframeType {
        WireframeDisabled,
        WireframeBoth,
        WireframeFront,
        WireframeBack
    };

    enum ColorWriteType{
        ColorWriteNone = 0,
        ColorWriteRed = 1,
        ColorWriteGreen = 2,
        ColorWriteBlue = 4,
        ColorWriteAlpha = 8,
        ColorWriteAll = 0x0f
    };

    class GLRenderState {
        //
        // change property events
        //
        void OnClearColorChange(aRibeiro::Property<aRibeiro::vec4> *prop);
        void OnCullFaceChange(aRibeiro::Property<CullFaceType> *prop);
        void OnFrontFaceChange(aRibeiro::Property<FrontFaceType> *prop);
        void OnDepthTestChange(aRibeiro::Property<DepthTestType> *prop);
        void OnClearDepthChange(aRibeiro::Property<float> *prop);
        void OnDepthWriteChange(aRibeiro::Property<bool> *prop);
        void OnBlendModeChange(aRibeiro::Property<BlendModeType> *prop);
        #ifndef ARIBEIRO_RPI
        void OnAlphaTestChange(aRibeiro::Property<AlphaTestType> *prop);
        void OnAlphaRefChange(aRibeiro::Property<float> *prop);
        void OnLineSmoothHintChange(aRibeiro::Property<HintType> *prop);
        void OnPointSizeChange(aRibeiro::Property<float> *prop);
        void OnWireframeChange(aRibeiro::Property<WireframeType> *prop);
        #endif
        void OnLineWidthChange(aRibeiro::Property<float> *prop);
        void OnCurrentShaderChange(aRibeiro::Property<openglWrapper::GLShader*> *prop);
        void OnViewportChange(aRibeiro::Property<iRect> *prop);
        void OnCurrentFramebufferObjectChange(aRibeiro::Property<openglWrapper::GLFramebufferObject*> *prop);
        void OnColorWriteChange(aRibeiro::Property<ColorWriteType> *prop);

        GLRenderState();
        //bool initialized;
    public:

        aRibeiro::Property<aRibeiro::vec4> ClearColor;
        aRibeiro::Property<CullFaceType> CullFace;
        aRibeiro::Property<FrontFaceType> FrontFace;
        aRibeiro::Property<DepthTestType> DepthTest;
        aRibeiro::Property<float> ClearDepth;
        aRibeiro::Property<bool> DepthWrite;
        aRibeiro::Property<BlendModeType> BlendMode;
        #ifndef ARIBEIRO_RPI
        aRibeiro::Property<AlphaTestType> AlphaTest;
        aRibeiro::Property<float> AlphaRef;
        aRibeiro::Property<HintType> LineSmoothHint;
        aRibeiro::Property<float> PointSize;
        aRibeiro::Property<WireframeType> Wireframe;
        #endif
        aRibeiro::Property<float> LineWidth;
        aRibeiro::Property<openglWrapper::GLShader*> CurrentShader;
        aRibeiro::Property<iRect> Viewport;
        aRibeiro::Property<openglWrapper::GLFramebufferObject*> CurrentFramebufferObject;
        aRibeiro::Property<ColorWriteType> ColorWrite;

        static GLRenderState *Instance();

        void initialize(int w,int h);
        void releaseResources();
    };

}

#endif
