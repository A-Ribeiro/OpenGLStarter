#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
#include "App.h"

App::App()
{
    //forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();
    
    Engine::Instance()->window->setMouseCursorVisible(false);

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate
    renderState->ClearColor = vec4(1.0f,1.0f,250.0f/255.0f,1.0f);
    renderState->FrontFace = FrontFaceCW;
#ifndef ARIBEIRO_RPI
    //renderState->Wireframe = WireframeBack;
    //renderState->CullFace = CullFaceNone;

    renderState->Wireframe = WireframeDisabled;
    renderState->CullFace = CullFaceBack;
#else
    renderState->CullFace = CullFaceBack;
#endif

    AppBase::OnGainFocus.add(this, &App::onGainFocus);
    AppBase::WindowSize.OnChange.add(this, &App::onWindowSizeChange);

    AppBase::OnMouseDown.add(this, &App::onMouseDown);
    
    fade = new Fade(&time);

    fade->fadeOut(2.0f, NULL);
    time.update();

    timer = 0.0f;
    state = 0;

    sceneGUI = NULL;
    sceneSplash = NULL;

    activeScene = NULL;

    currentScene = LoadActions_None;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_SkyBoxCubeTexture;
}

void App::load() {
    //sceneJesusCross = new SceneJesusCross(&time,&renderPipeline,&resourceHelper);
    //sceneJesusCross->load();

    sceneSplash = new SceneSplash(&time,&renderPipeline,&resourceHelper);
    sceneSplash->load();
}

App::~App(){

    if (activeScene != NULL)
        activeScene->unload();
    aRibeiro::setNullAndDelete(activeScene);

    if (sceneGUI != NULL)
        sceneGUI->unload();
    aRibeiro::setNullAndDelete(sceneGUI);

    if (sceneSplash != NULL)
        sceneSplash->unload();
    aRibeiro::setNullAndDelete(sceneSplash);

    aRibeiro::setNullAndDelete(fade);
    
    resourceHelper.finalize();
}

void App::draw() {
    time.update();

    //set min delta time (the passed time or the time to render at 24fps)
    time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

    StartEventManager::Instance()->processAllComponentsWithTransform();
    
    OnPreUpdate(&time);
    OnUpdate(&time);
    OnLateUpdate(&time);

    // pre process all scene graphs
    if (activeScene != NULL)
        activeScene->precomputeSceneGraphAndCamera();
    if (sceneGUI != NULL)
        sceneGUI->precomputeSceneGraphAndCamera();
    if (sceneSplash != NULL)
        sceneSplash->precomputeSceneGraphAndCamera();

    OnAfterGraphPrecompute(&time);

    if (activeScene != NULL)
        activeScene->draw();
    if (sceneGUI != NULL)
        sceneGUI->draw();
    if (sceneSplash != NULL)
        sceneSplash->draw();
    
    fade->draw();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        exitApp();
    
    if (fade->isFading)
        return;
    else {
        switch (state) {
        case 0: // splash wait 3 secs
            timer += time.deltaTime;
            if (timer > 3.0f)
            {
                timer = 0.0f;
                state = 1;
                fade->fadeIn(2.0f, NULL);
            }
            break;
        case 1: // load GUI
            state = 3; // load Mary Scene

            if (activeScene != NULL)
                activeScene->unload();
            aRibeiro::setNullAndDelete(activeScene);

            if (sceneGUI != NULL)
                sceneGUI->unload();
            aRibeiro::setNullAndDelete(sceneGUI);

            if (sceneSplash != NULL)
                sceneSplash->unload();
            aRibeiro::setNullAndDelete(sceneSplash);

            sceneGUI = new SceneGUI(&time, &renderPipeline, &resourceHelper);
            sceneGUI->load();

            break;

        case 2: // load Mary Scene - Pre fade
            state = 3; // load Mary Scene
            fade->fadeIn(2.0f, NULL);
            break;
        case 3: // load Mary Scene
            state = 1000;
            fade->fadeOut(2.0f, NULL);

            if (activeScene != NULL)
                activeScene->unload();
            aRibeiro::setNullAndDelete(activeScene);

            

            activeScene = new SceneMary(&time, &renderPipeline, &resourceHelper);
            activeScene->load();

            currentScene = LoadActions_Mary;
            time.update();//avoid long time advance in fade interpolation
            break;


        case 4: // load Jesus Scene - Pre fade
            state = 5; // load Jesus Scene
            fade->fadeIn(2.0f, NULL);
            break;
        case 5: // load Jesus Scene
            state = 1000;
            fade->fadeOut(2.0f, NULL);

            if (activeScene != NULL)
                activeScene->unload();
            aRibeiro::setNullAndDelete(activeScene);

            

            activeScene = new SceneJesus(&time, &renderPipeline, &resourceHelper);
            activeScene->load();

            currentScene = LoadActions_Jesus;
            time.update();//avoid long time advance in fade interpolation
            break;
        }
    }
}

