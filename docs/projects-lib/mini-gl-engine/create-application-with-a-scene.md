# OpenGLStarter

[Back to HOME](../../index)

## Creating an Application With a Scene

There are two classes that helps creating a new application in the __mini-gl-engine__.

The __AppBase__ and the __SceneBase__.

Both classes implements the basic event management and rendering stuff.

### AppBase

The __AppBase__ have all window event delegate like mouse events and keyboard events. You can add a listener to any according the app logic.

You can call the method __exitApp()__ to close the app.

#### AppBase: Input Event List

These events comes from the engine and are related to Mouse and Keyboard.

* __MouseEvent OnMouseDown;__
* __MouseEvent OnMouseUp;__
* __CallEvent OnMouseWheelUp;__
* __CallEvent OnMouseWheelDown;__
* __KeyboardEvent OnKeyDown;__
* __KeyboardEvent OnKeyUp;__

#### AppBase: Window State Event List

These events comes from the engine and are related to the window management.

* __CallEvent OnLostFocus;__
* __CallEvent OnGainFocus;__

#### AppBase: Game Logic Event List

These events comes from the App itself, and can be used by the components.

* __UpdateEvent OnUpdate;__ Called once per frame before __OnLateUpdate__.
* __UpdateEvent OnLateUpdate;__ Called once per frame after __OnUpdate__.
* __UpdateEvent OnAfterGraphPrecompute;__ Called once per frame after precompute all scene graphs in the scene.

#### AppBase: Mouse and Window Size Properties

These properties hold the current value of the window and mouse position state.

You can add a listener to it and check when they are modified.

* __aRibeiro::Property<sf::Vector2i> WindowSize;__ The current window size.
* __aRibeiro::Property<aRibeiro::vec2> MousePos;__ The current mouse pos.
* __aRibeiro::Property<aRibeiro::vec2> MousePosRelatedToCenter;__ Meant to be used with GUI elements.
* __aRibeiro::Property<aRibeiro::vec2> MousePosRelatedToCenterNormalized;__ Meant to be used to implement screen independent mouse move action.


#### AppBase: FPS style movement

These definitions are here to aid implement a FPS(First Person Shooter) like controller:

* __aRibeiro::vec2 MousePosCenter;__ Mouse FPS controller helper
* __sf::Vector2i screenCenterWindowSpace;__ used for FPS like mouse move
* __void moveMouseToScreenCenter();__ Move mouse to the screen center window space

## Inherit AppBase

To use the __AppBase__ you need to inherit it in your App class.

See the example below:

```cpp
#include <mini-gl-engine/mini-gl-engine.h>

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;
using namespace openglWrapper;

class App : public AppBase {
    RenderPipeline renderPipeline;
    PlatformTime time;
    ResourceHelper resourceHelper;

    Fade *fade;
    
    //fade aux variables
    float timer;
    int state;

public:
    // scene list
    GLEngine::SceneBase *activeScene;

    App(){
        //forward app reference that could be used by newly created components
        Engine::Instance()->app = this;
        resourceHelper.initialize();
        GLRenderState *renderState = GLRenderState::Instance();

        //setup renderstate
        renderState->ClearColor = vec4(1.0f,1.0f,250.0f/255.0f,1.0f);
        renderState->FrontFace = FrontFaceCW;
    #ifndef ARIBEIRO_RPI
        renderState->Wireframe = WireframeDisabled;
        renderState->CullFace = CullFaceBack;
    #else
        renderState->CullFace = CullFaceBack;
    #endif
        
        // event listening
        AppBase::OnGainFocus.add(this, &App::onGainFocus);
        AppBase::WindowSize.OnChange.add(this, &App::onWindowSizeChange);
        
        fade = new Fade(&time);
        fade->fadeOut(2.0f, NULL);
        time.update();
        activeScene = NULL;
        renderPipeline.ambientLight.lightMode = AmbientLightMode_SkyBoxCubeTexture;
    }
    virtual ~App(){
        if (activeScene != NULL)
            activeScene->unload();
        aRibeiro::setNullAndDelete(activeScene);

        aRibeiro::setNullAndDelete(fade);        
        resourceHelper.finalize();
    }
    virtual void draw(){
        time.update();

        //set min delta time (the passed time or the time to render at 24fps)
        time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

        StartEventManager::Instance()->processAllComponentsWithTransform();

        OnUpdate(&time);
        OnLateUpdate(&time);

        // pre process all scene graphs
        if (activeScene != NULL)
            activeScene->precomputeSceneGraphAndCamera();

        OnAfterGraphPrecompute(&time);

        if (activeScene != NULL)
            activeScene->draw();

        fade->draw();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            exitApp();
    }

    // occures after new operator... to guarantee app access to all events on engine...
    void load() {
        activeScene = new SceneSplash(&time,&renderPipeline,&resourceHelper);
        sceneSplash->load();
    }

    void onGainFocus(){
        time.update();
    }

    void onWindowSizeChange(Property<sf::Vector2i> *prop) {
        GLRenderState *renderState = GLRenderState::Instance();
        renderState->Viewport = GLEngine::iRect(prop->value.x, prop->value.y);
    }

};
```

