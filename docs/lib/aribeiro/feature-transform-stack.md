# OpenGLStarter

[Back to HOME](../../index)

## Handle the Transformation Stack

To make easy to do hierarchy operations with any type.

```cpp
TransformStack<mat4> projection;
projection.push();
projection.top = mat4::IdentityMatrix;
//use the projection matrix...
projection.pop();
```
