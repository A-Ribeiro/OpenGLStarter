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

            Character2D Character2D::fromStaticConfig(
                float radius, float radius_grounded, float offset_grounded,
                float jump_risingVelocity, float jump_minJumpHeight, float jump_maxJumpHeight, float jump_secondJumpHeight,
                const MathCore::vec2f &gravity,
                bool allow_double_jump,
                float skin_width)
            {
                using namespace MathCore;
                Character2D controller;

                controller.radius = radius;
                controller.radius_grounded = radius_grounded;
                controller.offset_grounded = offset_grounded;
                controller.allow_double_jump = allow_double_jump;

                controller.gravity = gravity;
                controller.gravity_mag = OP<vec2f>::length(gravity);
                controller.gravity_dir = gravity * (1.0f / controller.gravity_mag);

                controller.jumpState.configureJump(
                    jump_risingVelocity, jump_minJumpHeight, jump_maxJumpHeight, jump_secondJumpHeight, -controller.gravity_mag);
                
                controller.skin_width = skin_width;

                return controller;
            }

            void Character2D::update(
                Container::Container2D *physicsContainer,
                Container::ThreadState2D &thread_state,
                Platform::Time *time,
                float input_x_axis,
                float x_axis_velocity,
                bool jump_pressed,
                float max_velocity)
            {
                if (time->deltaTime == 0.0f)
                    return;

                using namespace MathCore;

                // reset acceleration
                acceleration = vec2f(0.0f, 0.0f);
                acceleration += gravity;

                velocity += acceleration * time->deltaTime;
                velocity = OP<vec2f>::quadraticClamp(vec2f(0.0f, 0.0f), velocity, max_velocity);

                vec2f gravity_up = -gravity_dir;
                float velocity_gravity_y = OP<vec2f>::dot(velocity, gravity_up);

                jumpState.updateVelocity(&velocity_gravity_y, // velocityY
                                         time->deltaTime,     // deltaTime
                                         -gravity_mag,        // gravity
                                         jump_pressed,        // jump_pressed
                                         allow_double_jump);  // allow_double_jump

                // apply back the jump velocity to the velocity vector
                // removing velocity in gravity direction and adding the jump velocity in the gravity direction
                velocity += gravity_up * (velocity_gravity_y - OP<vec2f>::dot(velocity, gravity_up));

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
                }

                // Debug::lineMounter->addLine(
                //     vec3f(position, 0.0f),
                //     vec3f(position + ground_axis * 100.0f, 0.0f),
                //     5.0f,
                //     ui::colorFromHex("#fff9bda7"));

                // const float x_axis_velocity = 600.0f;

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
                    float velocity_on_ground = OP<vec2f>::dot(velocity, ground_axis);
                    velocity -= ground_axis * velocity_on_ground;
                }

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
                    skin_width,
                    max_velocity);

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

            // set the position and make reset velocity and acceleration, useful for teleporting the player
            void Character2D::teleport(const MathCore::vec2f &position)
            {
                this->position = position;
                velocity = vec2f(0);
                acceleration = vec2f(0);
            }
        }
    }
}