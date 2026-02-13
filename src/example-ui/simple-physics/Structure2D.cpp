#include "Structure2D.h"
#include <InteractiveToolkit/AlgorithmCore/Polygon/Polygon2D.h>

namespace SimplePhysics
{
    void Structure2D::computeBox()
    {
        box.makeEmpty();

        for (const Segment2D &segment : segments)
            box.wrapLine(segment.a, segment.b);
    }

    Structure2D::Structure2D(int segment_count) : tag{'\0'}, segments(segment_count)
    {
        friction = 0.0f;
        type = StructureType::None;
    }

    Structure2D Structure2D::FromSegment(
        const char *tag,
        float friction,
        const Segment2D &segment)
    {
        Structure2D result(1);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Segment;
        result.segments[0] = segment;

        result.computeBox();

        return result;
    }

    Structure2D Structure2D::FromSegmentList(
        const char *tag,
        float friction,
        const std::vector<Segment2D> &segments)
    {
        Structure2D result(segments.size());

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Segment;
        result.segments.assign(segments.begin(), segments.end());

        result.computeBox();

        return result;
    }

    Structure2D Structure2D::FromClosedPolygon(
        const char *tag,
        float friction,
        const std::vector<MathCore::vec2f> &vertices)
    {
        Structure2D result(vertices.size());

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::ClosedPolygon;

        // force ccw order for polygons
        bool is_ccw = AlgorithmCore::Polygon::Polygon2DUtils::signedArea(vertices) >= 0;
        if (!is_ccw)
        {
            int last_i = (int)vertices.size() - 1;
            for (int i = 0; i < (int)vertices.size(); i++)
            {
                int reverse_i = last_i - i;
                int reverse_next_i = reverse_i - 1;
                reverse_next_i = reverse_next_i < 0 ? last_i : reverse_next_i;
                const MathCore::vec2f &p1 = vertices[reverse_i];
                const MathCore::vec2f &p2 = vertices[reverse_next_i];
                result.segments[i] = Segment2D(p1, p2);
            }
        }
        else
        {
            for (int i = 0; i < (int)vertices.size(); i++)
            {
                int next_i = i + 1;
                next_i = (i + 1) >= (int)vertices.size() ? 0 : next_i;
                const MathCore::vec2f &p1 = vertices[i];
                const MathCore::vec2f &p2 = vertices[next_i];
                result.segments[i] = Segment2D(p1, p2);
            }
        }

        result.computeBox();

        return result;
    }

    Structure2D Structure2D::FromBoxCenterSize(
        const char *tag,
        float friction,
        const MathCore::vec2f &center,
        const MathCore::vec2f &size)
    {

        Structure2D result(4);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Box;

        MathCore::vec2f half_size = size * 0.5f;

        result.box = Box2D(center - half_size, center + half_size);

        result.segments[0] = Segment2D(result.box.min, MathCore::vec2f(result.box.max.x, result.box.min.y));
        result.segments[1] = Segment2D(MathCore::vec2f(result.box.max.x, result.box.min.y), result.box.max);
        result.segments[2] = Segment2D(result.box.max, MathCore::vec2f(result.box.min.x, result.box.max.y));
        result.segments[3] = Segment2D(MathCore::vec2f(result.box.min.x, result.box.max.y), result.box.min);

        return result;
    }

    Structure2D Structure2D::FromBoxMinMax(
        const char *tag,
        float friction,
        const MathCore::vec2f &min,
        const MathCore::vec2f &max)
    {

        Structure2D result(4);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Box;

        result.box = Box2D(min, max);

        result.segments[0] = Segment2D(result.box.min, MathCore::vec2f(result.box.max.x, result.box.min.y));
        result.segments[1] = Segment2D(MathCore::vec2f(result.box.max.x, result.box.min.y), result.box.max);
        result.segments[2] = Segment2D(result.box.max, MathCore::vec2f(result.box.min.x, result.box.max.y));
        result.segments[3] = Segment2D(MathCore::vec2f(result.box.min.x, result.box.max.y), result.box.min);

        return result;
    }

    Structure2D Structure2D::FromCircle(
        const char *tag,
        float friction,
        const MathCore::vec2f &center,
        float radius,
        int segment_count)
    {

        Structure2D result(segment_count);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Circle;

        result.box.wrapCircle(center, radius);
        result.circle_radius = radius;

        // iterate segment count
        for (int i = 0; i < segment_count; i++)
        {
            const float _2pi = 2.0f * MathCore::CONSTANT<float>::PI;
            float angle0 = (float)(i) / segment_count * _2pi;
            float angle1 = (float)((i + 1) % segment_count) / segment_count * _2pi;

            MathCore::vec2f p1 = center + MathCore::vec2f(std::cos(angle0), std::sin(angle0)) * radius;
            MathCore::vec2f p2 = center + MathCore::vec2f(std::cos(angle1), std::sin(angle1)) * radius;

            result.segments[i] = Segment2D(p1, p2);
        }

        return result;
    }

    Structure2D Structure2D::FromCircleTol(
        const char *tag,
        float friction,
        const MathCore::vec2f &center,
        float radius,
        float tolerance)
    {
        const float _2pi = 2.0f * MathCore::CONSTANT<float>::PI;
        int segment_count = (int)(MathCore::OP<float>::ceil((_2pi * radius) / tolerance) + 0.5f);
        segment_count = MathCore::OP<int>::maximum(segment_count, 3);
        return FromCircle(
            tag,
            friction,
            center,
            radius,
            segment_count);
    }

    MathCore::vec2f Structure2D::getCenter() const
    {
        return box.getCenter();
    }
    MathCore::vec2f Structure2D::getSize() const
    {
        return box.getSize();
    }

    bool Structure2D::checkPointInside(const MathCore::vec2f &point) const
    {
        if (type == StructureType::Box)
        {
            return box.isPointInside(point);
        }
        else if (type == StructureType::Circle)
        {
            return MathCore::OP<MathCore::vec2f>::sqrDistance(box.getCenter(), point) <= (circle_radius * circle_radius);
        }
        else if (type == StructureType::ClosedPolygon)
        {
            if (!box.isPointInside(point))
                return false;
            
            bool inside = false;
            size_t count = segments.size();
            if (count < 3)
                return false; // A polygon must have at least 3 points
            size_t prev_i = count - 1;
            for (size_t i = 0; i < count; i++)
            {
                const MathCore::vec2f &pt_curr = segments[i].a;
                const MathCore::vec2f &pt_prev = segments[prev_i].a;
                // check if the point is within the y-bounds of the polygon edge
                if ((pt_curr.y > point.y) != (pt_prev.y > point.y))
                {
                    if (pt_curr.x > point.x && pt_prev.x > point.x)
                    {
                        // both points are to the right of the point
                        // the edge is always to the right of the point
                        inside = !inside;
                    }
                    else
                    {
                        // if pt_curr is at bottom
                        // if side is positive, the point is to the left of the edge (edge is at right)
                        // if side is negative, the point is to the right of the edge (edge is at left)

                        // if pt_prev is at bottom
                        // the side will be reversed
                        float side = MathCore::OP<MathCore::vec2f>::orientation(pt_curr, pt_prev, point);

                        // if prev is at bottom
                        // flip the side
                        if (pt_prev.y < pt_curr.y)
                            side = -side;

                        if (side > 0)
                            inside = !inside;
                    }
                }
                prev_i = i;
            }
            return inside;
        }
        return false;
    }

}