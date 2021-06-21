#include "ComponentFrustumCulling.h"
#include <mini-gl-engine/ComponentColorLine.h>
#include <mini-gl-engine/ComponentColorMesh.h>
//#include "ComponentColorMesh.h"

using namespace aRibeiro;

namespace GLEngine {

    namespace Components {

        const ComponentType ComponentFrustumCulling::Type = "ComponentFrustumCulling";

        ComponentFrustumCulling::ComponentFrustumCulling():Component(ComponentFrustumCulling::Type){
            cullingShape = CullingShapeNone;
            debugCollisionShapes = false;
        }

        ComponentFrustumCulling::~ComponentFrustumCulling() {
            //precisa de evento de attach to transform e detach from transform para lidar com essas situações
            if (transform.size()>0)
                transform[0]->OnVisited.remove(this, &ComponentFrustumCulling::OnTransformVisited);
        }

        void ComponentFrustumCulling::computeFinalPositions(bool visitedFlag){
            if (cullingShape == CullingShapeSphere){
                mat4 &m = transform[0]->getMatrix(visitedFlag);
                vec3 scale = transform[0]->getScale(visitedFlag);
                
                sphere = collision::Sphere(
                                        toVec3(m * toPtn4(sphereCenter)),
                                        sphereRadius * maximum(maximum(scale.x, scale.y), scale.z)
                                        );
            } else if (cullingShape == CullingShapeAABB){
                mat4 &m = transform[0]->getMatrix(visitedFlag);

                vec3 center = toVec3(m * toPtn4(aabbCenter));

                vec3 right = toVec3(m[0]) * aabbDimension_2.x;
                vec3 top = toVec3(m[1]) * aabbDimension_2.y;
                vec3 depth = toVec3(m[2]) * aabbDimension_2.z;

                vec3 vertices[8] = { 
                    center - right - top - depth, //000
                    center - right - top + depth, //001
                    center - right + top - depth, //010
                    center - right + top + depth, //011
                    center + right - top - depth, //100
                    center + right - top + depth, //101
                    center + right + top - depth, //110
                    center + right + top + depth  //111
                };

                aabb = collision::AABB(vertices[0], vertices[1]);
                for (int i = 2; i < 8; i++) {
                    aabb.min_box = minimum(aabb.min_box, vertices[i]);
                    aabb.max_box = maximum(aabb.max_box, vertices[i]);
                }

                /*
                vec3 transformed_vertex = toVec3( m * aabbVertex[0] );
                aabb = collision::AABB(transformed_vertex,transformed_vertex);
                for(int i=1;i<8;i++){
                    transformed_vertex = toVec3( m * aabbVertex[i] );
                    aabb.min_box = minimum(aabb.min_box, transformed_vertex);
                    aabb.max_box = maximum(aabb.max_box, transformed_vertex);
                }
                */
                
            }
            if (debugCollisionShapes)
                createCollisionLines();
        }

        void ComponentFrustumCulling::start() {
            computeFinalPositions(false);
        }

        void ComponentFrustumCulling::attachToTransform(Transform *t) {
            t->OnVisited.add(this, &ComponentFrustumCulling::OnTransformVisited);
        }

        void ComponentFrustumCulling::detachFromTransform(Transform *t) {
            t->OnVisited.remove(this, &ComponentFrustumCulling::OnTransformVisited);
        }

        void ComponentFrustumCulling::OnTransformVisited(Transform *t) {
            computeFinalPositions(true);
        }

        ComponentFrustumCulling *ComponentFrustumCulling::createShapeSphere(const vec3& sphereCenter, float sphereRadius) {
            ComponentFrustumCulling *result = new ComponentFrustumCulling();

            result->cullingShape = CullingShapeSphere;
            result->sphereCenter = sphereCenter;
            result->sphereRadius = sphereRadius;

            return result;
        }

