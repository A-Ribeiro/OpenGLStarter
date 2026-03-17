#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include "Segment2D.h"
#include "Box2D.h"

namespace SimplePhysics
{
    enum class StructureType : uint8_t
    {
        None,
        Segment,
        Box,
        Circle,
        ClosedPolygon
    };

    class Structure2D
    {
        void computeBox();

    public:
        struct QuadtreeIntegration
        {
            using type = Structure2D;
            ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item.checkBoxOverlap(min, max); }
            ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item.box.min; }
            ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item.box.max; }
        };
        float friction;
        char tag[32];

        StructureType type;

        std::vector<Segment2D> segments;

        Box2D box;

        // only valid for circle type
        float circle_radius;

        // direction in which movement can pass through this structure.
        // zero vector = fully solid (default).
        // e.g. vec2f(0,1) = player can pass through when moving upward (one-way platform from below)
        MathCore::vec2f pass_through_direction;

        // returns true if the given movement direction should pass through this structure
        bool shouldPassThrough(const MathCore::vec2f &move_direction) const;

        Structure2D(int segment_count = 0);

        static Structure2D FromSegment(
            const char *tag,
            float friction,
            const Segment2D &segment);

        static Structure2D FromSegmentList(
            const char *tag,
            float friction,
            const std::vector<Segment2D> &segments);

        static Structure2D FromClosedPolygon(
            const char *tag,
            float friction,
            const std::vector<MathCore::vec2f> &vertices);

        static Structure2D FromBoxCenterSize(
            const char *tag,
            float friction,
            const MathCore::vec2f &center,
            const MathCore::vec2f &size);

        static Structure2D FromBoxMinMax(
            const char *tag,
            float friction,
            const MathCore::vec2f &min,
            const MathCore::vec2f &max);

        static Structure2D FromCircle(
            const char *tag,
            float friction,
            const MathCore::vec2f &center,
            float radius,
            int segment_count);

        static Structure2D FromCircleTol(
            const char *tag,
            float friction,
            const MathCore::vec2f &center,
            float radius,
            float tolerance);

        MathCore::vec2f getCenter() const;
        MathCore::vec2f getSize() const;

        // valid for: Box, Circle, and ClosedPolygon
        bool checkPointInside(const MathCore::vec2f &point) const;
        bool checkBoxOverlap(const Box2D &other) const;
        bool checkBoxOverlap(const MathCore::vec2f &min, const MathCore::vec2f &max) const;
    };

}