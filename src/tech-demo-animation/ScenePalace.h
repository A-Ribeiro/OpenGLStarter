#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/Components/ComponentCameraRotateOnTarget.h>

#include <appkit-gl-engine/Components/ComponentSkinnedMesh.h>
#include <appkit-gl-engine/Components/ComponentAnimationMotion.h>
#include <appkit-gl-engine/Components/ComponentThirdPersonPlayerController.h>
#include <appkit-gl-engine/Components/ComponentThirdPersonCamera.h>

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
    
    
    AppKit::GLEngine::Transform* Model_Palace;
    AppKit::GLEngine::Transform* Model_Cross;
    //AppKit::GLEngine::Transform* Model_Guard;
    AppKit::OpenGL::GLTexture *Texture_Guard[3];//diffuse + normal + specular

    AppKit::GLEngine::Components::ComponentSkinnedMesh *skinnedMesh;
    AppKit::GLEngine::Components::ComponentAnimationMotion *animationMotion;
    AppKit::GLEngine::Components::ComponentThirdPersonPlayerController *thirdPersonPlayerController;
    AppKit::GLEngine::Components::ComponentThirdPersonCamera *thirdPersonCamera;

public:
    ScenePalace(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper);
    ~ScenePalace();
    
    virtual void draw();
};
