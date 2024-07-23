# OpenGLStarter

[Back to HOME](../../index)

## Shaders

The old OpenGL library is easy to do the first setup because it comes with a lot of fixed functions already implemented in the API.

After the programmable graphics hardware, the new version of OpenGL becomes more flexible. Now is a bit more complicated to write CG software, because you need to program the GPU also.

The library comes with 4 shaders already implemented to do the following tasks:

* Draw primitives with solid colors
* Draw primitives with a texture and a color
* Draw fonts
* Draw YUV buffers

Example:

```cpp
// global variable definition
GLShaderColor *shaderColor = nullptr;

int main(int argc, char* argv[]) {
  //OpenGL Inicialization
  ...
  shaderColor = new GLShaderColor();

  //Inside Main Loop
  while (window.isOpen()) {
  
    glClear(GL_COLOR_BUFFER_BIT);
    
    shaderColor->enable();
    // Set Shader Uniforms
    shaderColor->setColor(vec4(MathCore::vec3f(0.8f), 1.0));
    shaderColor->setMatrix(mvp);

    // Draw some triangles buffer
    glEnableVertexAttribArray(shader->vPosition);
    glVertexAttribPointer(shader->vPosition, COORDS_PER_POS, GL_FLOAT, false, STRUCTURE_STRIDE_BYTES_POS, vertexBuffer);
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT);
    glDisableVertexAttribArray(shader->vPosition);
    
    window.display();

    // window event processing...
    ...
  }
}
```
