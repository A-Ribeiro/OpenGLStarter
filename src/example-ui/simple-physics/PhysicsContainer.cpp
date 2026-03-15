#include "PhysicsContainer.h"
#include <appkit-gl-engine/Components/Core/ComponentLineMounter.h>
#include "../ui/common.h"

using namespace MathCore;
using namespace AppKit::GLEngine::Components;

#define DEBUG_DRAW 0

namespace Debug
{
    extern ComponentLineMounter *lineMounter;
}

namespace SimplePhysics
{

    void PhysicsContainer::buildStaticQuadtree(int32_t maxDepth_, int32_t minPointThresholdToSubdivide_)
    {
        static_quadtree = STL_Tools::make_unique<Quadtree<Structure2D::QuadtreeIntegration>>(&static_structures, maxDepth_, minPointThresholdToSubdivide_, game_area);
    }
    void PhysicsContainer::buildDynamicQuadtree(int32_t maxDepth_, int32_t minPointThresholdToSubdivide_)
    {
        dynamic_quadtree = STL_Tools::make_unique<Quadtree<Structure2D::QuadtreeIntegration>>(&dynamic_structures, maxDepth_, minPointThresholdToSubdivide_, game_area);
    }

    void PhysicsContainer::clearStatic()
    {
        static_structures.clear();
        static_quadtree.reset();
    }

    void PhysicsContainer::clearDynamic()
    {
        dynamic_structures.clear();
        dynamic_quadtree.reset();
    }

    void PhysicsContainer::setGameArea(const Box2D &box)
    {
        game_area = box;
        game_area_inequality_eq[GameAreaSide_Top] = Line2D::FromPointNormal(box.max, MathCore::vec2f(0, -1));
        game_area_inequality_eq[GameAreaSide_Bottom] = Line2D::FromPointNormal(box.min, MathCore::vec2f(0, 1));
        game_area_inequality_eq[GameAreaSide_Left] = Line2D::FromPointNormal(box.min, MathCore::vec2f(1, 0));
        game_area_inequality_eq[GameAreaSide_Right] = Line2D::FromPointNormal(box.max, MathCore::vec2f(-1, 0));
    }

    Box2D PhysicsContainer::computeStaticStructureBox() const
    {
        Box2D box;
        box.makeEmpty();
        for (const auto &structure : static_structures)
        {
            box.wrapBox(structure.box);
        }
        return box;
    }

    void PhysicsContainer::groundCheck(
        const std::vector<uint32_t> &static_ids,
        bool *ref_on_ground_called,
        const MathCore::vec2f &position,
        float radius_grounded,
        const EventCore::Callback<void(const Segment2D *on_segment)> &onGrounded)
    {
        using namespace MathCore;

        if (!*ref_on_ground_called)
        {
            for (uint32_t idx : static_ids)
            {
                const auto &structure = static_structures[idx];
                for (const auto &segment : structure.segments)
                {
                    if (Segment2D::circleIntersectsSegment(
                            position, radius_grounded,
                            segment.a, segment.b))
                    {
                        // emmit event grounded
                        if (!*ref_on_ground_called)
                        {
                            onGrounded(&segment);
                            *ref_on_ground_called = true;
                        }
                        return;
                    }
                }
            }
        }
    }

    void PhysicsContainer::pushOutOfSegments1(
        // const std::vector<uint32_t> &static_ids,
        MathCore::vec2f *ref_b,
        // MathCore::vec2f *ref_vel,
        float radius
        // float radius_grounded,
        // float offset_grounded
        // bool *ref_on_ground_called,
        // const EventCore::Callback<void()> &onGrounded
    )
    {
        using namespace MathCore;
        Box2D b_box = Box2D().wrapCircle(*ref_b, radius);

        const auto &static_ids = static_quadtree->query_box(b_box.min, b_box.max);

        // push out of all overlapping segments
        vec2f move_out_collision = vec2f(0);
        for (uint32_t idx : static_ids)
        {
            const auto &structure = static_structures[idx];
            for (const auto &segment : structure.segments)
            {
                Box2D segment_box = Box2D(segment.a, segment.b);
                if (!Box2D::overlaps(b_box.min, b_box.max, segment_box.min, segment_box.max))
                    continue;
                vec2f penetration;
                if (Segment2D::circleIntersectsSegment(
                        *ref_b, radius,
                        segment.a, segment.b,
                        &penetration))
                {
                    move_out_collision -= penetration;
                }
            }
        }

        if (OP<vec2f>::sqrLength(move_out_collision) > 1e-12f)
        {
            *ref_b += move_out_collision;

            // vec2f ground_center = *ref_b + vec2f(0, -offset_grounded);
            // // remove vel component in the direction of the push-out
            // vec2f segment_normal = OP<vec2f>::normalize(move_out_collision);
            // float vel_normal_component = OP<vec2f>::dot(*ref_vel, segment_normal);
            // *ref_vel -= segment_normal * vel_normal_component;

            // // ground check after push-out
            // groundCheck(
            //     static_ids,
            //     ref_on_ground_called,
            //     *ref_b + vec2f(0, -offset_grounded),
            //     radius_grounded,
            //     onGrounded);

            // break;
        }
    }

