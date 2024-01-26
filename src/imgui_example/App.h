#pragma once

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

//using namespace aRibeiro;
using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <backend/imgui_impl_window_gl.h>
#include <backend/imgui_impl_opengl3.h>

class App : public AppBase {
    RenderPipeline renderPipeline;
    Platform::Time time;
    ResourceHelper resourceHelper;

    Fade *fade;

    // Our state
    bool show_demo_window;
    bool show_another_window;
    ImVec4 clear_color;
public:

    App();
    ~App();
    virtual void draw();

    void load();
    void onGainFocus();
    void onLostFocus();
    void onViewportChange(const iRect &value, const iRect &oldValue);
};

