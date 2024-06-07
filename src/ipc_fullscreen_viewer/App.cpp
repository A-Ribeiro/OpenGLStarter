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
    Engine::Instance()->window->setMouseCursorVisible(false);
#endif

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate
    renderState->ClearColor = vec4f(0.0f,0.0f,0.0f,1.0f);
    renderState->FrontFace = FrontFaceCW;
#ifndef ITK_RPI
    //renderState->Wireframe = WireframeBack;
    //renderState->CullFace = CullFaceNone;

    renderState->Wireframe = WireframeDisabled;
    renderState->CullFace = CullFaceBack;
#else
    renderState->CullFace = CullFaceBack;
#endif

    AppBase::OnGainFocus.add(&App::onGainFocus, this);
    AppBase::screenRenderWindow.CameraViewport.OnChange.add(&App::onViewportChange, this);

    fade = new Fade(&time);

    fade->fadeOut(5.0f, NULL);
    time.update();

    mainScene = NULL;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow.setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow.setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);
}

void App::load() {
    mainScene = new MainScene(&time,&renderPipeline,&resourceHelper);
    mainScene->load();

    mainScene->setText(
        "\"For this is how God\n"
        "loved the world :\n\n"

        "he gave his only Son,\n"
        "so that everyone who\n\n"

        "believes in him may not perish\n"
        "but may have eternal life.\"\n\n"

        "John 3:16",
        64.0f, // px
        64.0f // margin/2
    );
}

App::~App(){
    if (mainScene != NULL){
        mainScene->unload();
        delete mainScene;
        mainScene = NULL;
    }
    if (fade != NULL){
        delete fade;
        fade = NULL;
    }
    resourceHelper.finalize();
}

void App::draw() {
    time.update();

    //set min delta time (the passed time or the time to render at 24fps)
    //time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

    StartEventManager::Instance()->processAllComponentsWithTransform();

    screenRenderWindow.OnPreUpdate(&time);
    screenRenderWindow.OnUpdate(&time);
    screenRenderWindow.OnLateUpdate(&time);

    // pre process all scene graphs
    if (mainScene != NULL)
        mainScene->precomputeSceneGraphAndCamera();

    screenRenderWindow.OnAfterGraphPrecompute(&time);

    if (mainScene != NULL)
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
    if (mainScene != NULL)
        mainScene->resize(vec2i(value.w, value.h));
}
