# OpenGLStarter

[Back to HOME](../../index)

## Math

The math part of the library is heavily C++ oriented. There are primitives and c++ operator overload to allow computation over the types.

### Primitives

There are several primitive types implemented in the framework.

They are:

* __vec2:__ 2D vector (x,y)
* __vec3:__ 3D vector (x,y,z)
* __vec4:__ 3D vector with homogeneous (x,y,z,w)
* __mat4:__ 3D matrix transformation with homogeneous coord
* __quat:__ Quaternion (x,y,z,w)

All vec2, vec3, vec4 and mat4 are implemented using the macro __INLINE_OPERATION_IMPLEMENTATION__.

This macro implements all arithmetic operations: +, -, *, / related to a single type.

Example:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

// I'll use the vec2 definition as example
class vec2{
public:
    //need to implement the copy constructors
    vec2(const float &v) { ... }
    vec2(const vec2 &v) { ... }

    //need to implement the arithmetic operators overload
    vec2& operator+=(const float &v) { ... }
    vec2& operator+=(const vec2& v) { ... }
    vec2 operator-()const { ... }
    vec2& operator-=(const float &v) { ... }
    vec2& operator-=(const vec2& v) { ... }
    vec2& operator*=(const float &v) { ... }
    vec2& operator*=(const vec2& v) { ... }
    vec2& operator/=(const float &v) { ... }
    vec2& operator/=(const vec2& v) { ... }
};

//Now this declaration will implement all
// kind of arithmetic operations overload.
//  The overload interacts with float also.
INLINE_OPERATION_IMPLEMENTATION(vec2)

//
// After the definition, it is possible 
// to use the aritmetic overload as follow:
//
vec2 a,b,c;

a = b - 0.5f;
c = ( a + b ) * 0.5f;
```

### Functions

There are a lot of functions in the _floatOps.h_ and _geometricOperations.h_.

I'll try to list them according the parameter it receives.

#### Function List: float

##### sign
```cpp
float result;
result = sign(25.0f); // 1.0f
result = sign(-25.0f); // -1.0f
```

#### Function List: vec3

##### slerp
```cpp
vec3 a = vec3( 1, 0, 0 );
vec3 b = vec3( 0, 10, 0 );

// 75% spherical interpolation from a to b
vec3 result = slerp(a, b, 0.75f);
```
##### cross
```cpp
vec3 a = vec3( 1 ,0 ,0 );
vec3 b = vec3( 0 ,1 ,0 );

// result  = vec3( 0, 0, 1)
vec3 result = cross(a, b);
```
##### refract
```cpp
float ior_air = 1.0f;
float ior_water = 1.33f;
vec3 rayDirection = normalize( vec3( 1 , -1 ,0 ) );
vec3 normal = vec3( 0 ,1 ,0 );

vec3 refracted;

// compute the refracted ray that comes from air to the water surface
if ( refract(rayDirection, normal, ior_air, ior_water, &refracted) ){
    // the refracted has the value of the direction in the second environment
    // ...
} else {
    // total internal reflection case...
}
```
##### barylerp
```cpp
// point inside the triangle
vec3 p;

// triangle vertex
vec3 a, b, c;

vec3 crossvec = cross(b-a, c-a);
vec3 cross_unit = normalize( crossvec );
float signed_triangle_area = dot( crossvec, cross_unit ) * 0.5f;

crossvec = cross(c-a, c-p);
float u = ( dot( crossvec, cross_unit )  * 0.5f ) / signed_triangle_area;

crossvec = cross(b-a, p-b);
float v = ( dot( crossvec, cross_unit )  * 0.5f ) / signed_triangle_area;

// now the color we want to interpolate
vec3 colorA, colorB, colorC;

vec3 colorResult = barylerp(u, v, colorA, colorB, colorC);
```
##### blerp
```cpp
vec3 dataA = vec3(0,0,0);
vec3 dataB = vec3(1,0,0);
vec3 dataC = vec3(1,1,0);
vec3 dataD = vec3(0,1,0);

// result = vec3( 0.5f, 0.5f, 0.0f )
vec3 result = blerp(dataA,dataB,dataC,dataD,0.5f,0.5f);
```
##### moveSlerp
```cpp
PlatformTime timer;

