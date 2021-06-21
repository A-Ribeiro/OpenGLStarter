#ifndef AABB_hpp
#define AABB_hpp

#include <aribeiro/common.h>

//I need a complete definition to instantiate a vec3 as class field
#include <aribeiro/vec3.h>
#include <aribeiro/vec2.h>

namespace aRibeiro {

//class vec2;

namespace collision {

class Sphere;
class LineSegment;
class Plane;
class Ray;
class Triangle;
class Frustum;

/// \brief Axis Aligned Bounding Box (AABB)
///
/// Stores 3D points to represent an Axis Aligned Bounding Box. <br/>
/// It can be used to make collision tests
///
/// \author Alessandro Ribeiro
///
class _SSE2_ALIGN_PRE AABB{
    public:
    vec3 min_box;///<Store the minimum values of the AABB Box
    vec3 max_box;///<Store the maximum values of the AABB Box
    //--------------------------------------------------------------------------
    /// \brief Construct a ZERO AABB class
    ///
    /// The ZERO AABB class have both points in the origin (0,0,0)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// AABB aabb = AABB();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    AABB();
    //--------------------------------------------------------------------------
    /// \brief Constructs a new bidimensional AABB class
    ///
    /// Requires to pass 2 points to construct of the AABB class
    ///
    /// Ex:
    /// <pre>
    ///  CornerA *--* CornerB
    ///          |  |
    ///  CornerC *--* CornerD
    /// </pre>
    ///
    /// You can use any combination: {CornerA,CornerD}, {CornerC,CornerB}, etc...
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// vec2 corner_a, corner_d;
    ///
    /// AABB aabb = AABB(corner_d, corner_a);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a An AABB corner
    /// \param b Another AABB corner
    /// \warning The z value of both min_box and max_box are -1 and 1 respectively
    ///
    AABB(const vec2 &a,const vec2 &b);
    //--------------------------------------------------------------------------
    /// \brief Constructs a new bidimensional AABB class
    ///
    /// Requires to pass 2 points to construct of the AABB class
    ///
    /// Ex:
    /// <pre>
    ///  CornerA *--* CornerB
    ///          |  |
    ///  CornerC *--* CornerD
    /// </pre>
    ///
    /// You can use any combination: {CornerA,CornerD}, {CornerC,CornerB}, etc...
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// vec3 corner_c, corner_b;
    ///
    /// AABB aabb = AABB(corner_b, corner_c);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a An AABB corner
    /// \param b Another AABB corner
    ///
    AABB(const vec3 &a,const vec3 &b);
    //--------------------------------------------------------------------------
    // Static methods
    //--------------------------------------------------------------------------

    /// \brief Test if a point is inside an AABB
    /// \author Alessandro Ribeiro
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// AABB aabb;
    /// vec2 point;
    ///
    /// if ( AABB::pointInsideAABB( point, aabb ) ){
    ///     ...
    /// }
    /// \endcode
    ///
    /// \param ptn 2D point to test against the AABB
    /// \param b The AABB
    /// \return true if the point is inside the AABB, otherwise false
    /// \warning The z is not used in the test
    ///
    static bool pointInsideAABB(const vec2& ptn,const AABB& b);
    /// \brief Test if a point is inside an AABB
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// AABB aabb;
    /// vec3 point;
    ///
    /// if ( AABB::pointInsideAABB( point, aabb ) ){
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param ptn 3D point to test against the AABB
    /// \param b The AABB
    /// \return true if the point is inside the AABB, otherwise false
    ///
    static bool pointInsideAABB(const vec3& ptn,const AABB& b);

    /// \brief Test if there is some overlaped area between two AABBs
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// AABB aabb_a;
    /// AABB aabb_b;
    ///
    /// if ( AABB::aabbOverlapsAABB( aabb_a, aabb_b ) ){
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a The first AABB to test
    /// \param b The second AABB to test against
    /// \return true if there are some overlaped area between the two AABBs, otherwise false
    ///
    static bool aabbOverlapsAABB(const AABB& a,const AABB& b);

    /// \brief Create an AABB that is the union result between the two AABBs
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// AABB aabb_a;
    /// AABB aabb_b;
    ///
    /// AABB aabb = AABB::joinAABB( aabb_a, aabb_b );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a The first AABB to consider in the union
    /// \param b The second AABB to consider in the union
    /// \return AABB of the union of the parameters
    ///
    static AABB joinAABB(const AABB& a,const AABB& b);

    /// \brief Create an AABB that that contains the triangle vertex a, b and c
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// // triangle vertex
    /// vec3 a, b, c;
    ///
    /// AABB aabb = AABB::fromTriangle( a, b, c );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a Triangle vertex a
    /// \param b Triangle vertex b
    /// \param c Triangle vertex c
    /// \return AABB containing the triangle
    ///
    static AABB fromTriangle(const vec3& a, const vec3& b, const vec3& c);

    /// \brief Create an AABB that that contains the triangle
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// Triangle triangle;
    ///
    /// AABB aabb = AABB::fromTriangle( triangle );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param triangle Triangle
    /// \return AABB containing the triangle
    ///
    static AABB fromTriangle(const Triangle& triangle);

    /// \brief Create an AABB that that contains the sphere with pos as position and radius
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// // sphere parameters
    /// vec3 sphere_pos;
    /// float sphere_radius;
    ///
    /// AABB aabb = AABB::fromSphere( sphere_pos, sphere_radius );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param pos sphere center position
    /// \param radius sphere radius
    /// \return AABB containing the sphere
    ///
    static AABB fromSphere(const vec3& pos, const float &radius);
    
