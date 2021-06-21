#include "ComponentCameraLookToNode.h"
#include <stdio.h>
using namespace aRibeiro;

namespace GLEngine {

    namespace Components {

        const ComponentType ComponentCameraLookToNode::Type = "ComponentCameraLookToNode";

        void ComponentCameraLookToNode::start() {
            camera = (ComponentCameraPerspective*)transform[0]->findComponent(ComponentCameraPerspective::Type);
            
            AppBase* app = Engine::Instance()->app;
            /*
            app->MousePos = app->MousePosCenter;//set app state do cursor center
            app->moveMouseToScreenCenter();//queue update to screen center
            */

            app->OnLateUpdate.add(this, &ComponentCameraLookToNode::OnLateUpdate);
            app->MousePos.OnChange.add(this, &ComponentCameraLookToNode::OnMousePosChanged);
            app->WindowSize.OnChange.add(this, &ComponentCameraLookToNode::OnWindowSizeChanged);

            OnWindowSizeChanged(&app->WindowSize);
            OnMousePosChanged(&app->MousePos);

        }

        void ComponentCameraLookToNode::OnLateUpdate(PlatformTime *time) {

            if (target == NULL || camera == NULL)
                return;

            up.setState(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W));
            down.setState(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S));

            float newDistance = distance_to_target;
            if (up.pressed)
                newDistance = move(newDistance, 0.01f, zoomSpeed * time->deltaTime);
            if (down.pressed)
                newDistance = move(newDistance, 10.0f, zoomSpeed * time->deltaTime);

            if (newDistance != distance_to_target) {
                distance_to_target = newDistance;
                vec3 backward = transform[0]->Rotation * vec3(0, 0, -1);
                transform[0]->Position = target->Position + backward * distance_to_target;
            }
        }

        void ComponentCameraLookToNode::OnWindowSizeChanged(Property<sf::Vector2i> *prop) {
            if (target != NULL && camera != NULL) {
                AppBase* app = Engine::Instance()->app;
                app->moveMouseToScreenCenter();
            }
        }

        void ComponentCameraLookToNode::OnMousePosChanged(Property<vec2> *prop) {

            if (target == NULL || camera == NULL)
                return;

            AppBase* app = Engine::Instance()->app;

            vec2 pos = prop->value;

            vec2 delta = pos - app->MousePosCenter;

            if (sqrLength(delta) > EPSILON) {
                if (camera->rightHanded) {
                    euler.y = fmod(euler.y - delta.x * angleSpeed, 2.0f*PI);
                    euler.x = euler.x + delta.y * angleSpeed;
                }
                else {
                    euler.y = fmod(euler.y + delta.x * angleSpeed, 2.0f*PI);
                    euler.x = euler.x - delta.y * angleSpeed;
                }

                euler.x = clamp(euler.x, -DEG2RAD(90.0f), DEG2RAD(90.0f));

                transform[0]->Rotation = quatFromEuler(euler.x, euler.y, euler.z);

                vec3 backward = transform[0]->Rotation * vec3(0, 0, -1);
                transform[0]->Position = target->Position + backward * distance_to_target;

                app->moveMouseToScreenCenter();
            }
        }

        Transform* ComponentCameraLookToNode::getTarget() {
            return target;
        }
        
        void ComponentCameraLookToNode::setTarget(Transform* _transform) {
            target = _transform;
            if (_transform != NULL) {

                AppBase* app = Engine::Instance()->app;

                app->MousePos = app->MousePosCenter;//set app state do cursor center
                app->moveMouseToScreenCenter();//queue update to screen center


                transform[0]->lookAtLeftHanded(_transform);

                distance_to_target = ::distance(transform[0]->Position, target->Position);


                //convert the transform camera to camera move euler angles...
                vec3 forward = toVec3(transform[0]->getMatrix()[2]);
                vec3 proj_y = vec3(forward.x, 0, forward.z);
                float length_proj_y = length(proj_y);
                proj_y = normalize(proj_y);
                vec3 cone_proj_x = normalize(vec3(length_proj_y, forward.y, 0));

                euler.x = -atan2(cone_proj_x.y, cone_proj_x.x);
                euler.y = atan2(proj_y.x, proj_y.z);
                euler.z = 0;

                while (euler.x < -DEG2RAD(90.0f))
                    euler.x += DEG2RAD(360.0f);
                while (euler.x > DEG2RAD(90.0f))
                    euler.x -= DEG2RAD(360.0f);

                transform[0]->Rotation = quatFromEuler(euler.x, euler.y, euler.z);


                vec3 backward = transform[0]->Rotation * vec3(0, 0, -1);
                transform[0]->Position = target->Position + backward * distance_to_target;
            }
        }

        ComponentCameraLookToNode::ComponentCameraLookToNode() :
            Component(ComponentCameraLookToNode::Type),
            Target(this,&ComponentCameraLookToNode::getTarget, &ComponentCameraLookToNode::setTarget){

            zoomSpeed = 1.0f;
            angleSpeed = DEG2RAD(0.10f);
        }

        ComponentCameraLookToNode::~ComponentCameraLookToNode() {
            AppBase* app = Engine::Instance()->app;

            app->OnLateUpdate.remove(this, &ComponentCameraLookToNode::OnLateUpdate);
            app->MousePos.OnChange.remove(this, &ComponentCameraLookToNode::OnMousePosChanged);
            app->WindowSize.OnChange.remove(this, &ComponentCameraLookToNode::OnWindowSizeChanged);
        }

    }
}
