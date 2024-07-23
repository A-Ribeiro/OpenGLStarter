#include "InnerViewport.h"
#include "App.h"
#include "SceneGUI.h"
#include "ImGui/ImGuiManager.h"
#include <InteractiveToolkit-Extension/image/PNG.h>

InnerViewport::InnerViewport(App *app, bool createFBO){
    sceneGUI = nullptr;

    renderWindow = RenderWindowRegion::CreateShared();

    this->visible = true;
    this->app = app;
    this->fade = new Fade(&app->time, this->renderWindow);

    renderWindow->WindowViewport = iRect(100,100,300,200);
    renderWindow->viewportScaleFactor = ImGuiManager::Instance()->GlobalScale;
    app->screenRenderWindow->addChild(renderWindow);

    if (createFBO) {
        app->screenRenderWindow->OnUpdate.add(&InnerViewport::OnUpdate, this);
        renderWindow->createFBO();
    }
    else {
        app->screenRenderWindow->OnAfterOverlayDraw.add(&InnerViewport::OnUpdate, this);
    }

    sceneGUI = new SceneGUI(app, renderWindow);
    sceneGUI->load();
}

InnerViewport::~InnerViewport(){

    if (sceneGUI != nullptr){
        sceneGUI->unload();
        delete sceneGUI;
        sceneGUI = nullptr;
    }

    app->screenRenderWindow->removeChild(renderWindow);
    app->screenRenderWindow->OnUpdate.remove(&InnerViewport::OnUpdate, this);
    app->screenRenderWindow->OnAfterOverlayDraw.remove(&InnerViewport::OnUpdate, this);

    if (fade != nullptr){
        delete fade;
        fade = nullptr;
    }
}

void InnerViewport::setVisible(bool v){
    if (visible != v){
        //printf("visible change...\n");
        visible = v;


        app->screenRenderWindow->OnUpdate.remove(&InnerViewport::OnUpdate, this);
        app->screenRenderWindow->OnAfterOverlayDraw.remove(&InnerViewport::OnUpdate, this);

        if (visible){
            //attach from onUpdate event
            if (renderWindow->fbo!=nullptr) {
                app->screenRenderWindow->OnUpdate.add(&InnerViewport::OnUpdate, this);

                //printf("Re-Render Update\n");
                Platform::Time time_aux;
                this->OnUpdate(&time_aux);
            }
            else {
                app->screenRenderWindow->OnAfterOverlayDraw.add(&InnerViewport::OnUpdate, this);
            }
        }

    }
}

void InnerViewport::OnUpdate(Platform::Time *time){

    renderWindow->OnPreUpdate(time);
    renderWindow->OnUpdate(time);
    renderWindow->OnLateUpdate(time);

    // pre process all scene graphs
    /*if (sceneJesusCross != nullptr)
        sceneJesusCross->precomputeSceneGraphAndCamera();*/
    if (sceneGUI != nullptr)
        sceneGUI->precomputeSceneGraphAndCamera();

    renderWindow->OnAfterGraphPrecompute(time);

    bool isFBO = renderWindow->fbo != nullptr;


    GLRenderState *renderState = GLRenderState::Instance();
    
    //renderState->ClearColor = vec4(0.4f, 0.4f, 1.0f, 1.0f);
    renderState->ClearColor = MathCore::vec4f(0.8f, 0.8f, 0.8f, 1.0f);
    // renderState->BlendMode = BlendModeAlpha;
    // renderState->ColorWrite = ColorWriteAll;

    // renderState->BlendMode.forceTriggerOnChange();
    // renderState->ColorWrite.forceTriggerOnChange();
    
    FrontFaceType old_front_face = renderState->FrontFace;
    DepthTestType old_depth_test = renderState->DepthTest;
    iRect old_viewport = renderState->Viewport;
    
    renderState->FrontFace = FrontFaceCW;
    renderState->DepthTest = DepthTestLess;

    if (isFBO) {
        renderState->Viewport = AppKit::GLEngine::iRect(renderWindow->fbo->width, renderWindow->fbo->height);
        renderWindow->fbo->enable();
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
    else {
        auto wViewport = renderWindow->WindowViewport.c_ptr();
        renderState->Viewport = AppKit::GLEngine::iRect(
            wViewport->x,
            app->screenRenderWindow->WindowViewport.c_ptr()->h - 1 - (wViewport->h - 1 + wViewport->y),
            wViewport->w,
            wViewport->h
        );

        glEnable(GL_SCISSOR_TEST);
        glScissor(renderState->Viewport.c_ptr()->x,
            renderState->Viewport.c_ptr()->y,
            renderState->Viewport.c_ptr()->w,
            renderState->Viewport.c_ptr()->h);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }

    /*if (sceneJesusCross != nullptr)
        sceneJesusCross->draw();*/
    if (sceneGUI != nullptr)
        sceneGUI->draw();

    fade->draw();
    
    if (isFBO) {
        // // Debug FBO output
        // {
        //     auto tex = renderWindow.fbo->readPixels();
        //     ITKExtension::Image::PNG::writePNG(
        //         "output.png",
        //         tex.width,
        //         tex.height,
        //         tex.input_component_count,
        //         (char*)tex.data,
        //         false
        //     );
        //     tex.dispose();
        //     Platform::Sleep::sec(5);
        // }

        // clear only the alpha channel - to render correctly in ImGUI
        // renderState->ColorWrite = ColorWriteAlpha;
        // glClear(GL_COLOR_BUFFER_BIT);
        // renderState->ColorWrite = ColorWriteAll;

        renderWindow->fbo->disable();
    }

    renderState->Viewport = old_viewport;
    renderState->FrontFace = old_front_face;
    renderState->DepthTest = old_depth_test;

    if (fade->isFading)
        return;
}

void InnerViewport::fadeFromBlack(float time_sec) {
    fade->fadeOut(time_sec, nullptr);
}

