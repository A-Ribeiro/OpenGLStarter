#include "PhysicsContainer.h"

namespace Debug
{
    extern std::vector<MathCore::vec2f> dir;
    extern std::vector<MathCore::vec2f> lines;
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

    void PhysicsContainer::movePlayer(
        const MathCore::vec3f &position,
        float radius, float radius_grounded, float offset_grounded,
        MathCore::vec3f *out_position,
        MathCore::vec3f *out_velocity,
        float delta_time,
        const EventCore::Callback<void()> &onGrounded)
    {
        Debug::dir.clear();
        Debug::lines.clear();
        using namespace MathCore;

        vec2f a = CVT<vec3f>::toVec2(position);
        vec2f b = CVT<vec3f>::toVec2(*out_position);
        vec2f vel = CVT<vec3f>::toVec2(*out_velocity);
        float length_vel = OP<vec2f>::length(vel);

        Debug::dir.push_back(vel);

        vec2f ab = b - a;
        float ab_mag = OP<vec2f>::length(ab);

        if (ab_mag == 0.0f || delta_time == 0.0f || length_vel == 0.0f)
        {
            *out_position = position;
            return;
        }

        float delta_time_inv = 1.0f / delta_time;

        vec2f penetration;
        float radius_sq = radius * radius;

        const Segment2D *segment_collision_to_ignore = nullptr;
        float moved_len = 0;

        vec2f previous_move_dir = OP<vec2f>::normalize(ab);
        float curr_move_length = ab_mag;

        bool on_ground_called = false;

        int max_iterations = 8;
        while (moved_len < ab_mag - EPSILON<float>::low_precision && max_iterations-- > 0)
        {
            Box2D move_box = Box2D(a, b).expand(radius);

            const Segment2D *segment_collision = nullptr;
            float move_t = 1.0f;

            vec2f move_dir_to_apply_next = vec2f(0.0f);

            const auto &static_ids = static_quadtree->query_segment_radius(a, b, radius);
            for (uint32_t idx : static_ids)
            {
                const auto &structure = static_structures[idx];
                for (const auto &segment : structure.segments)
                {
                    if (&segment == segment_collision_to_ignore)
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
                        move_dir_to_apply_next = out_dir;
                    }
                }
            }

            // // circle cast against game area
            // for (int i = 0; i < GameAreaSide_Count; i++)
            // {
            //     vec2f out_dir;
            //     float t = Line2D::circleCastIntersectsLine(a, b, radius, game_area_inequality_eq[i], &out_dir);
            //     if (t < move_t)
            //     {
            //         move_t = t;
            //         segment_collision = nullptr;
            //         move_dir_to_apply_next = out_dir;
            //     }
            // }

            if (move_t == 1.0f)
                break;

            segment_collision_to_ignore = segment_collision;
            if (segment_collision != nullptr)
            {
                float local_moved_length = curr_move_length * move_t;
                moved_len += local_moved_length;
                b = a + previous_move_dir * local_moved_length;
                // move_dir_to_apply_next = OP<vec2f>::normalize(segment_collision->b - segment_collision->a);
            }

            // ground check
            if (!on_ground_called)
            {
                for (uint32_t idx : static_ids)
                {
                    const auto &structure = static_structures[idx];
                    for (const auto &segment : structure.segments)
                    {
                        vec2f ground_center = b + vec2f(0, -offset_grounded);
                        if (Segment2D::circleIntersectsSegment(
                                ground_center, radius_grounded,
                                segment.a, segment.b))
                        {
                            // emmit event grounded
                            if (!on_ground_called)
                            {
                                onGrounded();
                                on_ground_called = true;
                            }
                            break;
                        }
                    }
                    if (on_ground_called)
                        break;
                }
            }

            if (segment_collision == nullptr)
                break;

            if (move_t == 0.0f)
            {
                b = a;

                // push out of all overlapping segments
                vec2f move_out_collision = vec2f(0);
                for (uint32_t idx : static_ids)
                {
                    const auto &structure = static_structures[idx];
                    for (const auto &segment : structure.segments)
                    {
                        Box2D segment_box = Box2D(segment.a, segment.b);
                        if (!Box2D::overlaps(move_box.min, move_box.max, segment_box.min, segment_box.max))
                            continue;

                        if (Segment2D::circleIntersectsSegment(
                                b, radius,
                                segment.a, segment.b,
                                &penetration))
                        {
                            move_out_collision -= penetration;
                        }
                    }
                }

                // for (int i = 0; i < GameAreaSide_Count; i++)
                // {
                //     if (i == GameAreaSide_Top)
                //         continue;
                //     if (Line2D::circleOverlapsLine(b, radius, game_area_inequality_eq[i], &penetration))
                //         move_out_collision -= penetration;
                // }

                if (OP<vec2f>::sqrLength(move_out_collision) > 1e-12f)
                {
                    b += move_out_collision;
                    vec2f ground_center = b + vec2f(0, -offset_grounded);
                    // remove vel component in the direction of the push-out
                    vec2f segment_normal = OP<vec2f>::normalize(move_out_collision);
                    float vel_normal_component = OP<vec2f>::dot(vel, segment_normal);
                    vel -= segment_normal * vel_normal_component;

                    // ground check after push-out
                    if (!on_ground_called)
                    {
                        for (uint32_t idx : static_ids)
                        {
                            const auto &structure = static_structures[idx];
                            for (const auto &segment : structure.segments)
                            {
                                if (Segment2D::circleIntersectsSegment(
                                        ground_center, radius_grounded,
                                        segment.a, segment.b))
                                {
                                    if (!on_ground_called)
                                    {
                                        onGrounded();
                                        on_ground_called = true;
                                    }
                                    break;
                                }
                            }
                            if (on_ground_called)
                                break;
                        }
                    }
                    // if (!on_ground_called)
                    // {
                    //     vec2f aux;
                    //     if (Line2D::circleOverlapsLine(
                    //             ground_center, radius_grounded,
                    //             game_area_inequality_eq[GameAreaSide_Bottom],
                    //             &aux))
                    //     {
                    //         // emmit event grounded
                    //         onGrounded();
                    //         on_ground_called = true;
                    //     }
                    // }
                    break;
                }
                // No real penetration (just touching) — fall through to use tangent redirect
            }

            // Redirect remaining movement along the collision tangent
            {
                curr_move_length = ab_mag - moved_len;
                a = b;
                b = a + move_dir_to_apply_next * curr_move_length;
                vel = move_dir_to_apply_next * length_vel;
                previous_move_dir = move_dir_to_apply_next;
            }
        }

