#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

// #include <appkit-gl-engine/SharedPointer/SharedPointer.h>

#include <appkit-gl-engine/Components/ComponentMaterial.h>
#include <appkit-gl-engine/Components/ComponentMesh.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            class ComponentThirdPersonCamera : public Component
            {
            public:
                static const ComponentType Type;

                float current_x_angle;
                float distance;
                float distance_offset;
                float pos_speed;
                float rotacional_speed_y;
                float rotacional_speed_x;

                std::weak_ptr<Transform> TargetRef;
                MathCore::vec3f transform_position_target;

                std::weak_ptr<Transform> Player_ForwardRef;

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

                // Transform *debugSphere;
                // Transform *debugSphere2;

                ComponentThirdPersonCamera() : Component(ComponentThirdPersonCamera::Type)
                {
                    current_x_angle = -1;
                    distance = -1;
                    distance_offset = 1;
                    pos_speed = 2.0f;
                    rotacional_speed_y = 50.0f;
                    rotacional_speed_x = 10.0f;
                    renderWindowRegionRef.reset();
                }

                void start()
                {
                    auto transform = getTransform();

                    // AppBase* app = Engine::Instance()->app;
                    renderWindowRegionRef = transform->renderWindowRegion;
                    auto renderWindowRegion = ToShared(renderWindowRegionRef);
                    renderWindowRegion->OnLateUpdate.add(&ComponentThirdPersonCamera::OnLateUpdate, this);

                    transform_position_target = transform->Position;

                    auto Target = ToShared(TargetRef);

                    if (Target == nullptr)
                        return;

                    if (distance <= 0)
                        distance = MathCore::OP<MathCore::vec3f>::distance(Target->Position, transform->Position);

                    if (current_x_angle < 0)
                    {
                        MathCore::vec3f cam2target = MathCore::OP<MathCore::vec3f>::normalize(Target->Position - transform_position_target);
                        MathCore::vec2f angle_x_aux = MathCore::vec2f(
                            MathCore::OP<float>::sqrt(cam2target.x * cam2target.x + cam2target.z * cam2target.z),
                            cam2target.y);
                        current_x_angle = MathCore::OP<float>::rad_2_deg(
                            -MathCore::OP<float>::atan2(angle_x_aux.y, angle_x_aux.x));
                    }

                    /*
                    debugSphere = new Transform();
                    debugSphere2 = new Transform();
                    transform[0]->getParent()->addChild(debugSphere);
                    transform[0]->getParent()->addChild(debugSphere2);
                    {
                        ComponentMaterial * newMaterial;

                        newMaterial = new ComponentMaterial();
                        newMaterial->type = Components::MaterialPBR;
                        newMaterial->pbr.albedoColor = MathCore::vec4f(1, 0, 0, 1);
                        newMaterial->pbr.metallic = 0.0f;
                        newMaterial->pbr.roughness = 1.0f;
                        newMaterial->pbr.texAlbedo = nullptr;
                        newMaterial->pbr.texNormal = nullptr;
                        newMaterial->pbr.texSpecular = nullptr;

                        debugSphere->addComponent(newMaterial);

                        debugSphere->addComponent(ComponentMesh::createSphere(0.1, 8, 8));


                        newMaterial = new ComponentMaterial();
                        newMaterial->type = Components::MaterialPBR;
                        newMaterial->pbr.albedoColor = MathCore::vec4f(0, 0, 1, 1);
                        newMaterial->pbr.metallic = 0.0f;
                        newMaterial->pbr.roughness = 1.0f;
                        newMaterial->pbr.texAlbedo = nullptr;
                        newMaterial->pbr.texNormal = nullptr;
                        newMaterial->pbr.texSpecular = nullptr;

                        debugSphere2->addComponent(newMaterial);
                        debugSphere2->addComponent(ComponentMesh::createSphere(0.1, 8, 8));
                    }
                    */
                }

                ~ComponentThirdPersonCamera()
                {
                    // AppBase* app = Engine::Instance()->app;
                    auto renderWindowRegion = ToShared(renderWindowRegionRef);

                    if (renderWindowRegion != nullptr)
                    {
                        renderWindowRegion->OnLateUpdate.remove(&ComponentThirdPersonCamera::OnLateUpdate, this);
                    }

                    auto Target = ToShared(TargetRef);

                    if (Target != nullptr)
                    {
                        // Target = nullptr;
                        TargetRef.reset();
                    }

                    auto Player_Forward = ToShared(Player_ForwardRef);
                    if (Player_Forward != nullptr)
                    {
                        // Player_Forward = nullptr;
                        Player_ForwardRef.reset();
                    }
                }

                void OnLateUpdate(Platform::Time *time)
                {
                    auto Target = ToShared(TargetRef);

                    if (Target == nullptr)
                        return;

                    Position_1st_Quadratic_Clamp(time);
                    Position_2nd_Quadratic_Clamp(time);
                    X_Rotate_Smooth(time);
                    Y_Rotate_Smooth(time);
                    // RaycastPlayerToScene(time);
                }

                void Position_1st_Quadratic_Clamp(Platform::Time *time)
                {
                    auto Target = ToShared(TargetRef);

                    // quadratic clamp
                    float new_distance = MathCore::OP<MathCore::vec3f>::distance(Target->Position, transform_position_target);
                    if (new_distance > distance)
                    {
                        MathCore::vec3f cam2target = MathCore::OP<MathCore::vec3f>::normalize(Target->Position - transform_position_target);
                        transform_position_target = Target->Position - cam2target * distance;
                    }

                    auto transform = getTransform();
                    // move towards
                    transform->Position = MathCore::OP<MathCore::vec3f>::move(transform->Position, transform_position_target, pos_speed * time->deltaTime);
                }

                void Position_2nd_Quadratic_Clamp(Platform::Time *time)
                {
                    auto Target = ToShared(TargetRef);
                    auto transform = getTransform();

                    MathCore::vec3f cam2target = MathCore::OP<MathCore::vec3f>::normalize(Target->Position - transform->Position);
                    float new_distance = MathCore::OP<MathCore::vec3f>::distance(Target->Position, transform->Position);

                    // quadratic clamp
                    if (new_distance > distance + distance_offset)
                    {
                        // force set without interpolation...
                        transform_position_target = Target->Position - cam2target * (distance + distance_offset);
                        transform->Position = transform_position_target;
                    }

                    transform->Rotation = MathCore::GEN<MathCore::quatf>::lookAtRotationLH(cam2target, MathCore::vec3f(0, 1, 0));
                }

                void X_Rotate_Smooth(Platform::Time *time)
                {
                    auto Target = ToShared(TargetRef);
                    auto transform = getTransform();

                    MathCore::vec3f cam2target = Target->Position - transform->Position;
                    if (MathCore::OP<MathCore::vec3f>::sqrLength(cam2target) <= 0.0002f)
                        return;
                    cam2target = MathCore::OP<MathCore::vec3f>::normalize(cam2target);
                    float new_distance = MathCore::OP<MathCore::vec3f>::distance(Target->Position, transform->Position);

                    MathCore::vec3f camera_forward = transform->Rotation * MathCore::vec3f(0, 0, 1);
                    MathCore::vec3f camera_projected_on_y = (MathCore::vec3f(camera_forward.x, 0, camera_forward.z));
                    if (MathCore::OP<MathCore::vec3f>::sqrLength(camera_projected_on_y) > 0.0002f)
                    {
                        camera_projected_on_y = MathCore::OP<MathCore::vec3f>::normalize(camera_projected_on_y);
                        MathCore::vec3f camera_left = transform->Rotation * MathCore::vec3f(-1, 0, 0);

                        MathCore::vec3f target_rotation_back = MathCore::GEN<MathCore::quatf>::fromAxisAngle(camera_left, MathCore::OP<float>::deg_2_rad(-current_x_angle)) * camera_projected_on_y;
                        target_rotation_back = MathCore::OP<MathCore::vec3f>::normalize(target_rotation_back);

                        target_rotation_back = MathCore::OP<MathCore::quatf>::moveSlerp(
                                                   MathCore::GEN<MathCore::quatf>::lookAtRotationLH(cam2target, MathCore::vec3f(0, 1, 0)),
                                                   MathCore::GEN<MathCore::quatf>::lookAtRotationLH(target_rotation_back, MathCore::vec3f(0, 1, 0)),
                                                   MathCore::OP<float>::deg_2_rad(rotacional_speed_x) * time->deltaTime) *
                                               MathCore::vec3f(0, 0, 1);
                        target_rotation_back = MathCore::OP<MathCore::vec3f>::normalize(target_rotation_back);

                        transform->Position = Target->Position - target_rotation_back * new_distance;
                        // the target just need to be rotated as the main position... but the distance keeps the same
                        transform_position_target = Target->Position - target_rotation_back * MathCore::OP<MathCore::vec3f>::distance(Target->Position, transform_position_target);

                        transform->Rotation = MathCore::GEN<MathCore::quatf>::lookAtRotationLH(target_rotation_back, MathCore::vec3f(0, 1, 0));
                    }
                }

                void Y_Rotate_Smooth(Platform::Time *time)
                // rotate to be in the back
                {
                    auto Target = ToShared(TargetRef);
                    auto Player_Forward = ToShared(Player_ForwardRef);
                    auto transform = getTransform();

                    MathCore::vec3f cam2target = Target->Position - transform->Position;
                    if (MathCore::OP<MathCore::vec3f>::sqrLength(cam2target) <= 0.0002f)
                        return;
                    cam2target = MathCore::OP<MathCore::vec3f>::normalize(cam2target);
                    float new_distance = MathCore::OP<MathCore::vec3f>::distance(Target->Position, transform->Position);
                    MathCore::vec2f angle_x_aux = MathCore::vec2f(sqrtf(cam2target.x * cam2target.x + cam2target.z * cam2target.z), cam2target.y);
                    float current_x_angle = MathCore::OP<float>::rad_2_deg(-atan2(angle_x_aux.y, angle_x_aux.x));

                    // MathCore::vec3f player_forward = Target.parent.forward;

                    MathCore::vec3f player_forward;

                    if (Player_Forward != nullptr)
                        player_forward = Player_Forward->Rotation * MathCore::vec3f(0, 0, 1);
                    else
                        player_forward = Target->getParent()->Rotation * MathCore::vec3f(0, 0, 1);

                    // MathCore::vec3f player_forward = Target->getParent()->Rotation * MathCore::vec3f(0,0,1);

                    MathCore::vec3f player_projected_on_y = (MathCore::vec3f(player_forward.x, 0, player_forward.z));
                    if (MathCore::OP<MathCore::vec3f>::sqrLength(player_projected_on_y) > 0.0002f)
                    {
                        player_projected_on_y = MathCore::OP<MathCore::vec3f>::normalize(player_projected_on_y);

                        MathCore::vec3f player_left;

                        if (Player_Forward != nullptr)
                            player_left = Player_Forward->Rotation * MathCore::vec3f(-1, 0, 0);
                        else
                            player_left = Target->getParent()->Rotation * MathCore::vec3f(-1, 0, 0);

                        // MathCore::vec3f player_left = Target->getParent()->Rotation * MathCore::vec3f(-1,0,0);

                        MathCore::vec3f target_rotation_back = MathCore::GEN<MathCore::quatf>::fromAxisAngle(player_left, MathCore::OP<float>::deg_2_rad(-current_x_angle)) * player_projected_on_y;
                        target_rotation_back = MathCore::OP<MathCore::vec3f>::normalize(target_rotation_back);

                        // debugSphere->setPosition(Target->getPosition() - cam2target);
                        // debugSphere2->setPosition(Target->getPosition() - target_rotation_back);

                        target_rotation_back = MathCore::OP<MathCore::quatf>::moveSlerp(
                                                   MathCore::GEN<MathCore::quatf>::lookAtRotationLH(cam2target, MathCore::vec3f(0, 1, 0)),
                                                   MathCore::GEN<MathCore::quatf>::lookAtRotationLH(target_rotation_back, MathCore::vec3f(0, 1, 0)),
                                                   MathCore::OP<float>::deg_2_rad(rotacional_speed_y) * time->deltaTime) *
                                               MathCore::vec3f(0, 0, 1);
                        target_rotation_back = MathCore::OP<MathCore::vec3f>::normalize(target_rotation_back);

                        transform->Position = Target->Position - target_rotation_back * new_distance;
                        // the target just need to be rotated as the main position... but the distance keeps the same
                        transform_position_target = Target->Position - target_rotation_back * MathCore::OP<MathCore::vec3f>::distance(Target->Position, transform_position_target);

                        transform->Rotation = MathCore::GEN<MathCore::quatf>::lookAtRotationLH(target_rotation_back, MathCore::vec3f(0, 1, 0));
                    }
                }

                void RaycastPlayerToScene(Platform::Time *time)
                {
                    auto Target = ToShared(TargetRef);
                    // auto Player_Forward = ToShared(Player_ForwardRef);
                    auto transform = getTransform();

                    MathCore::vec3f target2cam = transform->Position - Target->Position;
                    if (MathCore::OP<MathCore::vec3f>::sqrLength(target2cam) <= 0.0002f)
                        return;
                    target2cam = MathCore::OP<MathCore::vec3f>::normalize(target2cam);

                    float distance = MathCore::OP<MathCore::vec3f>::distance(transform->Position, Target->Position);

                    CollisionCore::Ray<MathCore::vec3f> ray = CollisionCore::Ray<MathCore::vec3f>(Target->Position, target2cam);
                    /*
                    //
                    // physics not implemented...
                    //
                    RaycastHit[] result = Physics.RaycastAll(ray, distance, 1 << LayerMask.NameToLayer("col.scene"));
                    if (result.Length > 0)
                    {
                        transform_position_target = Target->Position + target2cam * result[0].distance;
                        transform[0]->Position = transform_position_target;
                    }
                    */
                }

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone)
                {
                    auto result = Component::CreateShared<ComponentThirdPersonCamera>();

                    result->current_x_angle = this->current_x_angle;
                    result->distance = this->distance;
                    result->distance_offset = this->distance_offset;
                    result->pos_speed = this->pos_speed;
                    result->rotacional_speed_y = this->rotacional_speed_y;
                    result->rotacional_speed_x = this->rotacional_speed_x;

                    result->TargetRef = this->TargetRef;
                    result->transform_position_target = this->transform_position_target;

                    result->Player_ForwardRef = this->Player_ForwardRef;

                    return result;
                }
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
                {
                    {
                        auto found = transformMap.find(ToShared(this->TargetRef));
                        if (found != transformMap.end())
                            this->TargetRef = found->second;
                    }
                    {
                        auto found = transformMap.find(ToShared(this->Player_ForwardRef));
                        if (found != transformMap.end())
                            this->Player_ForwardRef = found->second;
                    }
                }

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                    writer.StartObject();
                    writer.String("type");
                    writer.String(ComponentThirdPersonCamera::Type);
                    writer.String("id");
                    writer.Uint64((intptr_t)self().get());
                    writer.EndObject();
                    
                }
                void Deserialize(rapidjson::Value &_value, std::unordered_map<intptr_t, std::shared_ptr<Transform>> &transform_map, std::unordered_map<intptr_t, std::shared_ptr<Component>> &component_map){
                    if (!_value.HasMember("type") || !_value["type"].IsString())
                        return;
                    if (!strcmp(_value["type"].GetString(), ComponentThirdPersonCamera::Type) == 0)
                        return;
                    
                }
            };

        }

    }

}