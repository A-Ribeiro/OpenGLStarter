#ifndef Ray_h
#define Ray_h

#include <aribeiro/common.h>

//I need a complete definition to instantiate a vec3 as class field
#include <aribeiro/vec3.h>

namespace aRibeiro {
namespace collision {
    class AABB;
    class Plane;
    class Sphere;
    class Triangle;

    /// \brief Ray representation
    ///
    /// Used in the raycast methods.
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE Ray {
    public:
        vec3 origin;
        vec3 dir;

        /// \brief Construct a Ray with origin = dir = vec3(0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Ray();

        /// \brief Construct a Ray
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 origin, direction;
        ///
        /// Ray ray = Ray( origin, direction );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Ray(const vec3 &origin, const vec3 &dir);

        //
        // Cloned methods from other collision classes
        //

        /// \brief Raycast test against an AABB
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against AABB a.
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// AABB aabb;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastAABB(ray, aabb, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param a The AABB
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the aabb
        ///
        static bool raycastAABB(const Ray &r, const AABB& a, float *outTmin, vec3 *outNormal);

        /// \brief Raycast test against a Plane
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against Plane
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// Plane plane;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastPlane(ray, aabb, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param plane The Plane
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the plane
        ///
        static bool raycastPlane(const Ray &r, const Plane &plane, float *outTmin, vec3 *outNormal);

        /// \brief Raycast test against a Sphere
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Sphere
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// //sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastSphere(ray, sphere_center, sphere_radius, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the sphere
        ///
        static bool raycastSphere(const Ray &r, const vec3 &center, const float& radius, float *outTmin, vec3 *outNormal);

        /// \brief Raycast test against a Sphere
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Sphere
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// Sphere sphere;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastSphere(ray, sphere, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param sphere The Sphere
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the sphere
        ///
        static bool raycastSphere(const Ray &r, const Sphere &sphere, float *outTmin, vec3 *outNormal);

        /// \brief Raycast test against a Triangle
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Triangle
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// //triangle
        /// vec3 a, b, c;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastTriangle(ray, a, b, c, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ray The Ray
        /// \param a The triangle a
        /// \param b The triangle b
        /// \param c The triangle c
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the sphere
        ///
        static bool raycastTriangle(const Ray &ray, const vec3 &a, const vec3 &b, const vec3&c, float *outTmin, vec3 *outNormal);

        /// \brief Raycast test against a Triangle
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Triangle
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// Triangle triangle;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastTriangle(ray, triangle, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ray The Ray
        /// \param t The Triangle
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the sphere
        ///
        static bool raycastTriangle(const Ray &ray, const Triangle &t, float *outTmin, vec3 *outNormal);

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}
}


#endif
