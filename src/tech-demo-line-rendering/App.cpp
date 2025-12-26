#include "App.h"

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

App::App() : executeOnMainThread(false)
{
    // forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();

#if (!defined(__APPLE__))
    Engine::Instance()->window->setMouseCursorVisible(true);
#endif

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate
    renderState->ClearColor = vec4f(0.129f, 0.129f, 0.129f, 1.0f);
    renderState->FrontFace = FrontFaceCCW;
#ifndef ITK_RPI
    // renderState->Wireframe = WireframeBack;
    // renderState->CullFace = CullFaceNone;

    renderState->Wireframe = WireframeDisabled;
    renderState->CullFace = CullFaceBack;
#else
    renderState->CullFace = CullFaceBack;
#endif

    AppBase::OnGainFocus.add(&App::onGainFocus, this);
    AppBase::screenRenderWindow->CameraViewport.OnChange.add(&App::onViewportChange, this);

    // fade->fadeOut(5.0f, nullptr);
    time.update();

    mainScene = nullptr;
    sceneGUI = nullptr;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow->setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow->setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);

    mainThread_EventHandlerSet = std::make_shared<EventHandlerSet>();
}

void App::load()
{
    mainScene = SceneBase::CreateShared<MainScene>(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow, false);
    mainScene->load();
    sceneGUI = SceneBase::CreateShared<SceneGUI>(&time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow);
    sceneGUI->load();

    fade = STL_Tools::make_unique<Fade>(&time, mainThread_EventHandlerSet);

    screenRenderWindow->inputManager.onMouseEvent.add(
        [this](const AppKit::Window::MouseEvent &evt)
        {
            if (sceneGUI == nullptr)
                return;
            if (evt.type == AppKit::Window::MouseEventType::ButtonPressed &&
                evt.button == AppKit::Window::Devices::MouseButton::Left)
            {
                executeOnMainThread.enqueue( //
                    [this]()
                    {
                        if (sceneGUI->button->selected)
                        {
                            mainScene.reset();
                            if (sceneGUI->button->rendered_text.compare("View in 2D") == 0)
                            {
                                mainScene = SceneBase::CreateShared<MainScene>(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow, false);
                                mainScene->load();
                                sceneGUI->button->updateText("View in 3D");
                            }
                            else
                            {
                                mainScene = SceneBase::CreateShared<MainScene>(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow, true);
                                mainScene->load();
                                sceneGUI->button->updateText("View in 2D");
                            }
                        }
                    });
            }
        });
}

App::~App()
{
    screenRenderWindow->inputManager.onMouseEvent.clear();
    mainScene.reset();
    sceneGUI.reset();
    fade.reset();
    resourceMap.clear();
    resourceHelper.finalize();

    mainThread_EventHandlerSet.reset();
}

void App::draw()
{
    while (executeOnMainThread.size() > 0)
    {
        auto callback = executeOnMainThread.dequeue(nullptr, true);
        callback();
    }
    time.update();
    this->fps_accumulator -= time.deltaTime;
    if (this->fps_accumulator < 0)
    {
        this->fps_accumulator = App::fps_time_sec;
        if (time.deltaTime > EPSILON<float>::high_precision)
            printf("%.2f FPS\n", 1.0f / time.deltaTime);
    }

    // set min delta time (the passed time or the time to render at 24fps)
    // time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

    SceneBase *scenes[] = {
        (SceneBase *)mainScene.get(),
        (SceneBase *)sceneGUI.get()};

    for (auto scene : scenes)
        if (scene != nullptr)
        {
            scene->startEventManager.processAllComponentsWithTransform();

            scene->OnPreUpdate(&time);
            scene->OnUpdate(&time);
            scene->OnLateUpdate(&time);

            // pre process all scene graphs
            scene->precomputeSceneGraphAndCamera();

            scene->OnAfterGraphPrecompute(&time);
        }

    mainThread_EventHandlerSet->OnUpdate(&time);

    for (auto scene : scenes)
        if (scene != nullptr)
            scene->draw();

    if (fade != nullptr)
        fade->draw();

    if (Keyboard::isPressed(KeyCode::Escape))
        exitApp();

    if (fade != nullptr && fade->isFading)
        return;
}

void App::onGainFocus()
{
    time.update();
}

void App::onViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue)
{
    GLRenderState *renderState = GLRenderState::Instance();
    renderState->Viewport = AppKit::GLEngine::iRect(value.w, value.h);
    if (mainScene != nullptr)
        mainScene->resize(vec2i(value.w, value.h));
    if (sceneGUI != nullptr)
        sceneGUI->resize(vec2i(value.w, value.h));
}
