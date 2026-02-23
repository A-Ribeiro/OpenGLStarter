#include "Quadtree2D.h"

namespace SimplePhysics
{

    QuadtreeNode::QuadtreeNode(const MathCore::vec2f &min_, const MathCore::vec2f &max_, int32_t depth_, int32_t initial_item_count)
    {
        indices = STL_Tools::make_unique<std::vector<uint32_t>>(initial_item_count);
        indices->clear();
        box = Box2D(min_, max_);
        depth = depth_;
        box_center = box.getCenter();
    }
    QuadtreeNode::~QuadtreeNode()
    {
    }

    void QuadtreeNode::subdivide(const std::vector<Structure2D> &items, int32_t initial_index_count)
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
            const Structure2D &item = items[idx];

            for (const auto &children : children)
            {
                if (children->intersects(item))
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

    void QuadtreeNode::query(const std::vector<Structure2D> &items,
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
                const Structure2D &item = items[idx];
                if (item.checkBoxOverlap(min, max))
                    result.push_back(idx);
            }
        }
    }

    void QuadtreeNode::recursive_insert(const std::vector<Structure2D> &items, uint32_t idx, int32_t maxDepth, int32_t minPointThresholdToSubdivide)
    {
        const Structure2D &item = items[idx];

        bool box_overlapps = intersects(item);
        if (!box_overlapps)
            return;

        if (hasChildren())
        {
            for (const auto &children : children)
                children->recursive_insert(items, idx, maxDepth, minPointThresholdToSubdivide);
            return;
        }
        // insert or subdivide
        if ((depth >= maxDepth || indices->size() < minPointThresholdToSubdivide))
        {
            indices->push_back(idx);
            return;
        }
        // subdivide and insert
        subdivide(items, minPointThresholdToSubdivide);
        for (const auto &children : children)
            children->recursive_insert(items, idx, maxDepth, minPointThresholdToSubdivide);
    }

    Quadtree::Quadtree(const std::vector<Structure2D> &items, int32_t maxDepth_, int32_t minPointThresholdToSubdivide_)
        : items(items)
    {
        maxDepth = maxDepth_;
        minPointThresholdToSubdivide = minPointThresholdToSubdivide_;

        if (items.empty())
        {
            root.reset();
            return;
        }
        MathCore::vec2f _min = items[0].box.min, _max = items[0].box.max;
        for (const auto &p : items)
        {
            _min = MathCore::OP<MathCore::vec2f>::minimum(_min, p.box.min);
            _max = MathCore::OP<MathCore::vec2f>::maximum(_max, p.box.max);
        }
        root = STL_Tools::make_unique<QuadtreeNode>(_min, _max, 0, minPointThresholdToSubdivide);
        for (uint32_t i = 0; i < (uint32_t)items.size(); ++i)
            root->recursive_insert(items, i, maxDepth, minPointThresholdToSubdivide);
    }

    Quadtree::~Quadtree()
    {
    }

    std::vector<uint32_t> &Quadtree::query(const MathCore::vec2f &min, const MathCore::vec2f &max)
    {
        last_query.clear();
        if (!root)
            return last_query; // No points to query

        root->query(items, min, max, last_query);

        return last_query;
    }

}