#include "DepthRenderer.h"

#include <mini-gl-engine/ComponentMaterial.h>
#include <mini-gl-engine/Engine.h>

namespace GLEngine {

    DepthShader::DepthShader() {
        format = model::CONTAINS_POS;
        
        const char vertexShaderCode[] = {
            "attribute vec4 aPosition;"
            "uniform mat4 uMVP;"
            //"varying vec4 projected_pos;"
            "void main() {"
            //"  projected_pos = uMVP * aPosition;"
            "  gl_Position = uMVP * aPosition;"
            "}" };
        
        const char fragmentShaderCode[] = {
            //"varying vec4 projected_pos;"
            "void main() {"
            //"  gl_FragColor = vec4(projected_pos.z/projected_pos.w * 0.5 + 0.5);"

            // uses this convertion back "  depth = (depth - 0.5) * 2.0;"
            //"  gl_FragColor = vec4(gl_FragCoord.z * 0.5 + 0.5);"

            "  gl_FragColor = vec4(1.0);"
            "}" };
        
        LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
        
        u_mvp = getUniformLocation("uMVP");
    }
    
    void DepthShader::setMVP(const aRibeiro::mat4 &mvp){
        setUniform(u_mvp, mvp);
    }



    void DepthRenderer::OnWindowSizeChange(aRibeiro::Property<sf::Vector2i> *prop) {
        int w = prop->value.x;
        int h = prop->value.y;

// RPI 3 does not support depth textures nor float textures...
#ifndef ARIBEIRO_RPI
        if (fbo.depthTexture == NULL)
            depthTexture.setSize(w,h,GL_DEPTH_COMPONENT24);

        fbo.setSize(w,h, GL_RGB, GL_DEPTH_COMPONENT24);
#endif

    }

    bool DepthRenderer::traverse_singlepass_render(Transform *element, void* userData) {
        Components::ComponentMaterial* material = (Components::ComponentMaterial*)element->findComponent(Components::ComponentMaterial::Type);
        if (material == NULL)
            return true;
        
        
        Components::ComponentCamera *camera = (Components::ComponentCamera*)userData;
        
        aRibeiro::mat4 *mvp;
        aRibeiro::mat4 *mv;
        aRibeiro::mat4 *mvIT;
        aRibeiro::mat4 *mvInv;
        element->computeRenderMatrix(camera->viewProjection,
                                    camera->view,
                                    camera->viewIT,
                                    camera->viewInv,
                                    &mvp,
                                    &mv,
                                    &mvIT,
                                    &mvInv);
        
        depthShader.setMVP(*mvp);
    
        //allMeshRender(element, frankenShader);
        for (int i = 0; i < element->getComponentCount(); i++) {
            Component *component = element->getComponentAt(i);
            if (component->compareType(Components::ComponentMesh::Type)) {
                Components::ComponentMesh *mesh = (Components::ComponentMesh*)component;
                mesh->setLayoutPointers(&depthShader);
                mesh->draw();
                mesh->unsetLayoutPointers(&depthShader);
            }
        }
                

        return true;
    }


    DepthRenderer::DepthRenderer() {
        Engine *engine = GLEngine::Engine::Instance();
        AppBase *app = engine->app;

        app->WindowSize.OnChange.add(this, &DepthRenderer::OnWindowSizeChange);

        int w = app->WindowSize.value.x;
        int h = app->WindowSize.value.y;

// RPI 3 does not support depth textures nor float textures...
#ifndef ARIBEIRO_RPI
        depthTexture.setSize(w,h,GL_DEPTH_COMPONENT24);
#endif
        depthTexture.active(0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        depthTexture.deactive(0);


        if ( !engine->isNVidiaCard )
            colorTexture.setSize(w,h,GL_RGB);

        
        fbo.setSize(w,h);
        #ifndef ARIBEIRO_RPI

        fbo.enable();
        fbo.setDepthTextureAttachment(&depthTexture);
        //fbo.setDrawBufferCount();
        //fbo.checkAttachment();

        // avoid allocating dummy color texture when is nvidia card
        if ( !engine->isNVidiaCard ) //&& !engine->isAMDCard )
            fbo.setColorAttachment(&colorTexture,0);
        
        fbo.disable();
        #endif
        

        /*
        fbo.setSize(w,h);
        depthTexture.setSize(w,h,GL_R32F);
        realDepthBuffer24.setSize(w,h,GL_DEPTH_COMPONENT24);
        fbo.setColorAttachment(&depthTexture,0);
        fbo.setDepthRenderBufferAttachment(&realDepthBuffer24);
        */

    }

    DepthRenderer::~DepthRenderer() {
        AppBase *app = GLEngine::Engine::Instance()->app;
        app->WindowSize.OnChange.remove(this, &DepthRenderer::OnWindowSizeChange);
    }

    // dont need another rendering pass...
    void DepthRenderer::method1_copy_from_current_framebuffer(){
        #ifndef ARIBEIRO_RPI
        if (GLEW_EXT_framebuffer_blit) {
            sf::Vector2i screen = GLEngine::Engine::Instance()->app->WindowSize.value;
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);//copy from framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.mFBO);// copy to fbo
            glBlitFramebuffer(0, 0, screen.x, screen.y, 0, 0, fbo.width, fbo.height,
                //GL_COLOR_BUFFER_BIT | 
                GL_DEPTH_BUFFER_BIT,
                GL_NEAREST);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);// back to framebuffer
        }
        else 
        {
            depthTexture.active(0);
            depthTexture.copyFrameBuffer();
            depthTexture.deactive(0);
        }
        #endif
    }

    void DepthRenderer::method2_render_just_depth(Transform*root, Components::ComponentCamera *camera) {

        fbo.enable();

        GLRenderState *state = GLRenderState::Instance();
        state->CurrentShader = &depthShader;

        state->BlendMode = BlendModeDisabled;//material->pbr.blendMode;
        state->DepthTest = DepthTestLessEqual;
        state->ColorWrite = ColorWriteNone;

        aRibeiro::vec4 oldClearColor = state->ClearColor;

        state->ClearColor = aRibeiro::vec4(1.0f);

        //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);


        root->traversePreOrder_DepthFirst(
            TransformTraverseMethod(this, &DepthRenderer::traverse_singlepass_render),
            camera);

        fbo.disable();

        state->ClearColor = oldClearColor;
        state->ColorWrite = ColorWriteAll;

    }
}
