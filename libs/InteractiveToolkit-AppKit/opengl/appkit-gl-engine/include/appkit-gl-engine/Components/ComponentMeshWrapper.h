#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

//#include <appkit-gl-base/opengl-wrapper.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            enum WrapShape
            {
                WrapShapeNone,
                WrapShapeSphere,
                WrapShapeAABB,
                WrapShapeOBB
            };

            class ComponentMeshWrapper : public Component
            {

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

                std::map<Component *, bool> visibleFromCameras;

                // sphere shape
                MathCore::vec3f sphereCenter;
                float sphereRadius;

                // aabb / obb
                MathCore::vec3f aabbDimension_2_or_aabbDimension;
                MathCore::vec3f aabbCenter;

                void createCollisionLines();

                void setShapeSphere(const MathCore::vec3f &sphereCenter, float sphereRadius);
                void setShapeAABB(const CollisionCore::AABB<MathCore::vec3f> &aabb);
                void setShapeOBB(const CollisionCore::AABB<MathCore::vec3f> &aabb);

                void OnAfterGraphComputeFinalPositionsDirty(Platform::Time *time);

            public:
                bool debugCollisionShapes;

                WrapShape wrapShape;

                // sphere shape
                CollisionCore::Sphere<MathCore::vec3f> sphere;

                // aabb
                CollisionCore::AABB<MathCore::vec3f> aabb;

                // oobb
                CollisionCore::OBB<MathCore::vec3f> obb;

                static const ComponentType Type;

                ComponentMeshWrapper();
                ~ComponentMeshWrapper();

                void computeFinalPositions(bool visitedFlag);

                void start();
                void attachToTransform(std::shared_ptr<Transform> t);
                void detachFromTransform(std::shared_ptr<Transform> t);

                void OnTransformVisited(std::shared_ptr<Transform> t);

                void updateMeshSphere();
                void updateMeshAABB();
                void updateMeshOBB();

                bool isOverlappingAABB(const CollisionCore::AABB<MathCore::vec3f> &aabb) const;
                bool isOverlappingOBB(const CollisionCore::OBB<MathCore::vec3f> &obb) const;
                bool isOverlappingSphere(const CollisionCore::Sphere<MathCore::vec3f> &sphere) const;
                bool isOverlappingFrustum(const CollisionCore::Frustum<MathCore::vec3f> &frustum) const;
                bool isIntersectingLineSegment(const CollisionCore::LineSegment<MathCore::vec3f> &lineSegment) const;
                bool isIntersectingTriangle(const CollisionCore::Triangle<MathCore::vec3f> &triangle) const;

                bool isOverlapping(const ComponentMeshWrapper *meshWrapper) const;
                MathCore::vec3f closestPoint(const MathCore::vec3f &p) const;
                bool raycast(const CollisionCore::Ray<MathCore::vec3f> &r, float *outTmin, MathCore::vec3f *outNormal) const;

                MathCore::vec3f getCenter();

                CollisionCore::AABB<MathCore::vec3f> getAABB();

                // always clone 
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone);
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap);

            };

        }
    }

}