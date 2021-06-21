# OpenGLStarter

[Back to HOME](../../index)

## Math aRibeiro::collision Namespace

This namespace have special types to deal with collision algorithms.

### AABB

Stores 3D points to represent an Axis Aligned Bounding Box.

#### Constructor
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

AABB aabb;

aabb = AABB();

vec3 corner_a, corner_d;
aabb = AABB(corner_d, corner_a);
```
#### pointInsideAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

AABB aabb;
vec3 point;

if ( AABB::pointInsideAABB( point, aabb ) ){
    // ...
}
```
#### aabbOverlapsAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

AABB aabb_a;
AABB aabb_b;

if ( AABB::aabbOverlapsAABB( aabb_a, aabb_b ) ){
        // ...
}
```
#### joinAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

AABB aabb_a;
AABB aabb_b;

AABB aabb = AABB::joinAABB( aabb_a, aabb_b );
```
#### fromTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 a, b, c;
    AABB aabb = AABB::fromTriangle( a, b, c );
}
// second case
{
    Triangle triangle;
    AABB aabb = AABB::fromTriangle( triangle );
}
```
#### fromSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 sphere_pos;
    float sphere_radius;
    AABB aabb = AABB::fromSphere( sphere_pos, sphere_radius );
}
// second case
{
    Sphere sphere;
    AABB aabb = AABB::fromSphere( sphere );
}
```
#### fromLineSegment
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 segment_a, segment_b;
    AABB aabb = AABB::fromLineSegment( segment_a, segment_b );
}
// second case
{
    LineSegment lineSegment;
    AABB aabb = AABB::fromLineSegment( lineSegment );
}
```
#### fromFrustum
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Frustum frustum;
AABB aabb = AABB::fromFrustum( frustum );
```
#### raycastAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Ray ray;
AABB aabb;

float tmin;
vec3 normal;
if ( AABB::raycastAABB(ray, aabb, &tmin, &normal) ) {
    vec3 collision_ptn = ray.origin + ray.dir * tmin;
    // ...
}
```
#### segmentIntersectsAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 a, b;
    AABB aabb;
    if ( AABB::segmentIntersectsAABB( a, b, aabb ) ) {
        // ...
    }
}
// second case
{
    LineSegment ls;
    AABB aabb;
    if ( AABB::segmentIntersectsAABB( ls, aabb ) ) {
        // ...
    }
}
```
#### closestPointToAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

vec3 ptn_to_input;
AABB aabb;

vec3 closest_point = AABB::closestPointToAABB( ptn_to_input, aabb );
```
#### sphereOverlapsAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 sphere_center;
    float sphere_radius;
    AABB aabb;
    
    vec3 penetration;
    if ( AABB::sphereOverlapsAABB( sphere_center, sphere_radius, aabb, &penetration ) ) {
        // ...
    }
}
// second case
{
    Sphere sphere;
    AABB aabb;
    
    vec3 penetration;
    if ( AABB::sphereOverlapsAABB( sphere, aabb, &penetration ) ) {
        // ...
    }
}
```
#### planeIntersectsAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Plane plane;
AABB aabb;

if ( AABB::planeIntersectsAABB( plane, aabb ) ) {
    // ...
}
```
#### triangleIntersectsAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 a, b, c;
    AABB aabb;
    
    if ( AABB::triangleIntersectsAABB( a, b, c, aabb ) ) {
        // ...
    }
}
// second case
{
    Triangle triangle;
    AABB aabb;
    
    if ( AABB::triangleIntersectsAABB( triangle, aabb ) ) {
        // ...
    }
}
```
#### frustumOverlapsAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Frustum frustum;
AABB aabb;

if ( AABB::frustumOverlapsAABB( frustum, aabb ) ) {
    // ...
}
```

### Frustum

Camera frustum representation.

#### Constructor
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    mat4 camera_projection;

    Frustum frustum = Frustum( camera_projection );
    Frustum frustum_a = camera_projection;
}
// second case
{
    mat4 camera_projection;
    mat4 camera_inverse_transformation;

    Frustum frustum = Frustum( camera_projection, camera_inverse_transformation );
}
```
#### Plane Indexing
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Frustum frustum;

