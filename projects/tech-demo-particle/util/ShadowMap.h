#ifndef shadowmap_h_
#define shadowmap_h_

#include "GLDynamicFBO.h"
#include <mini-gl-engine/GLRenderState.h>
#include <aribeiro/aribeiro.h>

namespace openglWrapper {
    
    class _SSE2_ALIGN_PRE ShadowMapRGBA{

        GLEngine::iRect oldViewport;
        aRibeiro::vec4 oldClearColor;
        float oldClearDepth;
        GLEngine::FrontFaceType oldFrontFace;
        #ifndef ARIBEIRO_RPI
        GLEngine::WireframeType oldWireframe;
        #endif
        GLEngine::CullFaceType oldCullFace;

        bool shouldReleaseDepthBuffer;
    public:
        GLDynamicFBO fbo;
        GLTexture depthTexture;
        GLRenderBuffer *realDepthBuffer24;

        aRibeiro::mat4 cameraMatrix;///<to be used in the shadow pass
        aRibeiro::mat4 shadowMatrix;///<to be used in the rendering shaders
        
        void setCameraMatrix(const aRibeiro::mat4 &m) {
            cameraMatrix = m;
            shadowMatrix = aRibeiro::translate(0.5f, 0.5f, 0.5f)*aRibeiro::scale(0.5f,0.5f,0.5f)*m;
        }

        ShadowMapRGBA(int w, int h, GLRenderBuffer *sharedRealDepthBuffer24 = NULL){

            if (sharedRealDepthBuffer24 == NULL){
                realDepthBuffer24 = new GLRenderBuffer();
                realDepthBuffer24->setSize(w,h,GL_DEPTH_COMPONENT24);
                shouldReleaseDepthBuffer = true;
            } else {
                realDepthBuffer24 = sharedRealDepthBuffer24;
                shouldReleaseDepthBuffer = false;
            }

            //the first call, just set the internal variables without any buffer set
            fbo.setSize(w,h);
            //set texture and render buffer size
            depthTexture.setSize(w,h,GL_RGBA);
            fbo.setColorAttachment(&depthTexture,0);
            fbo.setDepthRenderBufferAttachment(realDepthBuffer24);
        }

        virtual ~ShadowMapRGBA() {
            if (shouldReleaseDepthBuffer){
                shouldReleaseDepthBuffer = false;
                aRibeiro::setNullAndDelete(realDepthBuffer24);
            }
            realDepthBuffer24 = NULL;
        }
        
        //setar o save state no primeiro e ultimo
        void saveState() {
            GLEngine::GLRenderState *renderState = GLEngine::GLRenderState::Instance();
            oldViewport = renderState->Viewport;
            oldClearColor = renderState->ClearColor;
            oldClearDepth = renderState->ClearDepth;
            oldFrontFace = renderState->FrontFace;
            #ifndef ARIBEIRO_RPI
            oldWireframe = renderState->Wireframe;
            #endif
            oldCullFace = renderState->CullFace;
        }

        void restoreState() {
            GLEngine::GLRenderState *renderState = GLEngine::GLRenderState::Instance();
            renderState->Viewport = oldViewport;
            renderState->ClearColor = oldClearColor;
            renderState->ClearDepth = oldClearDepth;
            renderState->FrontFace = oldFrontFace;
            #ifndef ARIBEIRO_RPI
            renderState->Wireframe = oldWireframe;
            #endif
            renderState->CullFace = oldCullFace;
        }

        void beginDraw(bool setState){
            GLEngine::GLRenderState *renderState = GLEngine::GLRenderState::Instance();
            renderState->CurrentFramebufferObject = NULL;

            fbo.enable();
            if (setState){
                renderState->Viewport = GLEngine::iRect(fbo.width,fbo.height);
                renderState->ClearColor = aRibeiro::vec4(1.0f);
                renderState->ClearDepth = 1.0f;
                if (oldFrontFace == GLEngine::FrontFaceCW)
                    renderState->FrontFace = GLEngine::FrontFaceCCW;
                else
                    renderState->FrontFace = GLEngine::FrontFaceCW;
                #ifndef ARIBEIRO_RPI
                renderState->Wireframe = GLEngine::WireframeDisabled;
                #endif
                renderState->CullFace = GLEngine::CullFaceBack;
            }

            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        }

        void endDraw(){
            fbo.disable();
        }

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

};


#endif
