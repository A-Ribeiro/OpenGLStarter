#pragma once

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

#include "MainScene.h"
#include "GameScene.h"

//using namespace aRibeiro;
using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

class App : public AppBase {

    RenderPipeline renderPipeline;

    uint32_t below_min_hz_count;

    Platform::Time time;
    Platform::Time fps_timer;

    ResourceHelper resourceHelper;
    ResourceMap resourceMap;

    bool draw_stats_enabled;
    void drawStats();
public:

    std::unique_ptr<Fade> fade;
    
    std::shared_ptr<MainScene> mainScene;
    std::shared_ptr<GameScene> gameScene;

    Platform::ThreadPool threadPool;
    Platform::ObjectQueue<EventCore::Callback<void(void)>> executeOnMainThread;

    Platform::Semaphore semaphore_aux;


    float fps;
    bool gain_focus;

    App();
    ~App();
    virtual void draw();

    void load();
    void onGainFocus();
    void onViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue);
    void onWindowEvent(const AppKit::Window::WindowEvent &evt);


    void applySettingsChanges();

    void createGameScene();
};
