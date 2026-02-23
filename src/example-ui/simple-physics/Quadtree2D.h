#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include "Structure2D.h"

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

    struct QuadtreeNode
    {
        Box2D box;
        MathCore::vec2f box_center;
        std::unique_ptr<std::vector<uint32_t>> indices;
        std::unique_ptr<QuadtreeNode> children[Quadrant_COUNT];
        int32_t depth;

        QuadtreeNode(const MathCore::vec2f &min_, const MathCore::vec2f &max_, int32_t depth_, int32_t initial_item_count);
        ~QuadtreeNode();

        ITK_INLINE Quadrant computeQuadrant(const MathCore::vec2f &p) const { return QuadrantFromBitSet(p.x >= box_center.x, p.y >= box_center.y); }
        void subdivide(const std::vector<Structure2D> &items, int32_t initial_index_count);

        ITK_INLINE bool contains(const MathCore::vec2f &p) const { return box.isPointInside(p); }
        ITK_INLINE bool intersects(const MathCore::vec2f &min, const MathCore::vec2f &max) const { return box.overlaps(min, max); }
        ITK_INLINE bool intersects(const QuadtreeNode &other) const { return box.overlaps(other.box); }
        ITK_INLINE bool intersects(const Box2D &other) const { return box.overlaps(other); }
        ITK_INLINE bool intersects(const Structure2D &item) const { return item.checkBoxOverlap(box); }
        ITK_INLINE bool isLeaf() const { return children[0] == nullptr; }
        ITK_INLINE bool hasChildren() const { return children[0] != nullptr; }

        void query(const std::vector<Structure2D> &items,
                   const MathCore::vec2f &min, const MathCore::vec2f &max,
                   std::vector<uint32_t> &result) const;

        void recursive_insert(const std::vector<Structure2D> &items, uint32_t idx, int32_t maxDepth, int32_t minPointThresholdToSubdivide);
    };

    class Quadtree
    {

        std::unique_ptr<QuadtreeNode> root;
        const std::vector<Structure2D> &items;
        int32_t maxDepth, minPointThresholdToSubdivide;

    public:
        std::vector<uint32_t> last_query;

        Quadtree(const std::vector<Structure2D> &items, int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);
        ~Quadtree();

        std::vector<uint32_t> &query(const MathCore::vec2f &min, const MathCore::vec2f &max);
    };

}