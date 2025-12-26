#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/util/Button.h>

#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

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
    
    Scene3D(App *app, std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
public:
    friend class AppKit::GLEngine::SceneBase;

    App *app;
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow;

    std::shared_ptr<AppKit::GLEngine::Transform> mainCamera;

    EventCore::PressReleaseDetector m_to_move;
    EventCore::PressReleaseDetector mouse_right_btn_to_move;
    EventCore::PressReleaseDetector is_to_hide_mouse;

    MathCore::vec2i savedMouseCoordi;


    ~Scene3D();

    virtual void draw();

    std::shared_ptr<AppKit::GLEngine::Components::ComponentCameraPerspective> getCamera();
    std::shared_ptr<AppKit::GLEngine::Transform> getRoot();

    void setCamera(std::shared_ptr<AppKit::GLEngine::Components::ComponentCameraPerspective> v);
    void setRoot(std::shared_ptr<AppKit::GLEngine::Transform> v);

    void printHierarchy();

    void OnUpdate(Platform::Time *time);

    void setMainCamera(std::shared_ptr<AppKit::GLEngine::Components::ComponentCamera> camera);

    void ensureCameraExists();


};

