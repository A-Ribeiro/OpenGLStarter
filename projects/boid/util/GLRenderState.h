#ifndef Render_state_h
#define Render_state_h

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace aRibeiro;
using namespace openglWrapper;

#include "Size.h"
#include "Rect.h"

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
    BlendModeAddAlpha
};

#ifndef ARIBEIRO_RPI
enum AlphaTestType {
    AlphaTestDisabled,
    AlphaTestEqual,
    AlphaTestLess,
    AlphaTestLessEqual,
    AlphaTestGreater,
    AlphaTestGreaterEqual,
};
#endif

enum HintType {
    HintDisabled,
    HintFastest,
    HintNicest
};

#ifndef ARIBEIRO_RPI
enum WireframeType {
    WireframeDisabled,
    WireframeBoth,
    WireframeFront,
    WireframeBack
};
#endif

enum ColorWriteType{
    ColorWriteNone = 0,
    ColorWriteRed = 1,
    ColorWriteGreen = 2,
    ColorWriteBlue = 4,
    ColorWriteAlpha = 8,
    ColorWriteAll = 0x0f
};


/*
bool operator!=(const Rect& a, const Rect& b) {
    return
        a.x == b.x && a.y == b.y &&
        a.w == b.w && a.h == b.h;
}
*/

class GLRenderState {
    //
    // change property events
    //
    void OnClearColorChange(Property<vec4> *prop);
    void OnCullFaceChange(Property<CullFaceType> *prop);
    void OnFrontFaceChange(Property<FrontFaceType> *prop);
    void OnDepthTestChange(Property<DepthTestType> *prop);
    void OnClearDepthChange(Property<float> *prop);
    void OnDepthWriteChange(Property<bool> *prop);
    void OnBlendModeChange(Property<BlendModeType> *prop);
    #ifndef ARIBEIRO_RPI
    void OnAlphaTestChange(Property<AlphaTestType> *prop);
    void OnAlphaRefChange(Property<float> *prop);
    void OnLineSmoothHintChange(Property<HintType> *prop);
    void OnPointSizeChange(Property<float> *prop);
    void OnWireframeChange(Property<WireframeType> *prop);
    #endif
    void OnLineWidthChange(Property<float> *prop);
    void OnCurrentShaderChange(Property<GLShader*> *prop);
    void OnViewportChange(Property<iRect> *prop);
    void OnCurrentFramebufferObjectChange(Property<GLFramebufferObject*> *prop);
    void OnColorWriteChange(Property<ColorWriteType> *prop);

    GLRenderState();
    bool initialized;
public:

    Property<vec4> ClearColor;
    Property<CullFaceType> CullFace;
    Property<FrontFaceType> FrontFace;
    Property<DepthTestType> DepthTest;
    Property<float> ClearDepth;
    Property<bool> DepthWrite;
    Property<BlendModeType> BlendMode;
    #ifndef ARIBEIRO_RPI
    Property<AlphaTestType> AlphaTest;
    Property<WireframeType> Wireframe;
    Property<float> AlphaRef;
    Property<HintType> LineSmoothHint;
    Property<float> PointSize;
    #endif

    Property<float> LineWidth;
    Property<GLShader*> CurrentShader;
    Property<iRect> Viewport;
    Property<GLFramebufferObject*> CurrentFramebufferObject;
    Property<ColorWriteType> ColorWrite;

    static GLRenderState *getInstance();

    void initialize(int w,int h);
    void releaseResources();
};

#endif
