#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>

#include <InteractiveToolkit-Extension/image/PNG.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include <appkit-gl-engine/util/SpriteAtlas.h>

class App;

class GameScene : public AppKit::GLEngine::SceneBase
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

    GameScene(
        App *app,
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper,
        AppKit::GLEngine::ResourceMap *_resourceMap,
        std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
public:
    friend class AppKit::GLEngine::SceneBase;

    static GameScene *currentInstance;

    App *app;

    ITKCommon::Random32 random32;
    MathCore::MathRandomExt<ITKCommon::Random32> mathRandom;

    // std::vector<PlayerInputState> players;

    MathCore::vec2f screen_custom_size;


    ~GameScene();

    virtual void draw();

    void onCameraViewportUpdate(const MathCore::vec2i &viewport_size);

    void update(Platform::Time *elapsed);

    void printHierarchy();

    std::shared_ptr<AppKit::GLEngine::Components::ComponentCamera> getCamera() {
        return camera;
    }
    std::shared_ptr<AppKit::GLEngine::Transform> getRoot() {
        return root;
    }

};
