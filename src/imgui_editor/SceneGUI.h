#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/util/Button.h>

class App;

class SceneGUI : public AppKit::GLEngine::SceneBase
{
protected:
    // to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    // to load the scene graph
    virtual void loadGraph();
    // to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    // clear all loaded scene
    virtual void unloadAll();

    std::shared_ptr<AppKit::OpenGL::GLTexture> cursorTexture;
    AppKit::OpenGL::GLFont2Builder fontBuilder;

    SceneGUI(App *app, std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);

public:
    friend class AppKit::GLEngine::SceneBase;

    std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> fps;
    float f_fps;

    std::shared_ptr<AppKit::GLEngine::Transform> cursorTransform;

    App *app;
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow;

    ~SceneGUI();

    virtual void draw();

    void OnUpdate(Platform::Time *time);

    void OnViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue);
};