Plane right = frustum[0];
Plane left = frustum[1];
Plane bottom = frustum[2];
Plane top = frustum[3];
Plane near = frustum[4];
Plane far = frustum[5];
```
#### pointInsideFrustum
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Frustum frustum;
vec3 point;

if ( Frustum::pointInsideFrustum( point, frustum ) ) {
    // ...
}
```
#### sphereOverlapsFrustum
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Frustum frustum;
    
    vec3 sphere_center;
    float sphere_radius;
    
    if ( Frustum::sphereOverlapsFrustum( sphere_center, sphere_radius, frustum ) ) {
        // ...
    }
}
// second case
{
    Frustum frustum;
    Sphere sphere;
    
    if ( Frustum::sphereOverlapsFrustum( sphere, frustum ) ) {
        // ...
    }
}
```
#### aabbOverlapsFrustum
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Frustum frustum;
AABB aabb;

if ( Frustum::aabbOverlapsFrustum( aabb, frustum ) ) {
    // ...
}
```

### LineSegment

Line segment representation.

#### Constructor
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    LineSegment lineSegment;
}
// second case
{
    vec3 ptn_a, ptn_b;
    LineSegment lineSegment = LineSegment( ptn_a, ptn_b );
}
```
#### closestPointToSegment
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 ptn_a, ptn_b;
    vec3 ptn_input;
    vec3 closest_point = LineSegment::closestPointToSegment( ptn_to_input, ptn_a, ptn_b );
}
// second case
{
    LineSegment lineSegment;
    vec3 ptn_input;
    vec3 closest_point = LineSegment::closestPointToSegment( ptn_to_input, lineSegment );
}
```
#### aabbIntersectsSegment
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 a, b;
    AABB aabb;
    
    if ( LineSegment::aabbIntersectsSegment( aabb, a, b ) ) {
        // ...
    }
}
// second case
{
    LineSegment lineSegment;
    AABB aabb;
    
    if ( LineSegment::aabbIntersectsSegment( aabb, lineSegment ) ) {
        // ...
    }
}
```
#### planeIntersectsSegment
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 a, b;
    Plane plane;
    
    if ( LineSegment::planeIntersectsSegment( plane, a, b ) ) {
        // ...
    }
}
// second case
{
    LineSegment lineSegment;
    Plane plane;
    
    if ( LineSegment::planeIntersectsSegment( plane, lineSegment ) ) {
        // ...
    }
}
```
#### sphereIntersectsSegment
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 a, b;
    
    vec3 sphere_center;
    float sphere_radius;
    
    if ( LineSegment::sphereIntersectsSegment( sphere_center, sphere_radius, a, b ) ) {
        // ...
    }
}
// second case
{
    LineSegment lineSegment;

    vec3 sphere_center;
    float sphere_radius;
    
    if ( LineSegment::sphereIntersectsSegment( sphere_center, sphere_radius, lineSegment ) ) {
        // ...
    }
}
// third case
{
    vec3 a, b;
    Sphere sphere;
    
    if ( LineSegment::sphereIntersectsSegment( sphere, a, b ) ) {
        // ...
    }
}
// fourth case
{
    LineSegment lineSegment;
    Sphere sphere;
    
    if ( LineSegment::sphereIntersectsSegment( sphere, lineSegment ) ) {
        // ...
    }
}
```
#### triangleIntersectsSegment
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    // triangle
    vec3 a, b, c;
    // line segment
    vec3 p, q;
    
    // triangle baricentric coords u,v,w
    float u, v, w;
    if ( LineSegment::triangleIntersectsSegment( a, b, c, p, q, &u, &v, &w ) ) {
        // ...
    }
}
// second case
{
    // triangle
    vec3 a, b, c;
    LineSegment lineSegment;
    
    // triangle baricentric coords u,v,w
    float u, v, w;
    if ( LineSegment::triangleIntersectsSegment( a, b, c, lineSegment, &u, &v, &w ) ) {
        // ...
    }
}
// third case
{
    Triangle triangle;
    LineSegment lineSegment;
    
    // triangle baricentric coords u,v,w
    float u, v, w;
    if ( LineSegment::triangleIntersectsSegment( triangle, lineSegment, &u, &v, &w ) ) {
        // ...
    }
}
// fourth case
{
    Triangle triangle;
    // line segment
    vec3 p, q;
    
    // triangle baricentric coords u,v,w
    float u, v, w;
    if ( LineSegment::triangleIntersectsSegment( triangle, p, q, &u, &v, &w ) ) {
            ...
    }
}
```

### Plane