float moveSpeedAngle;
vec3 current;
vec3 target;

// MainLoop
{
    timer.update();
    ...
    // current will be modified to be the target,
    // but the delta time and move speed will make
    // this transition smoother.
    current = moveSlerp( current, target, time.deltaTime * moveSpeedAngle );
}
```

#### Function List: float, vec2, vec3 and vec4

##### absv
```cpp
vec3 input = vec3( -10, 20, -30 );
// result = vec3( 10, 20, 30 )
vec3 result = absv( input );
```
##### clamp
```cpp
vec3 result;
// result = vec3 ( 50, 3, 10 )
result = clamp( vec3( 300 , 3, 5 ), vec3( 0, -1, 10 ) , vec3( 50, 5, 15 ) );
```
##### sqrDistance
```cpp
vec3 a, b;
float result = sqrDistance( a, b );
```
##### distance
```cpp
vec3 a, b;
float result = distance( a, b );
```
##### maximum
```cpp
// first case
{
    vec3 input;
    float result = maximum( input );
}
// second case
{
    vec3 a, b;
    vec3 result = maximum( a, b );
}
```
##### minimum
```cpp
// first case
{
    vec3 input;
    float result = minimum( input );
}
// second case
{
    vec3 a, b;
    vec3 result = minimum( a, b );
}
```
##### lerp
```cpp
vec3 a = vec3( 0.0f );
vec3 b = vec3( 100.0f );

// result = vec3( 75.0f, 75.0f, 75.0f )
vec3 result = lerp( a, b, 0.75f );
```
##### move
```cpp
PlatformTime timer;

float moveSpeed;
vec3 current;
vec3 target;

// main loop
{
    timer.update();
    ...
    // current will be modified to be the target,
    // but the delta time and move speed will make
    // this transition smoother.
    current = move( current, target, time.deltaTime * moveSpeed );
}
```

#### Function List: vec2 and vec3

##### angleBetween
```cpp
vec3 a = vec3( 1, 0, 0 );
vec3 b = vec3( 0, 10, 0 );

float angle_radians = angleBetween(a, b);
```
##### splineCatmullRom
```cpp
vec3 p0,p1,p2,p3;

//the result is inside the range of p1 (0%) to p2 (100%)
vec3 result = splineCatmullRom(p0,p1,p2,p3,0.75f);
```

#### Function List: vec2, vec3 and vec4

##### dot
```cpp
vec3 a, b;
float result = dot( a, b ) ;
```
##### normalize
```cpp
vec2 a;
vec2 a_normalized = normalize( a );
```
##### reflect
```cpp
vec3 a = vec3( 1 , -1 ,0 );
vec3 normal = vec3( 0 ,1 ,0 );

// result  = vec3( 1, 1, 0)
vec3 reflected = reflect(a, normal);
```
##### sqrLength
```cpp
vec3 input;
float result = sqrLength(input);
```
##### length
```cpp
vec3 input;
float result = length(input);
```
##### parallelComponent
```cpp
vec3 a, unitV;
vec3 vOout = parallelComponent( a, unitV );
```
##### perpendicularComponent
```cpp
vec3 a, unitV;
vec3 vOout = perpendicularComponent( a, unitV );
```
##### vecDecomp
```cpp
vec3 a, unitV;
vec3 perpendicular, parallel;
vecDecomp( a, unitV, &perpendicular, &parallel );
```
##### quadraticClamp
```cpp
vec3 center = vec3( 0, 0, 0 );
float maxRadius = 10.0f;

vec3 point = vec3( 100, 0, 0 );

// result = vec3 ( 10, 0, 0 )
vec3 result = quadraticClamp( point, center, maxRadius );
```

#### Function List: mat4

##### lerp
```cpp
mat4 a = mat4( 0.0f );
mat4 b = mat4( 100.0f );