## Inherit SceneBase

This base class was created to aid the definition of a 3D or 2D scene.

You need to provide an instance of the timer, render pipeline and resource helper.

The class have the definition of a camera and the root of a scene graph.

The loading process when using this class is divided in three parts (your class need to implement this):

* __loadResources()__ You can load skybox, textures, cubemaps, 3DModels, setup materials textures and fonts here.
* __loadGraph()__ You can create any hierarchy using the __Transform__ class.
* __bindResourcesToGraph()__ You can use the hierarchy created to add components and link the resources to the components also.

Lets define our __SceneSplash__ as an example.

### Define the Main HEADER

```cpp
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
```

### Define the Main Implementation

```cpp
#include "SceneSplash.h"
#include <mini-gl-engine/mini-gl-engine.h>

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;

//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneSplash::loadResources(){
    
    GLEngine::Engine *engine = GLEngine::Engine::Instance();
    
    Milky_512_512 = resourceHelper->createTextureFromFile("resources/milkyway_logo_white.png",true && engine->sRGBCapable);

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

    texRefCount->add(Milky_512_512);

}
//to load the scene graph
void SceneSplash::loadGraph(){
    root = new Transform();

    Transform *t = root->addChild( new Transform() );
    t->Name = "Main Camera";

    t = root->addChild( new Transform() );
    t->Name = "Sprite";
}
//to bind the resources to the current graph
void SceneSplash::bindResourcesToGraph(){

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate
    renderState->ClearColor = vec4(0.0f, 0.0f, 0.0f,1.0f);

    Transform *mainCamera = root->findTransformByName("Main Camera");
    ComponentCameraOrthographic* componentCameraOrthographic;
    mainCamera->addComponent(camera = componentCameraOrthographic = new ComponentCameraOrthographic());
    
    componentCameraOrthographic->useSizeY = true;
    componentCameraOrthographic->sizeY = 512.0f * 2.5f;
    
    Transform *spriteTransform = root->findTransformByName("Sprite");

    ComponentMaterial *material;
    spriteTransform->addComponent(material = new ComponentMaterial());
    spriteTransform->addComponent(ComponentMesh::createPlaneXY(512.0f,512.0f) );

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

    material->type = MaterialUnlitTexture;
    material->unlit.color = vec4(1.0f);
    material->unlit.blendMode = BlendModeAlpha;
    material->unlit.tex = texRefCount->add(Milky_512_512);
}

//clear all loaded scene
void SceneSplash::unloadAll(){
    ResourceHelper::releaseTransformRecursive(&root);

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

    texRefCount->removeNoDelete(Milky_512_512);
    aRibeiro::setNullAndDelete(Milky_512_512);
}

SceneSplash::SceneSplash(
    aRibeiro::PlatformTime *_time,
    GLEngine::RenderPipeline *_renderPipeline,
    GLEngine::ResourceHelper *_resourceHelper) : GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper) {
    
    Milky_512_512 = NULL;

}

SceneSplash::~SceneSplash() {
    unload();
}
```

