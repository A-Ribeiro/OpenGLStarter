#include "ComponentPlayer.h"
#include "../App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <InteractiveToolkit/CollisionCore/CollisionCore.h>
// #include <appkit-gl-engine/Components/2d/ComponentRectangle.h>
#include <appkit-gl-engine/Components/Core/ComponentLineMounter.h>
// #include "../simple-physics/PhysicsContainer.h"

// #include "ComponentGameArea.h"

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {
            const ComponentType ComponentPlayer::Type = "ComponentPlayer";

            // CollisionCore::Sphere<MathCore::vec3f> ComponentPlayer::getSphere()
            // {
            //     auto transform = getTransform();
            //     auto position = transform->getLocalPosition();
            //     return CollisionCore::Sphere<MathCore::vec3f>(position, Radius.c_val());
            // }

            ComponentPlayer::ComponentPlayer() : Component(ComponentPlayer::Type), inputState(RawInputFromDevice::Keyboard)
            {

                debugDrawEnabled = false;
                debugDrawColor = ui::colorFromHex("#FF0000FF");
                debugDrawThickness = 3.0f;

                app = nullptr;

                Radius.OnChange.add(&ComponentPlayer::OnRadiusParameter, this);

                // jumpState.configureJump(
                //     1000.0f, // risingVelocity
                //     150.0f,  // minJumpHeight
                //     300.0f,  // maxJumpHeight
                //     200.0f,  // secondJumpHeight
                //     -5000.0f // gravity
                // );
            }

            ComponentPlayer::~ComponentPlayer()
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentPlayer::OnUpdate, this);
            }

            void ComponentPlayer::start()
            {
                character2D = AppKit::Physics::Controller::Character2D::CreateShared();
                *character2D = AppKit::Physics::Controller::Character2D::fromStaticConfig(
                    Radius.c_val(),
                    RadiusGrounded.c_val(),
                    OffsetGrounded.c_val(),
                    1000.0f,                         // risingVelocity
                    150.0f,                          // minJumpHeight
                    300.0f,                          // maxJumpHeight
                    200.0f,                          // secondJumpHeight
                    MathCore::vec2f(0.0f, -5000.0f), // gravity
                    true                             // allow_double_jump
                );
                // app->gameScene->physicsContainer->jumpingControllerList.push_back(physicsController);
                character2D->teleport(MathCore::CVT<MathCore::vec3f>::toVec2(getTransform()->getLocalPosition()));

                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.add(&ComponentPlayer::OnUpdate, this);

                if (debugDrawEnabled)
                {
                    auto transform = getTransform();
                    if (transform->findTransformByName("DebugDrawCircle", 1) != nullptr)
                        return; // already created
                    auto self_ref = this->self<ComponentPlayer>();
                    app->executeOnMainThread.enqueue([self_ref, transform]()
                                                     {
                        auto debugDrawTransform = transform->addChild(Transform::CreateShared("DebugDrawCircle"));
                        //debugDrawTransform->setLocalPosition(MathCore::vec3f(0,0,-1.0f));

                        std::shared_ptr<ComponentLineMounter> line_mounter = debugDrawTransform->addNewComponent<ComponentLineMounter>();
                        line_mounter->setCamera(&self_ref->app->resourceMap, self_ref->app->gameScene->getCamera(), true);

                        float inner_radius = self_ref->Radius.c_val() - self_ref->debugDrawThickness * 0.5f;
                        line_mounter->addLine(
                            MathCore::vec3f(0,0,0), // a
                            MathCore::vec3f(inner_radius, 0, 0), // b
                            self_ref->debugDrawThickness, // thickness
                            self_ref->debugDrawColor // color
                        );
                        line_mounter->addCircle(
                            MathCore::vec3f(0,0,0), // pos
                            inner_radius,    // radius
                            self_ref->debugDrawThickness, // thickness
                            self_ref->debugDrawColor, // color
                            32, // segment_count
                            MathCore::quatf() // rotation
                        );


                        line_mounter->addCircle(
                            MathCore::vec3f(0, -self_ref->OffsetGrounded.c_val(),0), // pos
                            self_ref->RadiusGrounded - 2.0f,    // radius
                            2.0f, // thickness
                            ui::colorFromHex("#0008ff"), // color
                            32, // segment_count
                            MathCore::quatf() // rotation
                        );

                        float radius_to_draw = inner_radius * 20.0f;
                        line_mounter->addLine(
                            MathCore::vec3f(-radius_to_draw, self_ref->character2D->jumpState.getMinJumpHeight() - inner_radius,0), // a
                            MathCore::vec3f(radius_to_draw, self_ref->character2D->jumpState.getMinJumpHeight() - inner_radius, 0), // b
                            self_ref->debugDrawThickness, // thickness
                            ui::colorFromHex("#FF0000FF") // color
                        );

                        line_mounter->addLine(
                            MathCore::vec3f(-radius_to_draw, self_ref->character2D->jumpState.getMaxJumpHeight() - inner_radius,0), // a
                            MathCore::vec3f(radius_to_draw, self_ref->character2D->jumpState.getMaxJumpHeight() - inner_radius, 0), // b
                            self_ref->debugDrawThickness, // thickness
                            ui::colorFromHex("#FF0000FF") // color
                        );

                        line_mounter->addLine(
                            MathCore::vec3f(-radius_to_draw, self_ref->character2D->jumpState.getSecondJumpHeight() - inner_radius,0), // a
                            MathCore::vec3f(radius_to_draw, self_ref->character2D->jumpState.getSecondJumpHeight() - inner_radius, 0), // b
                            self_ref->debugDrawThickness * 0.5f, // thickness
                            ui::colorFromHex("#f700ffff") // color
                        );

                        self_ref->app->gameScene->printHierarchy(); });
                }
            }

            void ComponentPlayer::attachToTransform(std::shared_ptr<Transform> t)
            {
                eventHandlerSetRef = t->eventHandlerSet;
            }
            void ComponentPlayer::detachFromTransform(std::shared_ptr<Transform> t)
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentPlayer::OnUpdate, this);
                eventHandlerSetRef.reset();
            }

            void ComponentPlayer::OnRadiusParameter(const float &value, const float &oldValue)
            {
            }

            void ComponentPlayer::OnUpdate(Platform::Time *time)
            {
                inputState.fillState();

                // MathCore::vec3f position = getTransform()->getLocalPosition();

                // const MathCore::vec3f gravity_px_per_sec = MathCore::vec3f(0, -5000.0f, 0.0f);

                // acceleration = MathCore::vec3f(0.0f, 0.0f, 0.0f); // reset acceleration
                // acceleration += gravity_px_per_sec;

                // // avoid affect the velocity, and give the jumpstate the control of the y velocity
                // // if (jumpState.isJumping())
                // //     acceleration.y = 0;

                // // vec3 traveled_position_aux = HeightAndTime::IntegrateAcceleration(velocity, acceleration, time->deltaTime);
                // velocity += acceleration * time->deltaTime;

                // const float & max_velocity = app->gameScene->physicsContainer->max_velocity;
                // //velocity = MathCore::OP<MathCore::vec3f>::quadraticClamp(MathCore::vec3f(0, 0, 0), velocity, max_velocity);
                // velocity.y = MathCore::OP<float>::clamp(velocity.y, -max_velocity, max_velocity);

                // jumpState.update(&velocity.y,             // velocityY
                //                  time->deltaTime,         // deltaTime
                //                  gravity_px_per_sec.y,    // gravity
                //                  inputState.jump.pressed, // jump_pressed
                //                  true);                   // allow_double_jump

                // float x = inputState.x_axis;

                // bool stop_player = true;

                // if (abs(x) > 0.02f)
                // {
                //     velocity.x = 600.0f * x;
                //     stop_player = false;
                // }

                // if (stop_player)
                // {
                //     using namespace MathCore;
                //     vec3f cancel_normal = vec3f(1, 0, 0);
                //     float vel_into_surface = OP<vec3f>::dot(velocity, cancel_normal);
                //     velocity -= cancel_normal * vel_into_surface;
                // }

                // position += velocity * time->deltaTime;

                // // force 2d logic
                // position.z = 0.0f;
                // velocity.z = 0;

                // app->gameScene->physicsContainer->movePlayer(
                //     getTransform()->getLocalPosition(),
                //     Radius.c_val(),
                //     RadiusGrounded.c_val(),
                //     OffsetGrounded.c_val(),
                //     &position, &velocity,
                //     time->deltaTime,
                //     // onGrounded callback
                //     [this]()
                //     {
                //         if (jumpState.getState() == JumpState::Falling)
                //             jumpState.setGrounded();
                //     });

                // getTransform()->setLocalPosition(position);

                AppKit::Physics::Container::ThreadState2D thread_state2d;
                character2D->update(app->gameScene->container2D.get(),
                                    thread_state2d,
                                    time,
                                    inputState.x_axis,
                                    inputState.jump.pressed,
                                    app->gameScene->container2D->max_velocity);

                getTransform()->setLocalPosition(MathCore::vec3f(character2D->position, 0.0f));

                auto debugDrawTransform = getTransform()->findTransformByName("DebugDrawCircle");
                if (debugDrawTransform)
                {
                    using namespace AppKit::Physics::VelocityHelpers;

                    auto mesh_line_drawer = debugDrawTransform->findComponent<ComponentMesh>();
                    MathCore::vec4f color;
                    auto state = character2D->jumpState.getState();
                    switch (state)
                    {
                    case JumpState::Grounded:
                        color = ui::colorFromHex("#ff8800ff");
                        break;
                    case JumpState::StartJump:
                    case JumpState::Rising:
                        color = ui::colorFromHex("#FFFF00FF");
                        break;
                    case JumpState::RisingBeforeNoUpImpulsion:
                    case JumpState::RisingNoUpImpulsion:
                        color = ui::colorFromHex("#0091ffff");
                        break;
                    case JumpState::Falling:
                        color = ui::colorFromHex("#000000ff");
                        break;
                    case JumpState::SetVelocityZeroBeforeFalling:
                        color = ui::colorFromHex("#ff0000ff");
                        break;
                    default:
                        color = ui::colorFromHex("#FFFFFFFF");
                        break;
                    }
                    for (size_t i = 0; i < mesh_line_drawer->color[0].size() - 8 * (3 + 32); i++)
                        mesh_line_drawer->color[0][i] = color;
                }
            }

            // always clone
            std::shared_ptr<Component> ComponentPlayer::duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
            {
                auto result = Component::CreateShared<ComponentPlayer>();

                result->eventHandlerSetRef = this->eventHandlerSetRef;
                result->debugDrawEnabled = this->debugDrawEnabled;
                result->debugDrawThickness = this->debugDrawThickness;
                result->debugDrawColor = this->debugDrawColor;
                result->app = this->app;
                result->Radius.setValueNoCallback(this->Radius.c_val());
                // result->gameArea = this->gameArea;

                return result;
            }

            void ComponentPlayer::fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentPlayer::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }

            void ComponentPlayer::Deserialize(rapidjson::Value &_value,
                                              std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                              std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                              ResourceSet &resourceSet)
            {
            }

        }
    }
}