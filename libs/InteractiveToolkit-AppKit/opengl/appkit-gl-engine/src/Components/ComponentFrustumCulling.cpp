#include <appkit-gl-engine/Components/ComponentFrustumCulling.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>
#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

// #include "ComponentColorMesh.h"

// using namespace aRibeiro;

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentFrustumCulling::Type = "ComponentFrustumCulling";

            ComponentFrustumCulling::ComponentFrustumCulling() : Component(ComponentFrustumCulling::Type)
            {
                cullingShape = CullingShapeNone;
                debugCollisionShapes = false;
            }

            ComponentFrustumCulling::~ComponentFrustumCulling()
            {
                // precisa de evento de attach to transform e detach from transform para lidar com essas situações
                if (transform.size() > 0)
                    transform[0]->OnVisited.remove(&ComponentFrustumCulling::OnTransformVisited, this);
            }

            void ComponentFrustumCulling::computeFinalPositions(bool visitedFlag)
            {
                if (cullingShape == CullingShapeSphere)
                {
                    MathCore::mat4f &m = transform[0]->getMatrix(visitedFlag);
                    MathCore::vec3f scale = transform[0]->getScale(visitedFlag);

                    sphere = CollisionCore::Sphere<MathCore::vec3f> (
                        MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(sphereCenter)),
                        sphereRadius * MathCore::OP<MathCore::vec3f>::maximum(scale));
                }
                else if (cullingShape == CullingShapeAABB)
                {
                    MathCore::mat4f &m = transform[0]->getMatrix(visitedFlag);

                    MathCore::vec3f center = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(aabbCenter));

                    MathCore::vec3f right = MathCore::CVT<MathCore::vec4f>::toVec3(m[0]) * aabbDimension_2.x;
                    MathCore::vec3f top = MathCore::CVT<MathCore::vec4f>::toVec3(m[1]) * aabbDimension_2.y;
                    MathCore::vec3f depth = MathCore::CVT<MathCore::vec4f>::toVec3(m[2]) * aabbDimension_2.z;

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
                if (debugCollisionShapes)
                    createCollisionLines();
            }

            void ComponentFrustumCulling::start()
            {
                computeFinalPositions(false);
            }

            void ComponentFrustumCulling::attachToTransform(Transform *t)
            {
                t->OnVisited.add(&ComponentFrustumCulling::OnTransformVisited, this);
            }

            void ComponentFrustumCulling::detachFromTransform(Transform *t)
            {
                t->OnVisited.remove(&ComponentFrustumCulling::OnTransformVisited, this);
            }

            void ComponentFrustumCulling::OnTransformVisited(Transform *t)
            {
                computeFinalPositions(true);
            }

            ComponentFrustumCulling *ComponentFrustumCulling::createShapeSphere(const MathCore::vec3f &sphereCenter, float sphereRadius)
            {
                ComponentFrustumCulling *result = new ComponentFrustumCulling();

                result->cullingShape = CullingShapeSphere;
                result->sphereCenter = sphereCenter;
                result->sphereRadius = sphereRadius;

                return result;
            }

            ComponentFrustumCulling *ComponentFrustumCulling::createShapeAABB(const CollisionCore::AABB<MathCore::vec3f> &aabb)
            {
                ComponentFrustumCulling *result = new ComponentFrustumCulling();

                result->cullingShape = CullingShapeAABB;
                result->aabbCenter = (aabb.min_box + aabb.max_box) * 0.5f;
                result->aabbDimension_2 = (aabb.max_box - aabb.min_box) * 0.5f;

                /*
                result->aabbVertex[0] = vec4(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z,1.0f);// 000
                result->aabbVertex[1] = vec4(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z,1.0f);// 001
                result->aabbVertex[2] = vec4(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z,1.0f);// 010
                result->aabbVertex[3] = vec4(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z,1.0f);// 011
                result->aabbVertex[4] = vec4(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z,1.0f);// 100
                result->aabbVertex[5] = vec4(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z,1.0f);// 101
                result->aabbVertex[6] = vec4(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z,1.0f);// 110
                result->aabbVertex[7] = vec4(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z,1.0f);// 111
                */

                return result;
            }

            void ComponentFrustumCulling::setVisibilityFromCamera(Component *cameraPerspective, bool visibility)
            {
                std::map<Component *, bool>::iterator it;
                it = visibleFromCameras.find(cameraPerspective);

                if (visibility)
                {
                    if (it == visibleFromCameras.end())
                    {
                        visibleFromCameras[cameraPerspective] = true;
                        OnBecomeVisible(this, cameraPerspective);
                    }
                }
                else
                {
                    if (it != visibleFromCameras.end())
                    {
                        visibleFromCameras.erase(it);
                        OnBecomeInvisible(this, cameraPerspective);
                    }
                }
            }

            void ComponentFrustumCulling::resetVisibility()
            {
                std::map<Component *, bool>::iterator it, aux;
                for (it = visibleFromCameras.begin(); it != visibleFromCameras.end(); it++)
                {
                    Component *cameraComponent = it->first;
                    aux = it;
                    visibleFromCameras.erase(aux);
                    // visibleFromCameras.erase(it++);
                    OnBecomeInvisible(this, cameraComponent);
                }
                // visibleFromCameras.clear();
            }

            bool ComponentFrustumCulling::isVisibleFromCamera(Component *cameraPerspective)
            {
                std::map<Component *, bool>::iterator it;
                it = visibleFromCameras.find(cameraPerspective);
                return it != visibleFromCameras.end();
            }

            void ComponentFrustumCulling::createCollisionLines()
            {

                ComponentColorLine *lines = (ComponentColorLine *)transform[0]->findComponent(ComponentColorLine::Type);
                if (lines == NULL)
                    lines = (ComponentColorLine *)transform[0]->addComponent(new ComponentColorLine());

                lines->vertices.clear();

                // global add vertices
                if (cullingShape == CullingShapeSphere)
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
                else if (cullingShape == CullingShapeAABB)
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

                MathCore::mat4f &m = transform[0]->worldToLocalMatrix();
                for (int i = 0; i < lines->vertices.size(); i++)
                {
                    lines->vertices[i] = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(lines->vertices[i]));
                }

                lines->syncVBODynamic();
            }

        }
    }
}