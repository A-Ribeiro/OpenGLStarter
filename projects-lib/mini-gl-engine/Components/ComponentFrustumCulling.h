#ifndef _component_frustum_culling_h_
#define _component_frustum_culling_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

namespace GLEngine {

    namespace Components {
        
        /*
        struct FrustumParameter{
            ComponentCameraPerspective *camera;
            collision::Frustum *frustum;
        };
        
        bool App::traverse_frustum(Transform *element, void *userData) {
            
            ComponentFrustumCulling *frustumCulling = (ComponentFrustumCulling *)element->findComponent(ComponentFrustumCulling::Type);
            
            if (frustumCulling != NULL) {
                FrustumParameter *parameter = (FrustumParameter *)userData;
                if (frustumCulling->cullingShape == CullingShapeSphere) {
                    frustumCulling->setVisibilityFromCamera(parameter->camera, collision::Frustum::sphereOverlapsFrustum(frustumCulling->sphere, *parameter->frustum));
                }
                else if (frustumCulling->cullingShape == CullingShapeAABB) {
                    frustumCulling->setVisibilityFromCamera(parameter->camera, collision::Frustum::aabbOverlapsFrustum(frustumCulling->aabb, *parameter->frustum));
                }
            }
            
            return true;
        }
        
        */

        enum CullingShape {
            CullingShapeNone,
            CullingShapeSphere,
            CullingShapeAABB
        };

        class ComponentFrustumCulling : public Component {

            std::map<Component*, bool> visibleFromCameras;

        public:

            bool debugCollisionShapes;

            CullingShape cullingShape;

            //sphere shape
            aRibeiro::vec3 sphereCenter;
            float sphereRadius;
            aRibeiro::collision::Sphere sphere;

            //aabb
            aRibeiro::vec3 aabbDimension_2;
            aRibeiro::vec3 aabbCenter;

            //aRibeiro::vec4 aabbVertex[8];

            aRibeiro::collision::AABB aabb;

            static const ComponentType Type;

            ComponentFrustumCulling();
            ~ComponentFrustumCulling();
            
            void computeFinalPositions(bool visitedFlag);
            
            void start();
            void attachToTransform(Transform *t);
            void detachFromTransform(Transform *t);
            
            
            void OnTransformVisited(Transform *t);

            static ComponentFrustumCulling *createShapeSphere(const aRibeiro::vec3& sphereCenter, float sphereRadius);
            static ComponentFrustumCulling *createShapeAABB(const aRibeiro::collision::AABB& aabb);

            //Property<bool> VisibleFromFrustum;
            //this, cameraPerspective
            TwoComponentsEvent OnBecomeVisible;
            TwoComponentsEvent OnBecomeInvisible;

            void setVisibilityFromCamera(Component* cameraPerspective, bool visibility);
            void resetVisibility();

            bool isVisibleFromCamera(Component* cameraPerspective);

            //    VoidEvent OnVisibleFromFrustum;

            void createCollisionLines();

        };

    }
}

#endif
