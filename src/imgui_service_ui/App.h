#pragma once

#include <InteractiveToolkit/EventCore/HandleCallback.h>

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <backend/imgui_impl_window_gl.h>
#include <backend/imgui_impl_opengl3.h>

#include "ServiceController.h"

class App : public AppBase {
    RenderPipeline renderPipeline;
    Platform::Time time;
    ResourceHelper resourceHelper;
    ResourceMap resourceMap;

    // Our state
    //bool show_demo_window;
    //bool show_another_window;
    //ImVec4 clear_color;

    ServiceController serviceController;
public:

    float GlobalScale;
    ImGuiStyle imGuiStyleBackup;
    MathCore::vec2i mainMonitorCenter;
    MathCore::vec2i windowResolution;

    std::unique_ptr<Fade> fade;

    std::shared_ptr<EventHandlerSet> mainThread_EventHandlerSet;

    App();
    ~App();
    virtual void draw();

    void load();
    void onGainFocus();
    void onLostFocus();
    void onViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue);

    void applyGlobalScale();
};