    bool PhysicsContainer::pushOutOfSegments(
        MathCore::vec2f point,
        float radius,
        MathCore::vec2f *output,
        MathCore::vec2f *offset,
        MathCore::vec2f *push_normal)
    {
        using namespace MathCore;
        Box2D b_box = Box2D().wrapCircle(point, radius);

        const auto &static_ids = static_quadtree->query_box(b_box.min, b_box.max);

        // push out of all overlapping segments
        vec2f move_out_collision = vec2f(0);
        for (uint32_t idx : static_ids)
        {
            const auto &structure = static_structures[idx];
            for (const auto &segment : structure.segments)
            {
                Box2D segment_box = Box2D(segment.a, segment.b);
                if (!Box2D::overlaps(b_box.min, b_box.max, segment_box.min, segment_box.max))
                    continue;
                vec2f penetration;
                if (Segment2D::circleIntersectsSegment(
                        point, radius,
                        segment.a, segment.b,
                        &penetration))
                {
                    move_out_collision -= penetration;
                }
            }
        }

        if (OP<vec2f>::sqrLength(move_out_collision) > 1e-12f)
        {
            *output = point + move_out_collision;
            *offset = move_out_collision;
            *push_normal = OP<vec2f>::normalize(move_out_collision);
            return true;
        }

        return false;
    }

