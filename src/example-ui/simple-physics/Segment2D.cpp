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

    MathCore::vec2f Segment2D::closestPointSegmentToSegment(const Segment2D &segment, MathCore::vec2f *self_src_point_output) const
    {
        return closestPointSegmentToSegment(a, b, segment.a, segment.b, self_src_point_output);
    }

    MathCore::vec2f Segment2D::closestPointSegmentToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *self_src_point_output) const
    {
        return closestPointSegmentToSegment(this->a, this->b, a, b, self_src_point_output);
    }

    bool Segment2D::intersects(const Segment2D &segment) const
    {
        return segmentsIntersect(a, b, segment.a, segment.b);
    }

    bool Segment2D::intersects(const MathCore::vec2f &a, const MathCore::vec2f &b) const
    {
        return segmentsIntersect(this->a, this->b, a, b);
    }

    bool Segment2D::intersectionPoint(const Segment2D &segment, MathCore::vec2f *output) const
    {
        return segmentsIntersectionPoint(a, b, segment.a, segment.b, output);
    }

    bool Segment2D::intersectionPoint(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output) const
    {
        return segmentsIntersectionPoint(this->a, this->b, a, b, output);
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

    bool Segment2D::segmentsIntersect(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2)
    {
#define ToSignInt(val) (val == 0 ? 0 : (val > 0 ? 1 : -1))
        using op = MathCore::OP<MathCore::vec2f>;

        int o1 = ToSignInt(op::orientation(a1, b1, a2));
        int o2 = ToSignInt(op::orientation(a1, b1, b2));
        int o3 = ToSignInt(op::orientation(a2, b2, a1));
        int o4 = ToSignInt(op::orientation(a2, b2, b1));

        return (o1 != o2 && o3 != o4);
    }

    bool Segment2D::segmentsIntersectionPoint(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2, MathCore::vec2f *output)
    {
        using namespace MathCore;
        if (!segmentsIntersect(a1, b1, a2, b2))
            return false;

        // Calculate intersection point using parametric form
        vec2f dir1 = b1 - a1;
        vec2f dir2 = b2 - a2;
        vec2f diff = a2 - a1;

        float cross = OP<vec2f>::cross_z_mag(dir1, dir2);
        if (OP<float>::abs(cross) > 1e-10f) // Not parallel
        {
            float t2 = OP<vec2f>::cross_z_mag(diff, dir1) / cross;
            *output = a2 + t2 * dir2; // Intersection point on segment2
            return true;
        }

        return false;
    }

    MathCore::vec2f Segment2D::closestPointSegmentToSegment(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2, MathCore::vec2f *a1b1_src_point_output)
    {
        using namespace MathCore;

        vec2f aux_intersect_point;
        if (segmentsIntersectionPoint(a1, b1, a2, b2, &aux_intersect_point))
        {
            if (a1b1_src_point_output)
                *a1b1_src_point_output = aux_intersect_point;
            return aux_intersect_point;
        }

        // Find closest point on segment2 to segment1
        // We need to check 4 candidates:
        // 1. Projection of a1 onto segment2
        // 2. Projection of b1 onto segment2
        // 3. a2
        // 4. b2

        // We basically need to check the distance to closest point on segment1 for each of these candidates and return the one with the minimum distance

        vec2f candidate1 = closestPointToSegment(a1, a2, b2); // Project a1 onto segment2
        vec2f candidate2 = closestPointToSegment(b1, a2, b2); // Project b1 onto segment2
        vec2f candidate3 = a2;
        vec2f candidate4 = b2;

        vec2f a1b1_src_point1 = closestPointToSegment(candidate1, a1, b1);
        vec2f a1b1_src_point2 = closestPointToSegment(candidate2, a1, b1);
        vec2f a1b1_src_point3 = closestPointToSegment(candidate3, a1, b1);
        vec2f a1b1_src_point4 = closestPointToSegment(candidate4, a1, b1);

        // Calculate squared distances
        float dist1 = OP<vec2f>::sqrDistance(candidate1, a1b1_src_point1);
        float dist2 = OP<vec2f>::sqrDistance(candidate2, a1b1_src_point2);
        float dist3 = OP<vec2f>::sqrDistance(candidate3, a1b1_src_point3);
        float dist4 = OP<vec2f>::sqrDistance(candidate4, a1b1_src_point4);

        // Find minimum distance and return corresponding point on segment2
        float minDist = dist1;
        vec2f result = candidate1;
        vec2f src_point = a1b1_src_point1;

        if (dist2 < minDist)
        {
            minDist = dist2;
            result = candidate2;
            src_point = a1b1_src_point2;
        }
        if (dist3 < minDist)
        {
            minDist = dist3;
            result = candidate3;
            src_point = a1b1_src_point3;
        }
        if (dist4 < minDist)
        {
            minDist = dist4;
            result = candidate4;
            src_point = a1b1_src_point4;
        }

        if (a1b1_src_point_output)
            *a1b1_src_point_output = src_point;
        return result;
    }
}
