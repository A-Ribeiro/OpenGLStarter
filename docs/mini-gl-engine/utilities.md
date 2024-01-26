# OpenGLStarter

[Back to HOME](../../index)

## Button

Creates a Button with hover implementation.

You can attach the transform of this button to your current scene.

There is an attribute called __selected__ that tells when the cursor is over the button or not.

You need to feed the mouse position to the button every frame.

Example:

```cpp
void SceneGUI::loadResources(){
    fontBuilder.load("resources/Roboto-Regular-32.basof2");

    button_SoftParticles = new Button( 
        0,// _position, 
        true,// _left, 
        "button_SoftParticles",//_id, 
        "Soft Particles ON",//_text, 
        &fontBuilder//_fontBuilder 
    );
}
void SceneGUI::loadGraph(){
    root = new Transform();
    t = root->addChild( button_SoftParticles->getTransform() );
    // ...
}

void SceneGUI::draw(){
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    MathCore::vec3f pos3D = MathCore::vec3f(engine->app->MousePosRelatedToCenter, 0.0f);

    if (button_SoftParticles != NULL)
        button_SoftParticles->update(pos3D);

    // ...
}

void SceneGUI::resize(const sf::Vector2i &size) {
    if (button_SoftParticles != NULL)
        button_SoftParticles->resize(size);
}

void SceneGUI::onMouseDown(sf::Mouse::Button button, MathCore::vec2f pos) {
    if (button_SoftParticles->selected) {
        // do action
    }
    // ...
}
```

## CubeMapHelper

This class copies one cubemap to another.

The destination cubemap can be with any resolution different from the original cubemap.

You can copy a specific MIP level with this technique.

Example:

```cpp
CubeMapHelper *cubeMapHelper = new CubeMapHelper();

if (sRGB)
    glEnable(GL_FRAMEBUFFER_SRGB);

int targetResolution = 1;
AppKit::OpenGL::GLCubeMap *src;
AppKit::OpenGL::GLCubeMap *target;

//1x1 resolution cubemap copy
cubeMapHelper->copyCubeMap(targetResolution, src, target);
    
if (sRGB)
    glDisable(GL_FRAMEBUFFER_SRGB);
```

## Interpolators

Implementation of a timeline of any structure.

The timeline could have several keys and the values can be interpolated linearly or spherically.

Example:

```cpp
LinearInterpolator<float> alpha;

// time, value
alpha.addKey(Key<float>(0.0f, 0.0f));
alpha.addKey(Key<float>(0.25f, 0.75f));
alpha.addKey(Key<float>(0.75f, 1.0f));

// now get the values
float v;

v = alpha.getValue(0.5f);
v = alpha.getValue(0.6f);
v = alpha.getValue(1.0f);
v = alpha.getValue(2.0f);

```

## ReferenceCounter

The reference counter counts the time you call the __add()__ method related to a pointer.

To decrement the counter, you need to call the __remove()__ method.

When the counter reachs zero, then the class will try to delete the object.

If you do not want to release the memory of the object you can call the __removeNoDelete()__, but the count needs to be one (1) at this time.

You can ask if the object will be deleted through the method __willDeleteOnRemove()__.

## ResourceHelper

This is an agregator class. It uses other classes to do resource creation or maintenance.

You can keep an instance linked with the app class.

You need to call __initialize()__ at the app launching. And __finalize()__ when the app finishes.

### ResourceHelper: Default Material and Textures

* defaultPBRMaterial
* defaultAlbedoTexture
* defaultNormalTexture

### ResourceHelper: Create Methods

The textures can be: _.jpg_ or _.png_.

* AppKit::GLEngine::GLCubeMapSkyBox * createSkybox(const std::string &name, bool sRGB, int maxResolution = 1024);
* AppKit::OpenGL::GLCubeMap * createCubeMap (const std::string &name, bool sRGB, int maxResolution = 1024);
* AppKit::OpenGL::GLTexture * createTextureFromFile(const std::string &path, bool sRGB);
* Transform *createTransformFromModel(const std::string &path);

### ResourceHelper: Tree Methods

* static void releaseTransformRecursive(Transform **root);
* static Transform* cloneTransformRecursive(Transform *root);

### ResourceHelper: Auxiliary Methods

* bool copyCubeMap(AppKit::OpenGL::GLCubeMap *src, AppKit::OpenGL::GLCubeMap *target, bool sRGB, int targetResolution);
* static MathCore::vec4f vec4ColorGammaToLinear(const MathCore::vec4f &v);
* static MathCore::vec4f vec4ColorLinearToGamma(const MathCore::vec4f &v);
* static MathCore::vec3f vec3ColorGammaToLinear(const MathCore::vec3f &v);
* static MathCore::vec3f vec3ColorLinearToGamma(const MathCore::vec3f &v);
* __static void setTexture( AppKit::OpenGL::GLTexture **dst, AppKit::OpenGL::GLTexture *src );__ Updates the texture reference counter and set this new pointer.

