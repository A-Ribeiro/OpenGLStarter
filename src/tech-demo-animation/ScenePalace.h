#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/Components/Util/ComponentCameraRotateOnTarget.h>

#include <appkit-gl-engine/Components/Ext/ComponentSkinnedMesh.h>
#include <appkit-gl-engine/Components/Ext/ComponentAnimationMotion.h>
#include <appkit-gl-engine/Components/Util/ComponentThirdPersonPlayerController.h>
#include <appkit-gl-engine/Components/Util/ComponentThirdPersonCamera.h>

/*
namespace AppKit { namespace GLEngine {
    namespace Components {
        class ComponentSkinnedMesh;
        class ComponentAnimationMotion;
        class ComponentThirdPersonPlayerController;
        class ComponentThirdPersonCamera;
    }
}
*/

class ScenePalace : public AppKit::GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    
    std::shared_ptr<AppKit::GLEngine::Transform> Model_Palace;
    //std::shared_ptr<AppKit::GLEngine::Transform> Model_Cross;
    //AppKit::GLEngine::Transform* Model_Guard;
    std::shared_ptr<AppKit::OpenGL::GLTexture> Texture_Guard[3];//diffuse + normal + specular

    std::shared_ptr<AppKit::GLEngine::Components::ComponentSkinnedMesh> skinnedMesh;
    std::shared_ptr<AppKit::GLEngine::Components::ComponentAnimationMotion> animationMotion;
    std::shared_ptr<AppKit::GLEngine::Components::ComponentThirdPersonPlayerController> thirdPersonPlayerController;
    std::shared_ptr<AppKit::GLEngine::Components::ComponentThirdPersonCamera> thirdPersonCamera;

    ScenePalace(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper,
        AppKit::GLEngine::ResourceMap *_resourceMap,
        std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
public:
    friend class AppKit::GLEngine::SceneBase;

    ~ScenePalace();
    
    virtual void draw();
};
