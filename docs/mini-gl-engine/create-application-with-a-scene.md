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

* __EventCore::Event<void(Platform::Time *time)> OnUpdate;__ Called once per frame before __OnLateUpdate__.
* __EventCore::Event<void(Platform::Time *time)> OnLateUpdate;__ Called once per frame after __OnUpdate__.
* __EventCore::Event<void(Platform::Time *time)> OnAfterGraphPrecompute;__ Called once per frame after precompute all scene graphs in the scene.

#### AppBase: Mouse and Window Size Properties

These properties hold the current value of the window and mouse position state.

You can add a listener to it and check when they are modified.

* __EventCore::Property<sf::Vector2i> WindowSize;__ The current window size.
* __EventCore::Property<MathCore::vec2f> MousePos;__ The current mouse pos.
* __EventCore::Property<MathCore::vec2f> MousePosRelatedToCenter;__ Meant to be used with GUI elements.
* __EventCore::Property<MathCore::vec2f> MousePosRelatedToCenterNormalized;__ Meant to be used to implement screen independent mouse move action.


#### AppBase: FPS style movement

These definitions are here to aid implement a FPS(First Person Shooter) like controller:

* __MathCore::vec2f MousePosCenter;__ Mouse FPS controller helper
* __sf::Vector2i screenCenterWindowSpace;__ used for FPS like mouse move
* __void moveMouseToScreenCenter();__ Move mouse to the screen center window space

## Inherit AppBase

To use the __AppBase__ you need to inherit it in your App class.

See the example below:

```cpp
#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>
#include <appkit-gl-engine/Renderer/Fade.h>

using namespace MathCore;
using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

class App : public AppBase {
    RenderPipeline renderPipeline;
    Platform::Time time;
    ResourceHelper resourceHelper;
    ResourceMap resourceMap;

    Fade *fade;
    
    //fade aux variables
    float timer;
    int state;

public:
    // scene list
    AppKit::GLEngine::SceneBase *activeScene;

    App(){
        //forward app reference that could be used by newly created components
        Engine::Instance()->app = this;
        resourceHelper.initialize();
        GLRenderState *renderState = GLRenderState::Instance();

        //setup renderstate
        renderState->ClearColor = vec4f(1.0f,1.0f,250.0f/255.0f,1.0f);
        renderState->FrontFace = FrontFaceCCW;
    #ifndef ITK_RPI
        renderState->Wireframe = WireframeDisabled;
        renderState->CullFace = CullFaceBack;
    #else
        renderState->CullFace = CullFaceBack;
    #endif
        
        // event listening
        AppBase::OnGainFocus.add(&App::onGainFocus, this);
        AppBase::screenRenderWindow.Viewport.OnChange.add(&App::onViewportChange, this);
        
        fade = new Fade(&time);
        fade->fadeOut(2.0f, nullptr);
        time.update();
        activeScene = nullptr;
        renderPipeline.ambientLight.lightMode = AmbientLightMode_SkyBoxCubeTexture;
    }
    ~App(){
        if (activeScene != nullptr){
            activeScene->unload();
            delete activeScene;
            activeScene = nullptr;
        }

        if (fade != nullptr)
        {
            delete fade;
            fade = nullptr;
        }
        resourceMap.clear();
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
        if (activeScene != nullptr)
            activeScene->precomputeSceneGraphAndCamera();

        OnAfterGraphPrecompute(&time);

        if (activeScene != nullptr)
            activeScene->draw();

        fade->draw();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            exitApp();
    }

    // occures after new operator... to guarantee app access to all events on engine...
    void load() {
        activeScene = new SceneSplash(&time, &renderPipeline, &resourceHelper, &resourceMap, screenRenderWindow);
        sceneSplash->load();
    }

    void onGainFocus(){
        time.update();
    }

    void onViewportChange(const iRect &value, const iRect &oldValue) {
        GLRenderState *renderState = GLRenderState::Instance();
        renderState->Viewport = AppKit::GLEngine::iRect(value.w, value.h);
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
#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>

class SceneSplash : public AppKit::GLEngine::SceneBase {
protected:
    //to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    //to load the scene graph
    virtual void loadGraph();
    //to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    //clear all loaded scene
    virtual void unloadAll();
    
    AppKit::OpenGL::GLTexture *Milky_512_512;

public:
    SceneSplash(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper,
        AppKit::GLEngine::ResourceMap *_resourceMap,
        std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
    ~SceneSplash();
};
```

### Define the Main Implementation

```cpp
#include "SceneSplash.h"
#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneSplash::loadResources(){
    
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    
    Milky_512_512 = resourceHelper->createTextureFromFile("resources/opengl_logo_white.png",true && engine->sRGBCapable);

    ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

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
    renderState->ClearColor = vec4f(0.0f, 0.0f, 0.0f,1.0f);

    Transform *mainCamera = root->findTransformByName("Main Camera");
    ComponentCameraOrthographic* componentCameraOrthographic;
    mainCamera->addComponent(camera = componentCameraOrthographic = new ComponentCameraOrthographic());
    
    componentCameraOrthographic->useSizeY = true;
    componentCameraOrthographic->sizeY = 512.0f * 2.5f;
    
    Transform *spriteTransform = root->findTransformByName("Sprite");

    ComponentMaterial *material;
    spriteTransform->addComponent(material = new ComponentMaterial());
    spriteTransform->addComponent(ComponentMesh::createPlaneXY(512.0f,512.0f) );

    ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    material->type = MaterialUnlitTexture;
    material->unlit.color = vec4f(1.0f);
    material->unlit.blendMode = BlendModeAlpha;
    material->unlit.tex = texRefCount->add(Milky_512_512);
}

//clear all loaded scene
void SceneSplash::unloadAll(){
    ResourceHelper::releaseTransformRecursive(&root);

    ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    if (Milky_512_512 != nullptr){
        texRefCount->removeNoDelete(Milky_512_512);
        delete Milky_512_512;
        Milky_512_512 = nullptr;
    }
}

SceneSplash::SceneSplash(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow)
    
    Milky_512_512 = nullptr;

}

SceneSplash::~SceneSplash() {
    unload();
}
```

