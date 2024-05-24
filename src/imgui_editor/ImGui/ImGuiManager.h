#pragma once

#include "View/common.h"
#include "View/all.h"

#include "ImGuiMenu.h"

class InnerViewport;

class ImGuiManager
{
    bool reset_layout;
    std::string base_path;

public:

    float GlobalScale;
    ImGuiStyle imGuiStyleBackup;

    //ImGuiMenu menu;

    AppKit::OpenGL::GLTexture* icons[(int)IconType::Count];
    bool stretch[(int)IconType::Count];

    InnerViewport *innerViewport;

    Hierarchy hierarchy;
    Project project;
    Console console;
    Scene scene;
    Game game;
    Inspector inspector;

    std::vector<View*> views;

    ImGuiManager();

    static ImGuiManager* Instance();

    void Initialize(
        AppKit::Window::GLWindow* window, 
        AppKit::Window::InputManager* inputManager, 
        const std::string& base_path);

    void Finalize();

    void SaveLayout();

    void LoadLayout();

    void DeleteLayout();

    void ResetLayout();

    void RenderAndLogic(AppKit::Window::GLWindow* window, Platform::Time* time);

    void applyGlobalScale();
};