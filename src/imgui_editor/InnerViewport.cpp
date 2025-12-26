#include "InnerViewport.h"
#include "App.h"
#include "SceneGUI.h"
#include "Scene3D.h"
#include "ImGui/ImGuiManager.h"
#include <InteractiveToolkit-Extension/image/PNG.h>

InnerViewport::InnerViewport(App *app, bool createFBO)
{
    sceneGUI = nullptr;
    scene3D = nullptr;

    renderWindow = RenderWindowRegion::CreateShared();

    this->visible = true;
    this->app = app;
    this->fade = std::make_unique<Fade>(&app->time, app->mainThread_EventHandlerSet);

    renderWindow->WindowViewport = iRect(100, 100, 300, 200);
    renderWindow->viewportScaleFactor = ImGuiManager::Instance()->GlobalScale;
    app->screenRenderWindow->addChild(renderWindow);

    if (createFBO)
    {
        app->mainThread_EventHandlerSet->OnUpdate.add(&InnerViewport::OnUpdate, this);
        renderWindow->createFBO();
    }
    else
    {
        app->mainThread_EventHandlerSet->OnAfterOverlayDraw.add(&InnerViewport::OnUpdate, this);
    }

    sceneGUI = SceneBase::CreateShared<SceneGUI>(app, renderWindow);
    sceneGUI->load();

    scene3D = SceneBase::CreateShared<Scene3D>(app, renderWindow);
    scene3D->load();
}

InnerViewport::~InnerViewport()
{

    sceneGUI.reset();
    scene3D.reset();

    app->screenRenderWindow->removeChild(renderWindow);
    app->mainThread_EventHandlerSet->OnUpdate.remove(&InnerViewport::OnUpdate, this);
    app->mainThread_EventHandlerSet->OnAfterOverlayDraw.remove(&InnerViewport::OnUpdate, this);

    fade.reset();
}

void InnerViewport::setVisible(bool v)
{
    if (visible != v)
    {
        // printf("visible change...\n");
        visible = v;

        app->mainThread_EventHandlerSet->OnUpdate.remove(&InnerViewport::OnUpdate, this);
        app->mainThread_EventHandlerSet->OnAfterOverlayDraw.remove(&InnerViewport::OnUpdate, this);

        if (visible)
        {
            // attach from onUpdate event
            if (renderWindow->fbo != nullptr)
            {
                app->mainThread_EventHandlerSet->OnUpdate.add(&InnerViewport::OnUpdate, this);

                // printf("Re-Render Update\n");
                Platform::Time time_aux;
                this->OnUpdate(&time_aux);
            }
            else
            {
                app->mainThread_EventHandlerSet->OnAfterOverlayDraw.add(&InnerViewport::OnUpdate, this);
            }
        }
    }
}

void InnerViewport::OnUpdate(Platform::Time *time)
{

    SceneBase *scenes[] = {
        (SceneBase *)scene3D.get(),
        (SceneBase *)sceneGUI.get()};

    for (auto scene : scenes)
        if (scene != nullptr)
        {
            scene->OnPreUpdate(time);
            scene->OnUpdate(time);
            scene->OnLateUpdate(time);

            // pre process all scene graphs
            /*if (sceneJesusCross != nullptr)
                sceneJesusCross->precomputeSceneGraphAndCamera();*/
            if (scene != nullptr)
                scene->precomputeSceneGraphAndCamera();
            if (scene != nullptr)
                scene->precomputeSceneGraphAndCamera();

            scene->OnAfterGraphPrecompute(time);
        }

    bool isFBO = renderWindow->fbo != nullptr;

    GLRenderState *renderState = GLRenderState::Instance();

    // renderState->ClearColor = vec4(0.4f, 0.4f, 1.0f, 1.0f);
    renderState->ClearColor = MathCore::vec4f(0.8f, 0.8f, 0.8f, 1.0f);
    // renderState->BlendMode = BlendModeAlpha;
    // renderState->ColorWrite = ColorWriteAll;

    // renderState->BlendMode.forceTriggerOnChange();
    // renderState->ColorWrite.forceTriggerOnChange();

    FrontFaceType old_front_face = renderState->FrontFace;
    DepthTestType old_depth_test = renderState->DepthTest;
    iRect old_viewport = renderState->Viewport;

    renderState->FrontFace = FrontFaceCCW;
    renderState->DepthTest = DepthTestLess;

    if (isFBO)
    {
        renderState->Viewport = AppKit::GLEngine::iRect(renderWindow->fbo->width, renderWindow->fbo->height);
        renderWindow->fbo->enable();
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
    else
    {
        auto wViewport = renderWindow->WindowViewport.c_ptr();
        renderState->Viewport = AppKit::GLEngine::iRect(
            wViewport->x,
            app->screenRenderWindow->WindowViewport.c_ptr()->h - 1 - (wViewport->h - 1 + wViewport->y),
            wViewport->w,
            wViewport->h);

        glEnable(GL_SCISSOR_TEST);
        glScissor(renderState->Viewport.c_ptr()->x,
                  renderState->Viewport.c_ptr()->y,
                  renderState->Viewport.c_ptr()->w,
                  renderState->Viewport.c_ptr()->h);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }

    for (auto scene : scenes)
        if (scene != nullptr)
            scene->draw();

    if (fade != nullptr)
        fade->draw();

    if (isFBO)
    {
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

    if (fade != nullptr && fade->isFading)
        return;
}

void InnerViewport::fadeFromBlack(float time_sec)
{
    fade->fadeOut(time_sec, nullptr);
}
