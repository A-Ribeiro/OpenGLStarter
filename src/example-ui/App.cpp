#include "App.h"

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

App::App()
{
    // forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();

#if (!defined(__APPLE__))
    Engine::Instance()->window->setMouseCursorVisible(true);
#endif

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate
    // renderState->ClearColor = vec4f(0.129f, 0.129f, 0.129f, 1.0f);
    renderState->ClearColor = vec4f(1, 1, 1, 1);
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
    // time.update();
    gain_focus = true;

    mainScene = nullptr;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow->setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow->setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);

    fps = 0;
}

void App::load()
{
    mainScene = new MainScene(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow);
    mainScene->load();
}

App::~App()
{
    if (mainScene != nullptr)
    {
        mainScene->unload();
        delete mainScene;
        mainScene = nullptr;
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
    // 1000 hz protection code
    if (time.unscaledDeltaTime <= 1.0f / 1000.0f)
        time.rollback_and_set_zero();
    // 2 hz protection code - for fallback from window move
    if (time.unscaledDeltaTime > 1.0f / 2.0f)
        time.reset();

    fps_timer.update();
    if (fps_timer.unscaledDeltaTime > 1.0f / 2.0f)
        fps_timer.reset();

    if (gain_focus)
    {
        gain_focus = false;
        time.reset();
        fps_timer.reset();
    }

    if (fps_timer.unscaledDeltaTime > EPSILON<float>::high_precision)
        fps = MathCore::OP<float>::lerp(fps, 1.0f / fps_timer.unscaledDeltaTime, 0.1f);

    this->fps_accumulator -= fps_timer.unscaledDeltaTime;
    if (this->fps_accumulator < 0)
    {
        this->fps_accumulator = App::fps_time_sec;
        printf("%.2f FPS\n", fps);
    }

    // set min delta time (the passed time or the time to render at 24fps)
    // time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

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

    // if (Keyboard::isPressed(KeyCode::Escape))
    //     exitApp();

    if (fade->isFading)
        return;
}

void App::onGainFocus()
{
    // time.update();
    gain_focus = true;
}

void App::onViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue)
{
    GLRenderState *renderState = GLRenderState::Instance();
    renderState->Viewport = AppKit::GLEngine::iRect(value.w, value.h);
    if (mainScene != nullptr)
        mainScene->resize(value, oldValue);
}

void App::applySettingsChanges()
{

    auto engine = AppKit::GLEngine::Engine::Instance();
    auto options = AppOptions::OptionsManager::Instance();

    {
        const char *vsyncMode = options->getGroupValueSelectedForKey("Video", "VSync");
        engine->window->glSetVSync(strcmp(vsyncMode, "ON") == 0);
    }

    mainScene->applySettingsChanges();
}