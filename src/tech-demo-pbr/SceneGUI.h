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
    
    std::shared_ptr<AppKit::OpenGL::GLTexture> cursorTexture;

    AppKit::OpenGL::GLFont2Builder fontBuilder;

    SceneGUI(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper,
        AppKit::GLEngine::ResourceMap *_resourceMap,
        std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
public:
    friend class AppKit::GLEngine::SceneBase;

    //left
    AppKit::GLEngine::Button* button_NormalMap;
    AppKit::GLEngine::Button* button_AmbientLight;
    AppKit::GLEngine::Button* button_SunLight;
    AppKit::GLEngine::Button* button_SunLightRotate;

    //right
    AppKit::GLEngine::Button* button_NextScene;
    std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> componentFontToMesh;
    std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> fps;
    float f_fps;

    std::shared_ptr<AppKit::GLEngine::Transform> cursorTransform;


    std::vector<AppKit::GLEngine::Button*> allButtons;

    bool anyButtonSelected();

    ~SceneGUI();

    virtual void draw();

    void resize(const MathCore::vec2i&size);

    void setText(const std::string &text);
};
