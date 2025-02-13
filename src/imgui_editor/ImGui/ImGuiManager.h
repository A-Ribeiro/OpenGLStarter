#pragma once

#include "View/common.h"
#include "View/all.h"

#include "ImGuiMenu.h"
#include "CustomSettings.h"

#include "ShortcutManager.h"
#include "ContextMenu.h"
#include "Dialogs.h"

class InnerViewport;

class ImGuiManager
{
    bool reset_layout;
    std::string base_path;

public:

    ImFont *font_half_size;

    float GlobalScale;
    ImGuiStyle imGuiStyleBackup;
    MathCore::vec2i mainMonitorCenter;
    MathCore::vec2i windowResolution;

    ImGuizmo::Style imGuizmoStyleBackup;

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

    ShortcutManager shortcutManager;

    ContextMenu contextMenu;
    Dialogs dialogs;

    std::vector<View*> views;

    EventCore::Event<void(View*,bool)> OnHover;
    EventCore::Event<void(View*,bool)> OnFocus;

    EventCore::Event<void()> PostAction;


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