// result = mat4( 75.0f )
mat4 result = lerp( a, b, 0.75f );
```
##### extractRotation
```cpp
mat4 transform = eulerRotate( DEG2RAD(20.0f),0,0 ) * translate(10,0,0);
mat4 just_rotation = extractRotation( transform );
```
##### extractXaxis
```cpp
mat4 transform = eulerRotate( DEG2RAD(20.0f),0,0 ) * translate(10,0,0);
// can be used as the strafe vector
vec3 x_axis = extractXaxis( transform );
```
##### extractYaxis
```cpp
mat4 transform = eulerRotate( DEG2RAD(20.0f),0,0 ) * translate(10,0,0);
// can be used as the up vector
vec3 y_axis = extractYaxis( transform );
```
##### extractZaxis
```cpp
mat4 transform = eulerRotate( DEG2RAD(20.0f),0,0 ) * translate(10,0,0);
// can be used as the forward vector
vec3 z_axis = extractZaxis( transform );
```
##### extractTranslation
```cpp
mat4 transform = eulerRotate( DEG2RAD(20.0f),0,0 ) * translate(10,0,0);
vec3 just_global_translation = extractTranslation( transform );
```
##### transpose
```cpp
mat4 transform = eulerRotate( DEG2RAD(20.0f),0,0 ) * translate(10,0,0);
mat4 transposed_matrix = transpose( transform );
```
##### mat4_determinant
```cpp
mat4 input;
float det = mat4_determinant( input );
```
##### inv
```cpp
mat4 transform = eulerRotate( DEG2RAD(20.0f),0,0 ) * translate(10,0,0);
mat4 inverse_matrix = inv( transform );
```
##### translate
```cpp
// first case
mat4 translate_matrix_a = translate(10,0,0);
// second case
vec3 translate_vec;
mat4 translate_matrix_b = translate( translate_vec );
```
##### scale
```cpp
//first case
mat4 scale_matrix_a = scale( 2, 2, 2 );
//second case
vec3 scale_vec;
mat4 scale_matrix_b = scale( scale_vec );
```
##### xRotate
```cpp
mat4 rotation_matrix = xRotate( DEG2RAD( 30.0f ) );
```
##### yRotate
```cpp
mat4 rotation_matrix = yRotate( DEG2RAD( 30.0f ) );
```
##### zRotate
```cpp
mat4 rotation_matrix = zRotate( DEG2RAD( 30.0f ) );
```
##### eulerRotate
```cpp
mat4 rotation_matrix = eulerRotate( DEG2RAD( 30.0f ), DEG2RAD( 10.0f ), DEG2RAD( 5.0f ) );
```
##### extractEuler
```cpp
float roll, pitch, yaw;
mat4 matrix;
extractEuler(matrix, &roll, &pitch, &yaw );
```
##### rotate
```cpp
mat4 rotation_matrix = rotate( DEG2RAD( 30.0f ), 1.0f, 0.0f, 0.0f );
```
##### projection_perspective_rh_negative_one
```cpp
// first case
{
    float FovY = 60.0f;
    float aspectX = screenWidth / screenHeight;
    float near = 0.001f;
    float far = 1000.0f;

    mat4 projection_matrix = projection_perspective_rh_negative_one(FovY,aspectX,near,far);
}
// second case
{
    float focalLength = 35.0f;
    float width = screenWidth;
    float height = screenHeight;
    float near = 0.001f;
    float far = 1000.0f;
    
    mat4 projection_matrix = projection_perspective_rh_negative_one(focalLength,width,height,near,far);
}
```
##### projection_perspective_lh_negative_one
```cpp
// first case
{
    float FovY = 60.0f;
    float aspectX = screenWidth / screenHeight;
    float near = 0.001f;
    float far = 1000.0f;

    mat4 projection_matrix = projection_perspective_lh_negative_one(FovY,aspectX,near,far);
}
// second case
{
    float focalLength = 35.0f;
    float width = screenWidth;
    float height = screenHeight;
    float near = 0.001f;
    float far = 1000.0f;
    
    mat4 projection_matrix = projection_perspective_lh_negative_one(focalLength,width,height,near,far);
}
```
##### projection_frustum_rh_negative_one
```cpp
float Left = -1.0f, Right = 1.0f;
float Bottom = -1.0f, Top = 1.0f;
float Near = 0.001f, Far = 1000.0f;

