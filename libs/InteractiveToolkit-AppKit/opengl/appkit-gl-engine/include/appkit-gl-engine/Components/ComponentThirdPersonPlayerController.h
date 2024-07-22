#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

//#include <appkit-gl-engine/SharedPointer/SharedPointer.h>

#include <appkit-gl-engine/Components/ComponentAnimationMotion.h>

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

                RenderWindowRegion *renderWindowRegion;

            public:
                static const ComponentType Type;

                SharedPointer<ComponentAnimationMotion> animationMotion;

                EventCore::PressReleaseDetector walk;
                EventCore::PressReleaseDetector run;

                EventCore::PressReleaseDetector right;
                EventCore::PressReleaseDetector left;

                SharedPointer<Transform> cameraLook;
                MathCore::vec4f cameraLook_bkp_relative_skinnerMesh;
                ThirdPersonRotateType rotateTargetNode;
                SharedPointer<ComponentSkinnedMesh> skinnedMesh; // becomes null when skinned mesh is deleted

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

                    renderWindowRegion = NULL;
                }

                void setCameraLook(Transform *camera_look)
                {
                    if (skinnedMesh == NULL)
                    {
                        skinnedMesh = (ComponentSkinnedMesh *)transform[0]->findComponent(ComponentSkinnedMesh::Type);
                        if (skinnedMesh == NULL)
                            skinnedMesh = (ComponentSkinnedMesh *)transform[0]->findComponentInChildren(ComponentSkinnedMesh::Type);
                        ITK_ABORT(skinnedMesh == NULL, "Failed to query skinned mesh\n.");
                    }
                    cameraLook = camera_look;
                    if (skinnedMesh != NULL && cameraLook != NULL)
                    {
                        cameraLook_bkp_relative_skinnerMesh = skinnedMesh->transform[0]->worldToLocalMatrix() * MathCore::CVT<MathCore::vec3f>::toPtn4(cameraLook->getPosition());
                        cameraLook_bkp_relative_skinnerMesh.w = 1.0f; // point
                    }
                }

                void start()
                {
                    // AppBase* app = Engine::Instance()->app;
                    renderWindowRegion = transform[0]->renderWindowRegion;
                    renderWindowRegion->OnUpdate.add(&ComponentThirdPersonPlayerController::OnUpdate, this);

                    animationMotion = (ComponentAnimationMotion *)transform[0]->findComponent(ComponentAnimationMotion::Type);
                    ITK_ABORT(animationMotion == NULL, "Failed to query animationMotion\n.");

                    // animationMotion->OnMove.add(this, &ComponentThirdPersonPlayerController::OnMoveAnimation);

                    setCameraLook(cameraLook);
                }

                ~ComponentThirdPersonPlayerController()
                {
                    // AppBase* app = Engine::Instance()->app;
                    if (renderWindowRegion != NULL)
                    {
                        renderWindowRegion->OnUpdate.remove(&ComponentThirdPersonPlayerController::OnUpdate, this);
                    }

                    if (animationMotion != NULL)
                    {
                        animationMotion = NULL;
                    }

                    if (skinnedMesh != NULL)
                    {
                        skinnedMesh = NULL;
                    }
                }

                void OnMoveAnimation(float right, float up, float forward, const MathCore::vec3f &final_local_position)
                {

                    transform[0]->LocalPosition = final_local_position;

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
                        switch (rotateTargetNode)
                        {
                        case ThirdPersonRotateSkinnedMesh:
                            if (right.pressed)
                                skinnedMesh->transform[0]->LocalRotation = skinnedMesh->transform[0]->LocalRotation * MathCore::GEN<MathCore::quatf>::fromEuler(0, -MathCore::OP<float>::deg_2_rad(turn_speed) * time->deltaTime, 0);
                            else if (left.pressed)
                                skinnedMesh->transform[0]->LocalRotation = skinnedMesh->transform[0]->LocalRotation * MathCore::GEN<MathCore::quatf>::fromEuler(0, MathCore::OP<float>::deg_2_rad(turn_speed) * time->deltaTime, 0);

                            // transform cameraLook node
                            if (cameraLook != NULL)
                            {
                                cameraLook->Position =
                                    MathCore::CVT<MathCore::vec4f>::toVec3(
                                        skinnedMesh->transform[0]->localToWorldMatrix() * cameraLook_bkp_relative_skinnerMesh);

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
                                transform[0]->LocalRotation = transform[0]->LocalRotation * MathCore::GEN<MathCore::quatf>::fromEuler(0, -MathCore::OP<float>::deg_2_rad(turn_speed) * time->deltaTime, 0);
                            else if (left.pressed)
                                transform[0]->LocalRotation = transform[0]->LocalRotation * MathCore::GEN<MathCore::quatf>::fromEuler(0, MathCore::OP<float>::deg_2_rad(turn_speed) * time->deltaTime, 0);
                            break;
                        }
                    }

                    /*
                    if (walk.pressed) {
                        transform[0]->LocalPosition = transform[0]->LocalPosition + transform[0]->LocalRotation * MathCore::vec3f(0, 0, time->deltaTime * 5.0f);
                    }
                    */
                }
            };

        }

    }

}