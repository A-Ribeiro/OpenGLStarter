#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/util/Button.h>

class SceneGUI : public AppKit::GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    AppKit::OpenGL::GLFont2Builder fontBuilder;

public:
    AppKit::GLEngine::Button* button;

    SceneGUI(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper,
        AppKit::GLEngine::ResourceMap *_resourceMap,
        std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
    ~SceneGUI();

    virtual void draw();

    void resize(const MathCore::vec2i&size);
};