mat4 projection_matrix = projection_frustum_rh_negative_one(Left,Right,Bottom,Top,Near,Far);
```
##### projection_frustum_lh_negative_one
```cpp
float Left = -1.0f, Right = 1.0f;
float Bottom = -1.0f, Top = 1.0f;
float Near = 0.001f, Far = 1000.0f;

mat4 projection_matrix = projection_frustum_lh_negative_one(Left,Right,Bottom,Top,Near,Far);
```
##### projection_ortho_rh_negative_one
```cpp
float Left = -screenWidth/2.0f, Right = screenWidth/2.0f;
float Bottom = -screenHeight/2.0f, Top = screenHeight/2.0f;
float Near = -1000.0f, Far = 1000.0f;

mat4 projection_matrix = projection_ortho_rh_negative_one(Left,Right,Bottom,Top,Near,Far);
```
##### projection_ortho_lh_negative_one
```cpp
float Left = -screenWidth/2.0f, Right = screenWidth/2.0f;
float Bottom = -screenHeight/2.0f, Top = screenHeight/2.0f;
float Near = -1000.0f, Far = 1000.0f;

mat4 projection_matrix = projection_ortho_lh_negative_one(Left,Right,Bottom,Top,Near,Far);
```

##### projection_perspective_rh_zero_one
```cpp
// first case
{
    float FovY = 60.0f;
    float aspectX = screenWidth / screenHeight;
    float near = 0.001f;
    float far = 1000.0f;

    mat4 projection_matrix = projection_perspective_rh_zero_one(FovY,aspectX,near,far);
}
// second case
{
    float focalLength = 35.0f;
    float width = screenWidth;
    float height = screenHeight;
    float near = 0.001f;
    float far = 1000.0f;
    
    mat4 projection_matrix = projection_perspective_rh_zero_one(focalLength,width,height,near,far);
}
```
##### projection_perspective_lh_zero_one
```cpp
// first case
{
    float FovY = 60.0f;
    float aspectX = screenWidth / screenHeight;
    float near = 0.001f;
    float far = 1000.0f;

    mat4 projection_matrix = projection_perspective_lh_zero_one(FovY,aspectX,near,far);
}
// second case
{
    float focalLength = 35.0f;
    float width = screenWidth;
    float height = screenHeight;
    float near = 0.001f;
    float far = 1000.0f;
    
    mat4 projection_matrix = projection_perspective_lh_zero_one(focalLength,width,height,near,far);
}
```
##### projection_frustum_rh_zero_one
```cpp
float Left = -1.0f, Right = 1.0f;
float Bottom = -1.0f, Top = 1.0f;
float Near = 0.001f, Far = 1000.0f;

mat4 projection_matrix = projection_frustum_rh_zero_one(Left,Right,Bottom,Top,Near,Far);
```
##### projection_frustum_lh_zero_one
```cpp
float Left = -1.0f, Right = 1.0f;
float Bottom = -1.0f, Top = 1.0f;
float Near = 0.001f, Far = 1000.0f;

mat4 projection_matrix = projection_frustum_lh_zero_one(Left,Right,Bottom,Top,Near,Far);
```
##### projection_ortho_rh_zero_one
```cpp
float Left = -screenWidth/2.0f, Right = screenWidth/2.0f;
float Bottom = -screenHeight/2.0f, Top = screenHeight/2.0f;
float Near = -1000.0f, Far = 1000.0f;

mat4 projection_matrix = projection_ortho_rh_zero_one(Left,Right,Bottom,Top,Near,Far);
```
##### projection_ortho_lh_zero_one
```cpp
float Left = -screenWidth/2.0f, Right = screenWidth/2.0f;
float Bottom = -screenHeight/2.0f, Top = screenHeight/2.0f;
float Near = -1000.0f, Far = 1000.0f;

mat4 projection_matrix = projection_ortho_lh_zero_one(Left,Right,Bottom,Top,Near,Far);
```

##### lookAt
```cpp
vec3 objPos;
vec3 cameraPos;

vec3 front = normalize( objPos - cameraPos );
vec3 up = vec3(0,1,0);

mat4 camera_matrix = lookAt(front, up, cameraPos);
```
##### modelLookAt
```cpp
vec3 otherObjPos;
vec3 objPos;

vec3 front = normalize( otherObjPos - objPos );
vec3 up = vec3(0,1,0);

