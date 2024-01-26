#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/Property.h>

#include <appkit-gl-base/GLShader.h>
#include <appkit-gl-base/GLFramebufferObject.h>

#include <appkit-gl-engine/types/iRect.h>

namespace AppKit
{
    namespace GLEngine
    {

        enum CullFaceType
        {
            CullFaceNone,
            CullFaceFront,
            CullFaceBack
        };

        enum FrontFaceType
        {
            FrontFaceCCW,
            FrontFaceCW
        };

        enum DepthTestType
        {
            DepthTestDisabled,
            DepthTestEqual,
            DepthTestLess,
            DepthTestLessEqual,
            DepthTestGreater,
            DepthTestGreaterEqual
        };

        enum BlendModeType
        {
            BlendModeDisabled,
            BlendModeAlpha,
            BlendModeAdd,
            BlendModeAddAlpha,
            BlendModeSubtract,
            BlendModeSubtractAlpha
        };

        enum AlphaTestType
        {
            AlphaTestDisabled,
            AlphaTestEqual,
            AlphaTestLess,
            AlphaTestLessEqual,
            AlphaTestGreater,
            AlphaTestGreaterEqual,
        };

        enum HintType
        {
            HintDisabled,
            HintFastest,
            HintNicest
        };

        enum WireframeType
        {
            WireframeDisabled,
            WireframeBoth,
            WireframeFront,
            WireframeBack
        };

        enum ColorWriteType
        {
            ColorWriteNone = 0,
            ColorWriteRed = 1,
            ColorWriteGreen = 2,
            ColorWriteBlue = 4,
            ColorWriteAlpha = 8,
            ColorWriteAll = 0x0f
        };

        class GLRenderState : public EventCore::HandleCallback
        {
            //
            // change property events
            //
            void OnClearColorChange(const MathCore::vec4f &value, const MathCore::vec4f &oldValue);
            void OnCullFaceChange(const CullFaceType &value, const CullFaceType &oldValue);
            void OnFrontFaceChange(const FrontFaceType &value, const FrontFaceType &oldValue);
            void OnDepthTestChange(const DepthTestType &value, const DepthTestType &oldValue);
            void OnClearDepthChange(const float &value, const float &oldValue);
            void OnDepthWriteChange(const bool &value, const bool &oldValue);
            void OnBlendModeChange(const BlendModeType &value, const BlendModeType &oldValue);
#ifndef ITK_RPI
            void OnAlphaTestChange(const AlphaTestType &value, const AlphaTestType &oldValue);
            void OnAlphaRefChange(const float &value, const float &oldValue);
            void OnLineSmoothHintChange(const HintType &value, const HintType &oldValue);
            void OnPointSizeChange(const float &value, const float &oldValue);
            void OnWireframeChange(const WireframeType &value, const WireframeType &oldValue);
#endif
            void OnLineWidthChange(const float &value, const float &oldValue);
            void OnCurrentShaderChange(AppKit::OpenGL::GLShader *const &value, AppKit::OpenGL::GLShader *const &oldValue);
            void OnViewportChange(const iRect &value, const iRect &oldValue);
            void OnCurrentFramebufferObjectChange(AppKit::OpenGL::GLFramebufferObject *const &value, AppKit::OpenGL::GLFramebufferObject *const &oldValue);
            void OnColorWriteChange(const ColorWriteType &value, const ColorWriteType &oldValue);

            GLRenderState();
            // bool initialized;
        public:
            EventCore::Property<MathCore::vec4f> ClearColor;
            EventCore::Property<CullFaceType> CullFace;
            EventCore::Property<FrontFaceType> FrontFace;
            EventCore::Property<DepthTestType> DepthTest;
            EventCore::Property<float> ClearDepth;
            EventCore::Property<bool> DepthWrite;
            EventCore::Property<BlendModeType> BlendMode;
#ifndef ITK_RPI
            EventCore::Property<AlphaTestType> AlphaTest;
            EventCore::Property<float> AlphaRef;
            EventCore::Property<HintType> LineSmoothHint;
            EventCore::Property<float> PointSize;
            EventCore::Property<WireframeType> Wireframe;
#endif
            EventCore::Property<float> LineWidth;
            EventCore::Property<AppKit::OpenGL::GLShader *> CurrentShader;
            EventCore::Property<iRect> Viewport;
            EventCore::Property<AppKit::OpenGL::GLFramebufferObject *> CurrentFramebufferObject;
            EventCore::Property<ColorWriteType> ColorWrite;

            static GLRenderState *Instance();

            void initialize(int w, int h);
            void releaseResources();
        };

    }

}