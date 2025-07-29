#include <appkit-gl-engine/GL/GLRenderState.h>

// using namespace aRibeiro;
// using namespace AppKit::OpenGL;

namespace AppKit
{
    namespace GLEngine
    {

        void GLRenderState::OnClearColorChange(const MathCore::vec4f &value, const MathCore::vec4f &oldValue)
        {
            glClearColor(value.r, value.g, value.b, value.a);
        }
        void GLRenderState::OnCullFaceChange(const CullFaceType &value, const CullFaceType &oldValue)
        {
            switch (value)
            {
            case CullFaceNone:
                glDisable(GL_CULL_FACE);
                break;
            case CullFaceFront:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                break;
            case CullFaceBack:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                break;
            }
        }
        void GLRenderState::OnFrontFaceChange(const FrontFaceType &value, const FrontFaceType &oldValue)
        {
            switch (value)
            {
            case FrontFaceCCW:
                glFrontFace(GL_CCW);
                break;
            case FrontFaceCW:
                glFrontFace(GL_CW);
                break;
            }
        }
        void GLRenderState::OnDepthTestChange(const DepthTestType &value, const DepthTestType &oldValue)
        {
            switch (value)
            {
            case DepthTestDisabled:
                glDisable(GL_DEPTH_TEST);
                break;
            case DepthTestEqual:
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_EQUAL);
                break;
            case DepthTestLess:
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                break;
            case DepthTestLessEqual:
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
                break;
            case DepthTestGreater:
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_GREATER);
                break;
            case DepthTestGreaterEqual:
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_GEQUAL);
                break;
            }
        }
        void GLRenderState::OnClearDepthChange(const float &value, const float &oldValue)
        {
#if defined(GLAD_GLES2)
            glClearDepthf(value);
#else
            glClearDepth(value);
#endif
        }
        void GLRenderState::OnDepthWriteChange(const bool &value, const bool &oldValue)
        {
            glDepthMask(value);
        }
        void GLRenderState::OnBlendModeChange(const BlendModeType &value, const BlendModeType &oldValue)
        {
            switch (value)
            {
            case BlendModeDisabled:
                glDisable(GL_BLEND);
                break;
            case BlendModeAlpha:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquation(GL_FUNC_ADD);
                break;
            case BlendModeAdd:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glBlendEquation(GL_FUNC_ADD);
                break;
            case BlendModeAddAlpha:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquation(GL_FUNC_ADD);
                break;
            case BlendModeSubtract:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glBlendEquation(GL_FUNC_SUBTRACT);
                break;
            case BlendModeSubtractAlpha:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquation(GL_FUNC_SUBTRACT);
                break;
            }
        }

#ifndef ITK_RPI
        // void GLRenderState::OnAlphaTestChange(const AlphaTestType &value, const AlphaTestType &oldValue)
        // {
        //     switch (value)
        //     {
        //     case AlphaTestDisabled:
        //         glDisable(GL_ALPHA_TEST);
        //         break;
        //     case AlphaTestEqual:
        //         glEnable(GL_ALPHA_TEST);
        //         glAlphaFunc(GL_EQUAL, AlphaRef.c_val());
        //         break;
        //     case AlphaTestLess:
        //         glEnable(GL_ALPHA_TEST);
        //         glAlphaFunc(GL_LESS, AlphaRef.c_val());
        //         break;
        //     case AlphaTestLessEqual:
        //         glEnable(GL_ALPHA_TEST);
        //         glAlphaFunc(GL_LEQUAL, AlphaRef.c_val());
        //         break;
        //     case AlphaTestGreater:
        //         glEnable(GL_ALPHA_TEST);
        //         glAlphaFunc(GL_GREATER, AlphaRef.c_val());
        //         break;
        //     case AlphaTestGreaterEqual:
        //         glEnable(GL_ALPHA_TEST);
        //         glAlphaFunc(GL_GEQUAL, AlphaRef.c_val());
        //         break;
        //     }
        // }
#endif

#ifndef ITK_RPI
        // void GLRenderState::OnAlphaRefChange(const float &value, const float &oldValue)
        // {
        //     // forward the alpha ref change to another processor
        //     OnAlphaTestChange(AlphaTest,AlphaTest);
        // }
#endif

