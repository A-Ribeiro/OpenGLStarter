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

    void PhysicsContainer::groundCheck(
        const std::vector<uint32_t> &static_ids,
        bool *ref_on_ground_called,
        const MathCore::vec2f &position,
        float radius_grounded,
        const EventCore::Callback<void()> &onGrounded)
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
                            onGrounded();
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
                *out_position = vec3f(b, out_position->z);
                *out_velocity = vec3f(vel, out_velocity->z);
            }
        }

        float length_vel = OP<vec2f>::length(vel);

        Debug::dir.push_back(vel);

        vec2f ab = b - a;
        float ab_mag = OP<vec2f>::length(ab);

        if (ab_mag == 0.0f || delta_time == 0.0f || length_vel == 0.0f)
        {
            *out_position = vec3f(a, position.z);
            return;
        }

        float delta_time_inv = 1.0f / delta_time;

        vec2f penetration;
        float radius_sq = radius * radius;

        float query_radius = offset_grounded + radius_grounded + 1.0f;

        // const Segment2D *segment_collision_to_ignore[16] = {};
        // int segment_collision_to_ignore_count = 0;
        const Segment2D *segment_collision_to_ignore = nullptr;
        float moved_len = 0;

        vec2f previous_move_dir = OP<vec2f>::normalize(ab);
        float curr_move_length = ab_mag;

        bool on_ground_called = false;

        int max_iterations = 8;
        while (moved_len < ab_mag - EPSILON<float>::low_precision)
        {
            Box2D move_box = Box2D(a, b).expand(radius);

            const Segment2D *segment_collision = nullptr;
            float move_t = 1.0f;

            vec2f move_dir_to_apply_next = vec2f(0.0f);

            const auto &static_ids = static_quadtree->query_segment_radius(a, b, query_radius);
            for (uint32_t idx : static_ids)
            {
                const auto &structure = static_structures[idx];
                for (const auto &segment : structure.segments)
                {
                    // bool should_ignore = false;
                    // for (int ig = 0; ig < segment_collision_to_ignore_count; ig++)
                    // {
                    //     if (&segment == segment_collision_to_ignore[ig])
                    //     {
                    //         should_ignore = true;
                    //         break;
                    //     }
                    // }
                    // if (should_ignore)
                    //     continue;
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
                        move_dir_to_apply_next = out_dir;
                    }
                }
            }

            if (move_t == 1.0f)
                break;

            // move b to the collision point
            // if (segment_collision_to_ignore_count < 16)
            //     segment_collision_to_ignore[segment_collision_to_ignore_count++] = segment_collision;
            segment_collision_to_ignore = segment_collision;
            float local_moved_length = curr_move_length * move_t;
            moved_len += local_moved_length;
            b = a + previous_move_dir * local_moved_length;

            // ground check
            groundCheck(
                static_ids,
                &on_ground_called,
                b + vec2f(0, -offset_grounded),
                radius_grounded,
                onGrounded);

            // Redirect remaining movement along the collision tangent
            if (max_iterations-- <= 0)
                break;
            if (moved_len < ab_mag - EPSILON<float>::low_precision) // test to avoid b to have move after the logic
            {
                curr_move_length = ab_mag - moved_len;
                a = b;
                b = a + move_dir_to_apply_next * curr_move_length;
                vel = move_dir_to_apply_next * length_vel;
                previous_move_dir = move_dir_to_apply_next;
            }
        }

        // // Safety: push b out of any remaining penetrating segments
        // {
        //     vec2f b_before_push = b;
        //     pushOutOfSegments(&b, radius);
        //     vec2f push_delta = b - b_before_push;
        //     if (OP<vec2f>::sqrLength(push_delta) > 1e-12f)
        //     {
        //         // cancel velocity component going into the surface
        //         vec2f push_normal = OP<vec2f>::normalize(push_delta);
        //         float vel_into_surface = OP<vec2f>::dot(vel, push_normal);
        //         if (vel_into_surface < 0.0f)
        //             vel -= push_normal * vel_into_surface;

        //         // ground check after push-out
        //         Box2D push_box = Box2D().wrapCircle(b, radius + query_radius);
        //         const auto &static_ids_push = static_quadtree->query_box(push_box.min, push_box.max);
        //         groundCheck(
        //             static_ids_push,
        //             &on_ground_called,
        //             b + vec2f(0, -offset_grounded),
        //             radius_grounded,
        //             onGrounded);
        //     }
        // }

        {
            vec2f push_offset, push_normal;
            if (pushOutOfSegments(b, radius, &b, &push_offset, &push_normal))
            {
                // cancel velocity component going into the surface
                float vel_into_surface = OP<vec2f>::dot(vel, push_normal);
                if (vel_into_surface < 0.0f)
                    vel -= push_normal * vel_into_surface;
            }
        }

        *out_position = vec3f(b, out_position->z);
        // *out_velocity = vec3f(vel, out_velocity->z);
        *out_velocity = MathCore::OP<MathCore::vec3f>::quadraticClamp(MathCore::vec3f(0, 0, 0), vec3f(vel, out_velocity->z), max_velocity);
    }

}