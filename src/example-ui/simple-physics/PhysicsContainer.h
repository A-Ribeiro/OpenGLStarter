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

        Line2D game_area_lines[GameAreaSide_Count];
        Box2D game_area;

        void buildStaticQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);
        void buildDynamicQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);

        void clearStatic();
        void clearDynamic();

        void setGameArea(const Box2D &box);

        Box2D computeStaticStructureBox() const;


    };

}