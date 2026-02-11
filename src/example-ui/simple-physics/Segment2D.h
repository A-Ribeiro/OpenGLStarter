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
        bool intersects(const Segment2D &segment) const;
        bool intersects(const MathCore::vec2f &a, const MathCore::vec2f &b) const;

        static MathCore::vec2f closestPointToSegment(const MathCore::vec2f &p, const MathCore::vec2f &a, const MathCore::vec2f &b);

        static bool segmentsIntersect(const MathCore::vec2f &p0, const MathCore::vec2f &p1, const MathCore::vec2f &p2, const MathCore::vec2f &p3);


    };

}