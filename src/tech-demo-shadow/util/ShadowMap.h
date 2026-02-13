#pragma once

#include <appkit-gl-base/GLDynamicFBO.h>

#include <appkit-gl-engine/GL/GLRenderState.h>

namespace AppKit
{

    namespace OpenGL
    {

        class ShadowMapRGBA
        {

            AppKit::GLEngine::iRect oldViewport;
            MathCore::vec4f oldClearColor;
            float oldClearDepth;
            AppKit::GLEngine::FrontFaceType oldFrontFace;
#ifndef ITK_RPI
            AppKit::GLEngine::WireframeType oldWireframe;
#endif
            AppKit::GLEngine::CullFaceType oldCullFace;

            bool shouldReleaseDepthBuffer;

        public:
            GLDynamicFBO fbo;
            GLTexture depthTexture;
            GLRenderBuffer *realDepthBuffer24;

            MathCore::mat4f cameraMatrix; ///< to be used in the shadow pass
            MathCore::mat4f shadowMatrix; ///< to be used in the rendering shaders

            void setCameraMatrix(const MathCore::mat4f &m)
            {
                cameraMatrix = m;
                shadowMatrix =
                    MathCore::GEN<MathCore::mat4f>::translateHomogeneous(0.5f, 0.5f, 0.5f) *
                    MathCore::GEN<MathCore::mat4f>::scaleHomogeneous(0.5f, 0.5f, 0.5f) *
                    m;
            }

            ShadowMapRGBA(int w, int h, GLRenderBuffer *sharedRealDepthBuffer24 = nullptr)
            {

                if (sharedRealDepthBuffer24 == nullptr)
                {
                    realDepthBuffer24 = new GLRenderBuffer();
                    realDepthBuffer24->setSize(w, h, GL_DEPTH_COMPONENT24);
                    shouldReleaseDepthBuffer = true;
                }
                else
                {
                    realDepthBuffer24 = sharedRealDepthBuffer24;
                    shouldReleaseDepthBuffer = false;
                }

                // the first call, just set the internal variables without any buffer set
                fbo.setSize(w, h);
                // set texture and render buffer size
                depthTexture.setSize(w, h, GL_RGBA);
                fbo.setColorAttachment(&depthTexture, 0);
                fbo.setDepthRenderBufferAttachment(realDepthBuffer24);
            }

            ~ShadowMapRGBA()
            {
                if (shouldReleaseDepthBuffer)
                {
                    shouldReleaseDepthBuffer = false;
                    delete realDepthBuffer24;
                    realDepthBuffer24 = nullptr;
                }
            }

            // setar o save state no primeiro e ultimo
            void saveState()
            {
                AppKit::GLEngine::GLRenderState *renderState = AppKit::GLEngine::GLRenderState::Instance();
                oldViewport = renderState->Viewport;
                oldClearColor = renderState->ClearColor;
                oldClearDepth = renderState->ClearDepth;
                oldFrontFace = renderState->FrontFace;
#ifndef ITK_RPI
                oldWireframe = renderState->Wireframe;
#endif
                oldCullFace = renderState->CullFace;
            }

            void restoreState()
            {
                AppKit::GLEngine::GLRenderState *renderState = AppKit::GLEngine::GLRenderState::Instance();
                renderState->Viewport = oldViewport;
                renderState->ClearColor = oldClearColor;
                renderState->ClearDepth = oldClearDepth;
                renderState->FrontFace = oldFrontFace;
#ifndef ITK_RPI
                renderState->Wireframe = oldWireframe;
#endif
                renderState->CullFace = oldCullFace;
            }

            void beginDraw(bool setState)
            {
                AppKit::GLEngine::GLRenderState *renderState = AppKit::GLEngine::GLRenderState::Instance();
                renderState->CurrentFramebufferObject = nullptr;

                fbo.enable();
                if (setState)
                {
                    renderState->Viewport = AppKit::GLEngine::iRect(fbo.width, fbo.height);
                    renderState->ClearColor = MathCore::vec4f(1.0f);
                    renderState->ClearDepth = 1.0f;
                    if (oldFrontFace == AppKit::GLEngine::FrontFaceCW)
                        renderState->FrontFace = AppKit::GLEngine::FrontFaceCCW;
                    else
                        renderState->FrontFace = AppKit::GLEngine::FrontFaceCW;
#ifndef ITK_RPI
                    renderState->Wireframe = AppKit::GLEngine::WireframeDisabled;
#endif
                    renderState->CullFace = AppKit::GLEngine::CullFaceBack;
                }

                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            }

            void endDraw()
            {
                fbo.disable();
            }
        };

    }

}