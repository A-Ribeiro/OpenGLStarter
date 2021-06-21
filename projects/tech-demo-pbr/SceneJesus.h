#ifndef SceneJesus__h__
#define SceneJesus__h__

#include <mini-gl-engine/SceneBase.h>
#include <mini-gl-engine/ComponentCameraRotateOnTarget.h>

class SceneJesus : public GLEngine::SceneBase {
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
    openglWrapper::GLTexture *JesusTextures[2];//diffuse + normal
    
public:
    GLEngine::Components::ComponentCameraRotateOnTarget *componentCameraRotateOnTarget;

    SceneJesus(
        aRibeiro::PlatformTime *_time,
        GLEngine::RenderPipeline *_renderPipeline,
        GLEngine::ResourceHelper *_resourceHelper);
    virtual ~SceneJesus();

    void draw();

    std::string getDescription(){
        return "Sacred Heart of Jesus\n"
                "Santa Maria Dei Miracoli Church\n"
                "Rome";
    }
    
};

#endif
