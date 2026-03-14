#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace SimplePhysics
{
    class Line2D
    {
    public:
        MathCore::vec2f normal;
        float distance;

        Line2D();

        static Line2D FromPointNormal(const MathCore::vec2f &point, const MathCore::vec2f &normal);

        static Line2D FromPoints(const MathCore::vec2f &a, const MathCore::vec2f &b);

        void normalize();

        static MathCore::vec2f closestPointToLine(const MathCore::vec2f &p, const Line2D &line);

        static float pointDistanceToLine(const MathCore::vec2f &p, const Line2D &line);

        static bool circleOverlapsLine(const MathCore::vec2f &center, const float &radius, const Line2D &line, MathCore::vec2f *penetration);

        static bool boxOverlapsLine(const MathCore::vec2f &min, const MathCore::vec2f &max, const Line2D &line, MathCore::vec2f *penetration);

        static bool segmentIntersectsLine(const MathCore::vec2f &a, const MathCore::vec2f &b, const Line2D &line, MathCore::vec2f *out_intersection_point);

        // check if segment has any point with distance <= 0 to line, if so, they overlap
        static bool segmentOverlapsLine(const MathCore::vec2f &a, const MathCore::vec2f &b, const Line2D &line);

        static float circleCastIntersectsLine(
            const MathCore::vec2f &center_from, const MathCore::vec2f &center_to,
            const float &radius,
            const Line2D &line,
            MathCore::vec2f *out_move_direction);
    };

}