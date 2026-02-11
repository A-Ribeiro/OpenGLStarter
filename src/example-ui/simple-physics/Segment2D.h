#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace SimplePhysics
{

    class Segment2D
    {
    public:
        MathCore::vec2f a;
        MathCore::vec2f b;

        Segment2D();
        Segment2D(const MathCore::vec2f &a, const MathCore::vec2f &b);

        MathCore::vec2f closestPoint(const MathCore::vec2f &p) const;

        MathCore::vec2f closestPointSegmentToSegment(const Segment2D &segment) const;
        MathCore::vec2f closestPointSegmentToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b) const;

        bool intersects(const Segment2D &segment) const;
        bool intersects(const MathCore::vec2f &a, const MathCore::vec2f &b) const;

        bool intersectionPoint(const Segment2D &segment, MathCore::vec2f *output) const;
        bool intersectionPoint(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output) const;

        static MathCore::vec2f closestPointToSegment(const MathCore::vec2f &p, const MathCore::vec2f &a, const MathCore::vec2f &b);
        static MathCore::vec2f closestPointSegmentToSegment(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2);
        static bool segmentsIntersect(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2);
        static bool segmentsIntersectionPoint(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2, MathCore::vec2f *output);


    };

}