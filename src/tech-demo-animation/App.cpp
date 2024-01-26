#include "App.h"

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

#include "util/FakeFPS.h"

// #include "CustomComponents/ComponentSpecularMesh.h"
// #include "CustomComponents/ComponentCameraLookToNode.h"

App::App()
{
    // Engine::Instance()->window->setVerticalSyncEnabled(false);

    // forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();

    // Engine::Instance()->window->setMouseCursorVisible(false);

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate
    renderState->ClearColor = vec4f(1.0f, 1.0f, 250.0f / 255.0f, 1.0f);
    renderState->FrontFace = FrontFaceCW;

    // renderState->DepthTest = DepthTestLess;
#ifndef ITK_RPI

    glDisable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

    // renderState->Wireframe = WireframeBack;
    // renderState->CullFace = CullFaceNone;

    renderState->Wireframe = WireframeDisabled;
    renderState->CullFace = CullFaceBack;

#else
    renderState->CullFace = CullFaceBack;
#endif

    AppBase::OnGainFocus.add(&App::onGainFocus, this);
    AppBase::screenRenderWindow.Viewport.OnChange.add(&App::onViewportChange, this);

    AppBase::screenRenderWindow.inputManager.onMouseEvent.add(&App::OnMouseEvent, this);

    // AppBase::OnMouseDown.add(this, &App::onMouseDown);
    // AppBase::OnMouseUp.add(this, &App::onMouseUp);

    mousePressed = false;

    fade = new Fade(&time);

    fade->fadeOut(2.0f, NULL);
    time.update();

    timer = 0.0f;
    state = 0;

    // sceneJesusCross = NULL;
    // sceneGUI = NULL;
    sceneSplash = NULL;
    scenePalace = NULL;

    screenRenderWindow.setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow.setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);
}

void App::load()
{
    // sceneJesusCross = new SceneJesusCross(&time,&renderPipeline,&resourceHelper);
    // sceneJesusCross->load();

    sceneSplash = new SceneSplash(&time, &renderPipeline, &resourceHelper);
    sceneSplash->load();
}

App::~App()
{

    /*
    if (sceneJesusCross != NULL)
        sceneJesusCross->unload();
    delete sceneJesusCross);

    if (sceneGUI != NULL)
        sceneGUI->unload();
    delete sceneGUI);
    */

    if (scenePalace != NULL)
    {
        scenePalace->unload();
        delete scenePalace;
        scenePalace = NULL;
    }

    if (sceneSplash != NULL)
    {
        sceneSplash->unload();
        delete sceneSplash;
        sceneSplash = NULL;
    }

    if (fade != NULL)
    {
        delete fade;
        fade = NULL;
    }

    resourceHelper.finalize();
}

void App::draw()
{
    time.update();

    // set min delta time (the passed time or the time to render at 24fps)
    time.deltaTime = OP<float>::minimum(time.deltaTime, 1.0f / 24.0f);

    /*
    static FakeFPS fakeFPS(10.0f);
    fakeFPS.update(time.deltaTime);
    time.deltaTime = fakeFPS.deltaTime;
    */

    StartEventManager::Instance()->processAllComponentsWithTransform();

    screenRenderWindow.OnPreUpdate(&time);
    screenRenderWindow.OnUpdate(&time);
    screenRenderWindow.OnLateUpdate(&time);

    // pre process all scene graphs
    /*if (sceneJesusCross != NULL)
        sceneJesusCross->precomputeSceneGraphAndCamera();
    if (sceneGUI != NULL)
        sceneGUI->precomputeSceneGraphAndCamera();*/
    if (sceneSplash != NULL)
        sceneSplash->precomputeSceneGraphAndCamera();
    if (scenePalace != NULL)
        scenePalace->precomputeSceneGraphAndCamera();

    screenRenderWindow.OnAfterGraphPrecompute(&time);

    /*if (sceneJesusCross != NULL)
        sceneJesusCross->draw();
    if (sceneGUI != NULL)
        sceneGUI->draw();*/
    if (sceneSplash != NULL)
        sceneSplash->draw();
    if (scenePalace != NULL)
        scenePalace->draw();

    fade->draw();

    if (Keyboard::isPressed(KeyCode::Escape))
        exitApp();

    if (fade->isFading)
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

                // time.timeScale = 0.2f;
                fade->fadeIn(2.0f, NULL);
            }
            break;
        case 1:
            state = 2;
            fade->fadeOut(2.0f, NULL);

            /*
            if (sceneJesusCross != NULL)
                sceneJesusCross->unload();
            delete sceneJesusCross);

            if (sceneGUI != NULL)
                sceneGUI->unload();
            delete sceneGUI);
            */

            if (scenePalace != NULL)
            {
                scenePalace->unload();
                delete scenePalace;
                scenePalace = NULL;
            }

            if (sceneSplash != NULL)
            {
                sceneSplash->unload();
                delete sceneSplash;
                sceneSplash = NULL;
            }

            /*
            sceneJesusCross = new SceneJesusCross(&time,&renderPipeline,&resourceHelper);
            sceneJesusCross->load();

            sceneGUI = new SceneGUI(&time,&renderPipeline,&resourceHelper);
            sceneGUI->load();
            */

            scenePalace = new ScenePalace(&time, &renderPipeline, &resourceHelper);
            scenePalace->load();

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

    // gui->resize(prop->value);
    /*if (sceneGUI != NULL)
        sceneGUI->resize(prop->value);*/
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
