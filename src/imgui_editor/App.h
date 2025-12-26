#pragma once

#include <InteractiveToolkit/EventCore/HandleCallback.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

#include <appkit-gl-engine/ResourceMap.h>

//using namespace aRibeiro;
using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <backend/imgui_impl_window_gl.h>
#include <backend/imgui_impl_opengl3.h>

//#include "ServiceController.h"

class App : public AppBase {
    //ServiceController serviceController;
public:
    
    RenderPipeline renderPipeline;
    Platform::Time time;
    ResourceHelper resourceHelper;
    ResourceMap resourceMap;
    
    std::unique_ptr<Fade> fade;

    // EventCore::PressReleaseDetector CtrlO_Detector;
    EventCore::PressReleaseDetector CtrlQ_Detector;
    EventCore::PressReleaseDetector CtrlS_Detector;

    std::shared_ptr<EventHandlerSet> mainThread_EventHandlerSet;

    App();
    ~App();
    virtual void draw();

    void load();
    void onGainFocus();
    void onLostFocus();
    //void onWindowSizeChange(Property<MathCore::vec2i> *prop);
};