    /// \brief Create an AABB that that contains the sphere
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// Sphere sphere;
    ///
    /// AABB aabb = AABB::fromSphere( sphere );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param sphere Sphere
    /// \return AABB containing the sphere
    ///
    static AABB fromSphere(const Sphere& sphere);

    /// \brief Create an AABB that that contains the line segment
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// // segment points
    /// vec3 segment_a, segment_b;
    ///
    /// AABB aabb = AABB::fromLineSegment( segment_a, segment_b );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a Point 'a' from the line segment
    /// \param b Point 'b' from the line segment
    /// \return AABB containing the line segment
    ///
    static AABB fromLineSegment(const vec3& a, const vec3& b);

    /// \brief Create an AABB that that contains the line segment
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// LineSegment lineSegment;
    ///
    /// AABB aabb = AABB::fromLineSegment( lineSegment );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param ls the line segment
    /// \return AABB containing the line segment
    ///
    static AABB fromLineSegment(const LineSegment& ls);

    /// \brief Create an AABB that that contains the frustum
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// Frustum frustum;
    ///
    /// AABB aabb = AABB::fromFrustum( frustum );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param frustum the frustum
    /// \return AABB containing the line segment
    ///
    static AABB fromFrustum(const Frustum& frustum);
    
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
    /// if ( AABB::raycastAABB(ray, aabb, &tmin, &normal) ) {
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

    /// \brief Test if a line segment intersects the AABB
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
    /// AABB aabb;
    ///
    /// if ( AABB::segmentIntersectsAABB( a, b, aabb ) ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param p0 line segment point a
    /// \param p1 line segment point b
    /// \param aabb The AABB
    /// \return true, if the aabb overlaps the line segment
    ///
    static bool segmentIntersectsAABB(const vec3& p0, const vec3& p1, const AABB &aabb);

    /// \brief Test if a line segment intersects the AABB
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// LineSegment ls;
    /// AABB aabb;
    ///
    /// if ( AABB::segmentIntersectsAABB( ls, aabb ) ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param ls line segment
    /// \param aabb The AABB
    /// \return true, if the aabb overlaps the line segment
    ///
    static bool segmentIntersectsAABB(const LineSegment& ls, const AABB &aabb);

    /// \brief Compute the point in the AABB surface that is closer to another specified point
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// vec3 ptn_to_input;
    /// AABB aabb;
    ///
    /// vec3 closest_point = AABB::closestPointToAABB( ptn_to_input, aabb );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param p The input point to compute closest point
    /// \param aabb The AABB
    /// \return The closest point in the AABB related to p
    ///
    static vec3 closestPointToAABB(const vec3 &p, const AABB &aabb);

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
    /// if ( AABB::sphereOverlapsAABB( sphere_center, sphere_radius, aabb, &penetration ) ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param center The sphere center
    /// \param radius The sphere radius
    /// \param aabb The AABB
    /// \param penetration **return** the amount one shape is inside the other
    /// \return true, if the aabb overlaps the sphere
    ///
    static bool sphereOverlapsAABB(const vec3 &center, const float &radius, const AABB& aabb, vec3 *penetration);

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
    /// if ( AABB::sphereOverlapsAABB( sphere, aabb, &penetration ) ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param sphere The Sphere
    /// \param aabb The AABB
    /// \param penetration **return** the amount one shape is inside the other
    /// \return true, if the aabb overlaps the sphere
    ///
    static bool sphereOverlapsAABB(const Sphere &sphere, const AABB& aabb, vec3 *penetration);

    /// \brief Test if a plane intersects the AABB
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// Plane plane;
    /// AABB aabb;
    ///
    /// if ( AABB::planeIntersectsAABB( plane, aabb ) ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param plane The Plane
    /// \param aabb The AABB
    /// \return true, if the aabb intersects the plane
    ///
    static bool planeIntersectsAABB(const Plane &plane, const AABB &aabb);

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
    /// if ( AABB::triangleIntersectsAABB( a, b, c, aabb ) ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v0 The triangle vertex a
    /// \param v1 The triangle vertex b
    /// \param v2 The triangle vertex c
    /// \param aabb The AABB
    /// \return true, if the aabb intersects the triangle
    ///
    static bool triangleIntersectsAABB(const vec3 &v0, const vec3 &v1, const vec3 &v2, const AABB &aabb);

    /// \brief Test if a triangle overlaps the AABB
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
    /// if ( AABB::triangleIntersectsAABB( triangle, aabb ) ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param t The triangle
    /// \param aabb The AABB
    /// \return true, if the aabb overlaps the triangle
    ///
    static bool triangleIntersectsAABB(const Triangle &t, const AABB &aabb);

    //
    // Cloned methods from other collision classes
    //

    /// \brief Test if a frustum overlaps the aabb
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    /// using namespace aRibeiro::collision;
    ///
    /// Frustum frustum;
    /// AABB aabb;
    ///
    /// if ( AABB::frustumOverlapsAABB( frustum, aabb ) ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param f The frustum
    /// \param aabb The AABB
    /// \return true, if the aabb overlaps the frustum
    ///
    static bool frustumOverlapsAABB(const Frustum &f, const AABB &aabb);

    SSE2_CLASS_NEW_OPERATOR
} _SSE2_ALIGN_POS;

}

}

#endif