void App::onGainFocus() {
    //printf("GainFocus...\n");

    //MousePos = MousePosCenter;//set app state do cursor center
    //moveMouseToScreenCenter();//queue update to screen center

    time.update();
}

void App::onWindowSizeChange(Property<sf::Vector2i> *prop) {
    GLRenderState *renderState = GLRenderState::Instance();
    renderState->Viewport = GLEngine::iRect(prop->value.x, prop->value.y);
    
    if (sceneGUI != NULL)
        sceneGUI->resize(prop->value);

}

void App::onMouseDown(sf::Mouse::Button button, const aRibeiro::vec2 &pos) {
    // check Mary and Jesus scene to lock the camera move...
    if (sceneGUI != NULL && activeScene != NULL){

        if (currentScene == LoadActions_Mary) {
            if (sceneGUI->anyButtonSelected())
                ((SceneMary*)activeScene)->componentCameraRotateOnTarget->enabled = false;
            else 
                ((SceneMary*)activeScene)->componentCameraRotateOnTarget->enabled = true;
        } else if (currentScene == LoadActions_Jesus) {
            if (sceneGUI->anyButtonSelected())
                ((SceneJesus*)activeScene)->componentCameraRotateOnTarget->enabled = false;
            else
                ((SceneJesus*)activeScene)->componentCameraRotateOnTarget->enabled = true;
        }


        if (sceneGUI->button_NextScene->selected) {
            if (currentScene == LoadActions_Mary)
                changeScene(LoadActions_Jesus);
            else
                changeScene(LoadActions_Mary);
        }
        else if (sceneGUI->button_NormalMap->selected) {
            renderPipeline.ShaderAlgorithmsEnum_allowedFlags ^= ShaderAlgorithms_NormalMap;
            if (renderPipeline.ShaderAlgorithmsEnum_allowedFlags & ShaderAlgorithms_NormalMap)
                sceneGUI->button_NormalMap->updateText("Normal Map ON");
            else
                sceneGUI->button_NormalMap->updateText("Normal Map OFF");
        }
        else if (sceneGUI->button_AmbientLight->selected) {

            if (renderPipeline.ambientLight.lightMode == AmbientLightMode_SkyBoxCubeTexture) {
                renderPipeline.ambientLight.lightMode = AmbientLightMode_SkyBoxSolidColor;
                sceneGUI->button_AmbientLight->updateText("Ambient Light SolidColor");
            }
            else if (renderPipeline.ambientLight.lightMode == AmbientLightMode_SkyBoxSolidColor) {
                renderPipeline.ambientLight.lightMode = AmbientLightMode_None;
                sceneGUI->button_AmbientLight->updateText("Ambient Light None");
            }
            else if (renderPipeline.ambientLight.lightMode == AmbientLightMode_None) {
                renderPipeline.ambientLight.lightMode = AmbientLightMode_SkyBoxCubeTexture;
                sceneGUI->button_AmbientLight->updateText("Ambient Light Skybox");
            }

        }
        else if (sceneGUI->button_SunLight->selected) {
            uint32_t allSunLight = ShaderAlgorithms_SunLight0 | ShaderAlgorithms_SunLight1 | ShaderAlgorithms_SunLight2 | ShaderAlgorithms_SunLight3;
            renderPipeline.ShaderAlgorithmsEnum_allowedFlags ^= allSunLight;

            if (renderPipeline.ShaderAlgorithmsEnum_allowedFlags & allSunLight)
                sceneGUI->button_SunLight->updateText("Sun Light ON");
            else
                sceneGUI->button_SunLight->updateText("Sun Light OFF");

        }
        else if (sceneGUI->button_SunLightRotate->selected) {
            if (aRibeiro::StringUtil::endsWith(sceneGUI->button_SunLightRotate->rendered_text, "ON"))
                sceneGUI->button_SunLightRotate->updateText("Sun Light Rotate OFF");
            else
                sceneGUI->button_SunLightRotate->updateText("Sun Light Rotate ON");
        }

    }
}


void App::changeScene(LoadActionsEnum scene) {
    
    currentScene = LoadActions_None;

    if (scene == LoadActions_Mary)
        state = 2;
    else if (scene == LoadActions_Jesus)
        state = 4;
}

