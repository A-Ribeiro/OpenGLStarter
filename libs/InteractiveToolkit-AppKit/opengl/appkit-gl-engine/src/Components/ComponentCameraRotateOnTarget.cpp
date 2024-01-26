#include <appkit-gl-engine/Components/ComponentCameraRotateOnTarget.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentCameraRotateOnTarget::Type = "ComponentCameraRotateOnTarget";

            Transform *ComponentCameraRotateOnTarget::getTarget()
            {
                return target;
            }

            void ComponentCameraRotateOnTarget::setTarget(Transform *_transform)
            {
                target = _transform;
                if (target == NULL)
                {
                    mouseMoving = false;
                    return;
                }

                Transform *toLookNode = target;

                distanceRotation = MathCore::OP<MathCore::vec3f>::distance(transform[0]->Position, toLookNode->Position);

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

                /*
                if (mouseMoving) {
                    AppBase* app = Engine::Instance()->app;
                    OnMouseDown(sf::Mouse::Button::Left, app->MousePos);
                }*/

                // time->timeScale = 1.0f;
                mouseMoving = false;
            }

            void ComponentCameraRotateOnTarget::start()
            {
                // AppBase* app = Engine::Instance()->app;

                renderWindowRegion = transform[0]->renderWindowRegion;

                renderWindowRegion->MousePosRelatedToCenterNormalized.OnChange.add(&ComponentCameraRotateOnTarget::OnNormalizedMousePosChanged, this);

                renderWindowRegion->inputManager.onMouseEvent.add(&ComponentCameraRotateOnTarget::onMouseEvent, this);

                // app->OnMouseDown.add(this, &ComponentCameraRotateOnTarget::OnMouseDown);
                // app->OnMouseUp.add(this, &ComponentCameraRotateOnTarget::OnMouseUp);
            }

            /*
            void ComponentCameraRotateOnTarget::OnMouseUp(sf::Mouse::Button button, const MathCore::vec2f &pos){
                //time->timeScale = 1.0f;
                mouseMoving = false;

            }

            void ComponentCameraRotateOnTarget::OnMouseDown(sf::Mouse::Button button, const MathCore::vec2f &pos){
                if (target == NULL)
                    return;

                AppBase* app = Engine::Instance()->app;

                lastPosition = app->WindowCoordToNormalizedCoord(pos);
                //time->timeScale = 0.0f;
                mouseMoving = true;
            }
            */

            void ComponentCameraRotateOnTarget::onMouseEvent(const AppKit::Window::MouseEvent &mouseEvent)
            {

                switch (mouseEvent.type)
                {
                case AppKit::Window::MouseEventType::ButtonPressed:
                {
                    if (target == NULL)
                        return;
                    // AppBase* app = Engine::Instance()->app;

                    lastPosition = renderWindowRegion->localCoordToNormalizedAppCoordRelativeToCenter(mouseEvent.position);
                    // time->timeScale = 0.0f;
                    mouseMoving = true;
                    break;
                }
                case AppKit::Window::MouseEventType::ButtonReleased:
                {
                    mouseMoving = false;
                    break;
                }
                default:
                    break;
                }
            }

            void ComponentCameraRotateOnTarget::OnNormalizedMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue)
            {
                if (!mouseMoving || target == NULL)
                    return;

                // AppBase* app = Engine::Instance()->app;

                if (!enabled)
                {
                    lastPosition = renderWindowRegion->MousePosRelatedToCenterNormalized.c_val();
                    return;
                }

                MathCore::vec2f delta = renderWindowRegion->MousePosRelatedToCenterNormalized.c_val() - lastPosition;
                lastPosition = renderWindowRegion->MousePosRelatedToCenterNormalized.c_val();

                Transform *toLookNode = target;

                {
                    euler.y = MathCore::OP<float>::fmod(euler.y + delta.x * speedAngle, 2.0f * MathCore::CONSTANT<float>::PI);
                    euler.x = euler.x - delta.y * speedAngle;
                }

                // euler.x = clamp(euler.x, -MathCore::OP<float>::deg_2_rad(90.0f), MathCore::OP<float>::deg_2_rad(90.0f));
                // euler.x = aRibeiro::clamp(euler.x, -MathCore::OP<float>::deg_2_rad(22.0f), MathCore::OP<float>::deg_2_rad(47.0f));
                euler.x = MathCore::OP<float>::clamp(euler.x, MathCore::OP<float>::deg_2_rad(rotation_x_deg_min), MathCore::OP<float>::deg_2_rad(rotation_x_deg_max));

                transform[0]->Rotation = MathCore::GEN<MathCore::quatf>::fromEuler(euler.x, euler.y, euler.z);

                MathCore::vec3f backward = transform[0]->Rotation * MathCore::vec3f(0, 0, -1);
                transform[0]->Position = toLookNode->getPosition() + backward * distanceRotation;
            }

            ComponentCameraRotateOnTarget::ComponentCameraRotateOnTarget() : Component(ComponentCameraRotateOnTarget::Type),
                                                                             Target(
                                                                                EventCore::CallbackWrapper(&ComponentCameraRotateOnTarget::getTarget, this),
                                                                                EventCore::CallbackWrapper(&ComponentCameraRotateOnTarget::setTarget, this)
                                                                                ),
                                                                             distanceRotation(1.0f)
            {

                speedAngle = MathCore::OP<float>::deg_2_rad(128.0f);
                mouseMoving = false;
                enabled = true;

                rotation_x_deg_min = -22.0f;
                rotation_x_deg_max = 47.0f;

                renderWindowRegion = NULL;
            }

            ComponentCameraRotateOnTarget::~ComponentCameraRotateOnTarget()
            {
                // AppBase* app = Engine::Instance()->app;

                if (renderWindowRegion != NULL)
                {
                    renderWindowRegion->MousePosRelatedToCenterNormalized.OnChange.remove(&ComponentCameraRotateOnTarget::OnNormalizedMousePosChanged, this);
                    // app->OnMouseDown.remove(this, &ComponentCameraRotateOnTarget::OnMouseDown);
                    // app->OnMouseUp.remove(this, &ComponentCameraRotateOnTarget::OnMouseUp);
                    renderWindowRegion->inputManager.onMouseEvent.remove(&ComponentCameraRotateOnTarget::onMouseEvent, this);
                }
            }

        }
    }
}
