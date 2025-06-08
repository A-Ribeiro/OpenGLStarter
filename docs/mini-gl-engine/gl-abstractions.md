# OpenGLStarter

[Back to HOME](../../index)

## OpenGL Abstraction

### SkyBox

This definition uses 6 plain 2D textures to draw a skybox.

Example:

```cpp

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/GL/GLSkyBox.h>

using namespace MathCore;
using namespace AppKit::OpenGL;
using namespace AppKit::GLEngine;

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
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/GL/GLCubeMapSkyBox.h>

using namespace aRibeiro;
using namespace AppKit::OpenGL;
using namespace AppKit::GLEngine;

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
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/GL/GLRenderState.h>

using namespace aRibeiro;
using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;

GLRenderState *renderState = GLRenderState::Instance();

renderState->ClearColor = vec4(1.0f,1.0f,250.0f/255.0f,1.0f);
renderState->FrontFace = FrontFaceCCW;
renderState->CullFace = CullFaceBack;
renderState->DepthWrite = true;
renderState->DepthTest = DepthTestLessEqual;
renderState->BlendMode = BlendModeAlpha;
renderState->ColorWrite = ColorWriteAll;
```
