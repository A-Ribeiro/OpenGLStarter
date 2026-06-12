#include <appkit-physics/core/Line2D.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Core
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

            float Line2D::circleCastIntersectsLine(
                const MathCore::vec2f &center_from, const MathCore::vec2f &center_to,
                const float &radius,
                const Line2D &line,
                MathCore::vec2f *out_move_direction)
            {
                using namespace MathCore;

                vec2f move_vector = center_to - center_from;

                // Signed distance from center_from to line
                float h0 = Line2D::pointDistanceToLine(center_from, line);

                // Check if already overlapping at start
                if (OP<float>::abs(h0) <= radius)
                {
                    vec2f closest_on_line = Line2D::closestPointToLine(center_from, line);
                    vec2f center_to_line = closest_on_line - center_from;

                    bool in_front = OP<vec2f>::dot(center_to_line, move_vector) > 0;
                    if (in_front)
                    {
                        vec2f tan_dir = OP<vec2f>::cross_z_up(line.normal);
                        tan_dir = (OP<vec2f>::dot(move_vector, tan_dir) < 0.0f) ? -tan_dir : tan_dir;
                        *out_move_direction = tan_dir;
                        return 0.0f;
                    }
                    // return 1.0f;
                }

                float move_len_sq = OP<vec2f>::dot(move_vector, move_vector);
                if (move_len_sq < 1e-12f)
                    return 1.0f;

                // h(t) = h0 + t * dh, solve for h(t) = ±radius
                float dh = OP<vec2f>::dot(move_vector, line.normal);

                if (OP<float>::abs(dh) > 1e-12f)
                {
                    float dh_inv = 1.0f / dh;
                    float t = (dh > 0) ? (-radius - h0) * dh_inv : (radius - h0) * dh_inv;
                    if (t >= 0.0f && t < 1.0f)
                    {
                        vec2f tan_dir = OP<vec2f>::cross_z_up(line.normal);
                        tan_dir = (OP<vec2f>::dot(move_vector, tan_dir) < 0.0f) ? -tan_dir : tan_dir;
                        *out_move_direction = tan_dir;
                        return t;
                    }
                }

                return 1.0f;
            }
        }
    }
}