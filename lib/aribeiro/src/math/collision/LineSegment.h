#ifndef LineSegment_hpp
#define LineSegment_hpp

#include <aribeiro/common.h>
#include <aribeiro/vec3.h>

namespace aRibeiro {

namespace collision {

    class AABB;
    class Plane;
    class Sphere;
    class LineSegment;
    class Triangle;

    /// \brief Line Segment representation
    ///
    /// Can be used to compute intersection with other shapes (Triangle, AABB, Sphere)
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE LineSegment {
    public:
        vec3 a, b;

        /// \brief Construct a line segment with point a and b in the coordinate origin. vec3(0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        LineSegment();

        /// \brief Construct a line segment with point a and b
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_a, ptn_b;
        ///
        /// LineSegment lineSegment = LineSegment( ptn_a, ptn_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The point a from line segment
        /// \param b The point b from line segment
        ///
        LineSegment(const vec3 &a, const vec3 &b);

        /// \brief Compute the point in the line that is closer to another specified point
        ///
        /// Given segment ab and point c, computes closest point d on ab.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_a, ptn_b;
        /// vec3 ptn_input;
        ///
        /// vec3 closest_point = LineSegment::closestPointToSegment( ptn_to_input, ptn_a, ptn_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param a The point a from line segment
        /// \param b The point b from line segment
        /// \return The closest point in the line related to p
        ///
        static vec3 closestPointToSegment(const vec3 &p, const vec3 &a, const vec3 &b);

        /// \brief Compute the point in the line that is closer to another specified point
        ///
        /// Given segment ab and point c, computes closest point d on ab.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// vec3 ptn_input;
        ///
        /// vec3 closest_point = LineSegment::closestPointToSegment( ptn_to_input, lineSegment );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param ls The line segment
        /// \return The closest point in the line related to p
        ///
        static vec3 closestPointToSegment(const vec3 &p, const LineSegment &ls);

        //
        // Cloned methods from other collision classes
        //
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
        /// if ( LineSegment::aabbIntersectsSegment( aabb, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param p0 line segment point a
        /// \param p1 line segment point b
        /// \return true, if the aabb overlaps the line segment
        ///
        static bool aabbIntersectsSegment(const AABB &aabb, const vec3& p0, const vec3& p1);

        /// \brief Test if a line segment intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// AABB aabb;
        ///
        /// if ( LineSegment::aabbIntersectsSegment( aabb, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param ls line segment
        /// \return true, if the aabb overlaps the line segment
        ///
        static bool aabbIntersectsSegment(const AABB &aabb, const LineSegment& ls);

        /// \brief Test if a line segment intersects the Plane
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
        /// Plane plane;
        ///
        /// if ( LineSegment::planeIntersectsSegment( plane, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param plane The Plane
        /// \param a line segment point a
        /// \param b line segment point b
        /// \return true, if the plane intersects the line segment
        ///
        static bool planeIntersectsSegment(const Plane &plane, const vec3 &a, const vec3 &b);

        /// \brief Test if a line segment intersects the Plane
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// Plane plane;
        ///
        /// if ( LineSegment::planeIntersectsSegment( plane, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param plane The Plane
        /// \param ls line segment
        /// \return true, if the plane intersects the line segment
        ///
        static bool planeIntersectsSegment(const Plane &plane, const LineSegment &ls);


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
        /// if ( LineSegment::sphereIntersectsSegment( sphere_center, sphere_radius, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The Sphere center
        /// \param radius The Sphere radius
        /// \param a line segment point a
        /// \param b line segment point b
        /// \return true, if the sphere intersects the line segment
        ///
        static bool sphereIntersectsSegment(const vec3 &center, const float &radius, const vec3& a, const vec3& b);

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
        /// if ( LineSegment::sphereIntersectsSegment( sphere_center, sphere_radius, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The Sphere center
        /// \param radius The Sphere radius
        /// \param ls line segment
        /// \return true, if the sphere intersects the line segment
        ///
        static bool sphereIntersectsSegment(const vec3 &center, const float &radius, const LineSegment& ls);


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
        /// if ( LineSegment::sphereIntersectsSegment( sphere, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param a line segment point a
        /// \param b line segment point b
        /// \return true, if the sphere intersects the line segment
        ///
        static bool sphereIntersectsSegment(const Sphere &sphere, const vec3& a, const vec3& b);

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
        /// if ( LineSegment::sphereIntersectsSegment( sphere, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param ls line segment
        /// \return true, if the sphere intersects the line segment
        ///
        static bool sphereIntersectsSegment(const Sphere &sphere, const LineSegment& ls);

        /// \brief Test if a line segment intersects the triangle
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
        /// // line segment
        /// vec3 p, q;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( LineSegment::triangleIntersectsSegment( a, b, c, p, q, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Triangle vertex a
        /// \param b Triangle vertex b
        /// \param c Triangle vertex c
        /// \param p line segment point a
        /// \param q line segment point b
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static bool triangleIntersectsSegment(
            const vec3& a, const vec3& b, const vec3& c,
            const vec3& p, const vec3& q,
            float *u, float *v, float *w);

        /// \brief Test if a line segment intersects the triangle
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
        /// LineSegment lineSegment;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( LineSegment::triangleIntersectsSegment( a, b, c, lineSegment, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Triangle vertex a
        /// \param b Triangle vertex b
        /// \param c Triangle vertex c
        /// \param ls line segment
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static bool triangleIntersectsSegment(
            const vec3& a, const vec3& b, const vec3& c,
            const LineSegment& ls,
            float *u, float *v, float *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// LineSegment lineSegment;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( LineSegment::triangleIntersectsSegment( triangle, lineSegment, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param t triangle
        /// \param ls line segment
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static bool triangleIntersectsSegment(
            const Triangle& t,
            const LineSegment& ls,
            float *u, float *v, float *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// // line segment
        /// vec3 p, q;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( LineSegment::triangleIntersectsSegment( triangle, p, q, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param t triangle
        /// \param p line segment point a
        /// \param q line segment point b
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static bool triangleIntersectsSegment(
            const Triangle& t,
            const vec3& p, const vec3& q,
            float *u, float *v, float *w);


        SSE2_CLASS_NEW_OPERATOR
    } _SSE2_ALIGN_POS;

}
}

#endif
