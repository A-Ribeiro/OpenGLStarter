#ifndef SceneGUI__h__
#define SceneGUI__h__

#include <mini-gl-engine/SceneBase.h>
#include <mini-gl-engine/Button.h>

class SceneGUI : public GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    //openglWrapper::GLTexture *Milky_512_512;

    openglWrapper::GLFont2Builder fontBuilder;

public:
    GLEngine::Button* button_SoftParticles;


    SceneGUI(
        aRibeiro::PlatformTime *_time,
        GLEngine::RenderPipeline *_renderPipeline,
        GLEngine::ResourceHelper *_resourceHelper);
    virtual ~SceneGUI();

    virtual void draw();

    void resize(const sf::Vector2i &size);
};

#endif
