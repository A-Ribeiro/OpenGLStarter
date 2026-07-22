#include <appkit-physics/controller/Character2D.h>
#include <appkit-physics/container/Container2D.h>

using namespace MathCore;

#define DEBUG_DRAW 0

namespace AppKit
{
    namespace Physics
    {

        namespace Controller
        {

            Character2D::Character2D()
            {
                has_last_collision_segment = false;
            }

            void Character2D::setGravity(const MathCore::vec2f &gravity)
            {
                if (gravityStack.size() == 0)
                    gravityStack.push_back(GravityDescriptor{});

                auto &gravityInfo = gravityStack.back();

                gravityInfo.gravity = gravity;
                gravityInfo.gravity_mag = OP<vec2f>::length(gravity);
                gravityInfo.gravity_dir = gravity * (1.0f / gravityInfo.gravity_mag);
            }

            void Character2D::pushGravity(const MathCore::vec2f &gravity)
            {
                gravityStack.push_back(GravityDescriptor{});
                setGravity(gravity);
            }
            void Character2D::popGravity()
            {
                if (gravityStack.size() > 1)
                    gravityStack.pop_back();
            }

            void Character2D::setStaticConfig(
                float radius, float radius_grounded, float offset_grounded,
                float jump_risingVelocity, float jump_minJumpHeight, float jump_maxJumpHeight, float jump_secondJumpHeight,
                const MathCore::vec2f &gravity,
                float skin_width,
                float offset_above_activation_line,
                float offset_below_deactivation_line)
            {
                using namespace MathCore;
                // std::shared_ptr<Character2D> controller = Character2D::CreateShared();

                this->radius = radius;
                this->radius_grounded = radius_grounded;
                this->offset_grounded = offset_grounded;

                setGravity(gravity);
                // this->gravity = gravity;
                // this->gravity_mag = OP<vec2f>::length(gravity);
                // this->gravity_dir = gravity * (1.0f / this->gravity_mag);

                const auto &gravityInfo = gravityStack.back();

                this->jumpState.configureJump(
                    jump_risingVelocity, jump_minJumpHeight, jump_maxJumpHeight, jump_secondJumpHeight, -gravityInfo.gravity_mag);

                this->skin_width = skin_width;
                this->offset_above_activation_line = offset_above_activation_line;
                this->offset_below_deactivation_line = offset_below_deactivation_line;
            }

