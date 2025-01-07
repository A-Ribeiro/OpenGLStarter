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
                auto transform = getTransform();

                camera = transform->findComponent<ComponentCameraPerspective>();

                AppBase *app = Engine::Instance()->app;
                /*
                app->MousePos = app->MousePosCenter;//set app state do cursor center
                app->moveMouseToScreenCenter();//queue update to screen center
                */
                renderWindowRegionRef = transform->renderWindowRegion;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);
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
                auto target = ToShared(targetRef);

                if (target == nullptr || camera == nullptr)
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
                    auto transform = getTransform();

                    distance_to_target = newDistance;
                    MathCore::vec3f backward = transform->Rotation * MathCore::vec3f(0, 0, -1);
                    transform->Position = target->Position + backward * distance_to_target;
                }
            }

            void ComponentCameraLookToNode::OnViewportChanged(const iRect &value, const iRect &oldValue)
            {
                auto target = ToShared(targetRef);
                if (target != nullptr && camera != nullptr)
                {
                    // AppBase* app = Engine::Instance()->app;
                    auto renderWindowRegion = ToShared(renderWindowRegionRef);
                    renderWindowRegion->moveMouseToScreenCenter();
                }
            }

            void ComponentCameraLookToNode::OnMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue)
            {
                auto target = ToShared(targetRef);
                if (target == nullptr || camera == nullptr)
                    return;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);


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

                    auto transform = getTransform();

                    transform->Rotation = MathCore::GEN<MathCore::quatf>::fromEuler(euler.x, euler.y, euler.z);

                    MathCore::vec3f backward = transform->Rotation * MathCore::vec3f(0, 0, -1);
                    transform->Position = target->Position + backward * distance_to_target;

                    renderWindowRegion->moveMouseToScreenCenter();
                }
            }

            std::shared_ptr<Transform> ComponentCameraLookToNode::getTarget()
            {
                return ToShared(targetRef);
            }

            void ComponentCameraLookToNode::setTarget(std::shared_ptr<Transform> _target)
            {
                targetRef = _target;
                if (_target != nullptr)
                {

                    // AppBase* app = Engine::Instance()->app;
                    auto renderWindowRegion = ToShared(renderWindowRegionRef);


                    renderWindowRegion->MousePos = renderWindowRegion->screenCenterF; // set app state do cursor center
                    renderWindowRegion->moveMouseToScreenCenter();                    // queue update to screen center

                    auto transform = getTransform();

                    transform->lookAtLeftHanded(_target);

                    distance_to_target = MathCore::OP<MathCore::vec3f>::distance(transform->Position, _target->Position);

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

                    MathCore::vec3f backward = transform->Rotation * MathCore::vec3f(0, 0, -1);
                    transform->Position = _target->Position + backward * distance_to_target;
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
                renderWindowRegionRef.reset();// = nullptr;
            }

            ComponentCameraLookToNode::~ComponentCameraLookToNode()
            {
                // AppBase* app = Engine::Instance()->app;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);
                if (renderWindowRegion != nullptr)
                {
                    renderWindowRegion->OnLateUpdate.remove(&ComponentCameraLookToNode::OnLateUpdate, this);
                    renderWindowRegion->MousePos.OnChange.remove(&ComponentCameraLookToNode::OnMousePosChanged, this);
                    renderWindowRegion->WindowViewport.OnChange.remove(&ComponentCameraLookToNode::OnViewportChanged, this);
                }
            }

            std::shared_ptr<Component> ComponentCameraLookToNode::duplicate_ref_or_clone(bool force_clone){
                auto result = Component::CreateShared<ComponentCameraLookToNode>();

                result->camera = this->camera;
                result->euler = this->euler;
                result->distance_to_target = this->distance_to_target;

                result->targetRef = this->targetRef;

                //EventCore::VirtualProperty<std::shared_ptr<Transform>> Target;

                result->zoomSpeed = this->zoomSpeed;
                result->angleSpeed = this->angleSpeed;

                return result;
            }
            void ComponentCameraLookToNode::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap){
                {
                    auto found = transformMap.find(ToShared(this->targetRef));
                    if (found != transformMap.end())
                        this->targetRef = found->second;
                }
                {
                    auto found = componentMap.find(this->camera);
                    if (found != componentMap.end())
                        this->camera = std::dynamic_pointer_cast<ComponentCameraPerspective>(found->second);
                }
            }

            void ComponentCameraLookToNode::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentCameraLookToNode::Type);
                writer.String("id");
                writer.Uint64((intptr_t)self().get());
                writer.EndObject();
                
            }
            void ComponentCameraLookToNode::Deserialize(rapidjson::Value &_value,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                  ResourceSet &resourceSet)
            {
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), ComponentCameraLookToNode::Type) == 0)
                    return;
                
            }


        }
    }

}
