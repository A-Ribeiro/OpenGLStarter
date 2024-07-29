#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/util/Button.h>

#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>

class App;

class Scene3D : public AppKit::GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
public:

    App *app;
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow;

    Scene3D(App *app, std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
    ~Scene3D();

    virtual void draw();

    std::shared_ptr<AppKit::GLEngine::Components::ComponentCameraPerspective> getCamera();
    std::shared_ptr<AppKit::GLEngine::Transform> getRoot();

    void setCamera(std::shared_ptr<AppKit::GLEngine::Components::ComponentCameraPerspective> v);
    void setRoot(std::shared_ptr<AppKit::GLEngine::Transform> v);

    void printHierarchy();

};

