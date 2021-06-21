# OpenGLStarter

[Back to HOME](../../index)

## OpenGL Abstraction

### SkyBox

This definition uses 6 plain 2D textures to draw a skybox.

Example:

```cpp
#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/mini-gl-engine.h>
using namespace aRibeiro;
using namespace openglWrapper;
using namespace GLEngine;

GLSkyBox *skybox = GLSkyBox(
    true && Engine::Instance()->isRGBCapable, // load sRGB
    "negz.jpg","posz.jpg",
    "negx.jpg","posx.jpg",
    "negy.jpg","posy.jpg",
    100.0f, // distance
    0.0f, // rotation
);

// ...

skybox->draw(viewMatrix,projectionMatrix);
```

### CubeMapSkyBox

Loads a cubemap and draw it as a SkyBox.

Example:

```cpp
#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/mini-gl-engine.h>
using namespace aRibeiro;
using namespace openglWrapper;
using namespace GLEngine;

GLCubeMapSkyBox *cube_skybox = GLCubeMapSkyBox(
    true && Engine::Instance()->isRGBCapable, // load sRGB
    "negz.jpg","posz.jpg",
    "negx.jpg","posx.jpg",
    "negy.jpg","posy.jpg",
    100.0f, // distance
    1024 // max cubeResolution
);

// ...

cube_skybox->draw(viewMatrix,projectionMatrix);
```

### RenderState

This abstraction hides the function calls to set the OpenGL state machine.

The way proposed by this class is to use properties and set the values acording the rendering needs.

Example:

```cpp
#include <mini-gl-engine/mini-gl-engine.h>

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;
using namespace openglWrapper;

GLRenderState *renderState = GLRenderState::Instance();

renderState->ClearColor = vec4(1.0f,1.0f,250.0f/255.0f,1.0f);
renderState->FrontFace = FrontFaceCW;
renderState->CullFace = CullFaceBack;
renderState->DepthWrite = true;
renderState->DepthTest = DepthTestLessEqual;
renderState->BlendMode = BlendModeAlpha;
renderState->ColorWrite = ColorWriteAll;
```
