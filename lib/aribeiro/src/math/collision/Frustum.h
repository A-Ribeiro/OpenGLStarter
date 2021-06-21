#ifndef Frustum__H
#define Frustum__H

#include <aribeiro/common.h>

//I need a complete definition to instantiate a vec3 as class field
#include <aribeiro/mat4.h>
#include <aribeiro/vec3.h>
#include <aribeiro/Plane.h>
#include <aribeiro/AABB.h>

namespace aRibeiro {
namespace collision {
    class Sphere;
    //class AABB;


    enum FrustumVertexEnum {
        FrustumVertex_Near_Right_Top = 0,
        FrustumVertex_Near_Right_Bottom = 1,
        FrustumVertex_Near_Left_Bottom = 2,
        FrustumVertex_Near_Left_Top = 3,

        FrustumVertex_Far_Right_Top = 4,
        FrustumVertex_Far_Right_Bottom = 5,
        FrustumVertex_Far_Left_Bottom = 6,
        FrustumVertex_Far_Left_Top = 7
    };

/// \brief Camera frustum representation
///
/// It can be constructed from the camera projection and modelview matrix.
///
/// Could be used to implement the frustum culling of other shapes (point, sphere, aabb)
///
/// \author Alessandro Ribeiro
///
    class _SSE2_ALIGN_PRE Frustum {
        void computePlanes(const mat4& clipMatrix, bool depth_zero_one);

        float minProjections[6];
        float maxProjections[6];

    public:

        Plane rightPlane, leftPlane, bottomPlane, topPlane, nearPlane, farPlane;
        vec3 vertices[8];
        AABB aabb;

        /// \brief Access one of the 6 planes that compose this frustum
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
        /// Plane right = frustum[0];
        /// Plane left = frustum[1];
        /// Plane bottom = frustum[2];
        /// Plane top = frustum[3];
        /// Plane near = frustum[4];
        /// Plane far = frustum[5];
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Plane& operator[](int idx);

        /// \brief Access one of the 6 planes that compose this frustum
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// void process_frustum( const Frustum &frustum ) {
        ///     Plane right = frustum[0];
        ///     Plane left = frustum[1];
        ///     Plane bottom = frustum[2];
        ///     Plane top = frustum[3];
        ///     Plane near = frustum[4];
        ///     Plane far = frustum[5];
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        const Plane& operator[](int idx)const;

        /// \brief Construct a frustum from the camera projection matrix
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// mat4 camera_projection;
        ///
        /// Frustum frustum = Frustum( camera_projection );
        ///
        /// Frustum frustum_a = camera_projection;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Frustum(const mat4& projection, bool depth_zero_one = false);

        /// \brief Construct a frustum from the camera projection matrix and the transformation_matrix
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// mat4 camera_projection;
        /// mat4 camera_inverse_transformation;
        ///
        /// Frustum frustum = Frustum( camera_projection, camera_inverse_transformation );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Frustum(const mat4& projection, const mat4& camera, bool depth_zero_one = false);

        /// \brief Test if a point is inside the frustum
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Frustum frustum;
        /// vec3 point;
        ///
        /// if ( Frustum::pointInsideFrustum( point, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The 3D point
        /// \param f The frustum
        /// \return true, if the point is inside the frustum
        ///
        static bool pointInsideFrustum(const vec3 &p,const Frustum &f);

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
        /// if ( Frustum::sphereOverlapsFrustum( sphere_center, sphere_radius, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param f The frustum
        /// \return true, if the sphere overlaps the frustum
        ///
        static bool sphereOverlapsFrustum(const vec3 &center, const float &radius, const Frustum &f);

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
        /// if ( Frustum::sphereOverlapsFrustum( sphere, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param s The sphere
        /// \param f The frustum
        /// \return true, if the sphere overlaps the frustum
        ///
        static bool sphereOverlapsFrustum(const Sphere &s, const Frustum &f);

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
        /// if ( Frustum::aabbOverlapsFrustum( aabb, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param f The frustum
        /// \return true, if the aabb overlaps the frustum
        ///
        static bool aabbOverlapsFrustum(const AABB &aabb, const Frustum &f);

        SSE2_CLASS_NEW_OPERATOR

    } _SSE2_ALIGN_POS;
}
}


#endif
