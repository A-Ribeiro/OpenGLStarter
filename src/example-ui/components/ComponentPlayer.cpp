#include "ComponentPlayer.h"
#include "../App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <InteractiveToolkit/CollisionCore/CollisionCore.h>
// #include <appkit-gl-engine/Components/2d/ComponentRectangle.h>
#include <appkit-gl-engine/Components/Core/ComponentLineMounter.h>

#include "ComponentGameArea.h"

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {
            const ComponentType ComponentPlayer::Type = "ComponentPlayer";

            CollisionCore::Sphere<MathCore::vec3f> ComponentPlayer::getSphere()
            {
                auto transform = getTransform();
                auto position = transform->getLocalPosition();
                return CollisionCore::Sphere<MathCore::vec3f>(position, Radius.c_val());
            }

            ComponentPlayer::ComponentPlayer() : Component(ComponentPlayer::Type), inputState(RawInputFromDevice::Keyboard)
            {

                debugDrawEnabled = false;
                debugDrawColor = ui::colorFromHex("#FF0000FF");
                debugDrawThickness = 3.0f;

                app = nullptr;

                Radius.OnChange.add(&ComponentPlayer::OnRadiusParameter, this);

                jumpState.configureJump(
                    1000.0f, // risingVelocity
                    150.0f,  // minJumpHeight
                    300.0f,  // maxJumpHeight
                    200.0f,  // secondJumpHeight
                    -5000.0f // gravity
                );
            }

            ComponentPlayer::~ComponentPlayer()
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentPlayer::OnUpdate, this);
            }

            void ComponentPlayer::start()
            {
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

                        float radius_to_draw = inner_radius * 20.0f;
                        line_mounter->addLine(
                            MathCore::vec3f(-radius_to_draw, self_ref->jumpState.getMinJumpHeight() - inner_radius,0), // a
                            MathCore::vec3f(radius_to_draw, self_ref->jumpState.getMinJumpHeight() - inner_radius, 0), // b
                            self_ref->debugDrawThickness, // thickness
                            ui::colorFromHex("#FF0000FF") // color
                        );

                        line_mounter->addLine(
                            MathCore::vec3f(-radius_to_draw, self_ref->jumpState.getMaxJumpHeight() - inner_radius,0), // a
                            MathCore::vec3f(radius_to_draw, self_ref->jumpState.getMaxJumpHeight() - inner_radius, 0), // b
                            self_ref->debugDrawThickness, // thickness
                            ui::colorFromHex("#FF0000FF") // color
                        );

                        line_mounter->addLine(
                            MathCore::vec3f(-radius_to_draw, self_ref->jumpState.getSecondJumpHeight() - inner_radius,0), // a
                            MathCore::vec3f(radius_to_draw, self_ref->jumpState.getSecondJumpHeight() - inner_radius, 0), // b
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

                MathCore::vec3f position = getTransform()->getLocalPosition();

                const MathCore::vec3f gravity_px_per_sec = MathCore::vec3f(0, -5000.0f, 0.0f);

                acceleration = MathCore::vec3f(0.0f, 0.0f, 0.0f); // reset acceleration
                acceleration += gravity_px_per_sec;

                // avoid affect the velocity, and give the jumpstate the control of the y velocity
                // if (jumpState.isJumping())
                //     acceleration.y = 0;

                // vec3 traveled_position_aux = HeightAndTime::IntegrateAcceleration(velocity, acceleration, time->deltaTime);
                velocity += acceleration * time->deltaTime;

                const float max_velocity = 5000.0f;
                velocity = MathCore::OP<MathCore::vec3f>::quadraticClamp(MathCore::vec3f(0, 0, 0), velocity, max_velocity);

                // check is grounded before update velocity for jump
                // stage limits
                auto gameAreaShared = gameArea.lock();
                if (gameAreaShared == nullptr)
                    return;

                jumpState.update(&velocity.y,             // velocityY
                                 time->deltaTime,         // deltaTime
                                 gravity_px_per_sec.y,    // gravity
                                 inputState.jump.pressed, // jump_pressed
                                 true);                   // allow_double_jump

                float x = inputState.x_axis;

                bool stop_player = true;

                if (abs(x) > 0.02f)
                {
                    velocity.x = 600.0f * x;
                    stop_player = false;
                }

                if (stop_player)
                    velocity.x = 0;

                position += velocity * time->deltaTime;

                {
                    auto stageArea = gameAreaShared->StageArea.c_val();

                    auto sphere = CollisionCore::Sphere<MathCore::vec3f>(position, Radius.c_val());
                    auto lower_limit_plane = CollisionCore::Plane<MathCore::vec3f>(
                        stageArea.min_box,       // point
                        MathCore::vec3f(0, 1, 0) // normal
                    );

                    MathCore::vec3f penetration;
                    if (CollisionCore::Plane<MathCore::vec3f>::sphereIntersectsPlane(sphere, lower_limit_plane, &penetration))
                    {
                        jumpState.setGrounded();

                        position -= penetration;

                        auto penetration_dir = MathCore::OP<MathCore::vec3f>::normalize(penetration);
                        auto parallel_penetration = MathCore::OP<MathCore::vec3f>::parallelComponent(
                            velocity,
                            penetration_dir);

                        // remove velocity component in the plane direction from velocity vector
                        velocity -= parallel_penetration;
                    }
                }

                // force 2d logic
                position.z = 0.0f;
                velocity.z = 0;

                getTransform()->setLocalPosition(position);

                auto debugDrawTransform = getTransform()->findTransformByName("DebugDrawCircle");
                if (debugDrawTransform)
                {
                    auto mesh_line_drawer = debugDrawTransform->findComponent<ComponentMesh>();
                    MathCore::vec4f color;
                    auto state = jumpState.getState();
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
                    default:
                        color = ui::colorFromHex("#FFFFFFFF");
                        break;
                    }
                    for( size_t i=0 ; i<mesh_line_drawer->color[0].size() - 8*3; i++ )
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
                result->gameArea = this->gameArea;

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