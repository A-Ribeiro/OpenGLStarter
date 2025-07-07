#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>
#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
#include <appkit-gl-engine/Components/ComponentMesh.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

// #include "ComponentColorMesh.h"

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentMeshWrapper::Type = "ComponentMeshWrapper";

            ComponentMeshWrapper::ComponentMeshWrapper() : Component(ComponentMeshWrapper::Type)
            {
                wrapShape = WrapShapeNone;
                debugCollisionShapes = false;
                renderWindowRegionRef.reset();
            }

            ComponentMeshWrapper::~ComponentMeshWrapper()
            {
                // precisa de evento de attach to transform e detach from transform para lidar com essas situações
                if (getTransformCount() > 0){
                    auto transform = getTransform();
                    if (transform != nullptr) {
                        transform->OnVisited.remove(&ComponentMeshWrapper::OnTransformVisited, this);
                    }
                }
                auto renderWindowRegion = ToShared(renderWindowRegionRef);
                if (renderWindowRegion != nullptr)
                    renderWindowRegion->OnAfterGraphPrecompute.remove(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
            }

            void ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty(Platform::Time *time)
            {
                auto renderWindowRegion = ToShared(renderWindowRegionRef);
                renderWindowRegion->OnAfterGraphPrecompute.remove(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
                computeFinalPositions(true);
            }

            void ComponentMeshWrapper::computeFinalPositions(bool visitedFlag)
            {
                if (this->OnBeforeComputeFinalPositions!= nullptr)
                    this->OnBeforeComputeFinalPositions(this);

                if (wrapShape == WrapShapeSphere)
                {
                    auto transform = getTransform();

                    MathCore::mat4f &m = transform->getMatrix(visitedFlag);

                    /*
                    MathCore::vec3f center = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(sphereCenter));

                    MathCore::vec3f right = absv(MathCore::CVT<MathCore::vec4f>::toVec3(m[0]));
                    MathCore::vec3f top = absv(MathCore::CVT<MathCore::vec4f>::toVec3(m[1]));
                    MathCore::vec3f depth = absv(MathCore::CVT<MathCore::vec4f>::toVec3(m[2]));

                    MathCore::vec3f max = maximum(maximum(right, top), depth);

                    MathCore::vec3f scale = MathCore::vec3f(
                        length(right),
                        length(top),
                        length(depth)
                    );

                    float scale_max = (maximum(max) + minimum(max)) * 0.5f;// maximum(scale);


                    float radius = 0.0f;
                    for (int i = 0; i < transform[0]->getComponentCount(); i++) {
                        Component* component = transform[0]->getComponentAt(i);
                        if (component->compareType(ComponentMesh::Type)) {
                            ComponentMesh* mesh = (ComponentMesh*)component;
                            for (size_t j = 0; j < mesh->pos.size(); j++) {
                                float sqr_dst = sqrDistance(center,
                                    MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(mesh->pos[j]))
                                );
                                if (sqr_dst > radius)
                                    radius = sqr_dst;
                            }
                        }
                    }
                    radius = sqrt(radius);


                    sphere = collision::Sphere(
                        center,
                        scale_max * sphereRadius
                    );
                    */

                    /*
                    MathCore::vec3f scale = transform[0]->getScale(visitedFlag);
                    float _box_size = maximum(scale);
                    sphere = collision::Sphere(
                        MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(sphereCenter)),
                        sphereRadius * _box_size
                    );
                    */

                    MathCore::vec3f center = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(aabbCenter));

                    MathCore::vec3f right = MathCore::CVT<MathCore::vec4f>::toVec3(m[0]) * aabbDimension_2_or_aabbDimension.x;
                    MathCore::vec3f top = MathCore::CVT<MathCore::vec4f>::toVec3(m[1]) * aabbDimension_2_or_aabbDimension.y;
                    MathCore::vec3f depth = MathCore::CVT<MathCore::vec4f>::toVec3(m[2]) * aabbDimension_2_or_aabbDimension.z;

                    MathCore::vec3f vertices = right + top + depth; // 111

                    sphere = CollisionCore::Sphere<MathCore::vec3f>(
                        center,          // MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(sphereCenter)),
                        MathCore::OP<MathCore::vec3f>::length(vertices) // sphereRadius * maximum(scale)
                    );
                }
                else if (wrapShape == WrapShapeAABB)
                {
                    auto transform = getTransform();

                    MathCore::mat4f &m = transform->getMatrix(visitedFlag);

                    MathCore::vec3f center = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(aabbCenter));

                    MathCore::vec3f right = MathCore::CVT<MathCore::vec4f>::toVec3(m[0]) * aabbDimension_2_or_aabbDimension.x;
                    MathCore::vec3f top = MathCore::CVT<MathCore::vec4f>::toVec3(m[1]) * aabbDimension_2_or_aabbDimension.y;
                    MathCore::vec3f depth = MathCore::CVT<MathCore::vec4f>::toVec3(m[2]) * aabbDimension_2_or_aabbDimension.z;

                    MathCore::vec3f vertices[8] = {
                        center - right - top - depth, // 000
                        center - right - top + depth, // 001
                        center - right + top - depth, // 010
                        center - right + top + depth, // 011
                        center + right - top - depth, // 100
                        center + right - top + depth, // 101
                        center + right + top - depth, // 110
                        center + right + top + depth  // 111
                    };

                    aabb = CollisionCore::AABB<MathCore::vec3f>(vertices[0], vertices[1]);
                    for (int i = 2; i < 8; i++)
                    {
                        aabb.min_box = MathCore::OP<MathCore::vec3f>::minimum(aabb.min_box, vertices[i]);
                        aabb.max_box = MathCore::OP<MathCore::vec3f>::maximum(aabb.max_box, vertices[i]);
                    }

                    /*
                    MathCore::vec3f transformed_vertex = MathCore::CVT<MathCore::vec4f>::toVec3( m * aabbVertex[0] );
                    aabb = CollisionCore::AABB<MathCore::vec3f>(transformed_vertex,transformed_vertex);
                    for(int i=1;i<8;i++){
                        transformed_vertex = MathCore::CVT<MathCore::vec4f>::toVec3( m * aabbVertex[i] );
                        aabb.min_box = minimum(aabb.min_box, transformed_vertex);
                        aabb.max_box = maximum(aabb.max_box, transformed_vertex);
                    }
                    */
                }
                else if (wrapShape == WrapShapeOBB)
                {
                    auto transform = getTransform();

                    MathCore::mat4f &m = transform->getMatrix(visitedFlag);

                    MathCore::vec3f center = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(aabbCenter));

                    MathCore::vec3f dimension = MathCore::vec3f(
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[0])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[1])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[2])));

                    MathCore::quatf rotation = MathCore::GEN<MathCore::quatf>::fromMat4(m);

                    dimension *= aabbDimension_2_or_aabbDimension; // * 2.0f;

                    obb.setOBB(center, dimension, rotation);
                }
                if (debugCollisionShapes)
                    createCollisionLines();
            }

            void ComponentMeshWrapper::start()
            {
                // computeFinalPositions(false);
            }

            void ComponentMeshWrapper::attachToTransform(std::shared_ptr<Transform> t)
            {
                t->OnVisited.add(&ComponentMeshWrapper::OnTransformVisited, this);
            }

            void ComponentMeshWrapper::detachFromTransform(std::shared_ptr<Transform> t)
            {
                t->OnVisited.remove(&ComponentMeshWrapper::OnTransformVisited, this);

                auto renderWindowRegion = ToShared(renderWindowRegionRef);
                if (renderWindowRegion != nullptr)
                    renderWindowRegion->OnAfterGraphPrecompute.remove(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
                renderWindowRegionRef.reset();

            }

            void ComponentMeshWrapper::OnTransformVisited(std::shared_ptr<Transform> t)
            {
                computeFinalPositions(true);
            }

            void ComponentMeshWrapper::makeDirtyToComputeFinalPositions(bool only_remove) {
                auto transform = getTransform();

                renderWindowRegionRef = transform->renderWindowRegion;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                renderWindowRegion->OnAfterGraphPrecompute.remove(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
                if (only_remove)
                    return;
                renderWindowRegion->OnAfterGraphPrecompute.add(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
            }

            void ComponentMeshWrapper::setShapeSphere(const MathCore::vec3f &_sphereCenter, float _sphereRadius, float ignore_after_graph_precompute_call)
            {
                wrapShape = WrapShapeSphere;
                sphereCenter = _sphereCenter;
                sphereRadius = _sphereRadius;

                if (!ignore_after_graph_precompute_call)
                    makeDirtyToComputeFinalPositions(false);
            }

            void ComponentMeshWrapper::setShapeAABB(const CollisionCore::AABB<MathCore::vec3f> &_aabb, float ignore_after_graph_precompute_call)
            {
                wrapShape = WrapShapeAABB;
                aabbCenter = (_aabb.min_box + _aabb.max_box) * 0.5f;
                aabbDimension_2_or_aabbDimension = (_aabb.max_box - _aabb.min_box) * 0.5f;

                if (!ignore_after_graph_precompute_call)
                    makeDirtyToComputeFinalPositions(false);
            }

            void ComponentMeshWrapper::setShapeOBB(const CollisionCore::AABB<MathCore::vec3f> &_aabb, float ignore_after_graph_precompute_call)
            {
                wrapShape = WrapShapeOBB;
                aabbCenter = (_aabb.min_box + _aabb.max_box) * 0.5f;
                aabbDimension_2_or_aabbDimension = (_aabb.max_box - _aabb.min_box); // * 0.5f;

                if (!ignore_after_graph_precompute_call)
                    makeDirtyToComputeFinalPositions(false);
            }

            void ComponentMeshWrapper::clearShape()
            {
                wrapShape = WrapShapeNone;
                makeDirtyToComputeFinalPositions(true);
            }

            void ComponentMeshWrapper::updateMeshSphere()
            {
                auto transform = getTransform();

                CollisionCore::AABB<MathCore::vec3f> _aabb;

                int count = 0;

                for (int i = 0; i < transform->getComponentCount(); i++)
                {
                    auto component = transform->getComponentAt(i);
                    if (component->compareType(ComponentMesh::Type))
                    {
                        auto mesh = std::dynamic_pointer_cast<ComponentMesh>(component);
                        for (size_t j = 0; j < mesh->pos.size(); j++)
                        {
                            if (count == 0)
                            {
                                _aabb.min_box = _aabb.max_box = mesh->pos[j];
                            }
                            else
                            {
                                _aabb.min_box = MathCore::OP<MathCore::vec3f>::minimum(_aabb.min_box, mesh->pos[j]);
                                _aabb.max_box = MathCore::OP<MathCore::vec3f>::maximum(_aabb.max_box, mesh->pos[j]);
                            }
                            count++;
                        }
                    }
                }

                if (count > 0)
                {
                    MathCore::vec3f center = (_aabb.min_box + _aabb.max_box) * 0.5f;
                    float radius = 0.0f;

                    for (int i = 0; i < transform->getComponentCount(); i++)
                    {
                        auto component = transform->getComponentAt(i);
                        if (component->compareType(ComponentMesh::Type))
                        {
                            auto mesh = std::dynamic_pointer_cast<ComponentMesh>(component);
                            for (size_t j = 0; j < mesh->pos.size(); j++)
                            {
                                float sqr_dst = MathCore::OP<MathCore::vec3f>::sqrDistance(center, mesh->pos[j]);
                                if (sqr_dst > radius)
                                    radius = sqr_dst;
                            }
                        }
                    }

                    radius = sqrt(radius);

                    // setShapeAABB(_aabb);
                    setShapeSphere(center, radius);
                }
            }

            void ComponentMeshWrapper::updateMeshAABB()
            {
                auto transform = getTransform();

                CollisionCore::AABB<MathCore::vec3f> _aabb;

                int count = 0;

                for (int i = 0; i < transform->getComponentCount(); i++)
                {
                    auto component = transform->getComponentAt(i);
                    if (component->compareType(ComponentMesh::Type))
                    {
                        auto mesh = std::dynamic_pointer_cast<ComponentMesh>(component);
                        for (size_t j = 0; j < mesh->pos.size(); j++)
                        {
                            if (count == 0)
                            {
                                _aabb.min_box = _aabb.max_box = mesh->pos[j];
                            }
                            else
                            {
                                _aabb.min_box = MathCore::OP<MathCore::vec3f>::minimum(_aabb.min_box, mesh->pos[j]);
                                _aabb.max_box = MathCore::OP<MathCore::vec3f>::maximum(_aabb.max_box, mesh->pos[j]);
                            }
                            count++;
                        }
                    }
                }

                if (count > 0)
                {
                    setShapeAABB(_aabb);
                }
            }

            void ComponentMeshWrapper::updateMeshOBB()
            {
                auto transform = getTransform();

                CollisionCore::AABB<MathCore::vec3f> _aabb;

                int count = 0;

                for (int i = 0; i < transform->getComponentCount(); i++)
                {
                    auto component = transform->getComponentAt(i);
                    if (component->compareType(ComponentMesh::Type))
                    {
                        auto mesh = std::dynamic_pointer_cast<ComponentMesh>(component);
                        for (size_t j = 0; j < mesh->pos.size(); j++)
                        {
                            if (count == 0)
                            {
                                _aabb.min_box = _aabb.max_box = mesh->pos[j];
                            }
                            else
                            {
                                _aabb.min_box = MathCore::OP<MathCore::vec3f>::minimum(_aabb.min_box, mesh->pos[j]);
                                _aabb.max_box = MathCore::OP<MathCore::vec3f>::maximum(_aabb.max_box, mesh->pos[j]);
                            }
                            count++;
                        }
                    }
                }

                if (count > 0)
                {
                    setShapeOBB(_aabb);
                }
            }

            bool ComponentMeshWrapper::isOverlappingAABB(const CollisionCore::AABB<MathCore::vec3f> &_aabb) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                {
                    MathCore::vec3f penetration;
                    return CollisionCore::AABB<MathCore::vec3f>::sphereOverlapsAABB(sphere, _aabb, &penetration);
                }
                case WrapShapeAABB:
                    return CollisionCore::AABB<MathCore::vec3f>::aabbOverlapsAABB(aabb, _aabb);
                case WrapShapeOBB:
                    return CollisionCore::AABB<MathCore::vec3f>::obbOverlapsAABB(obb, _aabb);
                default:
                    break;
                }
                return false;
            }
            bool ComponentMeshWrapper::isOverlappingOBB(const CollisionCore::OBB<MathCore::vec3f> &_obb) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                {
                    MathCore::vec3f penetration;
                    return CollisionCore::OBB<MathCore::vec3f>::sphereOverlapsOBB(sphere, _obb, &penetration);
                }
                case WrapShapeAABB:
                    return CollisionCore::OBB<MathCore::vec3f>::aabbOverlapsOBB(aabb, _obb);
                case WrapShapeOBB:
                    return CollisionCore::OBB<MathCore::vec3f>::obbOverlapsOBB(obb, _obb);
                default:
                    break;
                }
                return false;
            }
            bool ComponentMeshWrapper::isOverlappingSphere(const CollisionCore::Sphere<MathCore::vec3f> &_sphere) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                    return CollisionCore::Sphere<MathCore::vec3f>::sphereOverlapsSphere(sphere, _sphere);
                case WrapShapeAABB:
                {
                    MathCore::vec3f penetration;
                    return CollisionCore::Sphere<MathCore::vec3f>::aabbOverlapsSphere(aabb, _sphere, &penetration);
                }
                case WrapShapeOBB:
                {
                    MathCore::vec3f penetration;
                    return CollisionCore::Sphere<MathCore::vec3f>::obbOverlapsSphere(obb, _sphere, &penetration);
                }
                default:
                    break;
                }
                return false;
            }
            bool ComponentMeshWrapper::isOverlappingFrustum(const CollisionCore::Frustum<MathCore::vec3f> &_frustum) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                    return CollisionCore::Frustum<MathCore::vec3f>::sphereOverlapsFrustum(sphere, _frustum);
                case WrapShapeAABB:
                    return CollisionCore::Frustum<MathCore::vec3f>::aabbOverlapsFrustum(aabb, _frustum);
                case WrapShapeOBB:
                    return CollisionCore::Frustum<MathCore::vec3f>::obbOverlapsFrustum(obb, _frustum);
                default:
                    break;
                }
                return false;
            }
            bool ComponentMeshWrapper::isIntersectingLineSegment(const CollisionCore::LineSegment<MathCore::vec3f> &_lineSegment) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                    return CollisionCore::LineSegment<MathCore::vec3f>::sphereIntersectsSegment(sphere, _lineSegment);
                case WrapShapeAABB:
                    return CollisionCore::LineSegment<MathCore::vec3f>::aabbIntersectsSegment(aabb, _lineSegment);
                case WrapShapeOBB:
                    return CollisionCore::LineSegment<MathCore::vec3f>::obbIntersectsSegment(obb, _lineSegment);
                default:
                    break;
                }
                return false;
            }
            bool ComponentMeshWrapper::isIntersectingTriangle(const CollisionCore::Triangle<MathCore::vec3f> &_triangle) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                {
                    MathCore::vec3f penetration;
                    return CollisionCore::Triangle<MathCore::vec3f>::sphereIntersectsTriangle(sphere, _triangle, &penetration);
                }
                case WrapShapeAABB:
                    return CollisionCore::Triangle<MathCore::vec3f>::aabbIntersectsTriangle(aabb, _triangle);
                case WrapShapeOBB:
                    return CollisionCore::Triangle<MathCore::vec3f>::obbIntersectsTriangle(obb, _triangle);
                default:
                    break;
                }
                return false;
            }

            bool ComponentMeshWrapper::isOverlapping(const ComponentMeshWrapper *meshWrapper) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                    return meshWrapper->isOverlappingSphere(sphere);
                case WrapShapeAABB:
                    return meshWrapper->isOverlappingAABB(aabb);
                case WrapShapeOBB:
                    return meshWrapper->isOverlappingOBB(obb);
                default:
                    break;
                }
                return false;
            }

            MathCore::vec3f ComponentMeshWrapper::closestPoint(const MathCore::vec3f &p) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                    return CollisionCore::Sphere<MathCore::vec3f>::closestPointToSphere(p, sphere);
                case WrapShapeAABB:
                    return CollisionCore::AABB<MathCore::vec3f>::closestPointToAABB(p, aabb);
                case WrapShapeOBB:
                    return CollisionCore::OBB<MathCore::vec3f>::closestPointToOBB(p, obb);
                default:
                    break;
                }
                return MathCore::vec3f();
            }

            bool ComponentMeshWrapper::raycast(const CollisionCore::Ray<MathCore::vec3f> &r, float *outTmin, MathCore::vec3f *outNormal) const
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                    return CollisionCore::Sphere<MathCore::vec3f>::raycastSphere(r, sphere, outTmin, outNormal);
                case WrapShapeAABB:
                    return CollisionCore::AABB<MathCore::vec3f>::raycastAABB(r, aabb, outTmin, outNormal);
                case WrapShapeOBB:
                    return CollisionCore::OBB<MathCore::vec3f>::raycastOBB(r, obb, outTmin, outNormal);
                default:
                    break;
                }
                return false;
            }

            MathCore::vec3f ComponentMeshWrapper::getCenter()
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                    return sphere.center;
                case WrapShapeAABB:
                    return (aabb.min_box + aabb.max_box) * 0.5f;
                case WrapShapeOBB:
                    return obb.center;
                default:
                    break;
                }
                return MathCore::vec3f();
            }

            CollisionCore::AABB<MathCore::vec3f> ComponentMeshWrapper::getAABB()
            {
                switch (wrapShape)
                {
                case WrapShapeSphere:
                    return CollisionCore::AABB<MathCore::vec3f>::fromSphere(sphere);
                case WrapShapeAABB:
                    return aabb;
                case WrapShapeOBB:
                    return CollisionCore::AABB<MathCore::vec3f>::fromOBB(obb);
                default:
                    break;
                }
                return CollisionCore::AABB<MathCore::vec3f>();
            }

            void ComponentMeshWrapper::createCollisionLines()
            {
                auto transform = getTransform();

                auto lines = transform->findComponent<ComponentColorLine>();
                if (lines == nullptr)
                    lines = transform->addNewComponent<ComponentColorLine>();

                lines->vertices.clear();

                // global add vertices
                if (wrapShape == WrapShapeSphere)
                {

                    std::vector<MathCore::vec3f> vertices;
                    std::vector<uint32_t> indices;
                    setSphere(sphere.radius, 16, 16, &indices, &vertices);

                    for (size_t i = 0; i < indices.size(); i += 3)
                    {
                        lines->vertices.push_back(vertices[indices[i]] + sphere.center);
                        lines->vertices.push_back(vertices[indices[i + 1]] + sphere.center);
                    }
                }
                else if (wrapShape == WrapShapeAABB)
                {
                    MathCore::vec3f array[8] = {
                        MathCore::vec3f(aabb.min_box.x, aabb.min_box.y, aabb.min_box.z), // 000 0
                        MathCore::vec3f(aabb.min_box.x, aabb.min_box.y, aabb.max_box.z), // 001 1
                        MathCore::vec3f(aabb.min_box.x, aabb.max_box.y, aabb.min_box.z), // 010 2
                        MathCore::vec3f(aabb.min_box.x, aabb.max_box.y, aabb.max_box.z), // 011 3
                        MathCore::vec3f(aabb.max_box.x, aabb.min_box.y, aabb.min_box.z), // 100 4
                        MathCore::vec3f(aabb.max_box.x, aabb.min_box.y, aabb.max_box.z), // 101 5
                        MathCore::vec3f(aabb.max_box.x, aabb.max_box.y, aabb.min_box.z), // 110 6
                        MathCore::vec3f(aabb.max_box.x, aabb.max_box.y, aabb.max_box.z)  // 111 7
                    };

                    lines->vertices.push_back(array[0]);
                    lines->vertices.push_back(array[4]);

                    lines->vertices.push_back(array[0 + 1]);
                    lines->vertices.push_back(array[4 + 1]);

                    lines->vertices.push_back(array[0 + 2]);
                    lines->vertices.push_back(array[4 + 2]);

                    lines->vertices.push_back(array[0 + 3]);
                    lines->vertices.push_back(array[4 + 3]);

                    lines->vertices.push_back(array[0]);
                    lines->vertices.push_back(array[1]);

                    lines->vertices.push_back(array[1]);
                    lines->vertices.push_back(array[3]);

                    lines->vertices.push_back(array[3]);
                    lines->vertices.push_back(array[2]);

                    lines->vertices.push_back(array[2]);
                    lines->vertices.push_back(array[0]);

                    lines->vertices.push_back(array[4 + 0]);
                    lines->vertices.push_back(array[4 + 1]);

                    lines->vertices.push_back(array[4 + 1]);
                    lines->vertices.push_back(array[4 + 3]);

                    lines->vertices.push_back(array[4 + 3]);
                    lines->vertices.push_back(array[4 + 2]);

                    lines->vertices.push_back(array[4 + 2]);
                    lines->vertices.push_back(array[4 + 0]);
                }
                else if (wrapShape == WrapShapeOBB)
                {
                    MathCore::vec3f *array = obb.box_vertices;

                    lines->vertices.push_back(array[0]);
                    lines->vertices.push_back(array[4]);

                    lines->vertices.push_back(array[0 + 1]);
                    lines->vertices.push_back(array[4 + 1]);

                    lines->vertices.push_back(array[0 + 2]);
                    lines->vertices.push_back(array[4 + 2]);

                    lines->vertices.push_back(array[0 + 3]);
                    lines->vertices.push_back(array[4 + 3]);

                    lines->vertices.push_back(array[0]);
                    lines->vertices.push_back(array[1]);

                    lines->vertices.push_back(array[1]);
                    lines->vertices.push_back(array[3]);

                    lines->vertices.push_back(array[3]);
                    lines->vertices.push_back(array[2]);

                    lines->vertices.push_back(array[2]);
                    lines->vertices.push_back(array[0]);

                    lines->vertices.push_back(array[4 + 0]);
                    lines->vertices.push_back(array[4 + 1]);

                    lines->vertices.push_back(array[4 + 1]);
                    lines->vertices.push_back(array[4 + 3]);

                    lines->vertices.push_back(array[4 + 3]);
                    lines->vertices.push_back(array[4 + 2]);

                    lines->vertices.push_back(array[4 + 2]);
                    lines->vertices.push_back(array[4 + 0]);
                }
                else
                {
                    printf("[ComponentMeshWrapper] trying to draw empty mesh wrapper...\n");
                }

                /*
                MathCore::mat4f & m = transform[0]->worldToLocalMatrix();
                for (int i = 0; i < lines->vertices.size(); i++) {
                    lines->vertices[i] = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(lines->vertices[i]));
                }
                */

                lines->syncVBODynamic();
            }


            std::shared_ptr<Component> ComponentMeshWrapper::duplicate_ref_or_clone(bool force_clone){
                auto result = Component::CreateShared<ComponentMeshWrapper>();

                result->debugCollisionShapes = this->debugCollisionShapes;

                result->wrapShape = this->wrapShape;
                result->sphere = this->sphere;
                result->aabb = this->aabb;
                result->obb = this->obb;

                // sphere shape
                result->sphereCenter = this->sphereCenter;
                result->sphereRadius = this->sphereRadius;

                // aabb / obb
                result->aabbDimension_2_or_aabbDimension = this->aabbDimension_2_or_aabbDimension;
                result->aabbCenter = this->aabbCenter;

                return result;
            }
            void ComponentMeshWrapper::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap){

            }

            void ComponentMeshWrapper::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentMeshWrapper::Type);
                writer.String("id");
                writer.Uint64((intptr_t)self().get());


                writer.String("debugCollisionShapes");
                writer.Bool(debugCollisionShapes);

                if (wrapShape == WrapShape::WrapShapeSphere) {
                    writer.String("wrapShape");
                    writer.String("sphere");

                    writer.String("radius");
                    writer.Double( MathCore::CVT<float>::toDouble(sphere.radius) );

                    writer.String("center");
                    SerializerUtil::write(writer, sphere.center);

                } else if (wrapShape == WrapShape::WrapShapeAABB) {
                    writer.String("wrapShape");
                    writer.String("aabb");

                    writer.String("min");
                    SerializerUtil::write(writer, aabb.min_box);

                    writer.String("max");
                    SerializerUtil::write(writer, aabb.max_box);


                } else if (wrapShape == WrapShape::WrapShapeOBB) {
                    writer.String("wrapShape");
                    writer.String("obb");

                    writer.String("center");
                    SerializerUtil::write(writer, obb.center);

                    writer.String("dimension");
                    SerializerUtil::write(writer, obb.dimension_2 * 2.0f);

                    writer.String("orientation");
                    SerializerUtil::write(writer, obb.orientation);
                }

                writer.EndObject();
                
            }
            void ComponentMeshWrapper::Deserialize(rapidjson::Value &_value,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                  ResourceSet &resourceSet)
            {
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), ComponentMeshWrapper::Type) == 0)
                    return;
                
                if (_value.HasMember("debugCollisionShapes") && _value["debugCollisionShapes"].IsBool())
                    debugCollisionShapes = _value["debugCollisionShapes"].GetBool();

                if (!_value.HasMember("wrapShape") || !_value["wrapShape"].IsString())
                    return;

                auto _wrapShape = _value["wrapShape"].GetString();
                if (strcmp(_wrapShape, "sphere") == 0){

                    wrapShape = WrapShape::WrapShapeSphere;

                    if (_value.HasMember("radius") && _value["radius"].IsDouble())
                        sphere.radius = MathCore::CVT<double>::toFloat(_value["radius"].GetDouble());

                    sphere.center = SerializerUtil::read<MathCore::vec3f>(_value["center"]);

                } else if (strcmp(_wrapShape, "aabb") == 0){

                    wrapShape = WrapShape::WrapShapeAABB;

                    aabb.min_box = SerializerUtil::read<MathCore::vec3f>(_value["min"]);
                    aabb.max_box = SerializerUtil::read<MathCore::vec3f>(_value["max"]);

                } else if (strcmp(_wrapShape, "obb") == 0){

                    wrapShape = WrapShape::WrapShapeOBB;

                    auto _center = SerializerUtil::read<MathCore::vec3f>(_value["center"]);
                    auto _dimension = SerializerUtil::read<MathCore::vec3f>(_value["dimension"]);
                    auto _orientation = SerializerUtil::read<MathCore::quatf>(_value["orientation"]);

                    obb.setOBB(_center, _dimension, _orientation);

                }


            }

        }
    }
}