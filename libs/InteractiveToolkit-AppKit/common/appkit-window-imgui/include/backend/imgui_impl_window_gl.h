#pragma once
#include <imgui.h>      // IMGUI_IMPL_API

#include <AppKit/window/Window.h>

//void ImGui_WindowGL_InitForOpenGL(AppKit::Window::Window *window);

struct WindowUserData {
    AppKit::Window::GLWindow* window;
    AppKit::Window::InputManager* inputManager;
    static WindowUserData Create(AppKit::Window::GLWindow* window, AppKit::Window::InputManager* inputManager) {
        WindowUserData result;
        result.window = window;
        result.inputManager = inputManager;
        return result;
    }
};



void ImGui_WindowGL_InitForOpenGL(const WindowUserData& windowUserData);
void ImGui_WindowGL_Shutdown();
void ImGui_WindowGL_NewFrame();