#### Constructor
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Plane plane;
}
// second case
{
    vec3 point_in_plane;
    vec3 normal;
    
    Plane plane = Plane( point_in_plane, normal );
}
// third case
{
    // triangle a, b, c
    vec3 a, b, c;
    
    Plane plane = Plane( a, b, c );
}
```
#### normalize
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Plane plane;

plane.normalize();
```
#### fromTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 a, b, c;
    Plane plane = Plane::fromTriangle( a, b, c );
}
// second case
{
    Triangle triangle;
    Plane plane = Plane::fromTriangle( triangle );
}
```
#### closestPointToPlane
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

vec3 ptn_to_input;
Plane plane;

vec3 closest_point = Plane::closestPointToPlane( ptn_to_input, plane );
```
#### pointDistanceToPlane
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

vec3 point;
Plane plane;

float signed_distance = Plane::pointDistanceToPlane( point, plane );
```
#### raycastPlane
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Ray ray;
Plane plane;

float tmin;
vec3 normal;
if ( Plane::raycastPlane(ray, aabb, &tmin, &normal) ) {
    vec3 collision_ptn = ray.origin + ray.dir * tmin;
    // ...
}
```
#### segmentIntersectsPlane
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 a, b;
    Plane plane;
    
    if ( Plane::segmentIntersectsPlane( a, b, plane ) ) {
        //...
    }
}
// second case
{
    LineSegment lineSegment;
    Plane plane;
    
    if ( Plane::segmentIntersectsPlane( lineSegment, plane ) ) {
        // ...
    }
}
```
#### intersectPlaneToPlane
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Plane plane_a, plane_b;

vec3 line_position, line_direction;
if ( Plane::intersectPlaneToPlane( plane_a, plane_b, &line_position, &line_direction ) ) {
    // ...
}
```
#### intersectPlanes
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Plane plane_a, plane_b, plane_c;

vec3 position;
if ( Plane::intersectPlanes( plane_a, plane_b, plane_c, &position ) ) {
    // ...
}
```
#### aabbIntersectsPlane
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Plane plane;
AABB aabb;

if ( Plane::aabbIntersectsPlane( aabb, plane ) ) {
    // ...
}
```

### Ray

Ray representation.

#### Constructor
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Ray ray;
}
// second case
{
    vec3 origin, direction;
    
    Ray ray = Ray( origin, direction );
}
```
#### raycastAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Ray ray;
AABB aabb;

float tmin;
vec3 normal;
if ( Ray::raycastAABB(ray, aabb, &tmin, &normal) ) {
    vec3 collision_ptn = ray.origin + ray.dir * tmin;
    // ...
}
```
#### raycastPlane
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Ray ray;
Plane plane;

float tmin;
vec3 normal;
if ( Ray::raycastPlane(ray, aabb, &tmin, &normal) ) {
    vec3 collision_ptn = ray.origin + ray.dir * tmin;
    // ...
}
```
#### raycastSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Ray ray;

    vec3 sphere_center;
    float sphere_radius;
    
    float tmin;
    vec3 normal;
    if ( Ray::raycastSphere(ray, sphere_center, sphere_radius, &tmin, &normal) ) {
        vec3 collision_ptn = ray.origin + ray.dir * tmin;
        // ...
    }
}
// second case
{
    Ray ray;
    Sphere sphere;
    
    float tmin;
    vec3 normal;
    if ( Ray::raycastSphere(ray, sphere, &tmin, &normal) ) {
        vec3 collision_ptn = ray.origin + ray.dir * tmin;
        // ...
    }
}
```
#### raycastTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Ray ray;
    
    vec3 a, b, c;
    
    float tmin;
    vec3 normal;
    if ( Ray::raycastTriangle(ray, a, b, c, &tmin, &normal) ) {
        vec3 collision_ptn = ray.origin + ray.dir * tmin;
        // ...
    }
}
// second case
{
    Ray ray;
    Triangle triangle;
    
    float tmin;
    vec3 normal;
    if ( Ray::raycastTriangle(ray, triangle, &tmin, &normal) ) {
        vec3 collision_ptn = ray.origin + ray.dir * tmin;
        // ...
    }
}
```

### Sphere

Sphere representation.

#### Constructor
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Sphere sphere;
}
// second case
{
    vec3 sphere_center;
    float sphere_radius;
    
    Sphere sphere = Sphere( sphere_center, sphere_radius );
}
```
#### closestPointToSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{  
    vec3 ptn_to_input;
    
    vec3 sphere_center;
    float sphere_radius;
    
    vec3 closest_point = Sphere::closestPointToSphere( ptn_to_input, sphere_center, sphere_radius );
}
// second case
{
    vec3 ptn_to_input;
    Sphere sphere;
    
    vec3 closest_point = Sphere::closestPointToSphere( ptn_to_input, sphere );
}
```
#### sphereOverlapsSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Sphere sphere_a;
Sphere sphere_b;

if ( Sphere::sphereOverlapsSphere( sphere_a, sphere_b ) ){
    // ...
}
```
#### joinSpheres
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Sphere sphere_a;
Sphere sphere_b;

