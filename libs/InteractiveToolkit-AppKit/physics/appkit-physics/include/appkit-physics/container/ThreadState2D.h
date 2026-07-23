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

            const uint32_t QUERY_MASK_ONLY_TRIGGERS = 1;
            const uint32_t QUERY_MASK_ONLY_SOLID = 1 << 1;
            const uint32_t QUERY_MASK_ALL = QUERY_MASK_ONLY_TRIGGERS | QUERY_MASK_ONLY_SOLID;

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
                    bool clear_structure_ptrs,
                    uint32_t query_mask);

                void query_segment_radius(
                    Util::Quadtree<Structure2D::QuadtreeIntegration> *quadtree,
                    const std::vector<std::shared_ptr<Structure2D>> &structures,
                    const MathCore::vec2f &a, const MathCore::vec2f &b, float radius,
                    bool clear_structure_ptrs,
                    uint32_t query_mask);

                void add_from_list(const std::vector<const Structure2D *> &list, uint32_t query_mask);
            };
        }
    }
}