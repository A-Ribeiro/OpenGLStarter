# OpenGLStarter

[Back to HOME](../../index)

## Set Null and Delete

This template was designed to deal with multi-thread situation where you need to release a shared resource.

This implementation sets the parameter variable to __NULL__ and call delete after that.

Example:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

//
// Single Instance
//
vec2 *vec2_single_instance = new vec2();

// ...
// after this function call vec2_single_instance will be NULL.
setNullAndDelete( vec2_single_instance );

//
// Array Instance
//
vec2 *vec2_array_instance = new vec2[64];

// ...
// after this function call vec2_array_instance will be NULL.
setNullAndDeleteArray( vec2_array_instance );
```
