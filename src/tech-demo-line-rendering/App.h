#pragma once

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

#include "MainScene.h"
#include "SceneGUI.h"

#include <InteractiveToolkit/Platform/Core/ObjectQueue.h>

//using namespace aRibeiro;
using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

class App : public AppBase {
    RenderPipeline renderPipeline;
    Platform::Time time;
    ResourceHelper resourceHelper;
    ResourceMap resourceMap;

    const float fps_time_sec = 0.5f;
    float fps_accumulator;
public:
    Fade *fade;
    MainScene *mainScene;
    Platform::ThreadPool threadPool;
    SceneGUI *sceneGUI;
    Platform::ObjectQueue<EventCore::Callback<void(void)>> executeOnMainThread;


    App();
    ~App();
    virtual void draw();

    void load();
    void onGainFocus();
    void onViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue);
};
