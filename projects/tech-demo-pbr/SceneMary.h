#ifndef SceneMary__h__
#define SceneMary__h__

#include <mini-gl-engine/SceneBase.h>
#include <mini-gl-engine/ComponentCameraRotateOnTarget.h>

class SceneMary : public GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    GLEngine::Transform* Mary3DModel;
    openglWrapper::GLTexture *MaryTextures[2];//diffuse + normal
    
public:
    
    GLEngine::Components::ComponentCameraRotateOnTarget *componentCameraRotateOnTarget;

    SceneMary(
        aRibeiro::PlatformTime *_time,
        GLEngine::RenderPipeline *_renderPipeline,
        GLEngine::ResourceHelper *_resourceHelper);
    virtual ~SceneMary();

    void draw();

    std::string getDescription(){
        return "Virgin Mary\n"
                "Santa Maria Dei Miracoli Church\n"
                "Rome";
    }
    
};

#endif