#ifndef ITK_RPI
        void GLRenderState::OnLineSmoothHintChange(const HintType &value, const HintType &oldValue)
        {
            switch (value)
            {
            case HintDisabled:
                glDisable(GL_LINE_SMOOTH);
                break;
            case HintFastest:
                glEnable(GL_LINE_SMOOTH);
                glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
                break;
            case HintNicest:
                glEnable(GL_LINE_SMOOTH);
                glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
                break;
            }
        }

        void GLRenderState::OnPointSizeChange(const float &value, const float &oldValue)
        {
            glPointSize(value);
        }

        void GLRenderState::OnWireframeChange(const WireframeType &value, const WireframeType &oldValue)
        {
            switch (value)
            {
            case WireframeDisabled:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case WireframeBoth:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case WireframeFront:
                glPolygonMode(GL_FRONT, GL_LINE);
                break;
            case WireframeBack:
                glPolygonMode(GL_BACK, GL_LINE);
                break;
            }
        }

#endif

        void GLRenderState::OnLineWidthChange(const float &value, const float &oldValue)
        {
            glLineWidth(value);
        }

        void GLRenderState::OnCurrentShaderChange(AppKit::OpenGL::GLShader * const &value, AppKit::OpenGL::GLShader * const &oldValue)
        {
            if (value == nullptr)
                AppKit::OpenGL::GLShader::disable();
            else
                value->enable();
        }
        void GLRenderState::OnViewportChange(const iRect &value, const iRect &oldValue)
        {
            glViewport(value.x, value.y, value.w, value.h);
        }
        void GLRenderState::OnCurrentFramebufferObjectChange(AppKit::OpenGL::GLFramebufferObject * const &value, AppKit::OpenGL::GLFramebufferObject * const &oldValue)
        {
            if (value == nullptr)
                AppKit::OpenGL::GLFramebufferObject::disable();
            else
                value->enable();
        }

        void GLRenderState::OnColorWriteChange(const ColorWriteType &value, const ColorWriteType &oldValue)
        {
            ColorWriteType colw = value;
            glColorMask((colw & ColorWriteRed) != 0,
                        (colw & ColorWriteGreen) != 0,
                        (colw & ColorWriteBlue) != 0,
                        (colw & ColorWriteAlpha) != 0);
        }

        GLRenderState *GLRenderState::Instance()
        {
            static GLRenderState renderState;
            return &renderState;
        }

        void GLRenderState::initialize(int w, int h)
        {
            /*
            if (initialized)
                return;
            initialized = true;
            */
            // clear all
            ClearColor.OnChange.clear();
            CullFace.OnChange.clear();
            FrontFace.OnChange.clear();
            DepthTest.OnChange.clear();
            ClearDepth.OnChange.clear();
            DepthWrite.OnChange.clear();
            BlendMode.OnChange.clear();
#ifndef ITK_RPI
            AlphaTest.OnChange.clear();
            AlphaRef.OnChange.clear();
            LineSmoothHint.OnChange.clear();
            PointSize.OnChange.clear();
            Wireframe.OnChange.clear();
#endif
            LineWidth.OnChange.clear();
            CurrentShader.OnChange.clear();
            Viewport.OnChange.clear();
            CurrentFramebufferObject.OnChange.clear();
            ColorWrite.OnChange.clear();

            ClearColor = MathCore::vec4f(0, 0, 0, 1);
            CullFace = CullFaceBack;
            FrontFace = FrontFaceCCW;
            DepthTest = DepthTestLessEqual;
            ClearDepth = 1.0f;
            DepthWrite = true;
            BlendMode = BlendModeAlpha;
#ifndef ITK_RPI
            // AlphaTest = AlphaTestDisabled;
            AlphaTest = AlphaTestGreater;
            AlphaRef = 1.0f / 255.0f;
            // AlphaRef = 1.0f;
            LineSmoothHint = HintDisabled;
            PointSize = 1.0f;
            Wireframe = WireframeDisabled;
#endif
            LineWidth = 1.0f;
            CurrentShader = nullptr;
            Viewport = iRect(w, h);
            CurrentFramebufferObject = nullptr;
            ColorWrite = ColorWriteAll;

            //
            // assign change listeners
            //
            ClearColor.OnChange.add(&GLRenderState::OnClearColorChange, this);
            CullFace.OnChange.add( &GLRenderState::OnCullFaceChange, this);
            FrontFace.OnChange.add( &GLRenderState::OnFrontFaceChange, this);
            DepthTest.OnChange.add( &GLRenderState::OnDepthTestChange, this);
            ClearDepth.OnChange.add( &GLRenderState::OnClearDepthChange, this);
            DepthWrite.OnChange.add( &GLRenderState::OnDepthWriteChange, this);
            BlendMode.OnChange.add( &GLRenderState::OnBlendModeChange, this);
#ifndef ITK_RPI
            // AlphaTest.OnChange.add( &GLRenderState::OnAlphaTestChange, this);
            // AlphaRef.OnChange.add( &GLRenderState::OnAlphaRefChange, this);
            LineSmoothHint.OnChange.add( &GLRenderState::OnLineSmoothHintChange, this);
            PointSize.OnChange.add( &GLRenderState::OnPointSizeChange, this);
            Wireframe.OnChange.add( &GLRenderState::OnWireframeChange, this);
#endif
            LineWidth.OnChange.add( &GLRenderState::OnLineWidthChange, this);
            CurrentShader.OnChange.add( &GLRenderState::OnCurrentShaderChange, this);
            Viewport.OnChange.add(&GLRenderState::OnViewportChange, this);
            CurrentFramebufferObject.OnChange.add(&GLRenderState::OnCurrentFramebufferObjectChange, this);
            ColorWrite.OnChange.add(&GLRenderState::OnColorWriteChange, this);

            //
            // Call all listener to do first setup with the default values
            //
            ClearColor.forceTriggerOnChange();
            CullFace.forceTriggerOnChange();
            FrontFace.forceTriggerOnChange();
            DepthTest.forceTriggerOnChange();
            ClearDepth.forceTriggerOnChange();
            DepthWrite.forceTriggerOnChange();
            BlendMode.forceTriggerOnChange();
#ifndef ITK_RPI
            AlphaTest.forceTriggerOnChange();
            AlphaRef.forceTriggerOnChange();
            LineSmoothHint.forceTriggerOnChange();
            PointSize.forceTriggerOnChange();
            Wireframe.forceTriggerOnChange();
#endif
            LineWidth.forceTriggerOnChange();
            CurrentShader.forceTriggerOnChange();
            Viewport.forceTriggerOnChange();
            CurrentFramebufferObject.forceTriggerOnChange();
            ColorWrite.forceTriggerOnChange();

//
// Not using fixed pipeline lighting
//
#ifndef ITK_RPI
            glDisable(GL_LIGHTING);
#endif
        }

        void GLRenderState::releaseResources()
        {
            CurrentShader = nullptr;
            CurrentFramebufferObject = nullptr;
        }

        GLRenderState::GLRenderState()
        {
            // initialized = false;
            for (int i = 0; i < 32; i++)
                textureUnitActivation[i] = nullptr;
            textureUnitActivationCount = 0;
        }

        void GLRenderState::setTextureUnitActivationArray (OpenGL::VirtualTexture** textureUnitActivation, int size){
            int count = size;
            if (count > 32)
                count = 32;

            for(int i = count; i < textureUnitActivationCount; i++){
                this->textureUnitActivation[i]->deactive(i);
                this->textureUnitActivation[i] = nullptr;
            }
                
            textureUnitActivationCount = count;
            for (int i = 0; i < count; i++) {
                if (this->textureUnitActivation[i] == textureUnitActivation[i])
                    continue;
                // if (this->textureUnitActivation[i] != nullptr)
                //     this->textureUnitActivation[i]->deactive(i);
                this->textureUnitActivation[i] = textureUnitActivation[i];
                this->textureUnitActivation[i]->active(i);
            }
        }

        void GLRenderState::setTextureUnitActivationArray (const OpenGL::VirtualTexture** textureUnitActivation, int size){
            int count = size;
            if (count > 32)
                count = 32;

            for(int i = count; i < textureUnitActivationCount; i++){
                this->textureUnitActivation[i]->deactive(i);
                this->textureUnitActivation[i] = nullptr;
            }
                
            textureUnitActivationCount = count;
            for (int i = 0; i < count; i++) {
                if (this->textureUnitActivation[i] == textureUnitActivation[i])
                    continue;
                // if (this->textureUnitActivation[i] != nullptr)
                //     this->textureUnitActivation[i]->deactive(i);
                this->textureUnitActivation[i] = textureUnitActivation[i];
                this->textureUnitActivation[i]->active(i);
            }
        }

        void GLRenderState::clearTextureUnitActivationArray () {
            for(int i = 0; i < textureUnitActivationCount; i++){
                if (this->textureUnitActivation[i] == nullptr)
                    continue;
                this->textureUnitActivation[i]->deactive(i);
                this->textureUnitActivation[i] = nullptr;
            }
            textureUnitActivationCount = 0;
        }

    }

}
