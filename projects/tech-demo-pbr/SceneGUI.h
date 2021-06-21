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
    
    openglWrapper::GLTexture *cursorTexture;

    openglWrapper::GLFont2Builder fontBuilder;

public:

    //left
    GLEngine::Button* button_NormalMap;
    GLEngine::Button* button_AmbientLight;
    GLEngine::Button* button_SunLight;
    GLEngine::Button* button_SunLightRotate;

    //right
    GLEngine::Button* button_NextScene;

    GLEngine::Transform* cursorTransform;


    std::vector<GLEngine::Button*> allButtons;

    bool anyButtonSelected();

    SceneGUI(
        aRibeiro::PlatformTime *_time,
        GLEngine::RenderPipeline *_renderPipeline,
        GLEngine::ResourceHelper *_resourceHelper);
    virtual ~SceneGUI();

    virtual void draw();

    void resize(const sf::Vector2i &size);
};

#endif
