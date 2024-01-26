# OpenGLStarter

[Back to HOME](../../index)

## Regular 2D Texture

Textures in OpenGL have a lot of tricky parameters setup to use many kinds.

A texture object could be:

* Regular images with sRGB
* Regular images without sRGB
* Matrix with float content
* Matrix with depth 24 bit float content
    * This kind of texture could be sampled as shadowmap or not

Each texture setup could have the minification and magnification filters setup modified according the base type.

Textures can have the sample out-of-bound configured as:

* __Clamp:__ Out-of-bounds access returns the last near texel.
* __Repeat:__ Out-of-bounds access returns a copy of the original textures according the fract operation.

The image format to load to a texture could be PNG and JPG. The loader choose the correct format according the file extension passed as parameter.

Example:

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using namespace aRibeiro;
using namespace AppKit::OpenGL;

bool invertY = false;
bool source_Is_sRGB = true;
GLTexture *texture = GLTexture::loadFromFile("texture.png", invertY, source_Is_sRGB);

// setup hardware anisiotropic filtering
texture->active(0);
OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

//if the hardware does not support anisio setup, this method will not do nothing.
texture->setAnisioLevel(16.0f);

GLTexture::deactive(0);

// active this texture in the OpenGL texture unit 0
texture->active(0);

// OpenGL Drawing Code
// ...

// disable texture unit 0
GLTexture::deactive(0);
```

## Cubemap Texture

__GLCubeMap__ textures are special textures arrays with 6 textures inside it.

The access from shaders is done by using a 3D coord to the texture sampler.

The texel used is aquired according the cube projection of the 3D coord.

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using namespace aRibeiro;
using namespace AppKit::OpenGL;

bool load_as_sRGB_texture = true;
// cubemap takes a lot of memory, 
// with this variable we can control the 
// max resolution the texture will occupy.
int max_resolution = 1024;

GLCubeMap *texture = new GLCubeMap(load_as_sRGB_texture, max_resolution);
texture->loadFromFile(
    "_negz.jpg","_posz.jpg",
    "_negx.jpg","_posx.jpg",
    "_negy.jpg","_posy.jpg" );

// ...

// active this texture in the OpenGL texture unit 0
texture->active(0);

// OpenGL Drawing Code
// ...

// disable texture unit 0
GLTexture::deactive(0);
```
