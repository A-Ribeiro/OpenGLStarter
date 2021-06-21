#ifndef SceneSplash__h__
#define SceneSplash__h__

#include <mini-gl-engine/SceneBase.h>

class SceneSplash : public GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    openglWrapper::GLTexture *Milky_512_512;

public:
    SceneSplash(
        aRibeiro::PlatformTime *_time,
        GLEngine::RenderPipeline *_renderPipeline,
        GLEngine::ResourceHelper *_resourceHelper);
    virtual ~SceneSplash();
};

#endif
