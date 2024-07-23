# OpenGLStarter

[Back to HOME](../../index)

## Engine Standard Definitions

The engine is a component based engine.

The class __Engine__ deals with the window initialization and have some shared classes. The instance of the application will be stored here. To set the resolution, you need to call the engine resolution change method.

There are some default classes to keep in mind when is implementing the application:

* __Transform:__ This class holds the transformation information. It implements a tree structure with cache. The components can be added to this class.
* __Component:__ Any component created to interact with needs to inherits this class.
* __StartEventManager:__ This class calls the __start()__ method for each component recently created to all scene graph created with __Transform__.
* __DefaultEngineShader:__ Any shader that interacts with the __ComponentMesh__, __RenderPipeline__ or __FrankenShader__ needs to inherits this and take care of the mesh layout and shader format bitmask.

### Engine

As this class initializes the window, you can use it in your __void main()__ function.

Take a look at the example:

```cpp
#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/Thread.h>

#include "App.h"

static AppBase *CreateAppInstance()
{
    App *app = new App();
    app->load();
    return app;
}

int main(int argc, char* argv[]) {
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    Platform::Thread::staticInitialization();

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    AppKit::GLEngine::EngineWindowConfig engineConfig = AppKit::GLEngine::Engine::CreateDefaultRenderingConfig();

    strcpy(engineConfig.windowConfig.windowName, "Tech Demo: Animation");

    engineConfig.glContextConfig.majorVersion = 2;
    engineConfig.glContextConfig.minorVersion = 1;
    engineConfig.glContextConfig.antialiasingLevel = 1;
    engineConfig.glContextConfig.sRgbCapable = true;

    engineConfig.windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
    engineConfig.windowConfig.videoMode = AppKit::Window::VideoMode(852, 480);

    engine->configureWindow(engineConfig);

    AppKit::Window::VideoMode vm = AppKit::Window::Window::getDesktopVideoMode();
    engine->window->setPosition(
        (MathCore::vec2i(vm.width, vm.height)
        - engine->window->getSize()) / 2
    );

    engine->mainLoop();

    return 0;
}
```

You can query the system available resolutions using the __getResolutionList()__ method.

Every time you call the __setResolution__, the application is destroyed and recreated.

The engine has the application reference.

You can query the engine reference by the singleton method.

The engine instance has some hardware info like: 

* Is sRGB enabled
* Is NVidia Card
* Is AMD Card
* Is Intel Card

There are some reference counters also. One for textures references and other for components references.

To create one application you need to take a look at the [Creating an Application With a Scene](create-application-with-a-scene).

Example:

```cpp
Engine *engine = Engine::Instance();
/*
You can access:

    engine->companyName;
    engine->gameName;

    engine->window;
    engine->app;
            
    engine->textureReferenceCounter;
    engine->componentReferenceCounter;
            
    engine->sRGBCapable;

    engine->isNVidiaCard;
    engine->isAMDCard;
    engine->isIntelCard;
*/

printf("GameName: %s", engine->gameName.c_str());

```

### Transform & Component

The base graph representation of this mini engine is implemented through the __Transform__.

And the behaviour is implemented through the __Component__ .

#### Transform

The __Transform__ class contains ways to store and modify transformations with a __Component__ container and have data cache.

The main way to access or modify the transformation is through the properties.

To generate the final transform matrix, the __Transform__ class apply the transformation in this order:

```cpp
mat4 = GEN<mat4f>::translateHomogeneous(Position) * GEN<mat4f>::fromQuat(quatf(Rotation)) * GEN<mat4f>::scaleHomogeneous(Scale)
```

You can modify the transform by changing the local transformation properties:

* LocalPosition
* LocalEuler
* LocalRotation
* LocalScale

It is possible to set the World properties also:

* Position
* Euler
* Rotation
* Scale

__Notice:__ The world properties multiplies from the current transform until the root transform to get the world transform and set the correct values in this transform. In other worlds: use the Local properties to avoid matrix multiplications when setting global properties.

Example of creating a root node with an element:

