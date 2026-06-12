#include <appkit-physics/container/ThreadState2D.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {
            ThreadState2D::ThreadState2D()
            {
            }

            void ThreadState2D::query_box(
                Util::Quadtree<Structure2D::QuadtreeIntegration> *quadtree,
                const std::vector<std::unique_ptr<Structure2D>> &structures,
                const MathCore::vec2f &min, const MathCore::vec2f &max,
                bool clear_structure_ptrs)
            {
                quadtree_ids.clear();
                quadtree->query_box(min, max, &quadtree_ids, &tmp_array);
                if (clear_structure_ptrs)
                    structure_ptrs.clear();
                for (uint32_t idx : quadtree_ids)
                    structure_ptrs.push_back(structures[idx].get());
            }

            void ThreadState2D::query_segment_radius(
                Util::Quadtree<Structure2D::QuadtreeIntegration> *quadtree,
                const std::vector<std::unique_ptr<Structure2D>> &structures,
                const MathCore::vec2f &a, const MathCore::vec2f &b, float radius,
                bool clear_structure_ptrs)
            {
                quadtree_ids.clear();
                quadtree->query_segment_radius(a, b, radius, &quadtree_ids, &tmp_array);
                if (clear_structure_ptrs)
                    structure_ptrs.clear();
                for (uint32_t idx : quadtree_ids)
                    structure_ptrs.push_back(structures[idx].get());
            }
        }
    }
}