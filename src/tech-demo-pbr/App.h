#pragma once

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

#include "SceneSplash.h"
#include "SceneGUI.h"

#include "SceneMary.h"
#include "SceneJesus.h"

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
    ResourceMap resourceMap;

    //fade aux variables
    float timer;
    int state;

    LoadActionsEnum currentScene;
    
    const float fps_time_sec = 0.5f;
    float fps_accumulator;
public:

    std::shared_ptr<SceneGUI> sceneGUI;
    std::shared_ptr<SceneSplash> sceneSplash;

    std::shared_ptr<AppKit::GLEngine::SceneBase> activeScene;

    std::unique_ptr<Fade> fade;

    std::shared_ptr<EventHandlerSet> mainThread_EventHandlerSet;

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
