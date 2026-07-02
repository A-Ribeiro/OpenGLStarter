#include <appkit-physics/container/Container2D.h>

using namespace MathCore;

#define DEBUG_DRAW 0

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {
            std::vector<std::shared_ptr<Structure2D>> &Container2D::getStaticStructures() { return static_structures; }

            std::vector<std::shared_ptr<Structure2D>> &Container2D::getDynamicStructures() { return dynamic_structures; }

            void Container2D::addStaticStructure(std::shared_ptr<Structure2D> structure)
            {
                ITK_ABORT(structure->id != STRUCTURE2D_ID_INVALID, "Structure2D must have an invalid id when adding to Container2D");
                structure->id = uuid.next();
                static_structures.insert(
                    std::upper_bound(static_structures.begin(), static_structures.end(), structure,
                                     [](const std::shared_ptr<Structure2D> &a, const std::shared_ptr<Structure2D> &b)
                                     { return a->id < b->id; }),
                    structure);
            }
            void Container2D::removeStaticStructure(uint32_t idx)
            {
                auto it = std::lower_bound(static_structures.begin(), static_structures.end(), idx,
                                           [](const std::shared_ptr<Structure2D> &structure, uint32_t id)
                                           { return structure->id < id; });
                if (it == static_structures.end() || (*it)->id != idx)
                    return;
                static_structures.erase(it);
                (*it)->id = STRUCTURE2D_ID_INVALID;

                // for (auto &item : jumpingControllerList)
                //     item->object_state.pass_through_remove_id(idx);

                uuid.release(idx);
            }
            std::shared_ptr<Structure2D> Container2D::getStaticStructure(uint32_t idx)
            {
                auto it = std::lower_bound(static_structures.begin(), static_structures.end(), idx,
                                           [](const std::shared_ptr<Structure2D> &structure, uint32_t id)
                                           { return structure->id < id; });
                if (it == static_structures.end() || (*it)->id != idx)
                    return nullptr;
                return (*it);
            }

            void Container2D::addDynamicStructure(std::shared_ptr<Structure2D> structure)
            {
                structure->id = uuid.next();
                dynamic_structures.insert(
                    std::upper_bound(dynamic_structures.begin(), dynamic_structures.end(), structure,
                                     [](const std::shared_ptr<Structure2D> &a, const std::shared_ptr<Structure2D> &b)
                                     { return a->id < b->id; }),
                    structure);
            }
            void Container2D::removeDynamicStructure(uint32_t idx)
            {
                auto it = std::lower_bound(dynamic_structures.begin(), dynamic_structures.end(), idx,
                                           [](const std::shared_ptr<Structure2D> &structure, uint32_t id)
                                           { return structure->id < id; });
                if (it == dynamic_structures.end() || (*it)->id != idx)
                    return;
                dynamic_structures.erase(it);
                (*it)->id = STRUCTURE2D_ID_INVALID;

                // for (auto &item : jumpingControllerList)
                //     item->object_state.pass_through_remove_id(idx);

                uuid.release(idx);
            }

            std::shared_ptr<Structure2D> Container2D::getDynamicStructure(uint32_t idx)
            {
                auto it = std::lower_bound(dynamic_structures.begin(), dynamic_structures.end(), idx,
                                           [](const std::shared_ptr<Structure2D> &structure, uint32_t id)
                                           { return structure->id < id; });
                if (it == dynamic_structures.end() || (*it)->id != idx)
                    return nullptr;
                return (*it);
            }

            void Container2D::buildStaticQuadtree(int32_t maxDepth_, int32_t minPointThresholdToSubdivide_)
            {
                static_always_check.clear();

                for (uint32_t i = 0; i < static_structures.size(); ++i)
                {
                    if (static_structures[i]->always_check)
                        static_always_check.push_back(static_structures[i].get());
                }
                static_quadtree = STL_Tools::make_unique<Util::Quadtree<Structure2D::QuadtreeIntegration>>(&static_structures, maxDepth_, minPointThresholdToSubdivide_, game_area);
            }
            void Container2D::buildDynamicQuadtree(int32_t maxDepth_, int32_t minPointThresholdToSubdivide_)
            {
                dynamic_always_check.clear();

                for (uint32_t i = 0; i < dynamic_structures.size(); ++i)
                {
                    if (dynamic_structures[i]->always_check)
                        dynamic_always_check.push_back(dynamic_structures[i].get());
                }
                dynamic_quadtree = STL_Tools::make_unique<Util::Quadtree<Structure2D::QuadtreeIntegration>>(&dynamic_structures, maxDepth_, minPointThresholdToSubdivide_, game_area);
            }

            void Container2D::clearStatic()
            {
                for (auto &structure : static_structures)
                {
                    uuid.release(structure->id);
                    structure->id = STRUCTURE2D_ID_INVALID;
                }
                static_structures.clear();
                static_quadtree.reset();
            }

            void Container2D::clearDynamic()
            {
                for (auto &structure : dynamic_structures)
                {
                    uuid.release(structure->id);
                    structure->id = STRUCTURE2D_ID_INVALID;
                }
                dynamic_structures.clear();
                dynamic_quadtree.reset();
            }

            void Container2D::setGameArea(const Core::Box2D &box)
            {
                game_area = box;
                game_area_inequality_eq[GameAreaSide_Top] = Core::Line2D::FromPointNormal(box.max, MathCore::vec2f(0, -1));
                game_area_inequality_eq[GameAreaSide_Bottom] = Core::Line2D::FromPointNormal(box.min, MathCore::vec2f(0, 1));
                game_area_inequality_eq[GameAreaSide_Left] = Core::Line2D::FromPointNormal(box.min, MathCore::vec2f(1, 0));
                game_area_inequality_eq[GameAreaSide_Right] = Core::Line2D::FromPointNormal(box.max, MathCore::vec2f(-1, 0));
            }

            Core::Box2D Container2D::computeStaticStructureBox() const
            {
                Core::Box2D box;
                box.makeEmpty();
                for (const auto &structure : static_structures)
                    box.wrapBox(structure->box);
                box.wrapBox(game_area);
                return box;
            }

            void Container2D::groundCheck(
                bool *ref_on_ground_called,
                const MathCore::vec2f &position,
                float radius_grounded,
                const EventCore::Callback<void(const Core::Segment2D *on_segment)> &onGrounded,
                ThreadState2D &thread_state,
                ObjectState2D &object_state)
            {
                using namespace MathCore;

                if (*ref_on_ground_called)
                    return;

                float dst = FLT_MAX;
                Core::Segment2D closest_segment;
                bool found_ground = false;

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
                        if (Core::Segment2D::circleIntersectsSegment(
                                position, radius_grounded,
                                segment.a, segment.b))
                        {
                            auto nearest_pt = segment.closestPoint(position);
                            float current_segment_dst = OP<vec2f>::sqrDistance(nearest_pt, position);

                            if (current_segment_dst < dst)
                            {
                                dst = current_segment_dst;
                                closest_segment = segment;
                                found_ground = true;
                            }
                        }
                    }
                }

                // emmit event grounded
                if (found_ground)
                {
                    onGrounded(&closest_segment);
                    *ref_on_ground_called = true;
                }
            }

            // void Container2D::pushOutOfSegments1(
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

            bool Container2D::pushOutOfSegments(
                MathCore::vec2f point,
                float radius,
                MathCore::vec2f *output,
                MathCore::vec2f *offset,
                MathCore::vec2f *push_normal,
                const MathCore::vec2f &velocity_hint,
                ThreadState2D &thread_state,
                ObjectState2D &object_state)
            {
                Core::Box2D b_box = Core::Box2D().wrapCircle(point, radius);

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
                        Core::Box2D segment_box = Core::Box2D(segment.a, segment.b);
                        if (!Core::Box2D::overlaps(b_box.min, b_box.max, segment_box.min, segment_box.max))
                            continue;
                        vec2f penetration;
                        if (Core::Segment2D::circleIntersectsSegment(
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

            void Container2D::moveObject(
                const MathCore::vec2f &in_position,
                float radius, float radius_grounded, float offset_grounded,
                MathCore::vec2f *out_position,
                MathCore::vec2f *out_velocity,
                float delta_time,
                const EventCore::Callback<void(const Core::Segment2D *on_segment)> &onGrounded,
                const EventCore::Callback<void(const MathCore::vec2f &pos, const Core::Segment2D *on_segment)> &onMoveTouch,
                ThreadState2D &thread_state,
                ObjectState2D &object_state,
                float skin_width,
                float max_velocity,
                float offset_above_activation_line,
                float offset_below_deactivation_line)
            {
                using namespace MathCore;

                vec2f a = in_position;
                vec2f b = *out_position;
                vec2f vel = *out_velocity;

                {
                    vec2f push_offset, push_normal;
                    if (pushOutOfSegments(a, radius + skin_width * 0.1f, &a, &push_offset, &push_normal, vel, thread_state, object_state))
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
                    vec2f ground_pos = a + vec2f(0, offset_grounded);
                    Core::Box2D ground_box = Core::Box2D().wrapCircle(ground_pos, radius_grounded);
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
                        a + vec2f(0, offset_grounded),
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

                const Core::Segment2D *segment_collision_to_ignore = nullptr;

                vec2f remaining_dir_norm = ab * (1.0f / ab_mag);
                float remaining_move_mag = ab_mag;

                bool no_iteration = true;

                int max_iterations_without_move = 3;
                while (remaining_move_mag > skin_width * 0.66f)
                {
                    no_iteration = false;

                    Core::Box2D move_box = Core::Box2D(a, b).expand(radius);

                    const Core::Segment2D *segment_collision = nullptr;
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
                            bool below_deactivation_line = structure->pass_through_is_below_or_touching_deactivation_line(a, radius, offset_below_deactivation_line);
                            if (below_deactivation_line)
                                is_active = false;
                            else
                            {
                                bool above_activation_line = structure->pass_through_is_above_activation_line(a, radius, offset_above_activation_line);
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

                            Core::Box2D segment_box = Core::Box2D(segment.a, segment.b);
                            if (!Core::Box2D::overlaps(move_box.min, move_box.max, segment_box.min, segment_box.max))
                                continue;

                            MathCore::vec2f p_ab, p_segment;
                            p_ab = Core::Segment2D::closestPointSegmentToSegment(a, b, segment.a, segment.b, &p_segment);

                            float dist_sq = OP<vec2f>::sqrDistance(p_ab, p_segment);
                            if (dist_sq > radius_sq)
                                continue;

                            vec2f out_dir;
                            float t = Core::Segment2D::circleCastIntersectsSegment(a, b, radius, segment.a, segment.b, &out_dir, skin_width);
                            if ((remaining_move_mag * t) <= skin_width)
                                t = 0.0f;
                            if (t < move_t)
                            {
                                move_t = t;
                                segment_collision = &segment;
                                new_remaining_dir_norm = out_dir;

#if DEBUG_DRAW != 0

                                vec2f new_p = OP<vec2f>::lerp(a, b, t);
                                vec2f pt_in_segment = Core::Segment2D::closestPointToSegment(new_p, segment.a, segment.b);
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
                    {
                        // ground check when the move is completed
                        groundCheck(
                            &on_ground_called,
                            b + vec2f(0, offset_grounded),
                            radius_grounded,
                            onGrounded,
                            thread_state,
                            object_state);
                        break;
                    }

                    // move b to the collision point
                    segment_collision_to_ignore = segment_collision;
                    float remaining_actual_moved = remaining_move_mag * move_t;
                    b = a + remaining_dir_norm * remaining_actual_moved;
                    remaining_move_mag -= remaining_actual_moved;

                    onMoveTouch(b, segment_collision);

                    // ground check
                    groundCheck(
                        &on_ground_called,
                        b + vec2f(0, offset_grounded),
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
                    // if (remaining_move_mag > EPSILON<float>::low_precision) // test to avoid b to have move after the logic
                    if (remaining_move_mag > skin_width) // test to avoid b to have move after the logic
                    {
                        float remaining_move_cos = OP<vec2f>::dot(remaining_dir_norm, new_remaining_dir_norm);
                        remaining_move_cos = OP<float>::clamp(remaining_move_cos, -1.0f, 1.0f);
                        // MathCore::OP<float>::cos(MathCore::OP<float>::deg_2_rad(89.0f))
                        if (MathCore::OP<float>::abs(remaining_move_cos) <= 1.745240e-2) // if the remaining move is almost perpendicular to the collision surface, just stop
                        {
                            // printf("cosseno perpendicular: %f\n", remaining_move_cos);
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

                if (no_iteration)
                {
                    *out_position = a;

                    // do ground check
                    vec2f ground_pos = a + vec2f(0, offset_grounded);
                    Core::Box2D ground_box = Core::Box2D().wrapCircle(ground_pos, radius_grounded);
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
                        a + vec2f(0, offset_grounded),
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

                vel = remaining_dir_norm * OP<vec2f>::length(b - a_copy) * delta_time_inv;

                // If grounded and has upward velocity, cancel it to prevent floating/bouncing artifacts
                // This ensures consistent behavior regardless of framerate
                if (on_ground_called && vel.y > 0.0f)
                    vel.y = 0.0f;

                *out_position = b;
                // *out_velocity = vec3f(vel, out_velocity->z);
                *out_velocity = OP<vec2f>::quadraticClamp(vec2f(0, 0), vel, max_velocity);
            }

            void Container2D::dumpContent(const EventCore::Callback<void(const MathCore::vec2f &a, const MathCore::vec2f &b, bool is_pass_through)> &onLine,
                                          const EventCore::Callback<void(const MathCore::vec2f &center, const MathCore::vec2f &size)> &onBox,
                                          const EventCore::Callback<void(const MathCore::vec2f &center, float radius)> &onCircle,
                                          const EventCore::Callback<void(const MathCore::vec2f &center, const MathCore::vec2f &size)> &onGameArea)
            {
                auto iterate_through_elements = [&](std::vector<std::shared_ptr<Structure2D>> &ref_array)
                {
                    for (const auto &structure : ref_array)
                    {
                        if (structure->type == StructureType::Segment || structure->type == StructureType::ClosedPolygon)
                        {
                            for (const auto &segment : structure->segments)
                                onLine(segment.a, segment.b, structure->pass_through_set);
                        }
                        else if (structure->type == StructureType::Box)
                        {
                            auto box_center = structure->box.getCenter();
                            auto box_size = structure->box.getSize();
                            onBox(box_center, box_size);
                        }
                        else if (structure->type == StructureType::Circle)
                        {
                            auto circle_center = structure->box.getCenter();
                            onCircle(circle_center, structure->circle_radius);
                        }
                    }
                };
                auto box_center = game_area.getCenter();
                auto box_size = game_area.getSize();
                onGameArea(box_center, box_size);
                iterate_through_elements(static_structures);
                iterate_through_elements(dynamic_structures);
            }
        }
    }
}