mat4 object_matrix = modelLookAt(front, up, objPos);
```

#### Function List: Quaternion

##### conjugate
```cpp
quat result = quatFromEuler( DEG2RAD(15.0f), DEG2RAD(0.0f), DEG2RAD(50.0f) ) ;

// the conjugate is the inverse of a quaternion
quat result_inv = conjugate( result );
```
##### dot
```cpp
quat a, b;
float angle = acos( clamp( dot( a, b ), -1.0f, 1.0f ) ) * 2.0f;
```
##### normalize
```cpp
quat a,b,c;

// 'c' may result in a non unit quaternion
c = a ^ b;

// make 'c' a unit quaternion
c = normalize( c );
```
##### slerp
```cpp
quat a = quatFromEuler( DEG2RAD(30.0f), DEG2RAD(3.0f), DEG2RAD(20.0f) );
quat b = quatFromEuler( DEG2RAD(90.0f), DEG2RAD(0.0f), DEG2RAD(0.0f) );

// 75% spherical interpolation from a to b
quat result = slerp(a, b, 0.75f);
```
##### angleBetween
```cpp
quat a = quatFromEuler( DEG2RAD(30.0f), DEG2RAD(3.0f), DEG2RAD(20.0f) );
quat b = quatFromEuler( DEG2RAD(90.0f), DEG2RAD(0.0f), DEG2RAD(0.0f) );

float angle_radians = angleBetween(a, b);
```
##### sqrLength
```cpp
quat input;
float result = sqrLength(input);
```
##### length
```cpp
quat input;
float result = length(input);
```
##### quatLookAtRotation
```cpp
vec3 otherObjPos;
vec3 objPos;

vec3 front = normalize( otherObjPos - objPos );
vec3 up = vec3(0,1,0);

quat object_rotation = quatLookAtRotation(front, up, objPos);
```
##### quatLookAtRotationLH
```cpp
vec3 otherObjPos;
vec3 objPos;

vec3 front = normalize( otherObjPos - objPos );
vec3 up = vec3(0,1,0);

quat object_rotation = quatLookAtRotationLH(front, up, objPos);
```
##### quatFromAxisAngle
```cpp
vec3 axis = normalize( vec3( 1.0f, 1.0f, 0.0f ) );
float angle = DEG2RAD( 30.0f );
quat result = quatFromAxisAngle( axis, angle );
```
##### quatFromEuler
```cpp
quat result = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
```
##### extractAxisAngle
```cpp
quat rotation = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
vec3 axis;
float angle;    
extractAxisAngle(rotation, &axis, &angle);
```
##### extractEuler
```cpp
quat rotation = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
float roll, pitch, yaw;
extractEuler(rotation, &roll, &pitch, &yaw);
```
##### inv
```cpp
quat rotation = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
quat rotation_inv = inv( rotation );
```
##### moveSlerp
```cpp
PlatformTime timer;

float moveSpeedAngle;
quat current;
quat target;

// main loop
{
    timer.update();
    ...
    // current will be modified to be the target,
    // but the delta time and move speed will make
    // this transition smoother.
    current = moveSlerp( current, target, time.deltaTime * moveSpeedAngle );
}
```

#### Function List: Conversion

##### toVec3
```cpp
vec4 point = vec4( 1.0f, 2.0f, 3.0f, 1.0f );
vec4 vector = vec4( 4.0f, 5.0f, 6.0f, 0.0f );

vec3 result;

