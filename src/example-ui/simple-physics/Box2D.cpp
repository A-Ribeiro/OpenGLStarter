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
        if (max.x < other.min.x || min.x > other.max.x)
            return false;
        if (max.y < other.min.y || min.y > other.max.y)
            return false;
        return true;
    }

    MathCore::vec2f Box2D::closestPoint(const MathCore::vec2f &p) const
    {
        return closestPointToBox(p, min, max);
    }

    int Box2D::closestPointInBoxToSegment(const Segment2D &segment, MathCore::vec2f *output_array) const
    {
    }

    int Box2D::closestPointInBoxToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output_array) const
    {
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
        }
        else if (rc == 0)
        {
            if (isPointInside(a, min, max))
            {
                output_array[rc] = a;
                rc++;
            }
            if (isPointInside(b, min, max))
            {
                output_array[rc] = b;
                rc++;
            }
            return rc;
        }
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