```cpp
//the first transform does not affect the transformation. It is used as Root of the container.
Transform *root = new Transform();


Transform *_1st_child_ = root->addChild(Transform::CreateShared());

_1st_child_->Name = std::string("Element 1");
_1st_child_->LocalPosition = MathCore::vec3f(0, 10, 0);
_1st_child_->LocalRotation = MathCore::quatf(0, 0, 0, 1);
_1st_child_->LocalScale = MathCore::vec3f(1, 1, 1);

```

The transform also have the lookat implementation:

* lookAtRightHanded
* lookAtLeftHanded

##### Transform Cache

The main feature of the __Transform__ class is the cache.

If you modify part of the graph, the other part you did not modified will not compute the matrix again.

For this is used a flag called __visited__. When it is true, it will prevent matrix multiplication and returns the latest matrix computed.

Example:

```cpp
Transform root = new Transform();
// create all hierarchy
Transform *_1st_child_ = root->addChild(Transform::CreateShared());
// ...

//
// In the code, you can modify the transform according the logic of your app.
//

//
// To use the cache, you need to call
//   resetVisited and preComputeTransforms
//
// This two calls, will precompute all matrixes
// in the node hierarchy.
//
root->resetVisited();
root->preComputeTransforms();

//
// Now you cannot modify the transforms 
//  And to access the cache you need to use methods
//  with the flag true, indicating you are using cache 
//  instead of compute the matrixes values on-the-fly
//
_1st_child_->getPosition(true);
_1st_child_->getRotation(true);
_1st_child_->getScale(true);
//
// Global matrix does not need to multiply the parent matrixes
//
_1st_child_->getMatrix(true);
_1st_child_->getMatrixInverseTranspose(true);
_1st_child_->getMatrixInverse(true);
_1st_child_->worldToLocalMatrix(true);
_1st_child_->localToWorldMatrix(true);
```

In conjunction with the transform cache, you can use the render cache also.

Every time a __Transform__ is modified, it causes the render matrixes to be recalculated.

After call the __resetVisited__ and __preComputeTransforms__ you can call the __computeRenderMatrix__.

Example:

```cpp
root->resetVisited();
root->preComputeTransforms();
//
// now we are rendering things
//
MathCore::mat4f *mvp;
MathCore::mat4f *mv;
MathCore::mat4f *mvIT;
MathCore::mat4f *mvInv;
_1st_child_->computeRenderMatrix(camera->viewProjection, camera->view, camera->viewIT, camera->viewInv,
                                &mvp, &mv, &mvIT, &mvInv);
//
// You can use the mvp to pass information to the shaders for example.
//
```

##### Transform Events

There are two kinds of events:

* TransformVisitedEvent
    * Used to implement the OnVisited Event. It is called every time a transform is modified on the __preComputeTransforms()__.
    ```cpp
    //from frustum culling implementation
    void ComponentFrustumCulling::OnTransformVisited(Transform *t) {
        computeFinalPositions(true);
    }
    void ComponentFrustumCulling::attachToTransform(Transform *t) {
        t->OnVisited.add(&ComponentFrustumCulling::OnTransformVisited, this);
    }
    void ComponentFrustumCulling::detachFromTransform(Transform *t) {
        t->OnVisited.remove(&ComponentFrustumCulling::OnTransformVisited, this);
    }
    ```
* EventCore::CallbackWrapper
    * Used to implement the tree navigation methods.
    ```cpp
    //from RenderPipeline
    // return true when can skip the child nodes of the current node.
    bool RenderPipeline::traverse_singlepass_render(Transform *element, void* userData) {
        // ...
    }
    root->traversePreOrder_DepthFirst(
        EventCore::CallbackWrapper(&RenderPipeline::traverse_singlepass_render, this),
        camera);
    ```

#### Component

A component is a way to implement any behaviour related to the scene graph.

You can attach a component to any transform, and you can also create new ones.

For example: the render system uses __ComponentMesh__, __ComponentMaterial__ and __ComponentCamera__ to manage the scene graph and render it to the screen.

There are few methods overriding that affects directly the components. They occur like events:

