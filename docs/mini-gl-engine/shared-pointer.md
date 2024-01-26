# OpenGLStarter

[Back to HOME](../../index)

## Shared Pointer

The shared pointer of the engine is made to be used with __Component__ and __Transform__.

It basically holds references of a __Component__ or a __Transform__.

When the __Component__ or __Transform__ is deleted from memory, it sets itself to __NULL__.

This class can be used to make cross referencing among components, so when a component is deleted, the reference becomes NULL.

Example:

```cpp

Transform *node = new Transform();

// first we define our shared pointer
SharedPointer<Transform> target;

//sets the target
target = node;

//now we can use the target as a normal pointer
target->LocalPosition = MathCore::vec3f(0,0,0);

// ...

// In another part of the code
delete node;// this will cause to set the target to NULL

//the safest code will be:
if (target != NULL)
    target->LocalPosition = MathCore::vec3f(0,0,0);

```
