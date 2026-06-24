#include "App.h"

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

// #include "CustomComponents/ComponentSpecularMesh.h"
// #include "CustomComponents/ComponentCameraLookToNode.h"

App::App()
{
    // forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();

    Engine::Instance()->window->setMouseCursorVisible(false);

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate
    renderState->ClearColor = vec4f(1.0f, 1.0f, 250.0f / 255.0f, 1.0f);
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

    AppBase::screenRenderWindow->CameraScreenSize.OnChange.add(&App::onCameraScreenSizeChange, this);

    AppBase::screenRenderWindow->inputManager.onMouseEvent.add(&App::OnMouseEvent, this);

    // AppBase::OnMouseDown.add(this, &App::onMouseDown);
    // AppBase::OnMouseUp.add(this, &App::onMouseUp);

    mousePressed = false;

    timer = 0.0f;
    state = 0;

    sceneJesusCross = nullptr;
    sceneGUI = nullptr;
    sceneSplash = nullptr;

    screenRenderWindow->setHandleWindowCloseButtonEnabled(true);
    // screenRenderWindow->setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);

    this->fps_accumulator = App::fps_time_sec;

    mainThread_EventHandlerSet = std::make_shared<EventHandlerSet>();

    screenRenderWindow->inputManager.onWindowEvent.add(
        [this](const AppKit::Window::WindowEvent &evt)
        {
            if (evt.type == AppKit::Window::WindowEventType::Resized)
            {
                screenRenderWindow->viewportScaleFactor = (float)evt.resized.height / 768.0f;
                screenRenderWindow->setWindowViewport(AppKit::GLEngine::iRect(evt.resized.width, evt.resized.height));
            }
        });

    auto window_size = window->getSize();
    screenRenderWindow->viewportScaleFactor = (float)window_size.height / 768.0f;
    screenRenderWindow->setWindowViewport(AppKit::GLEngine::iRect(window_size.width, window_size.height));
    screenRenderWindow->WindowViewport.forceTriggerOnChange();
}

void App::load()
{
    // sceneJesusCross = new SceneJesusCross(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
    // sceneJesusCross->load();

    sceneSplash = SceneBase::CreateShared<SceneSplash>(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
    sceneSplash->load();

    fade = STL_Tools::make_unique<Fade>(&time, mainThread_EventHandlerSet);

    fade->fadeOut(2.0f, nullptr);
    time.update();
}

App::~App()
{
    AppBase::screenRenderWindow->inputManager.onMouseEvent.remove(&App::OnMouseEvent, this);

    sceneJesusCross.reset();
    sceneGUI.reset();
    sceneSplash.reset();
    fade.reset();
    resourceMap.clear();
    resourceHelper.finalize();

    mainThread_EventHandlerSet.reset();
}

void App::draw()
{
    time.update();

    this->fps_accumulator -= time.deltaTime;
    if (this->fps_accumulator < 0)
    {
        this->fps_accumulator = App::fps_time_sec;
        if (time.deltaTime > EPSILON<float>::high_precision)
            printf("%.2f FPS\n", 1.0f / time.deltaTime);
    }

    // set min delta time (the passed time or the time to render at 24fps)
    time.deltaTime = OP<float>::minimum(time.deltaTime, 1.0f / 24.0f);

    SceneBase *scenes[] = {
        (SceneBase *)sceneJesusCross.get(),
        (SceneBase *)sceneGUI.get(),
        (SceneBase *)sceneSplash.get()};

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
    else
    {
        switch (state)
        {
        case 0:
            timer += time.deltaTime;
            if (timer > 3.0f)
            {
                timer = 0.0f;
                state = 1;
                fade->fadeIn(2.0f, nullptr);
            }
            break;
        case 1:
            state = 2;
            fade->fadeOut(2.0f, nullptr);

            sceneJesusCross.reset();
            sceneGUI.reset();
            sceneSplash.reset();

            sceneJesusCross = SceneBase::CreateShared<SceneJesusCross>(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
            sceneJesusCross->load();

            sceneGUI = SceneBase::CreateShared<SceneGUI>(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
            sceneGUI->load();

            break;
        }
    }
}

void App::onGainFocus()
{
    // printf("GainFocus...\n");

    // MousePos = MousePosCenter;//set app state do cursor center
    // moveMouseToScreenCenter();//queue update to screen center

    time.update();
}

void App::onCameraScreenSizeChange(const MathCore::vec2f &value, const MathCore::vec2f &oldValue)
{
    GLRenderState *renderState = GLRenderState::Instance();
    
    renderState->Viewport = screenRenderWindow->WindowViewport.c_val();//AppKit::GLEngine::iRect(value.w, value.h);

    if (sceneGUI != nullptr)
        sceneGUI->resize(value);
}

void App::OnMouseEvent(const AppKit::Window::MouseEvent &evt)
{
    switch (evt.type)
    {
    case AppKit::Window::MouseEventType::ButtonPressed:
        mousePressed = true;
        break;
    case AppKit::Window::MouseEventType::ButtonReleased:
        mousePressed = false;
        break;
    default:
        break;
    }
}

/*
void App::onMouseDown(sf::Mouse::Button button, const MathCore::vec2f &pos) {
    mousePressed = true;
}

void App::onMouseUp(sf::Mouse::Button button, const MathCore::vec2f &pos) {
    mousePressed = false;
}
*/
