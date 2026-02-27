#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include "Segment2D.h"
#include "Box2D.h"
#include "Structure2D.h"

#include "Quadtree.h"

namespace SimplePhysics
{
    class PhysicsContainer
    {
    public:
        std::vector<Structure2D> static_structures;
        std::unique_ptr<Quadtree<Structure2D::QuadtreeIntegration>> static_quadtree;

        std::vector<Structure2D> dynamic_structures;
        std::unique_ptr<Quadtree<Structure2D::QuadtreeIntegration>> dynamic_quadtree;

        void buildStaticQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16, const Box2D &initial_box = Box2D());
        void buildDynamicQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16, const Box2D &initial_box = Box2D());

        void clearStatic();
        void clearDynamic();

    };

}