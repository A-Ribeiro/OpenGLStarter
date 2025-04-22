#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/Components/ComponentCameraRotateOnTarget.h>

class SceneMary : public AppKit::GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    std::shared_ptr<AppKit::GLEngine::Transform> Mary3DModel;
    std::shared_ptr<AppKit::OpenGL::GLTexture> MaryTextures[2];//diffuse + normal
    
public:
    
    std::shared_ptr<AppKit::GLEngine::Components::ComponentCameraRotateOnTarget> componentCameraRotateOnTarget;

    SceneMary(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper,
        AppKit::GLEngine::ResourceMap *_resourceMap,
        std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
    ~SceneMary();

    void draw();

    std::string getDescription(){
        return "Virgin Mary\n"
                "Santa Maria Dei Miracoli Church\n"
                "Rome";
    }
    
};
