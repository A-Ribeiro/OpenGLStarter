#include <appkit-gl-engine/Components/deprecated/ComponentCameraLookToNode.h>

#include <AppKit/window/InputDevices.h>


#include <stdio.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentCameraLookToNode::Type = "ComponentCameraLookToNode";

            void ComponentCameraLookToNode::start()
            {
                camera = (ComponentCameraPerspective *)transform[0]->findComponent(ComponentCameraPerspective::Type);

                AppBase *app = Engine::Instance()->app;
                /*
                app->MousePos = app->MousePosCenter;//set app state do cursor center
                app->moveMouseToScreenCenter();//queue update to screen center
                */
                renderWindowRegion = transform[0]->renderWindowRegion;
                renderWindowRegion->OnLateUpdate.add(&ComponentCameraLookToNode::OnLateUpdate, this);
                renderWindowRegion->MousePos.OnChange.add(&ComponentCameraLookToNode::OnMousePosChanged, this);
                renderWindowRegion->WindowViewport.OnChange.add(&ComponentCameraLookToNode::OnViewportChanged, this);

                //renderWindowRegion->Viewport.triggerOnChange();
                //renderWindowRegion->MousePos.triggerOnChange();
                OnViewportChanged(renderWindowRegion->WindowViewport, renderWindowRegion->WindowViewport);
                OnMousePosChanged(renderWindowRegion->MousePos, renderWindowRegion->MousePos);
            }

            void ComponentCameraLookToNode::OnLateUpdate(Platform::Time *time)
            {

                if (target == NULL || camera == NULL)
                    return;
                
                using namespace AppKit::Window::Devices;

                up.setState(Keyboard::isPressed(KeyCode::Up) || Keyboard::isPressed(KeyCode::W));
                down.setState(Keyboard::isPressed(KeyCode::Down) || Keyboard::isPressed(KeyCode::S));

                float newDistance = distance_to_target;
                if (up.pressed)
                    newDistance = MathCore::OP<float>::move(newDistance, 0.01f, zoomSpeed * time->deltaTime);
                if (down.pressed)
                    newDistance = MathCore::OP<float>::move(newDistance, 10.0f, zoomSpeed * time->deltaTime);

                if (newDistance != distance_to_target)
                {
                    distance_to_target = newDistance;
                    MathCore::vec3f backward = transform[0]->Rotation * MathCore::vec3f(0, 0, -1);
                    transform[0]->Position = target->Position + backward * distance_to_target;
                }
            }

            void ComponentCameraLookToNode::OnViewportChanged(const iRect &value, const iRect &oldValue)
            {
                if (target != NULL && camera != NULL)
                {
                    // AppBase* app = Engine::Instance()->app;
                    renderWindowRegion->moveMouseToScreenCenter();
                }
            }

            void ComponentCameraLookToNode::OnMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue)
            {

                if (target == NULL || camera == NULL)
                    return;

                // AppBase* app = Engine::Instance()->app;

                MathCore::vec2f pos = value;

                MathCore::vec2f delta = pos - renderWindowRegion->screenCenterF;

                if ( MathCore::OP<MathCore::vec2f>::sqrLength(delta) > MathCore::EPSILON<float>::high_precision)
                {
                    if (camera->rightHanded)
                    {
                        euler.y = MathCore::OP<float>::fmod(euler.y - delta.x * angleSpeed, 2.0f * MathCore::CONSTANT<float>::PI);
                        euler.x = euler.x + delta.y * angleSpeed;
                    }
                    else
                    {
                        euler.y = MathCore::OP<float>::fmod(euler.y + delta.x * angleSpeed, 2.0f * MathCore::CONSTANT<float>::PI);
                        euler.x = euler.x - delta.y * angleSpeed;
                    }

                    euler.x = MathCore::OP<float>::clamp(euler.x, -MathCore::OP<float>::deg_2_rad(90.0f), MathCore::OP<float>::deg_2_rad(90.0f));

                    transform[0]->Rotation = MathCore::GEN<MathCore::quatf>::fromEuler(euler.x, euler.y, euler.z);

                    MathCore::vec3f backward = transform[0]->Rotation * MathCore::vec3f(0, 0, -1);
                    transform[0]->Position = target->Position + backward * distance_to_target;

                    renderWindowRegion->moveMouseToScreenCenter();
                }
            }

            Transform *ComponentCameraLookToNode::getTarget()
            {
                return target;
            }

            void ComponentCameraLookToNode::setTarget(Transform *_transform)
            {
                target = _transform;
                if (_transform != NULL)
                {

                    // AppBase* app = Engine::Instance()->app;

                    renderWindowRegion->MousePos = renderWindowRegion->screenCenterF; // set app state do cursor center
                    renderWindowRegion->moveMouseToScreenCenter();                    // queue update to screen center

                    transform[0]->lookAtLeftHanded(_transform);

                    distance_to_target = MathCore::OP<MathCore::vec3f>::distance(transform[0]->Position, target->Position);

                    // convert the transform camera to camera move euler angles...
                    MathCore::vec3f forward = MathCore::CVT<MathCore::vec4f>::toVec3(transform[0]->getMatrix()[2]);
                    MathCore::vec3f proj_y = MathCore::vec3f(forward.x, 0, forward.z);
                    float length_proj_y = MathCore::OP<MathCore::vec3f>::length(proj_y);
                    proj_y = MathCore::OP<MathCore::vec3f>::normalize(proj_y);
                    MathCore::vec3f cone_proj_x = MathCore::OP<MathCore::vec3f>::normalize(MathCore::vec3f(length_proj_y, forward.y, 0));

                    euler.x = -atan2(cone_proj_x.y, cone_proj_x.x);
                    euler.y = atan2(proj_y.x, proj_y.z);
                    euler.z = 0;

                    while (euler.x < -MathCore::OP<float>::deg_2_rad(90.0f))
                        euler.x += MathCore::OP<float>::deg_2_rad(360.0f);
                    while (euler.x > MathCore::OP<float>::deg_2_rad(90.0f))
                        euler.x -= MathCore::OP<float>::deg_2_rad(360.0f);

                    transform[0]->Rotation = MathCore::GEN<MathCore::quatf>::fromEuler(euler.x, euler.y, euler.z);

                    MathCore::vec3f backward = transform[0]->Rotation * MathCore::vec3f(0, 0, -1);
                    transform[0]->Position = target->Position + backward * distance_to_target;
                }
            }

            ComponentCameraLookToNode::ComponentCameraLookToNode() : Component(ComponentCameraLookToNode::Type),
                                                                     Target(
                                                                        EventCore::CallbackWrapper( &ComponentCameraLookToNode::getTarget, this ),
                                                                        EventCore::CallbackWrapper( &ComponentCameraLookToNode::setTarget, this )
                                                                    )
            {

                zoomSpeed = 1.0f;
                angleSpeed = MathCore::OP<float>::deg_2_rad(0.10f);
                renderWindowRegion = NULL;
            }

            ComponentCameraLookToNode::~ComponentCameraLookToNode()
            {
                // AppBase* app = Engine::Instance()->app;

                if (renderWindowRegion != NULL)
                {
                    renderWindowRegion->OnLateUpdate.remove(&ComponentCameraLookToNode::OnLateUpdate, this);
                    renderWindowRegion->MousePos.OnChange.remove(&ComponentCameraLookToNode::OnMousePosChanged, this);
                    renderWindowRegion->WindowViewport.OnChange.remove(&ComponentCameraLookToNode::OnViewportChanged, this);
                }
            }

        }
    }

}
