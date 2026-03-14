#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include "Segment2D.h"
#include "Box2D.h"
#include "Line2D.h"
#include "Structure2D.h"

#include "Quadtree.h"

namespace SimplePhysics
{
    enum GameAreaSide
    {
        GameAreaSide_Top,
        GameAreaSide_Bottom,
        GameAreaSide_Left,
        GameAreaSide_Right,
        GameAreaSide_Count
    };

    class PhysicsContainer
    {
    public:
        std::vector<Structure2D> static_structures;
        std::unique_ptr<Quadtree<Structure2D::QuadtreeIntegration>> static_quadtree;

        std::vector<Structure2D> dynamic_structures;
        std::unique_ptr<Quadtree<Structure2D::QuadtreeIntegration>> dynamic_quadtree;

        Line2D game_area_inequality_eq[GameAreaSide_Count];
        Box2D game_area;

        void buildStaticQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);
        void buildDynamicQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);

        void clearStatic();
        void clearDynamic();

        void setGameArea(const Box2D &box);

        Box2D computeStaticStructureBox() const;


        void groundCheck(
            const std::vector<uint32_t> &static_ids,
            bool *ref_on_ground_called,
            const MathCore::vec2f &position, 
            float radius_grounded, 
            const EventCore::Callback<void()> &onGrounded
        );
        
        void pushOutOfSegments1(
            MathCore::vec2f *ref_b, 
            float radius
        );

        bool pushOutOfSegments(
            MathCore::vec2f point, 
            float radius,
            MathCore::vec2f *output,
            MathCore::vec2f *offset,
            MathCore::vec2f *push_normal
        );


        void movePlayer(
            const MathCore::vec3f &position, 
            float radius, 
            float radius_grounded, 
            float offset_grounded,
            MathCore::vec3f *out_position, 
            MathCore::vec3f *out_velocity,
            float delta_time,
            const EventCore::Callback<void()> &onGrounded
        );

        const float max_velocity = 5000.0f;

    };

}