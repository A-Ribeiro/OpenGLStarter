#ifndef Sphere_hpp
#define Sphere_hpp

#include <aribeiro/common.h>
#include <aribeiro/vec3.h>

namespace aRibeiro {

namespace collision {

    class Ray;
    class LineSegment;
    class AABB;
    class Frustum;
    class Triangle;
    
    /// \brief Sphere representation
    ///
    /// It stores the center and radius
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE Sphere {
    public:

        vec3 center;
        float radius;

        /// \brief Construct a Sphere with center = vec3(0,1,0) and radius = 1.0f
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Sphere();

        /// \brief Construct a Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// Sphere sphere = Sphere( sphere_center, sphere_radius );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center the center of the sphere
        /// \param radius the radius of the sphere
        ///
        Sphere(const vec3 &center, float radius);

        /// \brief Compute the point in the Sphere surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// vec3 closest_point = Sphere::closestPointToSphere( ptn_to_input, sphere_center, sphere_radius );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return The closest point in the Sphere related to p
        ///
        static vec3 closestPointToSphere(const vec3& p, const vec3& center, const float& radius);

        /// \brief Compute the point in the Sphere surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// Sphere sphere;
        ///
        /// vec3 closest_point = Sphere::closestPointToSphere( ptn_to_input, sphere );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param sphere The Sphere
        /// \return The closest point in the Sphere related to p
        ///
        static vec3 closestPointToSphere(const vec3& p, const Sphere& sphere);

        /// \brief Test if there is some overlaped area between two Spheres
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere_a;
        /// Sphere sphere_b;
        ///
        /// if ( Sphere::sphereOverlapsSphere( sphere_a, sphere_b ) ){
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first Sphere to test
        /// \param b The second Sphere to test against
        /// \return true if there are some overlaped area between the two Spheres, otherwise false
        ///
        static bool sphereOverlapsSphere(const Sphere& a,const Sphere& b);

        /// \brief Create an Sphere that is the union result between the two Spheres
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere_a;
        /// Sphere sphere_b;
        ///
        /// Sphere sphere = Sphere::joinSpheres( sphere_a, sphere_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first Sphere to consider in the union
        /// \param b The second Sphere to consider in the union
        /// \return Sphere of the union of the parameters
        ///
        static Sphere joinSpheres(const Sphere &a, const Sphere &b);

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
        /// if ( Sphere::raycastSphere(ray, sphere_center, sphere_radius, &tmin, &normal) ) {
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
        static bool raycastSphere(const Ray &r, const vec3 &center, const float &radius, float *outTmin, vec3 *outNormal);

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
        /// if ( Sphere::raycastSphere(ray, sphere, &tmin, &normal) ) {
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

        /// \brief Test if a line segment intersects the Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// //line segment a and b
        /// vec3 a, b;
        /// //sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( Sphere::segmentIntersectsSphere( a, b, sphere_center, sphere_radius ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a line segment point a
        /// \param b line segment point b
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return true, if the sphere intersects the line segment
        ///
        static bool segmentIntersectsSphere(const vec3& a, const vec3& b, const vec3 &center, const float &radius);

        /// \brief Test if a line segment intersects the Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// //sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( Sphere::segmentIntersectsSphere( lineSegment, sphere_center, sphere_radius ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return true, if the sphere intersects the line segment
        ///
        static bool segmentIntersectsSphere(const LineSegment& ls, const vec3 &center, const float &radius);

        /// \brief Test if a line segment intersects the Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// //line segment a and b
        /// vec3 a, b;
        /// Sphere sphere;
        ///
        /// if ( Sphere::segmentIntersectsSphere( a, b, sphere ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a line segment point a
        /// \param b line segment point b
        /// \param sphere The Sphere
        /// \return true, if the sphere intersects the line segment
        ///
        static bool segmentIntersectsSphere(const vec3& a, const vec3& b, const Sphere &sphere);

        /// \brief Test if a line segment intersects the Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// Sphere sphere;
        ///
        /// if ( Sphere::segmentIntersectsSphere( lineSegment, sphere ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param sphere The Sphere
        /// \return true, if the sphere intersects the line segment
        ///
        static bool segmentIntersectsSphere(const LineSegment& ls, const Sphere &sphere);

        /// \brief Test if a point is inside a Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 point;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( Sphere::pointInsideSphere( point, sphere_center, sphere_radius ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p line point to test
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return true, if the point is inside the sphere
        ///
        static bool pointInsideSphere(const vec3& p, const vec3 &center, const float &radius);

        /// \brief Test if a point is inside a Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 point;
        /// Sphere sphere;
        ///
        /// if ( Sphere::pointInsideSphere( point, sphere ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p line point to test
        /// \param sphere The Sphere
        /// \return true, if the point is inside the sphere
        ///
        static bool pointInsideSphere(const vec3& p, const Sphere &sphere);

        //
        // Cloned methods from other collision classes
        //

        /// \brief Test if a sphere overlaps the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 sphere_center;
        /// float sphere_radius;
        /// AABB aabb;
        ///
        /// vec3 penetration;
        /// if ( Sphere::aabbOverlapsSphere( aabb, sphere_center, sphere_radius, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the aabb overlaps the sphere
        ///
        static bool aabbOverlapsSphere(const AABB& aabb, const vec3 &center, const float &radius, vec3 *penetration);

        /// \brief Test if a sphere overlaps the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere;
        /// AABB aabb;
        ///
        /// vec3 penetration;
        /// if ( Sphere::aabbOverlapsSphere( aabb, sphere, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param sphere The Sphere
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the aabb overlaps the sphere
        ///
        static bool aabbOverlapsSphere(const AABB& aabb, const Sphere &sphere, vec3 *penetration);

        /// \brief Test if a frustum overlaps the sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Frustum frustum;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( Sphere::frustumOverlapsSphere( frustum, sphere_center, sphere_radius ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param f The frustum
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return true, if the sphere overlaps the frustum
        ///
        static bool frustumOverlapsSphere(const Frustum &f, const vec3 &center, const float &radius);

        /// \brief Test if a frustum overlaps the sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Frustum frustum;
        /// Sphere sphere;
        ///
        /// if ( Sphere::frustumOverlapsSphere( frustum, sphere ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param f The frustum
        /// \param s The Sphere
        /// \return true, if the sphere overlaps the frustum
        ///
        static bool frustumOverlapsSphere(const Frustum &f, const Sphere &s);

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
        /// if ( Sphere::triangleIntersectsSphere( a, b, c, sphere_center, sphere_radius, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param[out] penetration returns the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static bool triangleIntersectsSphere(const vec3& a, const vec3& b, const vec3& c, const vec3 &center, const float &radius, vec3 *penetration);

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
        /// if ( Sphere::triangleIntersectsSphere( a, b, c, sphere, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \param sphere The Sphere
        /// \param[out] penetration returns the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static bool triangleIntersectsSphere(const vec3& a, const vec3& b, const vec3& c, const Sphere& sphere, vec3 *penetration);

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
        /// if ( Sphere::triangleIntersectsSphere( triangle, sphere_center, sphere_radius, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param triangle The Triangle
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param[out] penetration returns the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static bool triangleIntersectsSphere(const Triangle& triangle, const vec3 &center, const float &radius, vec3 *penetration);

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
        /// if ( Sphere::triangleIntersectsSphere( triangle, sphere, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param triangle The Triangle
        /// \param sphere The Sphere
        /// \param[out] penetration returns the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static bool triangleIntersectsSphere(const Triangle& triangle, const Sphere& sphere, vec3 *penetration);

        /// \brief Compute the minimum bounding sphere from four points (Tetrahedron)
        ///
        /// Details are in:
        ///
        /// https://gamedev.stackexchange.com/questions/162731/welzl-algorithm-to-find-the-smallest-bounding-sphere
        ///
        ///https://mathworld.wolfram.com/Circumsphere.html
        ///
        /// \author Alessandro Ribeiro
        /// \param a Tetrahedron point a
        /// \param b Tetrahedron point b
        /// \param c Tetrahedron point c
        /// \param d Tetrahedron point d
        /// \return the minimum sphere
        ///
        static Sphere from4Points(const vec3&a, const vec3&b, const vec3&c, const vec3&d);

        /// \brief Compute the bounding sphere from a frustum
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param frustum The frustum
        /// \return the bouding sphere
        ///
        static Sphere fromFrustum(const Frustum& frustum);

        /// \brief Compute the bounding sphere from an AABB
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param discard_z If true, will compute just the x and y coords
        /// \return the bouding sphere
        ///
        static Sphere fromAABB(const AABB& aabb, bool discard_z = false);

        /// \brief Compute the bounding sphere from a line segment
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param ls The line segment
        /// \return the bouding sphere
        ///
        static Sphere fromLineSegment(const LineSegment& ls);

        /// \brief Compute the bounding sphere from a triangle (Not tested...)
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param triangle The Triangle
        /// \return the bouding sphere
        ///
        static Sphere fromTriangle(const Triangle& triangle);

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;
}

}

#endif
