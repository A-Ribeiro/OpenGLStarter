#pragma once

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

#include "SceneJesusCross.h"
#include "SceneSplash.h"
#include "SceneGUI.h"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

class App : public AppBase
{
    RenderPipeline renderPipeline;
    Platform::Time time;
    ResourceHelper resourceHelper;
    ResourceMap resourceMap;

    Fade *fade;

    float timer;
    int state;

public:
    SceneJesusCross *sceneJesusCross;
    SceneGUI *sceneGUI;
    SceneSplash *sceneSplash;

    bool mousePressed;

    App();
    ~App();
    virtual void draw();

    // occures after new operator... to guarantee app access to all events on engine...
    void load();

    void onGainFocus();
    void onViewportChange(const iRect &value, const iRect &oldValue);

    void OnMouseEvent(const AppKit::Window::MouseEvent &evt);
};

