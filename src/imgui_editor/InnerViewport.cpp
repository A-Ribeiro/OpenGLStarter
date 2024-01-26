#include "InnerViewport.h"
#include "App.h"
#include "SceneGUI.h"


InnerViewport::InnerViewport(App *app, bool createFBO){
    sceneGUI = NULL;

    this->visible = true;
    this->app = app;
    this->fade = new Fade(&app->time, &this->renderWindow);

    renderWindow.Viewport = iRect(100,100,300,200);
    app->screenRenderWindow.addChild(&renderWindow);

    if (createFBO) {
        app->screenRenderWindow.OnUpdate.add(&InnerViewport::OnUpdate, this);
        renderWindow.createFBO();
    }
    else {
        app->screenRenderWindow.OnAfterOverlayDraw.add(&InnerViewport::OnUpdate, this);
    }

    sceneGUI = new SceneGUI(app, &renderWindow);
    sceneGUI->load();
}

InnerViewport::~InnerViewport(){

    if (sceneGUI != NULL){
        sceneGUI->unload();
        delete sceneGUI;
        sceneGUI = NULL;
    }

    app->screenRenderWindow.removeChild(&renderWindow);
    app->screenRenderWindow.OnUpdate.remove(&InnerViewport::OnUpdate, this);
    app->screenRenderWindow.OnAfterOverlayDraw.remove(&InnerViewport::OnUpdate, this);

    if (fade != NULL){
        delete fade;
        fade = NULL;
    }
}

void InnerViewport::setVisible(bool v){
    if (visible != v){
        //printf("visible change...\n");
        visible = v;


        app->screenRenderWindow.OnUpdate.remove(&InnerViewport::OnUpdate, this);
        app->screenRenderWindow.OnAfterOverlayDraw.remove(&InnerViewport::OnUpdate, this);

        if (visible){
            //attach from onUpdate event
            if (renderWindow.fbo!=NULL) {
                app->screenRenderWindow.OnUpdate.add(&InnerViewport::OnUpdate, this);

                //printf("Re-Render Update\n");
                Platform::Time time_aux;
                this->OnUpdate(&time_aux);
            }
            else {
                app->screenRenderWindow.OnAfterOverlayDraw.add(&InnerViewport::OnUpdate, this);
            }
        }

    }
}

void InnerViewport::OnUpdate(Platform::Time *time){

    renderWindow.OnPreUpdate(time);
    renderWindow.OnUpdate(time);
    renderWindow.OnLateUpdate(time);

    // pre process all scene graphs
    /*if (sceneJesusCross != NULL)
        sceneJesusCross->precomputeSceneGraphAndCamera();*/
    if (sceneGUI != NULL)
        sceneGUI->precomputeSceneGraphAndCamera();

    renderWindow.OnAfterGraphPrecompute(time);

    bool isFBO = renderWindow.fbo != NULL;


    GLRenderState *renderState = GLRenderState::Instance();
    
    //renderState->ClearColor = vec4(0.4f, 0.4f, 1.0f, 1.0f);
    renderState->ClearColor = MathCore::vec4f(0.8f, 0.8f, 0.8f, 1.0f);
    
    FrontFaceType old_front_face = renderState->FrontFace;
    DepthTestType old_depth_test = renderState->DepthTest;
    iRect old_viewport = renderState->Viewport;
    
    renderState->FrontFace = FrontFaceCW;
    renderState->DepthTest = DepthTestLess;

    if (isFBO) {
        renderState->Viewport = AppKit::GLEngine::iRect(renderWindow.fbo->width, renderWindow.fbo->height);
        renderWindow.fbo->enable();
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
    else {
        renderState->Viewport = AppKit::GLEngine::iRect(
            renderWindow.Viewport.c_ptr()->x,
            app->screenRenderWindow.Viewport.c_ptr()->h - 1 - renderWindow.Viewport.c_ptr()->y - renderWindow.Viewport.c_ptr()->h,
            renderWindow.Viewport.c_ptr()->w,
            renderWindow.Viewport.c_ptr()->h
        );

        glEnable(GL_SCISSOR_TEST);
        glScissor(renderState->Viewport.c_ptr()->x,
            renderState->Viewport.c_ptr()->y,
            renderState->Viewport.c_ptr()->w,
            renderState->Viewport.c_ptr()->h);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }

    /*if (sceneJesusCross != NULL)
        sceneJesusCross->draw();*/
    if (sceneGUI != NULL)
        sceneGUI->draw();

    fade->draw();
    
    if (isFBO) {
        renderWindow.fbo->disable();
    }

    renderState->Viewport = old_viewport;
    renderState->FrontFace = old_front_face;
    renderState->DepthTest = old_depth_test;

    if (fade->isFading)
        return;
}

void InnerViewport::fadeFromBlack(float time_sec) {
    fade->fadeOut(time_sec, NULL);
}

