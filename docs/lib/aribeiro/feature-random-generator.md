# OpenGLStarter

[Back to HOME](../../index)

## Random Generator

To generate random numbers with all types from the framework.

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

//
// The random generates the range: [0..1]
//
double d = Random::getDouble();
float f = Random::getFloat();
vec2 v2 = Random::getVec2();
vec3 v3 = Random::getVec3();
vec4 v4 = Random::getVec4();
mat4 m4 = Random::getMat4();
vec4 v4_random_point = Random::getVec4Ptn(); // w = 1.0
vec4 v4_random_vec = Random::getVec4Vec(); // w = 0.0

//
// The random generates random Euler angles [0º..360º] in radians.
//
quat q = Random::getQuat();

//
// The random generates random values with magnitude of the vector equals to 1.
//
vec2 v2_unit = Random::getVec2Direction();
vec3 v3_unit = Random::getVec3Direction();
vec4 v4_unit_vec = Random::getVec4VecDirection(); // w = 0.0

//
// The random generates random values inside an integer range.
// Both min and max are included.
//
std::vector<int> array;
int randomIndex = Random::getRange(0,array.size()-1);
int element = array[randomIndex];
```