        ComponentFrustumCulling *ComponentFrustumCulling::createShapeAABB(const collision::AABB& aabb) {
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

        void ComponentFrustumCulling::setVisibilityFromCamera(Component* cameraPerspective, bool visibility) {
            std::map<Component*, bool>::iterator it;
            it = visibleFromCameras.find(cameraPerspective);

            if (visibility) {
                if (it == visibleFromCameras.end()) {
                    visibleFromCameras[cameraPerspective] = true;
                    OnBecomeVisible(this, cameraPerspective);
                }
            }
            else {
                if (it != visibleFromCameras.end()) {
                    visibleFromCameras.erase(it);
                    OnBecomeInvisible(this, cameraPerspective);
                }
            }
        }

        void ComponentFrustumCulling::resetVisibility() {
            std::map<Component*, bool>::iterator it,aux;
            for (it = visibleFromCameras.begin(); it != visibleFromCameras.end(); it++) {
                Component* cameraComponent = it->first;
                aux = it;
                visibleFromCameras.erase(aux);
                //visibleFromCameras.erase(it++);
                OnBecomeInvisible(this, cameraComponent);
            }
            //visibleFromCameras.clear();
        }

        bool ComponentFrustumCulling::isVisibleFromCamera(Component* cameraPerspective) {
            std::map<Component*, bool>::iterator it;
            it = visibleFromCameras.find(cameraPerspective);
            return it != visibleFromCameras.end();
        }

        void ComponentFrustumCulling::createCollisionLines() {


            ComponentColorLine *lines = (ComponentColorLine *)transform[0]->findComponent(ComponentColorLine::Type);
            if (lines == NULL)
                lines = (ComponentColorLine *)transform[0]->addComponent(new ComponentColorLine());

            lines->vertices.clear();

            //global add vertices
            if (cullingShape == CullingShapeSphere) {

                aRibeiro::aligned_vector<aRibeiro::vec3> vertices;
                std::vector<unsigned short> indices;
                setSphere(sphere.radius, 16, 16, &indices, &vertices);

                for (size_t i = 0; i < indices.size(); i+=3) {
                    lines->vertices.push_back(vertices[indices[i]] + sphere.center);
                    lines->vertices.push_back(vertices[indices[i+1]] + sphere.center);
                }

            }
            else if (cullingShape == CullingShapeAABB) {
                vec3 array[8] = {
                    vec3(aabb.min_box.x, aabb.min_box.y, aabb.min_box.z),// 000 0
                    vec3(aabb.min_box.x, aabb.min_box.y, aabb.max_box.z),// 001 1
                    vec3(aabb.min_box.x, aabb.max_box.y, aabb.min_box.z),// 010 2
                    vec3(aabb.min_box.x, aabb.max_box.y, aabb.max_box.z),// 011 3
                    vec3(aabb.max_box.x, aabb.min_box.y, aabb.min_box.z),// 100 4
                    vec3(aabb.max_box.x, aabb.min_box.y, aabb.max_box.z),// 101 5
                    vec3(aabb.max_box.x, aabb.max_box.y, aabb.min_box.z),// 110 6
                    vec3(aabb.max_box.x, aabb.max_box.y, aabb.max_box.z) // 111 7
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


                lines->vertices.push_back(array[4+0]);
                lines->vertices.push_back(array[4 + 1]);

                lines->vertices.push_back(array[4 + 1]);
                lines->vertices.push_back(array[4 + 3]);

                lines->vertices.push_back(array[4 + 3]);
                lines->vertices.push_back(array[4 + 2]);

                lines->vertices.push_back(array[4 + 2]);
                lines->vertices.push_back(array[4 + 0]);

            }
            
            mat4 & m = transform[0]->worldToLocalMatrix();
            for (int i = 0; i < lines->vertices.size(); i++) {
                lines->vertices[i] = toVec3(m * toPtn4(lines->vertices[i]));
            }

            lines->syncVBODynamic();
            
        }

    }
}