        // // check with ground
        // if (Line2D::circleOverlapsLine(b, radius, game_area_inequality_eq[GameAreaSide_Bottom], &penetration))
        // {
        //     b.y -= penetration.y;
        //     // vel = (b - a) * delta_time_inv;
        //     vel.y = 0;
        //     vec2f ground_check = b - vec2f(0, radius_grounded);
        //     vec2f aux;
        //     // if (Line2D::segmentIntersectsLine(
        //     //         b, ground_check,
        //     //         game_area_inequality_eq[GameAreaSide_Bottom],
        //     //         &aux))
        //     vec2f ground_center = b + vec2f(0, -offset_grounded);
        //     if (Line2D::circleOverlapsLine(
        //             ground_center, radius_grounded,
        //             game_area_inequality_eq[GameAreaSide_Bottom],
        //             &aux))
        //     {
        //         // emmit event grounded
        //         onGrounded();
        //     }
        // }

        // // check other sides of game area
        // for (int i = 0; i < GameAreaSide_Count; i++)
        // {
        //     if (i == GameAreaSide_Bottom || i == GameAreaSide_Top)
        //         continue;

        //     if (Line2D::circleOverlapsLine(b, radius, game_area_inequality_eq[i], &penetration))
        //     {
        //         b.x -= penetration.x;
        //         // vel = (b - a) * delta_time_inv;
        //         vel.x = 0;
        //         break;
        //     }
        // }

        *out_position = vec3f(b, out_position->z);

        // *out_velocity = vec3f(vel, out_velocity->z);
        *out_velocity = MathCore::OP<MathCore::vec3f>::quadraticClamp(MathCore::vec3f(0, 0, 0), vec3f(vel, out_velocity->z), max_velocity);
    }

}