#ifndef ScenePalace__h__
#define ScenePalace__h__

#include <mini-gl-engine/SceneBase.h>
#include <mini-gl-engine/ComponentCameraRotateOnTarget.h>

namespace GLEngine {
    namespace Components {
        class SkinnedMesh;
        class AnimationMotion;
        class ThirdPersonPlayerController;
        class ThirdPersonCamera;
    }
}

class ScenePalace : public GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    
    GLEngine::Transform* Model_Palace;
    GLEngine::Transform* Model_Cross;
    //GLEngine::Transform* Model_Guard;
    openglWrapper::GLTexture *Texture_Guard[3];//diffuse + normal + specular

    GLEngine::Components::SkinnedMesh *skinnedMesh;
    GLEngine::Components::AnimationMotion *animationMotion;
    GLEngine::Components::ThirdPersonPlayerController *thirdPersonPlayerController;
    GLEngine::Components::ThirdPersonCamera *thirdPersonCamera;

public:
    ScenePalace(
        aRibeiro::PlatformTime *_time,
        GLEngine::RenderPipeline *_renderPipeline,
        GLEngine::ResourceHelper *_resourceHelper);
    virtual ~ScenePalace();
    
    virtual void draw();
};

#endif
