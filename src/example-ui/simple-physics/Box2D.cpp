#pragma once

#include "Box2D.h"
#include "Segment2D.h"

namespace SimplePhysics
{

    Box2D::Box2D()
    {
        makeEmpty();
    }

    Box2D::Box2D(const MathCore::vec2f &a, const MathCore::vec2f &b)
    {
        min = MathCore::OP<MathCore::vec2f>::minimum(a, b);
        max = MathCore::OP<MathCore::vec2f>::maximum(a, b);
    }

    void Box2D::wrapLine(const MathCore::vec2f &a, const MathCore::vec2f &b)
    {
        min = MathCore::OP<MathCore::vec2f>::minimum(a, b);
        max = MathCore::OP<MathCore::vec2f>::maximum(a, b);
    }

    void Box2D::wrapPoint(const MathCore::vec2f &point)
    {
        min = MathCore::OP<MathCore::vec2f>::minimum(min, point);
        max = MathCore::OP<MathCore::vec2f>::maximum(max, point);
    }

    void Box2D::wrapCircle(const MathCore::vec2f &center, float radius)
    {
        MathCore::vec2f point_min = center - MathCore::vec2f(radius, radius);
        MathCore::vec2f point_max = center + MathCore::vec2f(radius, radius);
        wrapPoint(point_min);
        wrapPoint(point_max);
    }

    void Box2D::makeEmpty()
    {
        min = MathCore::vec2f(MathCore::FloatTypeInfo<float>::max);
        max = MathCore::vec2f(-MathCore::FloatTypeInfo<float>::max);
    }

    bool Box2D::isEmpty() const
    {
        return (min.x > max.x || min.y > max.y);
    }

    bool Box2D::isPointInside(const MathCore::vec2f &point) const
    {
        return isPointInside(point, min, max);
    }

    bool Box2D::overlaps(const Box2D &other) const
    {
        return overlaps(min, max, other.min, other.max);
    }

    MathCore::vec2f Box2D::closestPoint(const MathCore::vec2f &p) const
    {
        return closestPointToBox(p, min, max);
    }

    MathCore::vec2f Box2D::closestPointInBoxToBox(const MathCore::vec2f &min2, const MathCore::vec2f &max2) const
    {
        return closestPointInBoxToBox(min, max, min2, max2);
    }

    int Box2D::closestPointInBoxToSegment(const Segment2D &segment, MathCore::vec2f *output_array) const
    {
        return closestPointInBoxToSegment(min, max, segment.a, segment.b, output_array);
    }

