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
                    if (children->contains<QuadTreeIntegrationT>(item))
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

        ITK_INLINE static bool box_isPointInside(const MathCore::vec2f &point, const MathCore::vec2f &min, const MathCore::vec2f &max)
        {
            // [min, max) — point on min edge IS inside, point on max edge is NOT
            return (point.x >= min.x && point.x < max.x &&
                    point.y >= min.y && point.y < max.y);
        }

        ITK_INLINE static bool box_overlaps(const MathCore::vec2f &min1, const MathCore::vec2f &max1, const MathCore::vec2f &min2, const MathCore::vec2f &max2)
        {
            using namespace MathCore;

            for (int i = 0; i < 2; i++)
            {
                bool value_is_same1 = min1[i] == max1[i];
                bool value_is_same2 = min2[i] == max2[i];

                if (value_is_same1 && value_is_same2)
                {
                    // both are points on this axis: overlap only if equal
                    if (min1[i] != min2[i])
                        return false;
                }
                else if (value_is_same1)
                {
                    // box1 is a point on this axis: must lie inside [min2, max2)
                    if (min1[i] < min2[i] || min1[i] >= max2[i])
                        return false;
                }
                else if (value_is_same2)
                {
                    // box2 is a point on this axis: must lie inside [min1, max1)
                    if (min2[i] < min1[i] || min2[i] >= max1[i])
                        return false;
                }
                else
                {
                    // regular [min, max) half-open interval overlap check
                    if (max1[i] <= min2[i] || min1[i] >= max2[i])
                        return false;
                }
            }

            // // [min, max) half-open: touching edges do NOT overlap
            // if (max1.x <= min2.x || min1.x >= max2.x)
            //     return false;
            // if (max1.y <= min2.y || min1.y >= max2.y)
            //     return false;
            return true;
        }

        ITK_INLINE bool contains(const MathCore::vec2f &p) const { return box_isPointInside(p, box.min, box.max); }
        ITK_INLINE bool contains(const MathCore::vec2f &min, const MathCore::vec2f &max) const { return box_overlaps(box.min, box.max, min, max); }
        ITK_INLINE bool contains(const QuadtreeNode &other) const { return box_overlaps(box.min, box.max, other.box.min, other.box.max); }
        ITK_INLINE bool contains(const Box2D &other) const { return box_overlaps(box.min, box.max, other.min, other.max); }

        // Example of QuadtreeIntegration:
        // struct QuadtreeIntegration
        // {
        //     using type = Structure2D;
        //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
        // };
        template <typename QuadTreeIntegrationT>
        ITK_INLINE bool contains(const typename QuadTreeIntegrationT::type &item) const { return QuadTreeIntegrationT::CheckBoxOverlap(item, box.min, box.max); }

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
        void query_box(const std::vector<typename QuadTreeIntegrationT::type> &items,
                       const MathCore::vec2f &min, const MathCore::vec2f &max,
                       std::vector<uint32_t> &result) const
        {
            if (!contains(min, max))
                return; // No intersection with the query box
            if (hasChildren())
            {
                for (const auto &children : children)
                    children->query_box<QuadTreeIntegrationT>(items, min, max, result);
            }
            else
            {
                for (uint32_t idx : *indices)
                {
                    const typename QuadTreeIntegrationT::type &item = items[idx];
                    // if (QuadTreeIntegrationT::CheckBoxOverlap(item, min, max))
                    const MathCore::vec2f &item_min = QuadTreeIntegrationT::GetBoxMin(item);
                    const MathCore::vec2f &item_max = QuadTreeIntegrationT::GetBoxMax(item);
                    if (box_overlaps(item_min, item_max, min, max))
                        result.push_back(idx);
                }
            }
        }

        template <typename QuadTreeIntegrationT>
        void query_segment_radius(const std::vector<typename QuadTreeIntegrationT::type> &items,
                                  const Box2D &box,
                                  const MathCore::vec2f &a, const MathCore::vec2f &b, float radius,
                                  float radius_squared,
                                  std::vector<uint32_t> &result) const
        {
            if (!contains(box.min, box.max))
                return; // No intersection with the query box
            if (hasChildren())
            {
                for (const auto &children : children)
                    children->query_segment_radius<QuadTreeIntegrationT>(items, box, a, b, radius, radius_squared, result);
            }
            else
            {
                MathCore::vec2f closest_point_array[2];
                for (uint32_t idx : *indices)
                {
                    const typename QuadTreeIntegrationT::type &item = items[idx];
                    // if (QuadTreeIntegrationT::CheckBoxOverlap(item, min, max))
                    const MathCore::vec2f &item_min = QuadTreeIntegrationT::GetBoxMin(item);
                    const MathCore::vec2f &item_max = QuadTreeIntegrationT::GetBoxMax(item);
                    if (box_overlaps(item_min, item_max, box.min, box.max))
                    {
                        // check segment with box
                        auto box_pt_rc = Box2D::closestPointInBoxToSegment(item_min, item_max, a, b, closest_point_array);
                        if (box_pt_rc == SegmentPointReturnType_One_Outside)
                        {
                            MathCore::vec2f closest_point = Segment2D::closestPointToSegment(closest_point_array[0], a, b);
                            if (MathCore::OP<MathCore::vec2f>::sqrDistance(closest_point, closest_point_array[0]) <= radius_squared)
                                result.push_back(idx);
                        }
                        else if (box_pt_rc & SegmentPointReturnType_AnyIntersectOrInside_Bit)
                            result.push_back(idx);
                    }
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

            bool box_overlapps = contains<QuadTreeIntegrationT>(item);
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
        std::vector<typename QuadTreeIntegrationT::type> *items;
        size_t computed_count;
        int32_t maxDepth, minPointThresholdToSubdivide;

        //std::vector<uint32_t> tmp_array;

    public:
        // std::vector<uint32_t> last_query;

        // Example of QuadtreeIntegration:
        // struct QuadtreeIntegration
        // {
        //     using type = Structure2D;
        //     ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
        //     ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
        // };
        Quadtree(std::vector<typename QuadTreeIntegrationT::type> *items,
                 int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16,
                 const Box2D &initial_box = Box2D()) : items(items)
        {
            maxDepth = maxDepth_;
            minPointThresholdToSubdivide = minPointThresholdToSubdivide_;

            if (items->empty())
            {
                root.reset();
                return;
            }
            if (initial_box.isEmpty())
            {
                // Compute the bounding box of all items
                MathCore::vec2f _min = QuadTreeIntegrationT::GetBoxMin((*items)[0]),
                                _max = QuadTreeIntegrationT::GetBoxMax((*items)[0]);
                for (const auto &p : *items)
                {
                    _min = MathCore::OP<MathCore::vec2f>::minimum(_min, QuadTreeIntegrationT::GetBoxMin(p));
                    _max = MathCore::OP<MathCore::vec2f>::maximum(_max, QuadTreeIntegrationT::GetBoxMax(p));
                }
                // to avoid zero-size boxes, which can cause issues in the quadtree logic
                //_min -= MathCore::EPSILON<float>::low_precision;
                _max += 1.0f;
                root = STL_Tools::make_unique<QuadtreeNode>(_min, _max, 0, minPointThresholdToSubdivide);
            }
            else
            {
                // to avoid zero-size boxes, which can cause issues in the quadtree logic
                MathCore::vec2f _min = initial_box.min;
                MathCore::vec2f _max = initial_box.max + 1.0f;
                root = STL_Tools::make_unique<QuadtreeNode>(_min, _max, 0, minPointThresholdToSubdivide);
            }
            for (uint32_t i = 0; i < (uint32_t)items->size(); ++i)
                root->recursive_insert<QuadTreeIntegrationT>(*items, i, maxDepth, minPointThresholdToSubdivide);
            computed_count = items->size();
        }

        const QuadtreeNode *getRoot() const
        {
            return root.get();
        }

        void insertNewAdded()
        {
            if (items->size() <= computed_count)
                return; // No new items to insert
            for (uint32_t i = (uint32_t)computed_count; i < (uint32_t)items->size(); ++i)
                root->recursive_insert<QuadTreeIntegrationT>(*items, i, maxDepth, minPointThresholdToSubdivide);
            computed_count = items->size();
        }

        bool checkNeedsRebuildFromNewAdded() const
        {
            if (!root)
                return false; // No items, no need to rebuild
            Box2D root_box = root->box;
            for (uint32_t i = (uint32_t)computed_count; i < (uint32_t)items->size(); ++i)
            {
                const auto &item = (*items)[i];
                const auto &item_min = QuadTreeIntegrationT::GetBoxMin(item);
                const auto &item_max = QuadTreeIntegrationT::GetBoxMax(item);
                // if (!QuadTreeIntegrationT::CheckBoxOverlap(item, root_box.min, root_box.max))
                if (!QuadtreeNode::box_overlaps(item_min, item_max, root_box.min, root_box.max))
                    return true; // Found an item that is outside the root box, need to rebuild
            }
            return false; // All items are within the root box, no need to rebuild
        }

        ~Quadtree()
        {
        }

        void query_box(const MathCore::vec2f &min, const MathCore::vec2f &max,
        std::vector<uint32_t> *query_result,
        std::vector<uint32_t> *tmp_array)
        {
            query_result->clear();
            if (!root)
                return; // No points to query

            root->query_box<QuadTreeIntegrationT>(*items, min, max, *query_result);

            // sort query_result by index and remove duplicates
            tmp_array->resize(query_result->size());
            AlgorithmCore::Sorting::RadixCountingSort<uint32_t>::sort(query_result->data(), (uint32_t)query_result->size(), tmp_array->data());
            // std::sort(query_result->begin(), query_result->end());
            // std::unique expects the input to be sorted, and moves the duplicates to the end of the vector, returning an iterator to the new end of the vector
            query_result->resize(std::unique(query_result->begin(), query_result->end()) - query_result->begin());
        }

        void query_segment_radius(
            const MathCore::vec2f &a, const MathCore::vec2f &b, float radius,
            std::vector<uint32_t> *query_result, 
            std::vector<uint32_t> *tmp_array)
        {
            query_result->clear();
            if (!root)
                return; // No points to query

            Box2D query_box = Box2D(a, b).expand(MathCore::vec2f(radius));
            root->query_segment_radius<QuadTreeIntegrationT>(*items, query_box, a, b, radius, radius * radius, *query_result);

            // sort last_query by index and remove duplicates
            tmp_array->resize(query_result->size());
            AlgorithmCore::Sorting::RadixCountingSort<uint32_t>::sort(query_result->data(), (uint32_t)query_result->size(), tmp_array->data());
            // std::sort(query_result->begin(), query_result->end());
            // std::unique expects the input to be sorted, and moves the duplicates to the end of the vector, returning an iterator to the new end of the vector
            query_result->resize(std::unique(query_result->begin(), query_result->end()) - query_result->begin());
        }
    };

}