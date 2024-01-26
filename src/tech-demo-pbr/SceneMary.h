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
    
    AppKit::GLEngine::Transform* Mary3DModel;
    AppKit::OpenGL::GLTexture *MaryTextures[2];//diffuse + normal
    
public:
    
    AppKit::GLEngine::Components::ComponentCameraRotateOnTarget *componentCameraRotateOnTarget;

    SceneMary(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper);
    ~SceneMary();

    void draw();

    std::string getDescription(){
        return "Virgin Mary\n"
                "Santa Maria Dei Miracoli Church\n"
                "Rome";
    }
    
};