Sphere sphere = Sphere::joinSpheres( sphere_a, sphere_b );
```
#### raycastSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Ray ray;
    
    vec3 sphere_center;
    float sphere_radius;
    
    float tmin;
    vec3 normal;
    if ( Sphere::raycastSphere(ray, sphere_center, sphere_radius, &tmin, &normal) ) {
        vec3 collision_ptn = ray.origin + ray.dir * tmin;
        // ...
    }
}
// second case
{
    Ray ray;
    Sphere sphere;
    
    float tmin;
    vec3 normal;
    if ( Sphere::raycastSphere(ray, sphere, &tmin, &normal) ) {
        vec3 collision_ptn = ray.origin + ray.dir * tmin;
        // ...
    }
}
```
#### segmentIntersectsSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    //line segment a and b
    vec3 a, b;
    //sphere
    vec3 sphere_center;
    float sphere_radius;
    
    if ( Sphere::segmentIntersectsSphere( a, b, sphere_center, sphere_radius ) ) {
        // ...
    }
}
// second case
{
    LineSegment lineSegment;
    
    vec3 sphere_center;
    float sphere_radius;
    
    if ( Sphere::segmentIntersectsSphere( lineSegment, sphere_center, sphere_radius ) ) {
        // ...
    }
}
// third case
{
    //line segment a and b
    vec3 a, b;
    Sphere sphere;
    
    if ( Sphere::segmentIntersectsSphere( a, b, sphere ) ) {
        // ...
    }
}
// fourth case
{
    LineSegment lineSegment;
    Sphere sphere;
    
    if ( Sphere::segmentIntersectsSphere( lineSegment, sphere ) ) {
        // ...
    }
}
```
#### pointInsideSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 point;
    
    vec3 sphere_center;
    float sphere_radius;
    
    if ( Sphere::pointInsideSphere( point, sphere_center, sphere_radius ) ) {
        // ...
    }
}
// second case
{
    vec3 point;
    Sphere sphere;
    
    if ( Sphere::pointInsideSphere( point, sphere ) ) {
        // ...
    }
}
```
#### aabbOverlapsSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 sphere_center;
    float sphere_radius;
    AABB aabb;
    
    vec3 penetration;
    if ( Sphere::aabbOverlapsSphere( aabb, sphere_center, sphere_radius, &penetration ) ) {
        // ...
    }
}
// second case
{
    Sphere sphere;
    AABB aabb;
    
    vec3 penetration;
    if ( Sphere::aabbOverlapsSphere( aabb, sphere, &penetration ) ) {
        // ...
    }
}
```
#### frustumOverlapsSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Frustum frustum;
    
    vec3 sphere_center;
    float sphere_radius;
    
    if ( Sphere::frustumOverlapsSphere( frustum, sphere_center, sphere_radius ) ) {
        // ...
    }
}
// second case
{
    Frustum frustum;
    Sphere sphere;
    
    if ( Sphere::frustumOverlapsSphere( frustum, sphere ) ) {
        // ...
    }
}
```
#### triangleIntersectsSphere
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    // triangle
    vec3 a, b, c;
    
    vec3 sphere_center;
    float sphere_radius;
    
    vec3 penetration;
    if ( Sphere::triangleIntersectsSphere( a, b, c, sphere_center, sphere_radius, &penetration ) ) {
        // ...
    }
}
// second case
{
    // triangle
    vec3 a, b, c;
    Sphere sphere;
    
    vec3 penetration;
    if ( Sphere::triangleIntersectsSphere( a, b, c, sphere, &penetration ) ) {
        // ...
    }
}
// third case
{
    Triangle triangle;
    
    vec3 sphere_center;
    float sphere_radius;
    
    vec3 penetration;
    if ( Sphere::triangleIntersectsSphere( triangle, sphere_center, sphere_radius, &penetration ) ) {
        // ...
    }
}
// fourth case
{
    Triangle triangle;
    Sphere sphere;
    
    vec3 penetration;
    if ( Sphere::triangleIntersectsSphere( triangle, sphere, &penetration ) ) {
        // ...
    }
}
```
#### from4Points
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

vec3 tetrahedron_a,tetrahedron_b,tetrahedron_c,tetrahedron_d;
Sphere sphere = Sphere::from4Points( tetrahedron_a,tetrahedron_b,tetrahedron_c,tetrahedron_d );
```
#### fromFrustum
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Frustum frustum;
Sphere sphere = Sphere::fromFrustum( frustum );
```
#### fromAABB
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

AABB aabb;
Sphere sphere = Sphere::fromAABB( aabb );
```
#### fromLineSegment
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

