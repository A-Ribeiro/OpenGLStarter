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

    fade = new Fade(&time);

    // fade->fadeOut(5.0f, nullptr);
    time.update();

    mainScene = nullptr;
    sceneGUI = nullptr;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow->setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow->setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);
}

void App::load()
{
    mainScene = new MainScene(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow, false);
    mainScene->load();
    sceneGUI = new SceneGUI(&time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow);
    sceneGUI->load();

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
                            if (mainScene != nullptr)
                            {
                                mainScene->unload();
                                delete mainScene;
                                mainScene = nullptr;
                            }
                            if (sceneGUI->button->rendered_text.compare("View in 2D") == 0)
                            {
                                mainScene = new MainScene(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow, false);
                                mainScene->load();
                                sceneGUI->button->updateText("View in 3D");
                            }
                            else
                            {
                                mainScene = new MainScene(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow, true);
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
    if (mainScene != nullptr)
    {
        mainScene->unload();
        delete mainScene;
        mainScene = nullptr;
    }
    if (sceneGUI != nullptr)
    {
        sceneGUI->unload();
        delete sceneGUI;
        sceneGUI = nullptr;
    }
    if (fade != nullptr)
    {
        delete fade;
        fade = nullptr;
    }
    resourceMap.clear();
    resourceHelper.finalize();
}

void App::draw()
{
    while (executeOnMainThread.size() > 0)
    {
        auto callback = executeOnMainThread.dequeue(nullptr, true);
        callback();
    }
    time.update();

    // set min delta time (the passed time or the time to render at 24fps)
    // time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

    StartEventManager::Instance()->processAllComponentsWithTransform();

    screenRenderWindow->OnPreUpdate(&time);
    screenRenderWindow->OnUpdate(&time);
    screenRenderWindow->OnLateUpdate(&time);

    // pre process all scene graphs
    if (mainScene != nullptr)
        mainScene->precomputeSceneGraphAndCamera();
    if (sceneGUI != nullptr)
        sceneGUI->precomputeSceneGraphAndCamera();

    screenRenderWindow->OnAfterGraphPrecompute(&time);

    if (mainScene != nullptr)
        mainScene->draw();
    if (sceneGUI != nullptr)
        sceneGUI->draw();

    fade->draw();

    if (Keyboard::isPressed(KeyCode::Escape))
        exitApp();

    if (fade->isFading)
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
