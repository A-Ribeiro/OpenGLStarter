#include "App.h"

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

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
    AppBase::screenRenderWindow->CameraViewport.OnChange.add(&App::onViewportChange, this);
    AppBase::screenRenderWindow->inputManager.onMouseEvent.add(&App::OnMouseEvent, this);

    // AppBase::OnMouseDown.add(this, &App::onMouseDown);

    timer = 0.0f;
    state = 0;

    sceneGUI = nullptr;
    sceneSplash = nullptr;

    activeScene = nullptr;

    currentScene = LoadActions_None;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_SphereMapTexture; // AmbientLightMode_SkyBoxCubeTexture;

    screenRenderWindow->setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow->setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);

    this->fps_accumulator = App::fps_time_sec;

    mainThread_EventHandlerSet = std::make_shared<EventHandlerSet>();
}

void App::load()
{
    // sceneJesusCross = new SceneJesusCross(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
    // sceneJesusCross->load();

    sceneSplash = SceneBase::CreateShared<SceneSplash>(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
    sceneSplash->load();

    fade = std::make_unique<Fade>(&time, mainThread_EventHandlerSet);

    fade->fadeOut(2.0f, nullptr);
    time.update();
}

App::~App()
{
    mainThread_EventHandlerSet.reset();

    AppBase::OnGainFocus.remove(&App::onGainFocus, this);
    AppBase::screenRenderWindow->CameraViewport.OnChange.remove(&App::onViewportChange, this);
    AppBase::screenRenderWindow->inputManager.onMouseEvent.remove(&App::OnMouseEvent, this);

    activeScene.reset();
    sceneGUI.reset();
    sceneSplash.reset();
    fade.reset();
    resourceMap.clear();
    resourceHelper.finalize();
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
        (SceneBase *)activeScene.get(),
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
        case 0: // splash wait 3 secs
            timer += time.deltaTime;
            if (timer > 3.0f)
            {
                timer = 0.0f;
                state = 1;
                fade->fadeIn(2.0f, nullptr);
            }
            break;
        case 1:        // load GUI
            state = 3; // load Mary Scene

            activeScene.reset();
            sceneGUI.reset();
            sceneSplash.reset();

            sceneGUI = SceneBase::CreateShared<SceneGUI>(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
            sceneGUI->load();

            break;

        case 2:        // load Mary Scene - Pre fade
            state = 3; // load Mary Scene
            fade->fadeIn(2.0f, nullptr);
            break;
        case 3: // load Mary Scene
            state = 1000;
            fade->fadeOut(2.0f, nullptr);

            activeScene.reset();

            activeScene = SceneBase::CreateShared<SceneMary>(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
            activeScene->load();

            sceneGUI->setText(((SceneMary *)activeScene.get())->getDescription());

            currentScene = LoadActions_Mary;
            time.update(); // avoid long time advance in fade interpolation
            break;

        case 4:        // load Jesus Scene - Pre fade
            state = 5; // load Jesus Scene
            fade->fadeIn(2.0f, nullptr);
            break;
        case 5: // load Jesus Scene
            state = 1000;
            fade->fadeOut(2.0f, nullptr);

            activeScene.reset();
            activeScene = SceneBase::CreateShared<SceneJesus>(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
            activeScene->load();

            sceneGUI->setText(((SceneJesus *)activeScene.get())->getDescription());

            currentScene = LoadActions_Jesus;
            time.update(); // avoid long time advance in fade interpolation
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

void App::onViewportChange(const iRect &value, const iRect &oldValue)
{
    GLRenderState *renderState = GLRenderState::Instance();
    renderState->Viewport = AppKit::GLEngine::iRect(value.w, value.h);
    if (sceneGUI != nullptr)
        sceneGUI->resize(vec2i(value.w, value.h));
}

void App::OnMouseEvent(const AppKit::Window::MouseEvent &evt)
{

    switch (evt.type)
    {
    case AppKit::Window::MouseEventType::ButtonPressed:

        // check Mary and Jesus scene to lock the camera move...

        if (sceneGUI != nullptr && activeScene != nullptr)
        {

            /*
            if (currentScene == LoadActions_Mary) {
                if (sceneGUI->anyButtonSelected())
                    ((SceneMary*)activeScene)->componentCameraRotateOnTarget->enabled = false;
                else
                    ((SceneMary*)activeScene)->componentCameraRotateOnTarget->enabled = true;
            }
            else if (currentScene == LoadActions_Jesus) {
                if (sceneGUI->anyButtonSelected())
                    ((SceneJesus*)activeScene)->componentCameraRotateOnTarget->enabled = false;
                else
                    ((SceneJesus*)activeScene)->componentCameraRotateOnTarget->enabled = true;
            }
            */

            if (sceneGUI->button_NextScene->selected)
            {
                if (currentScene == LoadActions_Mary)
                    changeScene(LoadActions_Jesus);
                else
                    changeScene(LoadActions_Mary);
            }
            else if (sceneGUI->button_NormalMap->selected)
            {
                renderPipeline.ShaderAlgorithmsEnum_allowedFlags ^= ShaderAlgorithms_NormalMap;
                if (renderPipeline.ShaderAlgorithmsEnum_allowedFlags & ShaderAlgorithms_NormalMap)
                    sceneGUI->button_NormalMap->updateText("Normal Map ON");
                else
                    sceneGUI->button_NormalMap->updateText("Normal Map OFF");
            }
            else if (sceneGUI->button_AmbientLight->selected)
            {

                if (renderPipeline.ambientLight.lightMode == AmbientLightMode_SphereMapTexture)
                {
                    renderPipeline.ambientLight.lightMode = AmbientLightMode_SkyBoxCubeTexture;
                    sceneGUI->button_AmbientLight->updateText("Ambient Light Skybox");
                }
                else if (renderPipeline.ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture)
                {
                    renderPipeline.ambientLight.lightMode = AmbientLightMode_SkyBoxSolidColor;
                    sceneGUI->button_AmbientLight->updateText("Ambient Light SolidColor");
                }
                else if (renderPipeline.ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor)
                {
                    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;
                    sceneGUI->button_AmbientLight->updateText("Ambient Light None");
                }
                else if (renderPipeline.ambientLight.lightMode == AmbientLightMode_None)
                {
                    renderPipeline.ambientLight.lightMode = AmbientLightMode_SphereMapTexture;
                    sceneGUI->button_AmbientLight->updateText("Ambient Light SphereMap");
                }
            }
            else if (sceneGUI->button_SunLight->selected)
            {
                uint32_t allSunLight = ShaderAlgorithms_SunLight0 | ShaderAlgorithms_SunLight1 | ShaderAlgorithms_SunLight2 | ShaderAlgorithms_SunLight3;
                renderPipeline.ShaderAlgorithmsEnum_allowedFlags ^= allSunLight;

                if (renderPipeline.ShaderAlgorithmsEnum_allowedFlags & allSunLight)
                    sceneGUI->button_SunLight->updateText("Sun Light ON");
                else
                    sceneGUI->button_SunLight->updateText("Sun Light OFF");
            }
            else if (sceneGUI->button_SunLightRotate->selected)
            {
                if (ITKCommon::StringUtil::endsWith(sceneGUI->button_SunLightRotate->rendered_text, "ON"))
                    sceneGUI->button_SunLightRotate->updateText("Sun Light Rotate OFF");
                else
                    sceneGUI->button_SunLightRotate->updateText("Sun Light Rotate ON");
            }
        }

        break;
    default:
        break;
    }
}

void App::changeScene(LoadActionsEnum scene)
{

    currentScene = LoadActions_None;

    if (scene == LoadActions_Mary)
        state = 2;
    else if (scene == LoadActions_Jesus)
        state = 4;
}
