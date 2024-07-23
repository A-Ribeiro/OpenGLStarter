#include <appkit-gl-engine/Components/ComponentCameraMove.h>

#include <stdio.h>

// using namespace aRibeiro;
using namespace AppKit::Window::Devices;

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentCameraMove::Type = "ComponentCameraMove";

            void ComponentCameraMove::start()
            {
                auto transform = getTransform();

                auto camera = transform->findComponent<ComponentCameraPerspective>();
                cameraRef = camera;

                renderWindowRegionRef = transform->renderWindowRegion;

                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                renderWindowRegion->MousePos = renderWindowRegion->screenCenterF; // set app state do cursor center
                renderWindowRegion->moveMouseToScreenCenter();                    // queue update to screen center

                renderWindowRegion->MousePos.OnChange.add(&ComponentCameraMove::OnMousePosChanged, this);
                renderWindowRegion->WindowViewport.OnChange.add(&ComponentCameraMove::OnViewportChanged, this);

                renderWindowRegion->OnLateUpdate.add(&ComponentCameraMove::OnLateUpdate, this);

                OnViewportChanged(renderWindowRegion->WindowViewport,renderWindowRegion->WindowViewport);
                OnMousePosChanged(renderWindowRegion->MousePos,renderWindowRegion->MousePos);

                // AppBase* app = Engine::Instance()->app;

                // app->MousePos = app->screenCenterApp;//set app state do cursor center
                // app->moveMouseToScreenCenter();//queue update to screen center

                // app->OnLateUpdate.add(this, &ComponentCameraMove::OnLateUpdate);
                // app->MousePos.OnChange.add(this, &ComponentCameraMove::OnMousePosChanged);
                // app->WindowSize.OnChange.add(this, &ComponentCameraMove::OnWindowSizeChanged);

                // OnWindowSizeChanged(&app->WindowSize);
                // OnMousePosChanged(&app->MousePos);

                // convert the transform camera to camera move euler angles...
                MathCore::vec3f forward = MathCore::CVT<MathCore::vec4f>::toVec3(transform->getMatrix()[2]);
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

                transform->Rotation = MathCore::GEN<MathCore::quatf>::fromEuler(euler.x, euler.y, euler.z);
            }

            void ComponentCameraMove::OnLateUpdate(Platform::Time *time)
            {
                auto transform = getTransform();
                auto camera = ToShared(cameraRef);
                
                if (camera == nullptr)
                    return;

                left.setState(Keyboard::isPressed(KeyCode::Left) || Keyboard::isPressed(KeyCode::A));
                right.setState(Keyboard::isPressed(KeyCode::Right) || Keyboard::isPressed(KeyCode::D));
                up.setState(Keyboard::isPressed(KeyCode::Up) || Keyboard::isPressed(KeyCode::W));
                down.setState(Keyboard::isPressed(KeyCode::Down) || Keyboard::isPressed(KeyCode::S));

                MathCore::vec3f forwardVec;

                if (camera->rightHanded)
                    forwardVec = transform->Rotation * MathCore::vec3f(0, 0, -1);
                else
                    forwardVec = transform->Rotation * MathCore::vec3f(0, 0, 1);

                MathCore::vec3f rightVec = transform->Rotation * MathCore::vec3f(1, 0, 0);

                if (left.pressed)
                    transform->Position = transform->Position - rightVec * strafeSpeed * time->deltaTime;
                if (right.pressed)
                    transform->Position = transform->Position + rightVec * strafeSpeed * time->deltaTime;
                if (up.pressed)
                    transform->Position = transform->Position + forwardVec * strafeSpeed * time->deltaTime;
                if (down.pressed)
                    transform->Position = transform->Position - forwardVec * strafeSpeed * time->deltaTime;
            }

            void ComponentCameraMove::OnViewportChanged(const iRect &value, const iRect &oldValue)
            {

                // AppBase* app = Engine::Instance()->app;
                // app->moveMouseToScreenCenter();

                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                renderWindowRegion->moveMouseToScreenCenter();
            }

            void ComponentCameraMove::OnMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue)
            {
                // AppBase* app = Engine::Instance()->app;
                auto transform = getTransform();
                auto camera = ToShared(cameraRef);

                if (camera == nullptr)
                    return;

                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                MathCore::vec2f pos = value;

                MathCore::vec2f delta = pos - renderWindowRegion->screenCenterF;

                if (MathCore::OP<MathCore::vec2f>::sqrLength(delta) > MathCore::EPSILON<float>::high_precision)
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

                    transform->Rotation = MathCore::GEN<MathCore::quatf>::fromEuler(euler.x, euler.y, euler.z);

                    renderWindowRegion->moveMouseToScreenCenter();
                }
            }

            ComponentCameraMove::ComponentCameraMove() : Component(ComponentCameraMove::Type)
            {
                forwardSpeed = 1.0f;
                strafeSpeed = 1.0f;
                angleSpeed = MathCore::OP<float>::deg_2_rad(0.10f);
                renderWindowRegionRef.reset();
            }

            ComponentCameraMove::~ComponentCameraMove()
            {
                // AppBase* app = Engine::Instance()->app;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                if (renderWindowRegion != nullptr)
                {
                    renderWindowRegion->OnLateUpdate.remove(&ComponentCameraMove::OnLateUpdate, this);

                    renderWindowRegion->MousePos.OnChange.remove(&ComponentCameraMove::OnMousePosChanged, this);
                    renderWindowRegion->WindowViewport.OnChange.remove(&ComponentCameraMove::OnViewportChanged, this);
                }
            }

        }
    }

}
