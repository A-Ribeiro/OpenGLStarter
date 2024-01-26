#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

//#include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

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
                        frustumCulling->setVisibilityFromCamera(parameter->camera, CollisionCore::Frustum<MathCore::vec3f>::sphereOverlapsFrustum(frustumCulling->sphere, *parameter->frustum));
                    }
                    else if (frustumCulling->cullingShape == CullingShapeAABB) {
                        frustumCulling->setVisibilityFromCamera(parameter->camera, CollisionCore::Frustum<MathCore::vec3f>::aabbOverlapsFrustum(frustumCulling->aabb, *parameter->frustum));
                    }
                }

                return true;
            }

            */

            enum CullingShape
            {
                CullingShapeNone,
                CullingShapeSphere,
                CullingShapeAABB
            };

            class ComponentFrustumCulling : public Component
            {

                std::map<Component *, bool> visibleFromCameras;

            public:
                bool debugCollisionShapes;

                CullingShape cullingShape;

                // sphere shape
                MathCore::vec3f sphereCenter;
                float sphereRadius;
                
                CollisionCore::Sphere<MathCore::vec3f> sphere;

                // aabb
                MathCore::vec3f aabbDimension_2;
                MathCore::vec3f aabbCenter;

                // MathCore::vec4f aabbVertex[8];

                CollisionCore::AABB<MathCore::vec3f> aabb;

                static const ComponentType Type;

                ComponentFrustumCulling();
                ~ComponentFrustumCulling();

                void computeFinalPositions(bool visitedFlag);

                void start();
                void attachToTransform(Transform *t);
                void detachFromTransform(Transform *t);

                void OnTransformVisited(Transform *t);

                static ComponentFrustumCulling *createShapeSphere(const MathCore::vec3f &sphereCenter, float sphereRadius);
                static ComponentFrustumCulling *createShapeAABB(const CollisionCore::AABB<MathCore::vec3f> &aabb);

                // Property<bool> VisibleFromFrustum;
                // this, cameraPerspective
                EventCore::Event<void(Component *a, Component *b)> OnBecomeVisible;
                EventCore::Event<void(Component *a, Component *b)> OnBecomeInvisible;

                void setVisibilityFromCamera(Component *cameraPerspective, bool visibility);
                void resetVisibility();

                bool isVisibleFromCamera(Component *cameraPerspective);

                //    VoidEvent OnVisibleFromFrustum;

                void createCollisionLines();
            };

        }
    }

}