    int Box2D::closestPointInBoxToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output_array) const
    {
        return closestPointInBoxToSegment(min, max, a, b, output_array);
    }

    MathCore::vec2f Box2D::closestPointToBox(const MathCore::vec2f &p, const MathCore::vec2f &min, const MathCore::vec2f &max)
    {
        using namespace MathCore;

        vec2f q;
        // For each coordinate axis, if the point coordinate value is
        // outside box, clamp it to the box, else keep it as is
        for (int i = 0; i < 2; i++)
        {
            float v = p[i];
            // if (v < b.min_box[i])
            //     v = b.min_box[i]; // v = max(v, b.min[i])
            v = OP<float>::maximum(v, min[i]);
            // if (v > b.max_box[i])
            //     v = b.max_box[i]; // v = min(v, b.max[i])
            v = OP<float>::minimum(v, max[i]);
            q[i] = v;
        }
        return q;
    }

    bool Box2D::isPointInside(const MathCore::vec2f &point, const MathCore::vec2f &min, const MathCore::vec2f &max)
    {
        return (point.x < max.x && point.x > min.x &&
                point.y < max.y && point.y > min.y);
    }

    int Box2D::closestPointInBoxToSegment(const MathCore::vec2f &min, const MathCore::vec2f &max, const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output_array)
    {
        using namespace MathCore;
        int rc = Segment2D::segmentIntersectBox(a, b, min, max, output_array);

        if (rc == 2)
            return rc;
        else if (rc == 1)
        {
            if (isPointInside(a, min, max))
            {
                output_array[1] = a;
                return 2;
            }
            else if (isPointInside(b, min, max))
            {
                output_array[1] = b;
                return 2;
            }
            return 1;
        }
        else if (rc == 0)
        {
            // No intersection, check if endpoints are inside the box
            int count = 0;
            if (isPointInside(a, min, max))
            {
                output_array[count] = a;
                count++;
            }
            if (isPointInside(b, min, max))
            {
                output_array[count] = b;
                count++;
            }
            if (count > 0)
                return count;
        }

        // No intersection and no endpoints inside: find closest point on box boundary to segment
        // Check all 4 box edges against the segment
        vec2f edge_bottom_left = min;
        vec2f edge_bottom_right = vec2f(max.x, min.y);
        vec2f edge_top_right = max;
        vec2f edge_top_left = vec2f(min.x, max.y);

        // Get closest point on each box edge to the segment
        vec2f candidate1 = Segment2D::closestPointSegmentToSegment(edge_bottom_left, edge_bottom_right, a, b, nullptr);
        vec2f candidate2 = Segment2D::closestPointSegmentToSegment(edge_bottom_right, edge_top_right, a, b, nullptr);
        vec2f candidate3 = Segment2D::closestPointSegmentToSegment(edge_top_right, edge_top_left, a, b, nullptr);
        vec2f candidate4 = Segment2D::closestPointSegmentToSegment(edge_top_left, edge_bottom_left, a, b, nullptr);

        // Find which box edge point is closest to the segment
        vec2f seg_point1 = Segment2D::closestPointToSegment(candidate1, a, b);
        vec2f seg_point2 = Segment2D::closestPointToSegment(candidate2, a, b);
        vec2f seg_point3 = Segment2D::closestPointToSegment(candidate3, a, b);
        vec2f seg_point4 = Segment2D::closestPointToSegment(candidate4, a, b);

        float dist1 = OP<vec2f>::sqrDistance(candidate1, seg_point1);
        float dist2 = OP<vec2f>::sqrDistance(candidate2, seg_point2);
        float dist3 = OP<vec2f>::sqrDistance(candidate3, seg_point3);
        float dist4 = OP<vec2f>::sqrDistance(candidate4, seg_point4);

        float minDist = dist1;
        vec2f result = candidate1;

        if (dist2 < minDist)
        {
            minDist = dist2;
            result = candidate2;
        }
        if (dist3 < minDist)
        {
            minDist = dist3;
            result = candidate3;
        }
        if (dist4 < minDist)
        {
            minDist = dist4;
            result = candidate4;
        }

        output_array[0] = result;

        return 1;
    }

    bool Box2D::overlaps(const MathCore::vec2f &min1, const MathCore::vec2f &max1, const MathCore::vec2f &min2, const MathCore::vec2f &max2)
    {
        if (max1.x < min2.x || min1.x > max2.x)
            return false;
        if (max1.y < min2.y || min1.y > max2.y)
            return false;
        return true;
    }

    MathCore::vec2f Box2D::closestPointInBoxToBox(const MathCore::vec2f &min1, const MathCore::vec2f &max1, const MathCore::vec2f &min2, const MathCore::vec2f &max2)
    {
        using namespace MathCore;

        vec2f closest_point;

        // Work dimension-by-dimension to find the closest point in box1 to box2
        for (int i = 0; i < 2; i++)
        {
            if (max1[i] < min2[i])
            {
                // box1 is entirely to the left/below box2 in this dimension
                closest_point[i] = max1[i];
            }
            else if (min1[i] > max2[i])
            {
                // box1 is entirely to the right/above box2 in this dimension
                closest_point[i] = min1[i];
            }
            else
            {
                // boxes overlap in this dimension
                // Pick the center of the overlapping range
                float overlap_min = OP<float>::maximum(min1[i], min2[i]);
                float overlap_max = OP<float>::minimum(max1[i], max2[i]);
                closest_point[i] = (overlap_min + overlap_max) * 0.5f;
            }
        }

        return closest_point;
    }

    MathCore::vec2f Box2D::getCenter() const
    {
        return (min + max) * 0.5f;
    }
    MathCore::vec2f Box2D::getSize() const
    {
        return max - min;
    }

}