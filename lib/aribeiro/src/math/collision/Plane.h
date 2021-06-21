#ifndef Plane_hpp
#define Plane_hpp

#include <aribeiro/common.h>
#include <aribeiro/vec3.h>

namespace aRibeiro {

namespace collision {
    class Ray;
    class Triangle;
    class LineSegment;
    class AABB;

    /// \brief Plane representation
    ///
    /// It stores the plane normal and the distance to origin.
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE Plane {
    public:
        vec3 normal;
        float distance;

        /// \brief Construct a Plane with normal = vec3(0,1,0) and distance = 0
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Plane();

        /// \brief Construct a Plane
        ///
        /// With this constructor it is possible to define a plane using a 3d point and a normal.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 point_in_plane;
        /// vec3 normal;
        ///
        /// Plane plane = Plane( point_in_plane, normal );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param point a point in the plane
        /// \param normal the plane normal
        ///
        Plane(const vec3 &point, const vec3 &normal);

        /// \brief Construct a Plane
        ///
        /// With this constructor it is possible to define a plane using three other points (a triangle).
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle a, b, c
        /// vec3 a, b, c;
        ///
        /// Plane plane = Plane( a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a triangle vertex a
        /// \param b triangle vertex b
        /// \param c triangle vertex c
        ///
        Plane(const vec3 &a, const vec3 &b, const vec3 &c);

        /// \brief Make the plane normal to have the magnitude 1
        ///
        /// Normalize the normal from the plane equation.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane;
        ///
        /// plane.normalize();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void normalize();

        //
        // Static constructors
        //
        /// \brief Create a Plane from a triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle a, b, c
        /// vec3 a, b, c;
        ///
        /// Plane plane = Plane::fromTriangle( a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a triangle vertex a
        /// \param b triangle vertex b
        /// \param c triangle vertex c
        /// \return The plane from a triangle
        ///
        static Plane fromTriangle(const vec3 &a, const vec3 &b, const vec3 &c);

        /// \brief Create a Plane from a triangle
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
        /// Plane plane = Plane::fromTriangle( triangle );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param t Triangle
        /// \return The plane from a triangle
        ///
        static Plane fromTriangle(const Triangle &t);

        //
        // Point operations
        //

        /// \brief Compute the point in the plane that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// Plane plane;
        ///
        /// vec3 closest_point = Plane::closestPointToPlane( ptn_to_input, plane );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param plane The Plane
        /// \return The closest point in the plane related to p
        ///
        static vec3 closestPointToPlane(const vec3 &p, const Plane &plane);

        /// \brief Compute the distance of a point related to the plane
        ///
        /// This distance can be seen as the projection of that point in the plane's normal.
        ///
        /// The result has a sign, indicating the side the point is related to the plane.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 point;
        /// Plane plane;
        ///
        /// float signed_distance = Plane::pointDistanceToPlane( point, plane );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point
        /// \param plane The Plane
        /// \return Signed distance of the point related to the plane
        ///
        static float pointDistanceToPlane(const vec3 &p, const Plane &plane);

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
        /// if ( Plane::raycastPlane(ray, aabb, &tmin, &normal) ) {
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
        /// if ( Plane::segmentIntersectsPlane( a, b, plane ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a line segment point a
        /// \param b line segment point b
        /// \param plane The Plane
        /// \return true, if the plane intersects the line segment
        ///
        static bool segmentIntersectsPlane(const vec3 &a, const vec3 &b, const Plane &plane);

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
        /// if ( Plane::segmentIntersectsPlane( lineSegment, plane ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param plane The Plane
        /// \return true, if the plane intersects the line segment
        ///
        static bool segmentIntersectsPlane(const LineSegment &ls, const Plane &plane);

        /// \brief Compute a plane to plane intersection with two (2) planes
        ///
        /// Given planes p1 and p2, compute line L = p+t*d of their intersection.
        ///
        /// The plane to plane intersetion, when occurs, can lead to a line result.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane_a, plane_b;
        ///
        /// vec3 line_position, line_direction;
        /// if ( Plane::intersectPlaneToPlane( plane_a, plane_b, &line_position, &line_direction ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p1 The first plane
        /// \param p2 The second plane
        /// \param outP **return** line position
        /// \param outD **return** line direction
        /// \return true, if the plane to plane intersection has a result
        ///
        static bool intersectPlaneToPlane(const Plane &p1, const Plane &p2, vec3 *outP, vec3 *outD);
        
        /// \brief Compute a plane to plane intersection with three (3) planes
        ///
        /// Compute the point p at which the three planes p1, p2 and p3 intersect (if at all)
        ///
        /// The plane to plane (three planes) intersetion, when occurs, can lead to a point.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane_a, plane_b, plane_c;
        ///
        /// vec3 position;
        /// if ( Plane::intersectPlanes( plane_a, plane_b, plane_c, &position ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p1 The first plane
        /// \param p2 The second plane
        /// \param p3 The third plane
        /// \param outP **return** intersection position
        /// \return true, if the plane to plane to plane intersection has a result
        ///
        static bool intersectPlanes(const Plane &p1, const Plane &p2, const Plane &p3, vec3 *outP);

        //
        // Cloned methods from other collision classes
        //

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
        /// if ( Plane::aabbIntersectsPlane( aabb, plane ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param plane The Plane
        /// \return true, if the aabb intersects the plane
        ///
        static bool aabbIntersectsPlane(const AABB &aabb, const Plane &plane);

        SSE2_CLASS_NEW_OPERATOR

    }_SSE2_ALIGN_POS;
}
}

#endif
