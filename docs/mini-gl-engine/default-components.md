# OpenGLStarter

[Back to HOME](../../index)

## Default Components

There are some components definitions in the __projects-lib/mini-gl-engine/Components/__ .

The most importants are the ones that interacts directly with the render system.

* __ComponentCamera:__ have the view, projection and derived camera matrixes
    * ComponentCameraOrthographic
    * ComponentCameraPerspective
* __ComponentFontToMesh:__ Converts any text with help of the __AppKit::OpenGL::GLFont2Builder__ to the engine __ComponentMesh__.
* __ComponentFrustumCulling:__ Compute collision shapes.
* __ComponentLight:__ Light representation
    * The current implementation only consider sun light definition.
* __ComponentMaterial:__ Links a shader property list to a mesh.
    * Stores Unlit Shaders properties for opaque and transparent objects
    * Stores PBR Shaders properties for opaque objects
* __ComponentMesh:__ Stores any kind of mesh.
* __ComponentParticleSystem:__ Have a simple particle system definition.

