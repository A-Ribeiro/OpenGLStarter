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

    Structure2D *PhysicsContainer::addStaticStructure(const Structure2D &structure)
    {
        uint32_t new_id = uuid.next();
        std::unique_ptr<Structure2D> structure_copy = STL_Tools::make_unique<Structure2D>(structure);
        structure_copy->id = new_id;

        Structure2D * result = structure_copy.get();

        static_structures.insert(
            std::upper_bound(static_structures.begin(), static_structures.end(), structure_copy,
                             [](const std::unique_ptr<Structure2D> &a, const std::unique_ptr<Structure2D> &b)
                             { return a->id < b->id; }),
            std::move(structure_copy));

        return result;
    }
    void PhysicsContainer::removeStaticStructure(uint32_t idx)
    {
        auto it = std::lower_bound(static_structures.begin(), static_structures.end(), idx,
                                   [](const std::unique_ptr<Structure2D> &structure, uint32_t id)
                                   { return structure->id < id; });
        if (it == static_structures.end() || (*it)->id != idx)
            return;

        static_structures.erase(it);

        for (auto &item : jumpingControllerList)
            item->object_state.pass_through_remove_id(idx);

        uuid.release(idx);
    }
    Structure2D *PhysicsContainer::getStaticStructure(uint32_t idx)
    {
        auto it = std::lower_bound(static_structures.begin(), static_structures.end(), idx,
                                   [](const std::unique_ptr<Structure2D> &structure, uint32_t id)
                                   { return structure->id < id; });
        if (it == static_structures.end() || (*it)->id != idx)
            return nullptr;
        return (*it).get();
    }

    Structure2D *PhysicsContainer::addDynamicStructure(const Structure2D &structure)
    {
        uint32_t new_id = uuid.next();
        std::unique_ptr<Structure2D> structure_copy = STL_Tools::make_unique<Structure2D>(structure);
        structure_copy->id = new_id;

        Structure2D * result = structure_copy.get();

        dynamic_structures.insert(
            std::upper_bound(dynamic_structures.begin(), dynamic_structures.end(), structure_copy,
                             [](const std::unique_ptr<Structure2D> &a, const std::unique_ptr<Structure2D> &b)
                             { return a->id < b->id; }),
            std::move(structure_copy));

        return result;
    }
    void PhysicsContainer::removeDynamicStructure(uint32_t idx)
    {
        auto it = std::lower_bound(dynamic_structures.begin(), dynamic_structures.end(), idx,
                                   [](const std::unique_ptr<Structure2D> &structure, uint32_t id)
                                   { return structure->id < id; });
        if (it == dynamic_structures.end() || (*it)->id != idx)
            return;

        dynamic_structures.erase(it);

        for (auto &item : jumpingControllerList)
            item->object_state.pass_through_remove_id(idx);

        uuid.release(idx);
    }
    Structure2D *PhysicsContainer::getDynamicStructure(uint32_t idx)
    {
        auto it = std::lower_bound(dynamic_structures.begin(), dynamic_structures.end(), idx,
                                   [](const std::unique_ptr<Structure2D> &structure, uint32_t id)
                                   { return structure->id < id; });
        if (it == dynamic_structures.end() || (*it)->id != idx)
            return nullptr;
        return (*it).get();
    }

    void PhysicsContainer::buildStaticQuadtree(int32_t maxDepth_, int32_t minPointThresholdToSubdivide_)
    {
        static_always_check.clear();

        for (uint32_t i = 0; i < static_structures.size(); ++i)
        {
            if (static_structures[i]->always_check)
                static_always_check.push_back(static_structures[i].get());
        }
        static_quadtree = STL_Tools::make_unique<Quadtree<Structure2D::QuadtreeIntegration>>(&static_structures, maxDepth_, minPointThresholdToSubdivide_, game_area);
    }
    void PhysicsContainer::buildDynamicQuadtree(int32_t maxDepth_, int32_t minPointThresholdToSubdivide_)
    {
        dynamic_always_check.clear();

        for (uint32_t i = 0; i < dynamic_structures.size(); ++i)
        {
            if (dynamic_structures[i]->always_check)
                dynamic_always_check.push_back(dynamic_structures[i].get());
        }
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
            box.wrapBox(structure->box);
        box.wrapBox(game_area);
        return box;
    }

    void PhysicsContainer::groundCheck(
        bool *ref_on_ground_called,
        const MathCore::vec2f &position,
        float radius_grounded,
        const EventCore::Callback<void(const Segment2D *on_segment)> &onGrounded,
        ThreadState &thread_state,
        ObjectState &object_state)
    {
        using namespace MathCore;

        if (!*ref_on_ground_called)
        {
            for (const Structure2D *structure : thread_state.structure_ptrs)
            {
                // skip pass-through structures when player is on the pass-through side
                if (structure->pass_through_set)
                {
                    if (!object_state.pass_through_is_active(structure->id))
                        continue;
                }
                for (const auto &segment : structure->segments)
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

    // void PhysicsContainer::pushOutOfSegments1(
    //     // const std::vector<uint32_t> &static_ids,
    //     MathCore::vec2f *ref_b,
    //     // MathCore::vec2f *ref_vel,
    //     float radius
    //     // float radius_grounded,
    //     // float offset_grounded
    //     // bool *ref_on_ground_called,
    //     // const EventCore::Callback<void()> &onGrounded
    // )
    // {
    //     using namespace MathCore;
    //     Box2D b_box = Box2D().wrapCircle(*ref_b, radius);

    //     const auto &static_ids = static_quadtree->query_box(b_box.min, b_box.max);

    //     // push out of all overlapping segments
    //     vec2f move_out_collision = vec2f(0);
    //     for (uint32_t idx : static_ids)
    //     {
    //         const auto &structure = static_structures[idx];
    //         for (const auto &segment : structure.segments)
    //         {
    //             Box2D segment_box = Box2D(segment.a, segment.b);
    //             if (!Box2D::overlaps(b_box.min, b_box.max, segment_box.min, segment_box.max))
    //                 continue;
    //             vec2f penetration;
    //             if (Segment2D::circleIntersectsSegment(
    //                     *ref_b, radius,
    //                     segment.a, segment.b,
    //                     &penetration))
    //             {
    //                 move_out_collision -= penetration;
    //             }
    //         }
    //     }

    //     if (OP<vec2f>::sqrLength(move_out_collision) > 1e-12f)
    //     {
    //         *ref_b += move_out_collision;

    //         // vec2f ground_center = *ref_b + vec2f(0, -offset_grounded);
    //         // // remove vel component in the direction of the push-out
    //         // vec2f segment_normal = OP<vec2f>::normalize(move_out_collision);
    //         // float vel_normal_component = OP<vec2f>::dot(*ref_vel, segment_normal);
    //         // *ref_vel -= segment_normal * vel_normal_component;

    //         // // ground check after push-out
    //         // groundCheck(
    //         //     static_ids,
    //         //     ref_on_ground_called,
    //         //     *ref_b + vec2f(0, -offset_grounded),
    //         //     radius_grounded,
    //         //     onGrounded);

    //         // break;
    //     }
    // }

    bool PhysicsContainer::pushOutOfSegments(
        MathCore::vec2f point,
        float radius,
        MathCore::vec2f *output,
        MathCore::vec2f *offset,
        MathCore::vec2f *push_normal,
        const MathCore::vec2f &velocity_hint,
        ThreadState &thread_state,
        ObjectState &object_state)
    {
        using namespace MathCore;
        Box2D b_box = Box2D().wrapCircle(point, radius);

        thread_state.query_box(static_quadtree.get(), static_structures, b_box.min, b_box.max, true);
        if (dynamic_quadtree)
            thread_state.query_box(dynamic_quadtree.get(), dynamic_structures, b_box.min, b_box.max, false);
        if (thread_state.structure_ptrs.empty())
        {
            // for (const auto *structure : always_check_structures)
            //     thread_state.quadtree_ids.push_back(structure->id);
            thread_state.structure_ptrs.assign(static_always_check.begin(), static_always_check.end());
            thread_state.structure_ptrs.insert(thread_state.structure_ptrs.end(), dynamic_always_check.begin(), dynamic_always_check.end());
        }

        // push out of all overlapping segments
        vec2f move_out_collision = vec2f(0);
        for (const Structure2D *structure : thread_state.structure_ptrs)
        {
            // skip pass-through structures when velocity aligns with pass-through direction
            if (structure->pass_through_set)
            {
                if (!object_state.pass_through_is_active(structure->id))
                    continue;
            }
            for (const auto &segment : structure->segments)
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
        const EventCore::Callback<void(const Segment2D *on_segment)> &onGrounded,
        const EventCore::Callback<void(const MathCore::vec2f &pos, const Segment2D *on_segment)> &onMoveTouch,
        ThreadState &thread_state,
        ObjectState &object_state)
    {
        using namespace MathCore;

        vec2f a = position;
        vec2f b = *out_position;
        vec2f vel = *out_velocity;

        {
            vec2f push_offset, push_normal;
            if (pushOutOfSegments(a, radius + 1e-2f, &a, &push_offset, &push_normal, vel, thread_state, object_state))
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

        bool on_ground_called = false;

        if (ab_mag == 0.0f || delta_time == 0.0f || length_vel == 0.0f)
        {
            *out_position = a;

            // do ground check
            vec2f ground_pos = a + vec2f(0, -offset_grounded);
            Box2D ground_box = Box2D().wrapCircle(ground_pos, radius_grounded);
            // const auto &static_ids = static_quadtree->query_box(ground_box.min, ground_box.max);
            thread_state.query_box(static_quadtree.get(), static_structures, ground_box.min, ground_box.max, true);
            if (dynamic_quadtree)
                thread_state.query_box(dynamic_quadtree.get(), dynamic_structures, ground_box.min, ground_box.max, false);
            
            if (thread_state.structure_ptrs.empty())
            {
                // for (const auto *structure : always_check_structures)
                //     thread_state.quadtree_ids.push_back(structure->id);
                
                thread_state.structure_ptrs.assign(static_always_check.begin(), static_always_check.end());
                thread_state.structure_ptrs.insert(thread_state.structure_ptrs.end(), dynamic_always_check.begin(), dynamic_always_check.end());
            }

            // ground check
            groundCheck(
                &on_ground_called,
                a + vec2f(0, -offset_grounded),
                radius_grounded,
                onGrounded,
                thread_state,
                object_state);

            // for (uint32_t idx : static_ids)
            // {
            //     const auto &structure = static_structures[idx];
            //     // skip pass-through structures when player is on the pass-through side
            //     if (structure.pass_through_set && !structure.pass_through_is_active)
            //         continue;
            //     for (const auto &segment : structure.segments)
            //     {
            //         if (Segment2D::circleIntersectsSegment(
            //                 ground_pos, radius_grounded,
            //                 segment.a, segment.b))
            //         {
            //             onGrounded(&segment);
            //             // onMoveTouch(a, &segment);
            //         }
            //     }
            // }

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

        // Debug::lineMounter->addCircle(
        //     vec3f(a, 0.0f),
        //     radius,
        //     2.0f,
        //     ui::colorFromHex("#ff00004c"));
        // Debug::lineMounter->addLine(
        //     vec3f(a, -1.0f),
        //     vec3f(b, -1.0f),
        //     5.0f,
        //     ui::colorFromHex("#ff00004c"));

#endif

        float delta_time_inv = 1.0f / delta_time;

        vec2f penetration;
        float radius_sq = radius * radius;

        float query_radius =
            OP<float>::maximum(
                radius,
                OP<float>::abs(offset_grounded) + radius_grounded) +
            1.0f;

        const Segment2D *segment_collision_to_ignore = nullptr;

        vec2f remaining_dir_norm = ab * (1.0f / ab_mag);
        float remaining_move_mag = ab_mag;

        int max_iterations_without_move = 3;
        while (remaining_move_mag > EPSILON<float>::low_precision)
        {
            Box2D move_box = Box2D(a, b).expand(radius);

            const Segment2D *segment_collision = nullptr;
            float move_t = 1.0f;

            vec2f new_remaining_dir_norm = vec2f(0.0f);

            // const std::vector<uint32_t> &static_ids = static_quadtree->query_segment_radius(a, b, query_radius);
            thread_state.query_segment_radius(static_quadtree.get(), static_structures, a, b, query_radius, true);
            if (dynamic_quadtree)
                thread_state.query_segment_radius(dynamic_quadtree.get(), dynamic_structures, a, b, query_radius, false);
            if (thread_state.structure_ptrs.empty())
            {
                // for (const auto *structure : always_check_structures)
                //     thread_state.quadtree_ids.push_back(structure->id);
                thread_state.structure_ptrs.assign(static_always_check.begin(), static_always_check.end());
                thread_state.structure_ptrs.insert(thread_state.structure_ptrs.end(), dynamic_always_check.begin(), dynamic_always_check.end());
            }

            // const auto &static_ids = static_quadtree->query_box(move_box.min, move_box.max);
            for (const Structure2D *structure : thread_state.structure_ptrs)
            {
                // pass-through logic
                if (structure->pass_through_set)
                {
                    bool &is_active = object_state.pass_through_get_active_ref(structure->id);

                    // the current position is a
                    bool below_deactivation_line = structure->pass_through_is_below_or_touching_deactivation_line(a, radius);
                    if (below_deactivation_line)
                        is_active = false;
                    else
                    {
                        bool above_activation_line = structure->pass_through_is_above_activation_line(a, radius);
                        if (above_activation_line)
                            is_active = true;
                        // if (above_activation_line)
                        // {
                        //     bool inside_or_touching_left_right = structure.pass_through_is_inside_or_touching_left_right_bound(a, radius);
                        //     structure.pass_through_is_active = inside_or_touching_left_right;
                        // }
                    }

                    if (!is_active)
                        continue;
                }
                for (const auto &segment : structure->segments)
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

#if DEBUG_DRAW != 0

                        vec2f new_p = OP<vec2f>::lerp(a, b, t);
                        vec2f pt_in_segment = Segment2D::closestPointToSegment(new_p, segment.a, segment.b);
                        // vec2f normal = OP<vec2f>::normalize(new_p - pt_in_segment);
                        // new_remaining_dir_norm = OP<vec2f>::cross_z_up(normal);
                        // if (OP<vec2f>::dot(new_remaining_dir_norm, out_dir) < 0.0f)
                        //     new_remaining_dir_norm = -new_remaining_dir_norm;

                        Debug::lineMounter->addLine(
                            vec3f(pt_in_segment, -1.0f),
                            vec3f(pt_in_segment + new_remaining_dir_norm * 100.0f, -1.0f),
                            3.0f,
                            ui::colorFromHex("#00ff007f"));

                        Debug::lineMounter->addLine(
                            vec3f(segment.a, -1.0f),
                            vec3f(segment.b, -1.0f),
                            5.0f,
                            ui::colorFromHex("#ff00d04c"));

#endif
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

            onMoveTouch(b, segment_collision);

            // ground check
            groundCheck(
                &on_ground_called,
                b + vec2f(0, -offset_grounded),
                radius_grounded,
                onGrounded,
                thread_state,
                object_state);

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
                // Debug::lineMounter->addLine(
                //     vec3f(a, -1.0f),
                //     vec3f(b, -1.0f),
                //     5.0f,
                //     ui::colorFromHex("#00ffff4c"));

                auto pt = Segment2D::closestPointToSegment(
                    a,
                    segment_collision->a, segment_collision->b);

                Debug::lineMounter->addLine(
                    vec3f(pt, -1.0f),
                    vec3f(pt + new_remaining_dir_norm * remaining_move_mag, -1.0f),
                    5.0f,
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

        vel = remaining_dir_norm * OP<vec2f>::length(b - a_copy) * delta_time_inv;

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

    void JumpingController::update(
        PhysicsContainer *physicsContainer,
        ThreadState &thread_state,
        Platform::Time *time,
        float input_x_axis,
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

        jumpState.update(&velocity_gravity_y, // velocityY
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
        if (jumpState.getState() == JumpState::Grounded)
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

            float desired_velocity = 600.0f * input_x_axis * speed_factor;

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

        physicsContainer->movePlayer(
            position_before,
            radius,
            radius_grounded,
            offset_grounded,
            &position, &velocity,
            time->deltaTime,
            // onGrounded callback
            [this, &ground_touch](const Segment2D *on_segment)
            {
                // ground_touch = true;
                ground_touch = true;
                last_collision_segment = *on_segment;
            },
            // onMoveTouch callback
            [this, &ground_touch](const MathCore::vec2f &pos, const Segment2D *on_segment)
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
            object_state);

        // passed = passed || (position != position_before);
        // printf("passed: %d\n", passed);

        // last_collision_segment = *on_segment;
        if (ground_touch)
        {
            if (jumpState.getState() == JumpState::Falling)
                jumpState.setGrounded();
        }
        else
        {
            last_collision_segment = Segment2D();
            if (jumpState.getState() == JumpState::Grounded)
                jumpState.setFalling();
        }
    }

    // set the position and make reset velocity and acceleration, useful for teleporting the player
    void JumpingController::teleport(const MathCore::vec2f &position)
    {
        this->position = position;
        velocity = vec2f(0);
        acceleration = vec2f(0);
    }

}