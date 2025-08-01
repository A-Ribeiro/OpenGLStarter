#include "App.h"

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

App::App()
{
    //forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();

#if (!defined(__APPLE__))
    Engine::Instance()->window->setMouseCursorVisible(true);
#endif

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate
    renderState->ClearColor = vec4f(0.129f,0.129f,0.129f,1.0f);
    renderState->FrontFace = FrontFaceCCW;
#ifndef ITK_RPI
    //renderState->Wireframe = WireframeBack;
    //renderState->CullFace = CullFaceNone;

    renderState->Wireframe = WireframeDisabled;
    renderState->CullFace = CullFaceBack;
#else
    renderState->CullFace = CullFaceBack;
#endif

    AppBase::OnGainFocus.add(&App::onGainFocus, this);
    AppBase::screenRenderWindow->CameraViewport.OnChange.add(&App::onViewportChange, this);

    fade = new Fade(&time);

    //fade->fadeOut(5.0f, nullptr);
    time.update();

    mainScene = nullptr;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow->setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow->setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);
}

void App::load() {
    mainScene = new MainScene(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow);
    mainScene->load();
}

App::~App(){
    if (mainScene != nullptr){
        mainScene->unload();
        delete mainScene;
        mainScene = nullptr;
    }
    if (fade != nullptr){
        delete fade;
        fade = nullptr;
    }
    resourceMap.clear();
    resourceHelper.finalize();
}

void App::draw() {
    while (executeOnMainThread.size() > 0)
    {
        auto callback = executeOnMainThread.dequeue(nullptr, true);
        callback();
    }
    
    time.update();
    this->fps_accumulator -= time.deltaTime;
    if (this->fps_accumulator < 0){
        this->fps_accumulator = App::fps_time_sec;
        if (time.deltaTime > EPSILON<float>::high_precision)
            printf("%.2f FPS\n", 1.0f / time.deltaTime);
    }

    //set min delta time (the passed time or the time to render at 24fps)
    //time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

    StartEventManager::Instance()->processAllComponentsWithTransform();

    screenRenderWindow->OnPreUpdate(&time);
    screenRenderWindow->OnUpdate(&time);
    screenRenderWindow->OnLateUpdate(&time);

    // pre process all scene graphs
    if (mainScene != nullptr)
        mainScene->precomputeSceneGraphAndCamera();

    screenRenderWindow->OnAfterGraphPrecompute(&time);

    if (mainScene != nullptr)
        mainScene->draw();

    fade->draw();

    if (Keyboard::isPressed(KeyCode::Escape))
        exitApp();

    if (fade->isFading)
        return;
}

void App::onGainFocus() {
    time.update();
}

void App::onViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue) {
    GLRenderState *renderState = GLRenderState::Instance();
    renderState->Viewport = AppKit::GLEngine::iRect(value.w, value.h);
    if (mainScene != nullptr)
        mainScene->resize(vec2i(value.w, value.h));
}
