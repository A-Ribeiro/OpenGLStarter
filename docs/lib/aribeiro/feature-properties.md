# OpenGLStarter

[Back to HOME](../../index)

## Properties

A Property is a value that can have listeners attached to it when it is modified.

The __Property__ holds the value you define inside it.

It can be used to implement events related to value modification.

See the example below:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

// first you need to declare the property
Property<vec2> Size;

//functions you want to call
void OnSizeChange(Property<vec2> *prop) {
  ... use the prop->value
}

//set the listener
Size.onChange.add(&OnSizeChange);

// to change the property value
Size = vec2( 1.0f );

// to get the property value
vec2 result = Size;
```

## Virtual Properties

Virtual properties doesn't hold the value you define.

You need to implement the getter and setter to use a __VirtualProperty__.

Example:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

class Example {

    vec2 size_internal;

    vec2 size_get() const {
        return size_internal;
    }
    void size_set(const vec2& v) {
        size_internal = v;
    }
public:

    VirtualProperty<vec2> Size;

    Example() : Size(this, &Example::size_get, &Example::size_set) {

    }
};

Example example;

// set the value
example.Size = vec2(1.0);

// get the value
vec2 aux = (vec2)example.Size;

```
