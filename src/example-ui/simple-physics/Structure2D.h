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
        Circle
    };

    class Structure2D
    {
        void computeBox();

    public:
        float friction;
        char tag[32];

        StructureType type;

        std::vector<Segment2D> segments;

        Box2D box;

        // only valid for circle type
        float circle_radius;

        Structure2D(int segment_count = 0);

        static Structure2D FromSegment(
            const char *tag,
            float friction,
            const Segment2D &segment);

        static Structure2D FromSegmentList(
            const char *tag,
            float friction,
            const std::vector<Segment2D> &segments);

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
    };

}