    void PhysicsContainer::movePlayer(
        const MathCore::vec2f &position,
        float radius, float radius_grounded, float offset_grounded,
        MathCore::vec2f *out_position,
        MathCore::vec2f *out_velocity,
        float delta_time,
        const EventCore::Callback<void(const Segment2D *on_segment)> &onGrounded)
    {
        using namespace MathCore;

        vec2f a = position;
        vec2f b = *out_position;
        vec2f vel = *out_velocity;

        {
            vec2f push_offset, push_normal;
            if (pushOutOfSegments(a, radius, &a, &push_offset, &push_normal))
            {
                // need recompute velocity and b offset
                b += push_offset;
                // cancel velocity into the push-out surface
                float vel_into_surface = OP<vec2f>::dot(vel, push_normal);
                if (vel_into_surface < 0.0f)
                    vel -= push_normal * vel_into_surface;

                // force output of the correct position and velocity after push-out
                *out_position = b;
                *out_velocity = vel;
            }
        }

        vec2f a_copy = a;

        float length_vel = OP<vec2f>::length(vel);

        vec2f ab = b - a;
        float ab_mag = OP<vec2f>::length(ab);

        if (ab_mag == 0.0f || delta_time == 0.0f || length_vel == 0.0f)
        {
            *out_position = a;
            return;
        }

#if DEBUG_DRAW != 0

        // for (int i = 0; i <= 10; i++)
        // {
        //     float lrp = (float)i / 10.0f;
        //     Debug::lineMounter->addCircle(
        //         vec3f(OP<vec2f>::lerp(a, b, lrp), 0.0f),
        //         radius,
        //         5.0f,
        //         ui::colorFromHex("#ff00004c"));
        // }

        Debug::lineMounter->addCircle(
            vec3f(a, 0.0f),
            radius,
            2.0f,
            ui::colorFromHex("#ff00004c"));
        Debug::lineMounter->addLine(
            vec3f(a, -1.0f),
            vec3f(b, -1.0f),
            5.0f,
            ui::colorFromHex("#ff00004c"));

#endif

        float delta_time_inv = 1.0f / delta_time;

        vec2f penetration;
        float radius_sq = radius * radius;

        float query_radius = OP<float>::abs(offset_grounded) + radius_grounded + 1.0f;

        const Segment2D *segment_collision_to_ignore = nullptr;

        vec2f remaining_dir_norm = ab * (1.0f / ab_mag);
        float remaining_move_mag = ab_mag;

        bool on_ground_called = false;

        int max_iterations_without_move = 3;
        while (remaining_move_mag > EPSILON<float>::low_precision)
        {
            Box2D move_box = Box2D(a, b).expand(radius);

            const Segment2D *segment_collision = nullptr;
            float move_t = 1.0f;

            vec2f new_remaining_dir_norm = vec2f(0.0f);

            const auto &static_ids = static_quadtree->query_segment_radius(a, b, query_radius);
            // const auto &static_ids = static_quadtree->query_box(move_box.min, move_box.max);
            for (uint32_t idx : static_ids)
            {
                const auto &structure = static_structures[idx];
                for (const auto &segment : structure.segments)
                {
                    if (segment_collision_to_ignore == &segment)
                        continue;

                    Box2D segment_box = Box2D(segment.a, segment.b);
                    if (!Box2D::overlaps(move_box.min, move_box.max, segment_box.min, segment_box.max))
                        continue;

                    MathCore::vec2f p_ab, p_segment;
                    p_ab = Segment2D::closestPointSegmentToSegment(a, b, segment.a, segment.b, &p_segment);

                    float dist_sq = OP<vec2f>::sqrDistance(p_ab, p_segment);
                    if (dist_sq > radius_sq)
                        continue;

                    vec2f out_dir;
                    float t = Segment2D::circleCastIntersectsSegment(a, b, radius, segment.a, segment.b, &out_dir);
                    if (t < move_t)
                    {
                        move_t = t;
                        segment_collision = &segment;
                        new_remaining_dir_norm = out_dir;
                    }
                }
            }

            if (move_t == 1.0f)
                break;

            // move b to the collision point
            segment_collision_to_ignore = segment_collision;
            float remaining_actual_moved = remaining_move_mag * move_t;
            b = a + remaining_dir_norm * remaining_actual_moved;
            remaining_move_mag -= remaining_actual_moved;

            // ground check
            groundCheck(
                static_ids,
                &on_ground_called,
                b + vec2f(0, -offset_grounded),
                radius_grounded,
                onGrounded);

            // Redirect remaining movement along the collision tangent
            if (move_t == 0.0f)
            {
                if (max_iterations_without_move-- <= 0)
                    break;
            }
            else
                max_iterations_without_move = 3;
            if (remaining_move_mag > EPSILON<float>::low_precision) // test to avoid b to have move after the logic
            {
                float remaining_move_cos = OP<vec2f>::dot(remaining_dir_norm, new_remaining_dir_norm);
                remaining_move_cos = OP<float>::clamp(remaining_move_cos, -1.0f, 1.0f);
                if (remaining_move_cos < 1e-3f) // if the remaining move is almost perpendicular to the collision surface, just stop
                {
                    // if (remaining_move_cos < 0.0f)
                    //     printf("################### Error scabroso no slide negativo...\n");
                    break;
                }

                remaining_move_mag *= remaining_move_cos;

                a = b;
                b = a + new_remaining_dir_norm * remaining_move_mag;
                // vel = new_remaining_dir_norm * length_vel;
                remaining_dir_norm = new_remaining_dir_norm;

#if DEBUG_DRAW != 0

                // for (int i = 0; i <= 10; i++)
                // {
                //     float lrp = (float)i / 10.0f;
                //     Debug::lineMounter->addCircle(
                //         vec3f(OP<vec2f>::lerp(a, b, lrp), 0.0f),
                //         radius,
                //         5.0f,
                //         ui::colorFromHex("#00ffff4c"));
                // }
                Debug::lineMounter->addLine(
                    vec3f(a, -1.0f),
                    vec3f(b, -1.0f),
                    5.0f,
                    ui::colorFromHex("#00ffff4c"));

                auto pt = Segment2D::closestPointToSegment(
                    a,
                    segment_collision->a, segment_collision->b);

                Debug::lineMounter->addLine(
                    vec3f(pt, -1.0f),
                    vec3f(pt + new_remaining_dir_norm * remaining_move_mag, -1.0f),
                    2.0f,
                    ui::colorFromHex("#ffff004c"));

                // Debug::lineMounter->addLine(
                //     vec3f(a, -1.0f),
                //     vec3f(b, -1.0f),
                //     1.0f,
                //     ui::colorFromHex("#00ffff4c"));

#endif
            }
        }

        // // // Safety: push b out of any remaining penetrating segments
        // {
        //     vec2f push_offset, push_normal;
        //     if (pushOutOfSegments(b, radius, &b, &push_offset, &push_normal))
        //     {
        //         // cancel velocity component going into the surface
        //         float vel_into_surface = OP<vec2f>::dot(vel, push_normal);
        //         if (vel_into_surface < 0.0f)
        //             vel -= push_normal * vel_into_surface;
        //     }
        // }

#if DEBUG_DRAW != 0

        // Debug::lineMounter->addCircle(
        //     vec3f(b, -2.0f),
        //     radius,
        //     5.0f,
        //     ui::colorFromHex("#ffff004c"));

        Debug::lineMounter->addLine(
            vec3f(a_copy, -1.0f),
            vec3f(b, -1.0f),
            5.0f,
            ui::colorFromHex("#ffff004c"));
#endif

        vel = (b - a_copy) * delta_time_inv;

        *out_position = b;
        // *out_velocity = vec3f(vel, out_velocity->z);
        *out_velocity = OP<vec2f>::quadraticClamp(vec2f(0, 0), vel, max_velocity);
    }

