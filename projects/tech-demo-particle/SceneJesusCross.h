#ifndef SceneJesusCross__h__
#define SceneJesusCross__h__

#include <mini-gl-engine/SceneBase.h>
#include <mini-gl-engine/ComponentCameraRotateOnTarget.h>

class SceneJesusCross : public GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    
    GLEngine::Transform* Jesus3DModel;
    openglWrapper::GLTexture *JesusTextures[2];//diffuse
    
    GLEngine::Transform* Rocks02_3DModel;
    openglWrapper::GLTexture *Rock02Textures[2];//diffuse + normal
    
    GLEngine::Transform* Rocks03_3DModel;
    openglWrapper::GLTexture *Rock03Textures[2];//diffuse + normal

    openglWrapper::GLTexture *particleTexture;
    
    aRibeiro::PressReleaseDetector keyP;
    aRibeiro::PressReleaseDetector mouseBtn1;

    GLEngine::Components::ComponentCameraRotateOnTarget *componentCameraRotateOnTarget;

    /*
    float distanceRotation;
    aRibeiro::vec3 euler;
    aRibeiro::vec3 lastPosition;
    bool mouseMoving;
    */

public:
    SceneJesusCross(
        aRibeiro::PlatformTime *_time,
        GLEngine::RenderPipeline *_renderPipeline,
        GLEngine::ResourceHelper *_resourceHelper);
    virtual ~SceneJesusCross();
    
    virtual void draw();
};

#endif
