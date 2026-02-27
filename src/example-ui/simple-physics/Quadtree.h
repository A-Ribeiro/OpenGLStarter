#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include "Segment2D.h"
#include "Box2D.h"

#include <InteractiveToolkit/AlgorithmCore/Sorting/RadixCountingSort32.h>

namespace SimplePhysics
{

    enum Quadrant
    {
        Quadrant_bit_E = 0b001, // East
        Quadrant_bit_N = 0b010, // North

        Quadrant_SW = 0b000, // South-West
        Quadrant_SE = 0b001, // South-East
        Quadrant_NW = 0b010, // North-West
        Quadrant_NE = 0b011, // North-East
        Quadrant_COUNT = 0b100
    };

    constexpr inline Quadrant QuadrantFromBitSet(bool x_positive_east_bit, bool y_positive_north_bit)
    {
        return static_cast<Quadrant>(((int)(y_positive_north_bit) << 1) | (int)(x_positive_east_bit));
    }

    // Example of QuadtreeIntegration:
    // struct QuadtreeIntegration
    // {
    //     using type = Structure2D;
    //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
    //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
    //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
    // };

    struct QuadtreeNode
    {
        Box2D box;
        MathCore::vec2f box_center;
        std::unique_ptr<std::vector<uint32_t>> indices;
        std::unique_ptr<QuadtreeNode> children[Quadrant_COUNT];
        int32_t depth;

        QuadtreeNode(const MathCore::vec2f &min_, const MathCore::vec2f &max_, int32_t depth_, int32_t initial_item_count)
        {
            indices = STL_Tools::make_unique<std::vector<uint32_t>>(initial_item_count);
            indices->clear();
            box = Box2D(min_, max_);
            depth = depth_;
            box_center = box.getCenter();
        }
        ~QuadtreeNode()
        {
        }

        ITK_INLINE Quadrant computeQuadrant(const MathCore::vec2f &p) const { return QuadrantFromBitSet(p.x >= box_center.x, p.y >= box_center.y); }
        // Example of QuadtreeIntegration:
        // struct QuadtreeIntegration
        // {
        //     using type = Structure2D;
        //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
        // };
        template <typename QuadTreeIntegrationT>
        ITK_INLINE void subdivide(const std::vector<typename QuadTreeIntegrationT::type> &items, int32_t initial_index_count)
        {
            using namespace MathCore;
            int32_t depth_plus_one = depth + 1;
            children[Quadrant_SW] = STL_Tools::make_unique<QuadtreeNode>(box.min, box_center, depth_plus_one, initial_index_count);
            children[Quadrant_SE] = STL_Tools::make_unique<QuadtreeNode>(vec2f(box_center.x, box.min.y), vec2f(box.max.x, box_center.y), depth_plus_one, initial_index_count);
            children[Quadrant_NW] = STL_Tools::make_unique<QuadtreeNode>(vec2f(box.min.x, box_center.y), vec2f(box_center.x, box.max.y), depth_plus_one, initial_index_count);
            children[Quadrant_NE] = STL_Tools::make_unique<QuadtreeNode>(box_center, box.max, depth_plus_one, initial_index_count);

            if (indices->empty())
                return; // No indices to distribute

            for (auto idx : *indices)
            {
                const typename QuadTreeIntegrationT::type &item = items[idx];

                for (const auto &children : children)
                {
                    if (children->intersects<QuadTreeIntegrationT>(item))
                        children->indices->push_back(idx);
                }

                // Quadrant quad = computeQuadrant(items[idx]);
                // children[quad]->indices->push_back(idx);
            }
            // do not need to clear indices,
            // as we are moving them to the children nodes
            // the query is done only in the Leaf nodes
            indices.reset();
        }

        ITK_INLINE bool contains(const MathCore::vec2f &p) const { return box.isPointInside(p); }
        ITK_INLINE bool intersects(const MathCore::vec2f &min, const MathCore::vec2f &max) const { return box.overlaps(min, max); }
        ITK_INLINE bool intersects(const QuadtreeNode &other) const { return box.overlaps(other.box); }
        ITK_INLINE bool intersects(const Box2D &other) const { return box.overlaps(other); }

        // Example of QuadtreeIntegration:
        // struct QuadtreeIntegration
        // {
        //     using type = Structure2D;
        //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
        // };
        template <typename QuadTreeIntegrationT>
        ITK_INLINE bool intersects(const typename QuadTreeIntegrationT::type &item) const { return QuadTreeIntegrationT::CheckBoxOverlap(item, box.min, box.max); }

        ITK_INLINE bool isLeaf() const { return children[0] == nullptr; }
        ITK_INLINE bool hasChildren() const { return children[0] != nullptr; }

