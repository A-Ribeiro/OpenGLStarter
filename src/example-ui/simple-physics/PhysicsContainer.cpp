#include "PhysicsContainer.h"

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
        const EventCore::Callback<void()> &onGrounded)
    {
        using namespace MathCore;

        vec2f a = CVT<vec3f>::toVec2(position);
        vec2f b = CVT<vec3f>::toVec2(*out_position);
        vec2f vel = CVT<vec3f>::toVec2(*out_velocity);
        Box2D b_box = Box2D(b - radius, b + radius);

        vec2f ground_center = b + vec2f(0, -offset_grounded);

        vec2f penetration;

        const auto &static_idx = static_quadtree->query(b_box.min, b_box.max);
        for (uint32_t idx : static_idx)
        {
            const auto &structure = static_structures[idx];
            for (const auto &segment : structure.segments)
            {
                if (Segment2D::circleIntersectsSegment(
                        b, radius,
                        segment.a, segment.b,
                        &penetration))
                {
                    b -= penetration;
                    vel = b - a;
                    vec2f aux;
                    // vec2f ground_check = b - vec2f(0, radius_grounded);
                    // if (Segment2D::segmentsIntersect(
                    //         b, ground_check,
                    //         segment.a, segment.b))
                    if (Segment2D::circleIntersectsSegment(
                            ground_center, radius_grounded,
                            segment.a, segment.b,
                            &aux))
                    {
                        // emmit event grounded
                        onGrounded();
                    }
                }
            }
        }

        // check with ground
        if (Line2D::circleOverlapsLine(b, radius, game_area_inequality_eq[GameAreaSide_Bottom], &penetration))
        {
            b -= penetration;
            vel = b - a;
            vec2f ground_check = b - vec2f(0, radius_grounded);
            vec2f aux;
            // if (Line2D::segmentIntersectsLine(
            //         b, ground_check,
            //         game_area_inequality_eq[GameAreaSide_Bottom],
            //         &aux))
            if (Line2D::circleOverlapsLine(
                    ground_center, radius_grounded,
                    game_area_inequality_eq[GameAreaSide_Bottom],
                    &aux))
            {
                // emmit event grounded
                onGrounded();
            }
        }

        // check other sides of game area
        for (int i = 0; i < GameAreaSide_Count; i++)
        {
            if (i == GameAreaSide_Bottom || i == GameAreaSide_Top)
                continue;
            if (Line2D::circleOverlapsLine(b, radius, game_area_inequality_eq[i], &penetration))
            {
                b -= penetration;
                vel = b - a;
            }
        }

        *out_position = vec3f(b, out_position->z);
        *out_velocity = vec3f(vel, out_velocity->z);
    }

}