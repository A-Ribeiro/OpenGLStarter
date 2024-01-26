#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/Components/ComponentCameraRotateOnTarget.h>

class SceneJesusCross : public AppKit::GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    AppKit::GLEngine::Transform* Jesus3DModel;
    AppKit::OpenGL::GLTexture *JesusTextures[2];//diffuse
    
    AppKit::GLEngine::Components::ComponentCameraRotateOnTarget *componentCameraRotateOnTarget;

public:

    AppKit::GLEngine::Components::ComponentLight *light;

    SceneJesusCross(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper);
    ~SceneJesusCross();
    
    virtual void draw();
};
