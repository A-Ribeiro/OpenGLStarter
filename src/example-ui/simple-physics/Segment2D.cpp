#pragma once

#include "Segment2D.h"

namespace SimplePhysics
{

    Segment2D::Segment2D()
    {
    }

    Segment2D::Segment2D(const MathCore::vec2f &a, const MathCore::vec2f &b)
    {
        this->a = a;
        this->b = b;
    }

    MathCore::vec2f Segment2D::closestPoint(const MathCore::vec2f &p) const
    {
        return closestPointToSegment(p, a, b);
    }

    MathCore::vec2f Segment2D::closestPointToSegment(const MathCore::vec2f &p, const MathCore::vec2f &a, const MathCore::vec2f &b)
    {
        using namespace MathCore;

        vec2f ab = b - a;
        // Project p onto ab, computing parameterized position d(t) = a + t*(b - a)
        float t = OP<vec2f>::dot(p - a, ab) / OP<vec2f>::dot(ab, ab);
        // If outside segment, clamp t (and therefore d) to the closest endpoint
        t = OP<float>::clamp(t, 0.0f, 1.0f);
        // Compute projected position from the clamped t
        return a + t * ab;
    }

    bool Segment2D::segmentsIntersect(const MathCore::vec2f &p0, const MathCore::vec2f &p1, const MathCore::vec2f &p2, const MathCore::vec2f &p3)
    {
        #define ToSignInt(val) (val == 0 ? 0 : (val > 0 ? 1 : -1))
        using op = MathCore::OP<MathCore::vec2f>;

        int o1 = ToSignInt(op::orientation(p0, p1, p2));
        int o2 = ToSignInt(op::orientation(p0, p1, p3));
        int o3 = ToSignInt(op::orientation(p2, p3, p0));
        int o4 = ToSignInt(op::orientation(p2, p3, p1));

        return (o1 != o2 && o3 != o4);
    }
}
