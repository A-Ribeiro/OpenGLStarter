#pragma once

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

#include "SceneSplash.h"
#include "SceneGUI.h"

#include "SceneMary.h"
#include "SceneJesus.h"

//using namespace aRibeiro;
using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

enum LoadActionsEnum{
    LoadActions_None,
    LoadActions_Mary,
    LoadActions_Jesus
};

class App : public AppBase {
    RenderPipeline renderPipeline;
    Platform::Time time;
    ResourceHelper resourceHelper;

    Fade *fade;
    
    //fade aux variables
    float timer;
    int state;

    LoadActionsEnum currentScene;
    
public:

    SceneGUI *sceneGUI;
    SceneSplash *sceneSplash;
    AppKit::GLEngine::SceneBase *activeScene;

    App();
    ~App();
    virtual void draw();

    // occures after new operator... to guarantee app access to all events on engine...
    void load();

    void onGainFocus();
    
    void onViewportChange(const iRect &value, const iRect &oldValue);

    void OnMouseEvent(const AppKit::Window::MouseEvent &evt);

    void changeScene(LoadActionsEnum scene);

};
