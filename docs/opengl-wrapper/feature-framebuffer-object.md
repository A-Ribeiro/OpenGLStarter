# OpenGLStarter

[Back to HOME](../../index)

## New Framebuffer Object Abstraction

The class __GLDynamicFBO__ creates the FBO id in OpenGL.

With this ID, you can attach and detach textures to the color buffer, or depth buffer on-the-fly.

With this abstraction it is possible to implement a only-depth renderer, a color-buffer renderer, or a cubemap renderer (one render step to each cubemap step).

Example with depth and color texture:

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using openglWrapper;

GLTexture depthTexture;
GLTexture colorTexture;
GLDynamicFBO fbo;

depthTexture.setSize(w,h,GL_DEPTH_COMPONENT24);
colorTexture.setSize(w,h,GL_RGB);

// setup the attachments
fbo.enable();
fbo.setDepthTextureAttachment(&depthTexture);
fbo.setColorAttachment(&colorTexture,0);
fbo.disable();

// using the FBO
fbo.enable();
//
// draw code
//
fbo.disable();
```

Example with cubemap:

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using openglWrapper;

GLTexture depthTexture;
GLCubeMap targetcubemap;
GLDynamicFBO fbo;

depthTexture.setSize(w,h,GL_DEPTH_COMPONENT24);
targetcubemap.setSizeAndFormat(w,h,GL_RGB);

// setup the attachments
fbo.enable();
fbo.setDepthTextureAttachment(&depthTexture);
fbo.disable();

// using the FBO
fbo.enable();

fbo.setColorAttachmentCube(&targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0);
// draw code
fbo.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0);
// draw code
fbo.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0);
// draw code
fbo.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0);
// draw code
fbo.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0);
// draw code
fbo.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0);
// draw code

fbo.setColorAttachmentCube(nullptr, 0, 0);

fbo.disable();
```

## Deprecated Framebuffer Object Abstraction

This deprecated abstraction does not have the ability to change the buffers attachments on-the-fly. It is mainly used to render color buffers with a depth attachment.

Example:

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using namespace aRibeiro;
using namespace AppKit::OpenGL;

// ...

GLFramebufferObject fbo = new GLFramebufferObject();

// Setup the color attachments and depth mode
fbo->useRenderbufferDepth = true;
fbo->color.push_back(new GLTexture());

// resize all color and depth attachments
fbo->setSize(512, 512);

// construct fbo and setup attachments
fbo->attachTextures();

// setup for draw
fbo->enable();
glViewport(0,0,fbo->width,fbo->height);

// draw code
// ...

// close fbo
GLFramebufferObject::disable();
glViewport(0,0,screen_width,screen_height);

// use the textures from this fbo in the main draw routine
// ...
```

