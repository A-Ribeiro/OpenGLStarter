#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace SimplePhysics
{
    class Box2D;

    class Segment2D
    {
    public:
        MathCore::vec2f a;
        MathCore::vec2f b;

        Segment2D();
        Segment2D(const MathCore::vec2f &a, const MathCore::vec2f &b);

        MathCore::vec2f closestPoint(const MathCore::vec2f &p) const;
        int closestPointInSegmentToBox(const Box2D &box, MathCore::vec2f *output_array) const;
        int closestPointInSegmentToBox(const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array) const;

        MathCore::vec2f closestPointInSegmentToSegment(const Segment2D &segment, MathCore::vec2f *segment_point_output = nullptr) const;
        MathCore::vec2f closestPointInSegmentToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *segment_point_output = nullptr) const;

        bool intersectsBox(const Box2D &box) const;
        bool intersectsBox(const MathCore::vec2f &min, const MathCore::vec2f &max) const;

        bool intersects(const Segment2D &segment) const;
        bool intersects(const MathCore::vec2f &a, const MathCore::vec2f &b) const;

        bool intersectionPoint(const Segment2D &segment, MathCore::vec2f *output) const;
        bool intersectionPoint(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output) const;

        static MathCore::vec2f closestPointToSegment(const MathCore::vec2f &p, const MathCore::vec2f &a, const MathCore::vec2f &b);
        static MathCore::vec2f closestPointSegmentToSegment(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2, MathCore::vec2f *a1b1_src_point_output = nullptr);
        static bool segmentsIntersect(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2);
        static bool segmentsIntersectionPoint(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2, MathCore::vec2f *output);
        static bool segmentIntersectBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max);

        static int segmentIntersectBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array);

        static int closestPointInSegmentToBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array);


    };

}