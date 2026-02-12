#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace SimplePhysics
{
    class Segment2D;

    class Box2D
    {
    public:
        MathCore::vec2f min;
        MathCore::vec2f max;

        Box2D();

        Box2D(const MathCore::vec2f &a, const MathCore::vec2f &b);
        
        void wrapLine(const MathCore::vec2f &a, const MathCore::vec2f &b);
        void wrapPoint(const MathCore::vec2f &point);
        void wrapCircle(const MathCore::vec2f &center, float radius);
        void makeEmpty();
        bool isEmpty() const;
        bool isPointInside(const MathCore::vec2f &point) const;
        bool overlaps(const Box2D &other) const;

        MathCore::vec2f closestPoint(const MathCore::vec2f &p) const;
        int closestPointInBoxToSegment(const Segment2D &segment, MathCore::vec2f *output_array) const;
        int closestPointInBoxToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output_array) const;

        MathCore::vec2f getCenter() const;
        MathCore::vec2f getSize() const;

        static MathCore::vec2f closestPointToBox(const MathCore::vec2f &p, const MathCore::vec2f &min, const MathCore::vec2f &max);
        static bool isPointInside(const MathCore::vec2f &point, const MathCore::vec2f &min, const MathCore::vec2f &max);
        static int closestPointInBoxToSegment(const MathCore::vec2f &min, const MathCore::vec2f &max, const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output_array);

    };

}