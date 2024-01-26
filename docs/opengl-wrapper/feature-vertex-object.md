# OpenGLStarter

[Back to HOME](../../index)

## Vertex Buffer Object

Vertex buffer object are memory buffers allocated in the video memory that feeds the vertex shader.

You need to define the vertex layout (what is a position, what is an uv, etc...).

It can be used as vertex attribute source or index attribute source.

Example using non-indexed triangles VBO:

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using namespace aRibeiro;
using namespace AppKit::OpenGL;

struct VertexAttrib {
    vec3 position;
    vec2 uv;
};
aligned_vector<VertexAttrib> vertices;

GLVertexBufferObject *data = new GLVertexBufferObject();

data->uploadData((void*)&vertices[0].position, sizeof(VertexAttrib)*vertices.size());

// drawing code
data->setLayout(shader_position_attrib, 3, GL_FLOAT, sizeof(VertexAttrib), 0);
data->setLayout(shader_uv_attrib, 2, GL_FLOAT, sizeof(VertexAttrib), sizeof(vec3) );

data->drawArrays(GL_TRIANGLES, vertices.size());

data->unsetLayout(shader_position_attrib);
data->unsetLayout(shader_uv_attrib);
```

Example using indexed triangles VBO:

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using namespace aRibeiro;
using namespace AppKit::OpenGL;

struct VertexAttrib {
    vec3 position;
    vec2 uv;
};
aligned_vector<VertexAttrib> vertices;
std::vector<unsigned short> indices;

GLVertexBufferObject *data = new GLVertexBufferObject();
GLVertexBufferObject *index = new GLVertexBufferObject();

data->uploadData((void*)&vertices[0].position, sizeof(VertexAttrib)*vertices.size());
index->uploadIndex((void*)&indices[0], indices.size()*sizeof(unsigned short));

// drawing code
data->setLayout(shader_position_attrib, 3, GL_FLOAT, sizeof(VertexAttrib), 0);
data->setLayout(shader_uv_attrib, 2, GL_FLOAT, sizeof(VertexAttrib), sizeof(vec3) );
index->setIndex();

index->drawIndex(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT);

data->unsetLayout(shader_position_attrib);
data->unsetLayout(shader_uv_attrib);
index->unsetIndex();
```

## Vertex Array Object

The vertex array object are abstraction over the VBO. It stores the layout setup of a VBO and do it as one command to OpenGL API.

You need to keep the VBO even after the VAO binding.

__Notice:__ It does not work with Raspberry Pi 3 Model B+. But works with the Intel HD 3000.

Example with non-indexed geometry:

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using namespace aRibeiro;
using namespace AppKit::OpenGL;

struct VertexAttrib {
    vec3 position;
    vec2 uv;
};
aligned_vector<VertexAttrib> vertices;

GLVertexBufferObject *data = new GLVertexBufferObject();

data->uploadData((void*)&vertices[0].position, sizeof(VertexAttrib)*vertices.size());

GLVertexArrayObject *vao = new GLVertexArrayObject();

// setup VAO code
vao->enable();
data->setLayout(shader_position_attrib, 3, GL_FLOAT, sizeof(VertexAttrib), 0);
data->setLayout(shader_uv_attrib, 2, GL_FLOAT, sizeof(VertexAttrib), sizeof(vec3) );
vao->disable();
data->unsetLayout(shader_position_attrib);
data->unsetLayout(shader_uv_attrib);

// Drawing code
vao->enable();
vao->drawArrays(GL_TRIANGLES, vertices.size());
vao->disable();
```

Example with indexed geometry:

```cpp
#include <aribeiro/aribeiro.h>
#include <appkit-gl-base/opengl-wrapper.h>
using namespace aRibeiro;
using namespace AppKit::OpenGL;

struct VertexAttrib {
    vec3 position;
    vec2 uv;
};
aligned_vector<VertexAttrib> vertices;
std::vector<unsigned short> indices;

GLVertexBufferObject *data = new GLVertexBufferObject();
GLVertexBufferObject *index = new GLVertexBufferObject();

data->uploadData((void*)&vertices[0].position, sizeof(VertexAttrib)*vertices.size());
index->uploadIndex((void*)&indices[0], indices.size()*sizeof(unsigned short));

GLVertexArrayObject *vao = new GLVertexArrayObject();

// setup VAO code
vao->enable();
data->setLayout(shader_position_attrib, 3, GL_FLOAT, sizeof(VertexAttrib), 0);
data->setLayout(shader_uv_attrib, 2, GL_FLOAT, sizeof(VertexAttrib), sizeof(vec3) );
index->setIndex();
vao->disable();
data->unsetLayout(shader_position_attrib);
data->unsetLayout(shader_uv_attrib);
index->unsetIndex();

// Drawing code
vao->enable();
vao->drawIndex(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT);
vao->disable();
```