    JumpingController::JumpingController()
    {
        has_last_collision_segment = false;
    }

    JumpingController JumpingController::fromStaticConfig(
        float radius, float radius_grounded, float offset_grounded,
        float jump_risingVelocity, float jump_minJumpHeight, float jump_maxJumpHeight, float jump_secondJumpHeight,
        const MathCore::vec2f &gravity,
        bool allow_double_jump)
    {
        using namespace MathCore;
        JumpingController controller;

        controller.radius = radius;
        controller.radius_grounded = radius_grounded;
        controller.offset_grounded = offset_grounded;
        controller.allow_double_jump = allow_double_jump;

        controller.gravity = gravity;
        controller.gravity_mag = OP<vec2f>::length(gravity);
        controller.gravity_dir = gravity * (1.0f / controller.gravity_mag);

        controller.jumpState.configureJump(
            jump_risingVelocity, jump_minJumpHeight, jump_maxJumpHeight, jump_secondJumpHeight, -controller.gravity_mag);

        return controller;
    }

    void JumpingController::update(PhysicsContainer *physicsContainer, Platform::Time *time, float input_x_axis, bool jump_pressed, float max_velocity)
    {
        using namespace MathCore;

        // reset acceleration
        acceleration = vec2f(0.0f, 0.0f);
        acceleration += gravity;

        velocity += acceleration * time->deltaTime;
        velocity = OP<vec2f>::quadraticClamp(vec2f(0.0f, 0.0f), velocity, max_velocity);

        vec2f gravity_up = -gravity_dir;
        float velocity_gravity_y = OP<vec2f>::dot(velocity, gravity_up);

        jumpState.update(&velocity_gravity_y, // velocityY
                         time->deltaTime,     // deltaTime
                         -gravity_mag,        // gravity
                         jump_pressed,        // jump_pressed
                         allow_double_jump);  // allow_double_jump

        // apply back the jump velocity to the velocity vector
        // removing velocity in gravity direction and adding the jump velocity in the gravity direction
        velocity += gravity_up * (velocity_gravity_y - OP<vec2f>::dot(velocity, gravity_up));

        // x-axis movement logic

        const vec2f x_axis = vec2f(1, 0);
        bool stop_player = true;
        if (OP<float>::abs(input_x_axis) > 0.02f)
        {
            velocity += x_axis * (600.0f * input_x_axis - OP<vec2f>::dot(velocity, x_axis));
            stop_player = false;
        }
        else
        {
            vec2f cancel_normal = vec2f(1, 0);
            float vel_into_surface = OP<vec2f>::dot(velocity, cancel_normal);
            velocity -= cancel_normal * vel_into_surface;
        }

        if (jumpState.getState() == JumpState::Grounded)
        {
            vec2f segment = last_collision_segment.b - last_collision_segment.a;
            float segment_mag_2 = OP<vec2f>::sqrLength(segment);
            if (segment_mag_2 > 1e-8f)
            {
                vec2f segment_dir = segment * OP<float>::rsqrt(segment_mag_2);
                vec2f segment_normal = OP<vec2f>::cross_z_up(segment_dir);

                // remove normal component of the velocity
                float vel_into_surface = OP<vec2f>::dot(velocity, segment_normal);
                velocity -= segment_normal * vel_into_surface;
            }
        }
        else
            last_collision_segment = Segment2D();

        vec2f position_before = position;

        position += velocity * time->deltaTime;

        // set 0
        // last_collision_segment = Segment2D();

        physicsContainer->movePlayer(
            position_before,
            radius,
            radius_grounded,
            offset_grounded,
            &position, &velocity,
            time->deltaTime,
            // onGrounded callback
            [this](const Segment2D *on_segment)
            {
                last_collision_segment = *on_segment;
                if (jumpState.getState() == JumpState::Falling)
                    jumpState.setGrounded();
            });
    }

    // set the position and make reset velocity and acceleration, useful for teleporting the player
    void JumpingController::teleport(const MathCore::vec2f &position)
    {
        this->position = position;
        velocity = vec2f(0);
        acceleration = vec2f(0);
    }

}