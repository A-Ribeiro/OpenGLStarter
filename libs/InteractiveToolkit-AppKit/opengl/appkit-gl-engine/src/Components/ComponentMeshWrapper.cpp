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
                renderWindowRegion = NULL;
            }

            ComponentMeshWrapper::~ComponentMeshWrapper()
            {
                // precisa de evento de attach to transform e detach from transform para lidar com essas situações
                if (transform.size() > 0)
                    transform[0]->OnVisited.remove(&ComponentMeshWrapper::OnTransformVisited, this);
                if (renderWindowRegion != NULL)
                    renderWindowRegion->OnAfterGraphPrecompute.remove(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
            }

            void ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty(Platform::Time *time)
            {
                renderWindowRegion->OnAfterGraphPrecompute.remove(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
                computeFinalPositions(true);
            }

            void ComponentMeshWrapper::computeFinalPositions(bool visitedFlag)
            {
                if (wrapShape == WrapShapeSphere)
                {
                    MathCore::mat4f &m = transform[0]->getMatrix(visitedFlag);

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
                    MathCore::mat4f &m = transform[0]->getMatrix(visitedFlag);

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
                    MathCore::mat4f &m = transform[0]->getMatrix(visitedFlag);

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

            void ComponentMeshWrapper::attachToTransform(Transform *t)
            {
                t->OnVisited.add(&ComponentMeshWrapper::OnTransformVisited, this);
            }

            void ComponentMeshWrapper::detachFromTransform(Transform *t)
            {
                t->OnVisited.remove(&ComponentMeshWrapper::OnTransformVisited, this);
            }

            void ComponentMeshWrapper::OnTransformVisited(Transform *t)
            {
                computeFinalPositions(true);
            }

            void ComponentMeshWrapper::setShapeSphere(const MathCore::vec3f &_sphereCenter, float _sphereRadius)
            {
                wrapShape = WrapShapeSphere;
                sphereCenter = _sphereCenter;
                sphereRadius = _sphereRadius;

                // computeFinalPositions(false);
                // AppKit::GLEngine::Engine::Instance()->app->OnAfterGraphPrecompute.remove(this, &ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty);
                renderWindowRegion = transform[0]->renderWindowRegion;
                renderWindowRegion->OnAfterGraphPrecompute.add(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
            }

            void ComponentMeshWrapper::setShapeAABB(const CollisionCore::AABB<MathCore::vec3f> &_aabb)
            {
                wrapShape = WrapShapeAABB;
                aabbCenter = (_aabb.min_box + _aabb.max_box) * 0.5f;
                aabbDimension_2_or_aabbDimension = (_aabb.max_box - _aabb.min_box) * 0.5f;

                // computeFinalPositions(false);

                // AppKit::GLEngine::Engine::Instance()->app->OnAfterGraphPrecompute.remove(this, &ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty);
                renderWindowRegion = transform[0]->renderWindowRegion;
                renderWindowRegion->OnAfterGraphPrecompute.add(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
            }

            void ComponentMeshWrapper::setShapeOBB(const CollisionCore::AABB<MathCore::vec3f> &_aabb)
            {

                wrapShape = WrapShapeOBB;
                aabbCenter = (_aabb.min_box + _aabb.max_box) * 0.5f;
                aabbDimension_2_or_aabbDimension = (_aabb.max_box - _aabb.min_box); // * 0.5f;

                // computeFinalPositions(false);

                // AppKit::GLEngine::Engine::Instance()->app->OnAfterGraphPrecompute.remove(this, &ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty);
                renderWindowRegion = transform[0]->renderWindowRegion;
                renderWindowRegion->OnAfterGraphPrecompute.add(&ComponentMeshWrapper::OnAfterGraphComputeFinalPositionsDirty, this);
            }

            void ComponentMeshWrapper::updateMeshSphere()
            {

                CollisionCore::AABB<MathCore::vec3f> _aabb;

                int count = 0;

                for (int i = 0; i < transform[0]->getComponentCount(); i++)
                {
                    Component *component = transform[0]->getComponentAt(i);
                    if (component->compareType(ComponentMesh::Type))
                    {
                        ComponentMesh *mesh = (ComponentMesh *)component;
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

                    for (int i = 0; i < transform[0]->getComponentCount(); i++)
                    {
                        Component *component = transform[0]->getComponentAt(i);
                        if (component->compareType(ComponentMesh::Type))
                        {
                            ComponentMesh *mesh = (ComponentMesh *)component;
                            for (size_t j = 0; j < mesh->pos.size(); j++)
                            {
                                float sqr_dst = MathCore::OP<MathCore::vec3f>::sqrDistance(center, mesh->pos[j]);
                                if (sqr_dst > radius)
                                    radius = sqr_dst;
                            }
                        }
                    }

                    radius = sqrt(radius);

                    setShapeAABB(_aabb);
                    setShapeSphere(center, radius);
                }
            }

            void ComponentMeshWrapper::updateMeshAABB()
            {

                CollisionCore::AABB<MathCore::vec3f> _aabb;

                int count = 0;

                for (int i = 0; i < transform[0]->getComponentCount(); i++)
                {
                    Component *component = transform[0]->getComponentAt(i);
                    if (component->compareType(ComponentMesh::Type))
                    {
                        ComponentMesh *mesh = (ComponentMesh *)component;
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
                CollisionCore::AABB<MathCore::vec3f> _aabb;

                int count = 0;

                for (int i = 0; i < transform[0]->getComponentCount(); i++)
                {
                    Component *component = transform[0]->getComponentAt(i);
                    if (component->compareType(ComponentMesh::Type))
                    {
                        ComponentMesh *mesh = (ComponentMesh *)component;
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

                ComponentColorLine *lines = (ComponentColorLine *)transform[0]->findComponent(ComponentColorLine::Type);
                if (lines == NULL)
                    lines = (ComponentColorLine *)transform[0]->addComponent(new ComponentColorLine());

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

        }
    }
}