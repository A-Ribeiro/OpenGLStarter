#pragma once

#include "App.h"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;

class SceneGUI;
class App;

class InnerViewport: public EventCore::HandleCallback
{
    bool visible;

public:
    App *app;
    std::shared_ptr<RenderWindowRegion> renderWindow;
    Fade *fade;

    SceneGUI *sceneGUI;

    InnerViewport(App *app, bool createFBO);

    ~InnerViewport();

    void OnUpdate(Platform::Time *time);

    void fadeFromBlack(float time_sec = 0.3f);

    void setVisible(bool v);
};