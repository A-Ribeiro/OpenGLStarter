#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
#include "App.h"
#include "util/FakeFPS.h"
//#include "CustomComponents/ComponentSpecularMesh.h"
//#include "CustomComponents/ComponentCameraLookToNode.h"

App::App()
{
    //Engine::Instance()->window->setVerticalSyncEnabled(false);

    //forward app reference that could be used by newly created components
    Engine::Instance()->app = this;
    
    resourceHelper.initialize();
    
    //Engine::Instance()->window->setMouseCursorVisible(false);

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate
    renderState->ClearColor = vec4(1.0f,1.0f,250.0f/255.0f,1.0f);
    renderState->FrontFace = FrontFaceCW;

    //renderState->DepthTest = DepthTestLess;
#ifndef ARIBEIRO_RPI


    glDisable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);


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
    AppBase::OnMouseUp.add(this, &App::onMouseUp);

    mousePressed = false;
    
    fade = new Fade(&time);

    fade->fadeOut(2.0f, NULL);
    time.update();

    timer = 0.0f;
    state = 0;

    //sceneJesusCross = NULL;
    //sceneGUI = NULL;
    sceneSplash = NULL;
    scenePalace = NULL;
}

void App::load() {
    //sceneJesusCross = new SceneJesusCross(&time,&renderPipeline,&resourceHelper);
    //sceneJesusCross->load();

    sceneSplash = new SceneSplash(&time,&renderPipeline,&resourceHelper);
    sceneSplash->load();
}

App::~App(){
    
    /*
    if (sceneJesusCross != NULL)
        sceneJesusCross->unload();
    aRibeiro::setNullAndDelete(sceneJesusCross);

    if (sceneGUI != NULL)
        sceneGUI->unload();
    aRibeiro::setNullAndDelete(sceneGUI);
    */

    if (scenePalace != NULL)
        scenePalace->unload();
    aRibeiro::setNullAndDelete(scenePalace);

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

    /*
    static FakeFPS fakeFPS(10.0f);
    fakeFPS.update(time.deltaTime);
    time.deltaTime = fakeFPS.deltaTime;
    */

    StartEventManager::Instance()->processAllComponentsWithTransform();
    
    OnPreUpdate(&time);
    OnUpdate(&time);
    OnLateUpdate(&time);

    // pre process all scene graphs
    /*if (sceneJesusCross != NULL)
        sceneJesusCross->precomputeSceneGraphAndCamera();
    if (sceneGUI != NULL)
        sceneGUI->precomputeSceneGraphAndCamera();*/
    if (sceneSplash != NULL)
        sceneSplash->precomputeSceneGraphAndCamera();
    if (scenePalace != NULL)
        scenePalace->precomputeSceneGraphAndCamera();

    OnAfterGraphPrecompute(&time);

    /*if (sceneJesusCross != NULL)
        sceneJesusCross->draw();
    if (sceneGUI != NULL)
        sceneGUI->draw();*/
    if (sceneSplash != NULL)
        sceneSplash->draw();
    if (scenePalace != NULL)
        scenePalace->draw();
    
    fade->draw();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        exitApp();
    
    if (fade->isFading)
        return;
    else {
        switch (state) {
        case 0:
            timer += time.deltaTime;
            if (timer > 3.0f)
            {
                timer = 0.0f;
                state = 1;

                //time.timeScale = 0.2f;
                fade->fadeIn(2.0f, NULL);
            }
            break;
        case 1:
            state = 2;
            fade->fadeOut(2.0f, NULL);

            /*
            if (sceneJesusCross != NULL)
                sceneJesusCross->unload();
            aRibeiro::setNullAndDelete(sceneJesusCross);

            if (sceneGUI != NULL)
                sceneGUI->unload();
            aRibeiro::setNullAndDelete(sceneGUI);
            */

            if (scenePalace != NULL)
                scenePalace->unload();
            aRibeiro::setNullAndDelete(scenePalace);

            if (sceneSplash != NULL)
                sceneSplash->unload();
            aRibeiro::setNullAndDelete(sceneSplash);

            /*
            sceneJesusCross = new SceneJesusCross(&time,&renderPipeline,&resourceHelper);
            sceneJesusCross->load();

            sceneGUI = new SceneGUI(&time,&renderPipeline,&resourceHelper);
            sceneGUI->load();
            */

            scenePalace = new ScenePalace(&time,&renderPipeline,&resourceHelper);
            scenePalace->load();

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

    //gui->resize(prop->value);
    /*if (sceneGUI != NULL)
        sceneGUI->resize(prop->value);*/
    
}

void App::onMouseDown(sf::Mouse::Button button, const aRibeiro::vec2 &pos) {
    mousePressed = true;
}

void App::onMouseUp(sf::Mouse::Button button, const aRibeiro::vec2 &pos) {
    mousePressed = false;
}

