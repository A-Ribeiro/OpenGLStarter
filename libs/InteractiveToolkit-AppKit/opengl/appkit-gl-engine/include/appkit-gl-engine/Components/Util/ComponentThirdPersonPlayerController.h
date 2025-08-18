#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

//#include <appkit-gl-engine/SharedPointer/SharedPointer.h>

#include <appkit-gl-engine/Components/Ext/ComponentAnimationMotion.h>

#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

//using namespace AppKit::Window::Devices;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            enum ThirdPersonRotateType
            {
                ThirdPersonRotateSkinnedMesh,
                ThirdPersonRotateSelf
            };

            class ComponentThirdPersonPlayerController : public Component
            {

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

            public:
                static const ComponentType Type;

                std::shared_ptr<ComponentAnimationMotion> animationMotion;

                EventCore::PressReleaseDetector walk;
                EventCore::PressReleaseDetector run;

                EventCore::PressReleaseDetector right;
                EventCore::PressReleaseDetector left;

                std::weak_ptr<Transform> cameraLookRef;
                MathCore::vec4f cameraLook_bkp_relative_skinnerMesh;
                ThirdPersonRotateType rotateTargetNode;
                std::shared_ptr<ComponentSkinnedMesh> skinnedMesh; // becomes nullptr when skinned mesh is deleted

                float turn_speed;

                std::string animation_str_idle;
                std::string animation_str_walk;
                std::string animation_str_run;

                ComponentThirdPersonPlayerController() : Component(ComponentThirdPersonPlayerController::Type)
                {
                    turn_speed = 100.0f;

                    rotateTargetNode = ThirdPersonRotateSkinnedMesh;

                    animation_str_idle = "idle";
                    animation_str_walk = "walk";
                    animation_str_run = "run";

                    renderWindowRegionRef.reset();// = nullptr;
                }

                void setCameraLook(std::shared_ptr<Transform> camera_look)
                {
                    auto transform = getTransform();
                    if (skinnedMesh == nullptr)
                    {
                        skinnedMesh = transform->findComponent<ComponentSkinnedMesh>();
                        if (skinnedMesh == nullptr)
                            skinnedMesh = transform->findComponentInChildren<ComponentSkinnedMesh>();
                        ITK_ABORT(skinnedMesh == nullptr, "Failed to query skinned mesh\n.");
                    }
                    cameraLookRef = camera_look;
                    if (skinnedMesh != nullptr && camera_look != nullptr)
                    {
                        auto skinnedMesh_transform = skinnedMesh->getTransform();
                        cameraLook_bkp_relative_skinnerMesh = skinnedMesh_transform->worldToLocalMatrix() * 
                            MathCore::CVT<MathCore::vec3f>::toPtn4(camera_look->getPosition());
                        cameraLook_bkp_relative_skinnerMesh.w = 1.0f; // point
                    }
                }

                void start()
                {
                    auto transform = getTransform();

                    // AppBase* app = Engine::Instance()->app;
                    renderWindowRegionRef = transform->renderWindowRegion;
                    auto renderWindowRegion = ToShared(renderWindowRegionRef);
                    renderWindowRegion->OnUpdate.add(&ComponentThirdPersonPlayerController::OnUpdate, this);

                    animationMotion = transform->findComponent<ComponentAnimationMotion>();
                    ITK_ABORT(animationMotion == nullptr, "Failed to query animationMotion\n.");

                    // animationMotion->OnMove.add(this, &ComponentThirdPersonPlayerController::OnMoveAnimation);

                    setCameraLook(ToShared(cameraLookRef));
                }

                ~ComponentThirdPersonPlayerController()
                {
                    // AppBase* app = Engine::Instance()->app;
                    auto renderWindowRegion = ToShared(renderWindowRegionRef);
                    if (renderWindowRegion != nullptr)
                    {
                        renderWindowRegion->OnUpdate.remove(&ComponentThirdPersonPlayerController::OnUpdate, this);
                    }

                    if (animationMotion != nullptr)
                        animationMotion = nullptr;

                    if (skinnedMesh != nullptr)
                        skinnedMesh = nullptr;
                }

                void OnMoveAnimation(float right, float up, float forward, const MathCore::vec3f &final_local_position)
                {
                    auto transform = getTransform();

                    transform->LocalPosition = final_local_position;

                    /*
                    switch (rotateTargetNode)
                    {
                    case ThirdPersonRotateSkinnedMesh: {
                        MathCore::vec3f delta = (skinnedMesh->transform[0]->LocalRotation * MathCore::vec3f(0, 0, forward));
                        transform[0]->LocalPosition = transform[0]->LocalPosition + delta;
                        break;
                    }
                    case ThirdPersonRotateSelf: {
                        //skinnedMesh->transform[0]->LocalPosition = final_local_position;
                        transform[0]->LocalPosition = final_local_position;
                        break;
                    }
                    }
                    */
                   
                }

                void OnUpdate(Platform::Time *time)
                {
                    using namespace AppKit::Window::Devices;

                    right.setState(Keyboard::isPressed(KeyCode::A));
                    left.setState(Keyboard::isPressed(KeyCode::D));
                    walk.setState(Keyboard::isPressed(KeyCode::W) || (right.pressed ^ left.pressed));
                    run.setState(Keyboard::isPressed(KeyCode::W) && Keyboard::isPressed(KeyCode::LShift));

                    if (run.down)
                        animationMotion->TriggerClip(animation_str_run);
                    else if (walk.down || (walk.pressed && run.up))
                    {
                        animationMotion->TriggerClip(animation_str_walk);
                    }
                    else if (walk.up || run.up)
                        animationMotion->TriggerClip(animation_str_idle);

                    if (right.pressed ^ left.pressed)
                    {
                        auto transform = getTransform();
                        auto skinnedMesh_transform = skinnedMesh->getTransform();
                        auto cameraLook = ToShared(cameraLookRef); 

                        switch (rotateTargetNode)
                        {
                        case ThirdPersonRotateSkinnedMesh:
                            if (right.pressed)
                                skinnedMesh_transform->LocalRotation = skinnedMesh_transform->LocalRotation * MathCore::GEN<MathCore::quatf>::fromEuler(0, -MathCore::OP<float>::deg_2_rad(turn_speed) * time->deltaTime, 0);
                            else if (left.pressed)
                                skinnedMesh_transform->LocalRotation = skinnedMesh_transform->LocalRotation * MathCore::GEN<MathCore::quatf>::fromEuler(0, MathCore::OP<float>::deg_2_rad(turn_speed) * time->deltaTime, 0);

                            // transform cameraLook node
                            if (cameraLook != nullptr)
                            {
                                cameraLook->Position =
                                    MathCore::CVT<MathCore::vec4f>::toVec3(
                                        skinnedMesh_transform->localToWorldMatrix() * 
                                        cameraLook_bkp_relative_skinnerMesh
                                    );

                                /*
                                animator.transform.localToWorldMatrix.MultiplyPoint(
                                animator.transform.rotation *
                                animator.transform.worldToLocalMatrix.MultiplyPoint(cameraLook_bkp.position)
                            );
                            */
                            }

                            break;
                        case ThirdPersonRotateSelf:
                            if (right.pressed)
                                transform->LocalRotation = transform->LocalRotation * MathCore::GEN<MathCore::quatf>::fromEuler(0, -MathCore::OP<float>::deg_2_rad(turn_speed) * time->deltaTime, 0);
                            else if (left.pressed)
                                transform->LocalRotation = transform->LocalRotation * MathCore::GEN<MathCore::quatf>::fromEuler(0, MathCore::OP<float>::deg_2_rad(turn_speed) * time->deltaTime, 0);
                            break;
                        }
                    }

                    /*
                    if (walk.pressed) {
                        transform[0]->LocalPosition = transform[0]->LocalPosition + transform[0]->LocalRotation * MathCore::vec3f(0, 0, time->deltaTime * 5.0f);
                    }
                    */
                }

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
                {
                    auto result = Component::CreateShared<ComponentThirdPersonPlayerController>();

                    result->animationMotion = this->animationMotion;

                    result->cameraLookRef = this->cameraLookRef;
                    result->cameraLook_bkp_relative_skinnerMesh = this->cameraLook_bkp_relative_skinnerMesh;
                    result->rotateTargetNode = this->rotateTargetNode;
                    result->skinnedMesh = this->skinnedMesh;

                    result->turn_speed = this->turn_speed;

                    result->animation_str_idle = this->animation_str_idle;
                    result->animation_str_walk = this->animation_str_walk;
                    result->animation_str_run = this->animation_str_run;

                    return result;
                }
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
                {
                    {
                        auto found = transformMap.find(ToShared(this->cameraLookRef));
                        if (found != transformMap.end())
                            this->cameraLookRef = found->second;
                    }
                    {
                        auto found = componentMap.find(this->animationMotion);
                        if (found != componentMap.end())
                            this->animationMotion = std::dynamic_pointer_cast<ComponentAnimationMotion>(found->second);
                    }
                    {
                        auto found = componentMap.find(this->skinnedMesh);
                        if (found != componentMap.end())
                            this->skinnedMesh = std::dynamic_pointer_cast<ComponentSkinnedMesh>(found->second);
                    }
                }

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                    writer.StartObject();
                    writer.String("type");
                    writer.String(ComponentThirdPersonPlayerController::Type);
                    writer.String("id");
                    writer.Uint64((intptr_t)self().get());
                    writer.EndObject();
                    
                }
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet){
                    if (!_value.HasMember("type") || !_value["type"].IsString())
                        return;
                    if (!strcmp(_value["type"].GetString(), ComponentThirdPersonPlayerController::Type) == 0)
                        return;
                    
                }
            };

        }

    }

}