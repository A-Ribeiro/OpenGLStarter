#ifndef Triangle_hpp
#define Triangle_hpp

#include <aribeiro/common.h>
#include <aribeiro/vec3.h>

namespace aRibeiro {
namespace collision {
    class Ray;
    class Sphere;
    class LineSegment;
    class AABB;

    /// \brief Triangle representation
    ///
    /// THe methods of this class can be used with the vertex parameters directly.
    ///
    /// It is possible to use it with a vertex list.
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE Triangle {

    public:
        vec3 a, b, c;

        /// \brief Construct a Triangle with a = b = c = vec3(0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Triangle();

        /// \brief Construct a Triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 a, b, c;
        ///
        /// Triangle triangle( a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The triangle a
        /// \param b The triangle b
        /// \param c The triangle c
        ///
        Triangle(const vec3 &a, const vec3 &b, const vec3 &c);

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
        /// if ( Triangle::raycastTriangle(ray, a, b, c, &tmin, &normal) ) {
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
        /// if ( Triangle::raycastTriangle(ray, triangle, &tmin, &normal) ) {
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

        //returns the closest point inside the triangle relative to the parameter p

        /// \brief Compute the point in the Triangle surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// // triangle
        /// vec3 a, b, c;
        ///
        /// vec3 closest_point = Triangle::closestPointToTriangle( ptn_to_input, a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \return The closest point in the Triangle related to p
        ///
        static vec3 closestPointToTriangle(const vec3& p, const vec3& a, const vec3& b, const vec3& c);

        /// \brief Compute the point in the Triangle surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// Triangle triangle;
        ///
        /// vec3 closest_point = Triangle::closestPointToTriangle( ptn_to_input, triangle );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param t The Triangle
        /// \return The closest point in the Triangle related to p
        ///
        static vec3 closestPointToTriangle(const vec3& p, const Triangle &t);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Given line pq and ccw triangle abc, return whether line pierces triangle. <br />
        /// If so, also return the barycentric coordinates (u,v,w) of the intersection point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // line segment
        /// vec3 p, q;
        /// // triangle
        /// vec3 a, b, c;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( Triangle::segmentIntersectsTriangle( p, q, a, b, c, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p line segment point a
        /// \param q line segment point b
        /// \param a triangle vertex a
        /// \param b triangle vertex b
        /// \param c triangle vertex c
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static bool segmentIntersectsTriangle(const vec3& p, const vec3& q,
            const vec3& a, const vec3& b, const vec3& c,
            float *u, float *v, float *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Given line pq and ccw triangle abc, return whether line pierces triangle. <br />
        /// If so, also return the barycentric coordinates (u,v,w) of the intersection point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// // triangle
        /// vec3 a, b, c;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( Triangle::segmentIntersectsTriangle( lineSegment, a, b, c, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param a triangle vertex a
        /// \param b triangle vertex b
        /// \param c triangle vertex c
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static bool segmentIntersectsTriangle(const LineSegment& ls,
            const vec3& a, const vec3& b, const vec3& c,
            float *u, float *v, float *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Given line pq and ccw triangle abc, return whether line pierces triangle. <br />
        /// If so, also return the barycentric coordinates (u,v,w) of the intersection point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // line segment
        /// vec3 p, q;
        /// Triangle triangle;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( Triangle::segmentIntersectsTriangle( p, q, triangle, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p line segment point a
        /// \param q line segment point b
        /// \param t Triangle
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static bool segmentIntersectsTriangle(const vec3& p, const vec3& q,
            const Triangle& t,
            float *u, float *v, float *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Given line pq and ccw triangle abc, return whether line pierces triangle. <br />
        /// If so, also return the barycentric coordinates (u,v,w) of the intersection point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// Triangle triangle;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( Triangle::segmentIntersectsTriangle( lineSegment, triangle, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param t Triangle
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static bool segmentIntersectsTriangle(const LineSegment& ls,
            const Triangle& t,
            float *u, float *v, float *w);


        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle
        /// vec3 a, b, c;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// vec3 penetration;
        /// if ( Triangle::sphereIntersectsTriangle( sphere_center, sphere_radius, a, b, c, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static bool sphereIntersectsTriangle(const vec3& center, const float &radius, const vec3& a, const vec3& b, const vec3& c, vec3 *penetration);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle
        /// vec3 a, b, c;
        /// Sphere sphere;
        ///
        /// vec3 penetration;
        /// if ( Triangle::sphereIntersectsTriangle( sphere, a, b, c, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static bool sphereIntersectsTriangle(const Sphere& sphere, const vec3& a, const vec3& b, const vec3& c, vec3 *penetration);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// vec3 penetration;
        /// if ( Triangle::sphereIntersectsTriangle( sphere_center, sphere_radius, triangle, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param t The Triangle
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static bool sphereIntersectsTriangle(const vec3& center, const float &radius, const Triangle& t, vec3 *penetration);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// Sphere sphere;
        ///
        /// vec3 penetration;
        /// if ( Triangle::sphereIntersectsTriangle( sphere, triangle, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param t The Triangle
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static bool sphereIntersectsTriangle(const Sphere& sphere, const Triangle& t, vec3 *penetration);

        //
        // Cloned methods from other collision classes
        //
        /// \brief Test if a triangle intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle vertex a, b, c
        /// vec3 a, b, c;
        /// AABB aabb;
        ///
        /// if ( Triangle::aabbIntersectsTriangle( aabb, a, b, c ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param v0 The triangle vertex a
        /// \param v1 The triangle vertex b
        /// \param v2 The triangle vertex c
        /// \return true, if the aabb intersects the triangle
        ///
        static bool aabbIntersectsTriangle(const AABB &aabb, const vec3 &v0, const vec3 &v1, const vec3 &v2);

        /// \brief Test if a triangle intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// AABB aabb;
        ///
        /// if ( Triangle::aabbIntersectsTriangle( aabb, triangle ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param triangle The Triangle
        /// \return true, if the aabb intersects the triangle
        ///
        static bool aabbIntersectsTriangle(const AABB &aabb, const Triangle &triangle);

        SSE2_CLASS_NEW_OPERATOR

    }_SSE2_ALIGN_POS;

}
}

#endif