// result = vec3( 1.0f, 2.0f, 3.0f );
result = toVec3(point);
// result = vec3( 4.0f, 5.0f, 6.0f );
result = toVec3(vector);
```
##### toVec3_PerspDiv
```cpp
mat4 projection = projection_perspective( 60.0f, screenWidth/screenHeight, 0.001f, 1000.0f );
vec4 projected_point_projective_space = projection * vec4( 0, 0, -10.0f, 1.0f );
vec3 projected_point_euclidian_space = toVec3_PerspDiv( projected_point_projective_space );
```
##### toVec4
```cpp
vec3 _3DVector;
// homogeneous_vector = vec4( _3DVector.x, _3DVector.y, _3DVector.z, 0.0f )
vec4 homogeneous_vector = toVec4( _3DVector );
```
##### toPtn4
```cpp
vec3 _3DPoint;
// homogeneous_point = vec4( _3DPoint.x, _3DPoint.y, _3DPoint.z, 1.0f )
vec4 homogeneous_point = toPtn4( _3DPoint );
```
##### polarToVec2
```cpp
// polar coord with angle = 30 degrees and radius = 10
vec2 polarRepresentation = polarToVec2( DEG2RAD(30.0f), 10.0f );
```
##### extractQuat
```cpp
mat4 transformation = eulerRotate( DEG2RAD(30.0f), DEG2RAD(3.0f), DEG2RAD(20.0f) );
// converts the transform matrix to a quaternion representation
quat transformation_quaternion = extractQuat( transformation );
```
##### toQuat
```cpp
vec3 axis = normalize( vec3( 1.0f, 1.0f, 0.0f ) );
quat result = toQuat( axis );
```
##### toMat4
```cpp
quat rotation = quatFromEuler( DEG2RAD( 30.0f ), DEG2RAD( 15.0f ), DEG2RAD( 90.0f ) );
mat4 result = toMat4( rotation );
```

#### Operator Overload

* vec4 <- mat4 * vec4
    ```cpp
    mat4 a;
    vec4 b;
    vec4 result = a * b;
    ```
* vec4 <- vec4 * mat4
    ```cpp
    vec4 a;
    mat4 b;
    vec4 result = a * b;
    ```
* quat <- quat ^ quat
    ```cpp
    quat a;
    quat b;
    quat result = a ^ b;
    ```
* quat <- quat * quat
    ```cpp
    quat a;
    quat b;
    quat result = a * b;
    ```
* vec3 <- quat * vec3
    ```cpp
    quat a;
    vec3 b;
    vec3 result = a * b;
    ```
* vec4 <- quat * vec4
    ```cpp
    quat a;
    vec4 b;
    vec4 result = a * b;
    ```

#### Aux Functions

##### eulerEquivalent
```cpp
float roll, pitch, yaw;
float variant_roll, variant_pitch, variant_yaw;
eulerEquivalent(roll, pitch, yaw, &variant_roll, &variant_pitch, &variant_yaw );
```
##### unproject
```cpp
// screen center at near plane (OpenGL)
vec3 pointInWindow = vec3( screenWidth / 2.0f, screenHeight / 2.0f, -1.0f );
// any 3D rigid transform form scene graph
mat4 modelViewMatrix = ...;
// the projection used
mat4 projectionMatrix = ...;
// window coordinate system
int viewportX = 0, viewportY = 0;
int viewportW = screenWidth, int viewportH = screenHeight;

vec3 point_world_coordinate;

bool success = unproject(pointInWindow, modelViewMatrix, projectionMatrix, viewportX, viewportY, viewportW, viewportH, &point_world_coordinate);
```
##### project
```cpp
// a point from 3D world
vec3 point_world_coordinate;
// any 3D rigid transform form scene graph
mat4 modelViewMatrix = ...;
// the projection used
mat4 projectionMatrix = ...;
// window coordinate system
int viewportX = 0, viewportY = 0;
int viewportW = screenWidth, int viewportH = screenHeight;

vec3 pointInWindow;

bool success = project(point_world_coordinate, modelViewMatrix, projectionMatrix, viewportX, viewportY, viewportW, viewportH, &pointInWindow);
```

## More Examples

```cpp
//
//vector operation
//
vec3 a(1.0f,0.0f,0.0f);
vec3 b(0.0f,1.0f,0.0f);
vec3 ab = a-b;
//
//matrix multiplication
//
mat4 rotation = rotate( DEG2RAD(30.0f), 1.0f,0.0f,0.0f );
vec3 ab_rotated = toVec3(rotation * toPtn4(ab));
//
//quaternion
//
quat qrotationA = quatFromEuler(DEG2RAD(30.0f),0.0f,0.0f);
quat qrotationB = quatFromEuler(0.0f,DEG2RAD(30.0f),0.0f);
quat qrotation = slerp(qrotationA, qrotationB, 0.5f);
rotation = toMat4(qrotation);
```