* __virtual void start();__
    * It is called once, when a component is attached for the first time to a __Transform__. If the same component is shared between several __Transform__ objects, it is called just once for the first attached __Transform__.
* __virtual void attachToTransform(Transform *t);__
    * Occurs when the curren __Component__ is attached to a __Transform__
* __virtual void detachFromTransform(Transform *t);__
    * Occurs when the curren __Component__ is detached from a __Transform__

Component class definition example:

```cpp
class ComponentCameraRotateOnTarget :public Component {

    float distanceRotation;
    MathCore::vec3f euler;
    MathCore::vec2f lastPosition;
    bool mouseMoving;

    SharedPointer<Transform> target;

    Transform* getTarget();
    void setTarget(Transform* _transform);

protected:

    void start();
    void OnMouseUp(sf::Mouse::Button button, MathCore::vec2f pos);
    void OnMouseDown(sf::Mouse::Button button, MathCore::vec2f pos);
    void OnNormalizedMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue);

public:

    EventCore::VirtualProperty<Transform*> Target;

    float speedAngle;
    bool enabled;

    static const ComponentType Type;
    
    ComponentCameraRotateOnTarget();
    ~ComponentCameraRotateOnTarget();

};
```

You can attach a __Component__ to a __Transform__ or find components inside a __Transform__.

Take a loot at the example below:

```cpp
Transform *mainCamera = root->findTransformByName("Main Camera");
Transform *toLookNode = root->findTransformByName("RotationPivot");
mainCamera->lookAtLeftHanded(toLookNode);

//componentCameraRotateOnTarget
ComponentCameraRotateOnTarget *componentCameraRotateOnTarget;
mainCamera->addComponent(componentCameraRotateOnTarget = new ComponentCameraRotateOnTarget());
componentCameraRotateOnTarget->Target = toLookNode;

//
// query component inside a transform
//
componentCameraRotateOnTarget = (ComponentCameraRotateOnTarget*)mainCamera->findComponent(ComponentCameraRotateOnTarget::Type);
```

### StartEventManager

This class need to be called once per frame, to execute the __Component__ __start()__ event.

```cpp
//main loop
StartEventManager::Instance()->processAllComponentsWithTransform();
```

### DefaultEngineShader

This class defines how the layout of a vertex stream will be linked to shaders vertex layout.

It is dynamic, but have a default order of definition of attributes.

Example of the FontShader definition:

```cpp
class Unlit_tex_vertcolor_font_PassShader : public DefaultEngineShader{
    int u_mvp;
    int u_texture;
    int u_color;
public:
    Unlit_tex_vertcolor_font_PassShader() {
        //
        // this shader works with meshs that have pos, uv0 and color0 attributes
        //
        format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;
        
        const char vertexShaderCode[] = {
            "attribute vec4 aPosition;"
            "attribute vec3 aUV0;"
            "attribute vec4 aColor0;"
            "uniform mat4 uMVP;"
            "varying vec2 uv;"
            "varying vec4 color;"
            "void main() {"
            "  uv = aUV0.xy;"
            "  color = aColor0;"
            "  gl_Position = uMVP * aPosition;"
            "}" };
        
        const char fragmentShaderCode[] = {
            "varying vec2 uv;"
            "varying vec4 color;"
            "uniform vec4 uColor;"
            "uniform sampler2D uTexture;"
            "void main() {"
            "  vec4 texel = vec4( 1.0,1.0,1.0, texture2D(uTexture, uv).a);"
            "  vec4 result = texel * color * uColor;"
            "  gl_FragColor = result;"
            "}" };
        
        compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
        DefaultEngineShader::setupAttribLocation();
        link(__FILE__, __LINE__);
        
        u_mvp = getUniformLocation("uMVP");
        u_texture = getUniformLocation("uTexture");
        u_color = getUniformLocation("uColor");
    }
    
    void setMVP(const MathCore::mat4f &mvp){
        setUniform(u_mvp, mvp);
    }
    void setTexture(int texunit){
        setUniform(u_texture, texunit);
    }
    void setColor(const MathCore::vec4f &color){
        setUniform(u_color, color);
    }
};
```