            void Character2D::update(
                Container::Container2D *physicsContainer,
                Container::ThreadState2D &thread_state,
                Platform::Time *time,
                float input_x_axis,
                float x_axis_velocity,
                bool jump_pressed,
                float max_velocity,
                JumpBehavior jumpBehavior,
                bool dash_pressed, VelocityHelpers::DashState::State dash_to_apply,
                DashBehavior dashBehavior)
            {
                if (time->deltaTime == 0.0f)
                    return;
                // copy of the element,
                // to avoid problem if any event change it in this method
                auto gravityInfo = gravityStack.back();

                using namespace MathCore;

                // integrate gravity to current velocity
                velocity += gravityInfo.gravity * time->deltaTime;
                velocity = OP<vec2f>::quadraticClamp(vec2f(0.0f, 0.0f), velocity, max_velocity);

                vec2f gravity_up = -gravityInfo.gravity_dir;

                // x-axis movement logic
                vec2f x_axis = OP<vec2f>::cross_z_down(gravity_up);
                vec2f ground_axis = x_axis;
                if (jumpState.getState() == VelocityHelpers::JumpState::Grounded)
                {
                    vec2f segment_point = last_collision_segment.closestPoint(position);
                    vec2f normal = position - segment_point;
                    float normal_mag_2 = OP<vec2f>::sqrLength(normal);
                    if (normal_mag_2 > 1e-8f)
                    {
                        normal *= OP<float>::rsqrt(normal_mag_2);
                        ground_axis = OP<vec2f>::cross_z_up(normal);
                        // ground axis pointing to the same direction x_axis points to
                        if (OP<vec2f>::dot(ground_axis, x_axis) < 0.0f)
                            ground_axis = -ground_axis;

                        // if (OP<vec2f>::dot(ground_axis, x_axis) < OP<float>::cos( OP<float>::deg_2_rad(45.0f) ) )
                        //     ground_axis = x_axis;
                    }

                    // if (OnDebugDrawLine)
                    // {
                    //     OnDebugDrawLine(
                    //         segment_point,
                    //         segment_point + ground_axis * 100.0f,
                    //         COLOR_CODE_GROUND_AXIS_ON_SEGMENT);
                    // }
                }

                // Dash Logic
                float velocity_dash_x = OP<vec2f>::dot(velocity, x_axis);
                dashState.updateVelocity(
                    &velocity_dash_x, // float *velocityX,
                    time->deltaTime,  // float deltaTime,
                    dash_pressed,     // bool dash_pressed_,
                    dash_to_apply     // DashState::State dash_to_apply
                );
                bool block_x_move_from_dash = (dashState.getState() != VelocityHelpers::DashState::State::None);

                // velocity += x_axis * (velocity_dash_x - OP<vec2f>::dot(velocity, x_axis));
                if (block_x_move_from_dash)
                {
                    velocity = x_axis * velocity_dash_x; // replace entire jump move
                    jumpState.setFalling(true);
                }
                else
                {
                    // Jump Logic
                    float velocity_gravity_y = OP<vec2f>::dot(velocity, gravity_up);
                    bool allow_double_jump = (jumpBehavior != JumpBehavior::SimpleJump);
                    bool double_jump_at_any_time = (jumpBehavior == JumpBehavior::DoubleJumpAnyTime);
                    jumpState.updateVelocity(&velocity_gravity_y,      // velocityY
                                             time->deltaTime,          // deltaTime
                                             -gravityInfo.gravity_mag, // gravity
                                             jump_pressed,             // jump_pressed
                                             allow_double_jump,        // allow_double_jump
                                             double_jump_at_any_time); // double_jump_at_any_time
                    // apply back the jump velocity to the velocity vector
                    // removing velocity in gravity direction and adding the jump velocity in the gravity direction
                    velocity += gravity_up * (velocity_gravity_y - OP<vec2f>::dot(velocity, gravity_up));

                    // X-Axis Logic
                    move_x_detector.setState(OP<float>::abs(input_x_axis) > 0.02f);
                    if (move_x_detector.pressed)
                    {

                        float move_direction = OP<float>::sign(input_x_axis);

                        // Debug::lineMounter->addLine(
                        //     vec3f(position, -1.0f),
                        //     vec3f(position + move_direction * ground_axis * 100, -1.0f),
                        //     5.0f,
                        //     ui::colorFromHex("#ff00004c"));

                        float speed_factor = 1.0f;
                        float up_down_detector = OP<vec2f>::dot(move_direction * ground_axis, gravity_up);
                        float slope_factor = OP<vec2f>::dot(ground_axis, x_axis);
                        if (OP<float>::abs(slope_factor) > EPSILON<float>::low_precision)
                        {
                            if (up_down_detector > EPSILON<float>::low_precision)
                            {
                                speed_factor = OP<vec2f>::dot(ground_axis, x_axis);
                                // printf("up hill speed: %f\n", speed_factor);
                            }
                            else if (up_down_detector < -EPSILON<float>::low_precision)
                            {
                                speed_factor = 1.0f / OP<vec2f>::dot(ground_axis, x_axis);
                                // printf("down hill speed: %f\n", speed_factor);
                            }
                        }

                        float desired_velocity = x_axis_velocity * input_x_axis * speed_factor;

                        float curr_velocity_on_ground = OP<vec2f>::dot(velocity, ground_axis);
                        velocity += ground_axis * (desired_velocity - curr_velocity_on_ground);
                    }
                    else
                    {
                        // When stopped, remove velocity in the horizontal plane (x_axis direction)
                        // instead of along ground_axis to prevent mini-jumps at platform edges
                        // This preserves vertical velocity for gravity/jump calculations
                        if (jumpState.getState() == VelocityHelpers::JumpState::Grounded)
                        {
                            float velocity_on_x_axis = OP<vec2f>::dot(velocity, x_axis);
                            velocity -= x_axis * velocity_on_x_axis;
                        }
                        // else
                        {
                            // When airborne, use ground_axis for consistency
                            float velocity_on_ground = OP<vec2f>::dot(velocity, ground_axis);
                            velocity -= ground_axis * velocity_on_ground;
                        }
                    }
                }

                // if (OnDebugDrawLine)
                // {
                //     OnDebugDrawLine(
                //         position,
                //         position + ground_axis * 100.0f,
                //         COLOR_CODE_GROUND_AXIS);
                // }
                // Debug::lineMounter->addLine(
                //     vec3f(position, 0.0f),
                //     vec3f(position + ground_axis * 100.0f, 0.0f),
                //     5.0f,
                //     ui::colorFromHex("#fff9bda7"));

                // const float x_axis_velocity = 600.0f;

                vec2f position_before = position;
                // static bool passed = false;
                // if (!passed)
                // {
                //     position_before = vec2f(1083.843994, 49.999996);
                //     position = position_before; // 1087.049561 150.038300
                //     velocity = vec2f(600.000000, 1000.000000);

                //     // p + v = 1087.049561 150.038300;
                //     vec2f v_diff = vec2f(1144.804810, 151.601395) - position_before;
                //     time->deltaTime = OP<vec2f>::length(v_diff) / OP<vec2f>::length(velocity);
                //     // passed = true;
                // }
                // time->deltaTime = 0.000234200008f;

                position += velocity * time->deltaTime;

                // printf("position_before: (%f, %f), position: (%f, %f), velocity: (%f, %f)\n",
                //     position_before.x, position_before.y,
                //     position.x, position.y,
                //     velocity.x, velocity.y);

                bool ground_touch = false;

                physicsContainer->moveObject(
                    position_before,
                    radius,
                    radius_grounded,
                    offset_grounded,
                    &position, &velocity,
                    time->deltaTime,
                    // onGrounded callback
                    [this, &ground_touch](const Core::Segment2D *on_segment)
                    {
                        // ground_touch = true;
                        ground_touch = true;
                        last_collision_segment = *on_segment;
                    },
                    // onMoveTouch callback
                    [this, &ground_touch](const MathCore::vec2f &pos, const Core::Segment2D *on_segment)
                    {
                        // MathCore::vec2f p_ground_check = pos + MathCore::vec2f(0, -offset_grounded);
                        // if (Segment2D::circleIntersectsSegment(
                        //         p_ground_check, this->radius_grounded,
                        //         on_segment->a, on_segment->b))
                        // {
                        //     ground_touch = true;
                        //     last_collision_segment = *on_segment;
                        // }
                    },
                    thread_state,
                    object_state,
                    &trigger_probes,
                    skin_width,
                    max_velocity,
                    offset_above_activation_line,
                    offset_below_deactivation_line);

                // passed = passed || (position != position_before);
                // printf("passed: %d\n", passed);

                // last_collision_segment = *on_segment;
                if (ground_touch)
                {
                    if (jumpState.getState() == VelocityHelpers::JumpState::Falling)
                        jumpState.setGrounded();
                }
                else
                {
                    last_collision_segment = Core::Segment2D();
                    if (jumpState.getState() == VelocityHelpers::JumpState::Grounded)
                        jumpState.setFalling();
                }
            }

            // set the position and make reset velocity, useful for teleporting the player
            void Character2D::teleport(const MathCore::vec2f &position)
            {
                this->position = position;
                velocity = vec2f(0);
            }
        }
    }
}