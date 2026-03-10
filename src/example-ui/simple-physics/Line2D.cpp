#include "Line2D.h"

namespace SimplePhysics
{
    Line2D::Line2D()
    {
        normal.y = 1.0f;
        distance = 0.0f;
    }

    Line2D Line2D::FromPointNormal(const MathCore::vec2f &point, const MathCore::vec2f &normal)
    {
        using namespace MathCore;
        Line2D line;
        line.normal = OP<vec2f>::normalize(normal);
        line.distance = OP<vec2f>::dot(line.normal, point);
        return line;
    }

    Line2D Line2D::FromPoints(const MathCore::vec2f &a, const MathCore::vec2f &b)
    {
        using namespace MathCore;
        Line2D line;
        line.normal = OP<vec2f>::cross_z_up(b - a);
        line.distance = OP<vec2f>::dot(line.normal, a);
        return line;
    }

    void Line2D::normalize()
    {
        using namespace MathCore;
        float mag2 = OP<vec2f>::dot(normal, normal);
        float mag2_rsqrt = OP<float>::rsqrt(mag2);
        normal *= mag2_rsqrt;
        distance *= mag2_rsqrt;
    }

    MathCore::vec2f Line2D::closestPointToLine(const MathCore::vec2f &p, const Line2D &line)
    {
        using namespace MathCore;

        float t = (OP<vec2f>::dot(line.normal, p) - line.distance); // / dot(plane.normal, plane.normal);
        return p - t * line.normal;
    }

    float Line2D::pointDistanceToLine(const MathCore::vec2f &p, const Line2D &line)
    {
        using namespace MathCore;

        float t = (OP<vec2f>::dot(line.normal, p) - line.distance); // / dot(plane.normal, plane.normal);
        return t;
    }

    bool Line2D::circleOverlapsLine(const MathCore::vec2f &center, const float &radius, const Line2D &line, MathCore::vec2f *penetration)
    {
        using namespace MathCore;

        float distance_to_line = pointDistanceToLine(center, line);
        float sphere_dst_to_line_abs = MathCore::OP<float>::abs(distance_to_line - radius);

        if (sphere_dst_to_line_abs > 0.004f && distance_to_line < radius) // 0.00002f
        {
            // EPSILON - to avoid process the same triangle again...
            const float EPSILON = 0.002f;
            *penetration = -line.normal * (sphere_dst_to_line_abs + EPSILON);
            // printf ("+"); fflush(stdout);
            return true;
        }
        // printf ("."); fflush(stdout);
        return false;
    }

    bool Line2D::boxOverlapsLine(const MathCore::vec2f &min, const MathCore::vec2f &max, const Line2D &line, MathCore::vec2f *penetration)
    {
        using namespace MathCore;

        // get closest point in box to line
        vec2f closestPointInBox = OP<vec2f>::clamp(closestPointToLine(min, line), min, max);

        float distance_to_line = pointDistanceToLine(closestPointInBox, line);

        if (distance_to_line < 0.004f) // 0.00002f
        {
            // EPSILON - to avoid process the same triangle again...
            const float EPSILON = 0.002f;
            *penetration = -line.normal * (MathCore::OP<float>::abs(distance_to_line) + EPSILON);
            return true;
        }

        return false;
    }

    bool Line2D::segmentIntersectsLine(const MathCore::vec2f &a, const MathCore::vec2f &b, const Line2D &line, MathCore::vec2f *out_intersection_point)
    {
        using namespace MathCore;
        
        float da = pointDistanceToLine(a, line);
        float db = pointDistanceToLine(b, line);

        if (da * db > 0)
            return false; // both points are on the same side of the line

        float t = da / (da - db);
        *out_intersection_point = a + t * (b - a);
        return true;

    }

    // check if segment has any point with distance <= 0 to line, if so, they overlap
    bool Line2D::segmentOverlapsLine(const MathCore::vec2f &a, const MathCore::vec2f &b, const Line2D &line)
    {
        using namespace MathCore;

        float da = pointDistanceToLine(a, line);
        float db = pointDistanceToLine(b, line);

        return da <= 0 || db <= 0;
    }
}