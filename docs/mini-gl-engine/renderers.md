# OpenGLStarter

[Back to HOME](../../index)

The renderers are special classes to deal with rendering of any kind.

## Depth Renderer

Although the class name is __DepthRenderer__ It is a class to deal with depth-buffer rendering.

The main reason to exists is to extract the depth buffer from the current framebuffer or render a scene depth buffer from a camera point of view.

Example:

```cpp
DepthRenderer depthRenderer = new DepthRenderer();

depthRenderer->method1_copy_from_current_framebuffer();

// can use the depthRenderer->depthTexture inside the shader
```

## Fade Renderer

This class renders two triangles that cover the entire screen to do the fadeIn/fadeOut FX.

Example:
```cpp
fade = new Fade(&time);

fade->fadeOut(2.0f, NULL);
time.update();

//main loop
{
    time.update();
    //
    // ... app draw commands
    //
    fade->draw(); // will draw the current state of the fade
}
```
## Particle System Renderer

As the name said, this is a ParticleSystemRenderer.

You need to configure the emission, texture, etc... of the particle system in order to make it work.

The __RenderPipeline__ implements a kind of management of particle systems. 

It sorts and organize the particle systems and the particles itself before use the draw commands.

Example:

```cpp
// ...
MathCore::vec3f cameraPosition = camera->transform[0]->getPosition(true);
MathCore::vec3f cameraDirection = camera->transform[0]->getRotation(true) * MathCore::vec3f(0,0,1);

for(int i=0;i<sceneParticleSystem.size();i++)
    sceneParticleSystem[i]->distance_to_camera = aRibeiro::sqrDistance(sceneParticleSystem[i]->aabb_center,cameraPosition);

std::sort(sceneParticleSystem.begin(),sceneParticleSystem.end(),__compare__particle__system__reverse__);

for(int i=0;i<sceneParticleSystem.size();i++)
    sceneParticleSystem[i]->sortPositions(cameraPosition, cameraDirection);

//draw particles...
for(int i=0;i<sceneParticleSystem.size();i++) {

    if (sceneParticleSystem[i]->soft)
        particleSystemRenderer.drawSoftDepthComponent24( 
            camera, sceneParticleSystem[i],
            &depthRenderer->depthTexture
        ); 
    else
        particleSystemRenderer.draw( camera, sceneParticleSystem[i] );
}
```

## Render Pipeline Renderer

The class __RenderPipeline__ implements the whole process of rendering a scene.

It counts the lights, particle systems, materials and meshs and organize the execution of draw commands to render all things.

You can use the main method to render the scene you created with __SceneBase__ using the __root__ and __camera__ references.

Example:

```cpp
// from SceneBase
void SceneBase::draw(){
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
    
    renderPipeline->runSinglePassPipeline(root, camera, true);
    
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
}
```