LineSegment lineSegment;
Sphere sphere = Sphere::fromLineSegment( lineSegment );
```
#### fromTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

Triangle triangle;
Sphere sphere = Sphere::fromTriangle( triangle );
```

### Triangle

Triangle representation.

#### Constructor
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Triangle triangle;
}
// second case
{
    vec3 a, b, c;
    Triangle triangle( a, b, c );
}
```
#### raycastTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    Ray ray;
    //triangle
    vec3 a, b, c;
    
    float tmin;
    vec3 normal;
    if ( Triangle::raycastTriangle(ray, a, b, c, &tmin, &normal) ) {
        vec3 collision_ptn = ray.origin + ray.dir * tmin;
        // ...
    }
}
// second case
{
    Ray ray;
    Triangle triangle;
    
    float tmin;
    vec3 normal;
    if ( Triangle::raycastTriangle(ray, triangle, &tmin, &normal) ) {
        vec3 collision_ptn = ray.origin + ray.dir * tmin;
        // ...
    }
}
```
#### closestPointToTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    vec3 ptn_to_input;
    // triangle
    vec3 a, b, c;
    
    vec3 closest_point = Triangle::closestPointToTriangle( ptn_to_input, a, b, c );
}
// second case
{
    vec3 ptn_to_input;
    Triangle triangle;
    
    vec3 closest_point = Triangle::closestPointToTriangle( ptn_to_input, triangle );
}
```
#### segmentIntersectsTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    // line segment
    vec3 p, q;
    // triangle
    vec3 a, b, c;
    
    // triangle baricentric coords u,v,w
    float u, v, w;
    if ( Triangle::segmentIntersectsTriangle( p, q, a, b, c, &u, &v, &w ) ) {
        // ...
    }
}
// second case
{
    LineSegment lineSegment;
    // triangle
    vec3 a, b, c;
    
    // triangle baricentric coords u,v,w
    float u, v, w;
    if ( Triangle::segmentIntersectsTriangle( lineSegment, a, b, c, &u, &v, &w ) ) {
        // ...
    }
}
// third case
{
    // line segment
    vec3 p, q;
    Triangle triangle;
    
    // triangle baricentric coords u,v,w
    float u, v, w;
    if ( Triangle::segmentIntersectsTriangle( p, q, triangle, &u, &v, &w ) ) {
        // ...
    }
}
// fourth case
{
    LineSegment lineSegment;
    Triangle triangle;
    
    // triangle baricentric coords u,v,w
    float u, v, w;
    if ( Triangle::segmentIntersectsTriangle( lineSegment, triangle, &u, &v, &w ) ) {
        // ...
    }
}
```
#### sphereIntersectsTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    // triangle
    vec3 a, b, c;
    // sphere
    vec3 sphere_center;
    float sphere_radius;
    
    vec3 penetration;
    if ( Triangle::sphereIntersectsTriangle( sphere_center, sphere_radius, a, b, c, &penetration ) ) {
        // ...
    }
}
// second case
{
    // triangle
    vec3 a, b, c;
    Sphere sphere;
    
    vec3 penetration;
    if ( Triangle::sphereIntersectsTriangle( sphere, a, b, c, &penetration ) ) {
        // ...
    }
}
// third case
{
    Triangle triangle;
    
    vec3 sphere_center;
    float sphere_radius;
    
    vec3 penetration;
    if ( Triangle::sphereIntersectsTriangle( sphere_center, sphere_radius, triangle, &penetration ) ) {
        // ...
    }
}
// fourth case
{
    Triangle triangle;
    Sphere sphere;
    
    vec3 penetration;
    if ( Triangle::sphereIntersectsTriangle( sphere, triangle, &penetration ) ) {
        // ...
    }
}
```
#### aabbIntersectsTriangle
```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

// first case
{
    // triangle vertex a, b, c
    vec3 a, b, c;
    AABB aabb;
    
    if ( Triangle::aabbIntersectsTriangle( aabb, a, b, c ) ) {
        // ...
    }
}
// second case
{
    Triangle triangle;
    AABB aabb;
    
    if ( Triangle::aabbIntersectsTriangle( aabb, triangle ) ) {
        // ...
    }
}
```