        // Example of QuadtreeIntegration:
        // struct QuadtreeIntegration
        // {
        //     using type = Structure2D;
        //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
        // };
        template <typename QuadTreeIntegrationT>
        void query(const std::vector<typename QuadTreeIntegrationT::type> &items,
                   const MathCore::vec2f &min, const MathCore::vec2f &max,
                   std::vector<uint32_t> &result) const
        {
            if (!intersects(min, max))
                return; // No intersection with the query box
            if (hasChildren())
            {
                for (const auto &children : children)
                    children->query(items, min, max, result);
            }
            else
            {
                for (uint32_t idx : *indices)
                {
                    const typename QuadTreeIntegrationT::type &item = items[idx];
                    if (QuadTreeIntegrationT::CheckBoxOverlap(item, min, max))
                        result.push_back(idx);
                }
            }
        }

        // Example of QuadtreeIntegration:
        // struct QuadtreeIntegration
        // {
        //     using type = Structure2D;
        //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
        // };
        template <typename QuadTreeIntegrationT>
        void recursive_insert(const std::vector<typename QuadTreeIntegrationT::type> &items, uint32_t idx, int32_t maxDepth, int32_t minPointThresholdToSubdivide)
        {
            const typename QuadTreeIntegrationT::type &item = items[idx];

            bool box_overlapps = intersects<QuadTreeIntegrationT>(item);
            if (!box_overlapps)
                return;

            if (hasChildren())
            {
                for (const auto &children : children)
                    children->recursive_insert<QuadTreeIntegrationT>(items, idx, maxDepth, minPointThresholdToSubdivide);
                return;
            }
            // insert or subdivide
            if ((depth >= maxDepth || indices->size() < minPointThresholdToSubdivide))
            {
                indices->push_back(idx);
                return;
            }
            // subdivide and insert
            subdivide<QuadTreeIntegrationT>(items, minPointThresholdToSubdivide);
            for (const auto &children : children)
                children->recursive_insert<QuadTreeIntegrationT>(items, idx, maxDepth, minPointThresholdToSubdivide);
        }
    };

    // Example of QuadtreeIntegration:
    // struct QuadtreeIntegration
    // {
    //     using type = Structure2D;
    //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
    //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
    //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
    // };
    template <typename QuadTreeIntegrationT>
    class Quadtree
    {

        std::unique_ptr<QuadtreeNode> root;
        const std::vector<typename QuadTreeIntegrationT::type> &items;
        int32_t maxDepth, minPointThresholdToSubdivide;

        std::vector<uint32_t> tmp_array;

    public:
        std::vector<uint32_t> last_query;

        // Example of QuadtreeIntegration:
        // struct QuadtreeIntegration
        // {
        //     using type = Structure2D;
        //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
        // };
        Quadtree(const std::vector<typename QuadTreeIntegrationT::type> &items, int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16) : items(items)
        {
            maxDepth = maxDepth_;
            minPointThresholdToSubdivide = minPointThresholdToSubdivide_;

            if (items.empty())
            {
                root.reset();
                return;
            }
            MathCore::vec2f _min = QuadTreeIntegrationT::GetBoxMin(items[0]),
                            _max = QuadTreeIntegrationT::GetBoxMax(items[0]);
            for (const auto &p : items)
            {
                _min = MathCore::OP<MathCore::vec2f>::minimum(_min, QuadTreeIntegrationT::GetBoxMin(p));
                _max = MathCore::OP<MathCore::vec2f>::maximum(_max, QuadTreeIntegrationT::GetBoxMax(p));
            }
            root = STL_Tools::make_unique<QuadtreeNode>(_min, _max, 0, minPointThresholdToSubdivide);
            for (uint32_t i = 0; i < (uint32_t)items.size(); ++i)
                root->recursive_insert<QuadTreeIntegrationT>(items, i, maxDepth, minPointThresholdToSubdivide);
        }

        ~Quadtree()
        {
        }

        std::vector<uint32_t> &query(const MathCore::vec2f &min, const MathCore::vec2f &max)
        {
            last_query.clear();
            if (!root)
                return last_query; // No points to query

            root->query<QuadTreeIntegrationT>(items, min, max, last_query);

            // sort last_query by index and remove duplicates
            tmp_array.resize(last_query.size());
            AlgorithmCore::Sorting::RadixCountingSort<uint32_t>::sort(last_query.data(), (uint32_t)last_query.size(), tmp_array.data());
            // std::sort(last_query.begin(), last_query.end());
            last_query.erase(std::unique(last_query.begin(), last_query.end()), last_query.end());

            return last_query;
        }
    };

}