#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-physics/container/Structure2D.h>
#include <appkit-physics/util/Quadtree.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {

            struct ThreadState2D
            {
                // aux for quadtree queries
                std::vector<uint32_t> quadtree_ids;
                std::vector<uint32_t> tmp_array;

                // aux for structure query
                std::vector<const Structure2D *> structure_ptrs;

                ThreadState2D();

                void query_box(
                    Util::Quadtree<Structure2D::QuadtreeIntegration> *quadtree,
                    const std::vector<std::shared_ptr<Structure2D>> &structures,
                    const MathCore::vec2f &min, const MathCore::vec2f &max,
                    bool clear_structure_ptrs);

                void query_segment_radius(
                    Util::Quadtree<Structure2D::QuadtreeIntegration> *quadtree,
                    const std::vector<std::shared_ptr<Structure2D>> &structures,
                    const MathCore::vec2f &a, const MathCore::vec2f &b, float radius,
                    bool clear_structure_ptrs);
            };
        }
    }
}