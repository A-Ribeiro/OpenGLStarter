#pragma once

#include "Segment2D.h"
#include "Box2D.h"

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

    int Segment2D::closestPointInSegmentToBox(const Box2D &box, MathCore::vec2f *output_array) const
    {
        return closestPointInSegmentToBox(a, b, box.min, box.max, output_array);
    }

    int Segment2D::closestPointInSegmentToBox(const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array) const
    {
        return closestPointInSegmentToBox(a, b, min, max, output_array);
    }

    MathCore::vec2f Segment2D::closestPointInSegmentToSegment(const Segment2D &segment, MathCore::vec2f *other_src_point_output) const
    {
        return closestPointSegmentToSegment(a, b, segment.a, segment.b, other_src_point_output);
    }

    MathCore::vec2f Segment2D::closestPointInSegmentToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *other_src_point_output) const
    {
        return closestPointSegmentToSegment(this->a, this->b, a, b, other_src_point_output);
    }

    bool Segment2D::intersectsBox(const Box2D &box) const
    {
        return segmentIntersectBox(a, b, box.min, box.max);
    }

    bool Segment2D::intersectsBox(const MathCore::vec2f &min, const MathCore::vec2f &max) const
    {
        return segmentIntersectBox(a, b, min, max);
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

    MathCore::vec2f Segment2D::closestPointSegmentToSegment(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2, MathCore::vec2f *a2b2_src_point_output)
    {
        using namespace MathCore;

        vec2f aux_intersect_point;
        if (segmentsIntersectionPoint(a2, b2, a1, b1, &aux_intersect_point))
        {
            if (a2b2_src_point_output)
                *a2b2_src_point_output = aux_intersect_point;
            return aux_intersect_point;
        }

        // Find closest point on segment2 to segment1
        // We need to check 4 candidates:
        // 1. Projection of a1 onto segment2
        // 2. Projection of b1 onto segment2
        // 3. a2
        // 4. b2

        // We basically need to check the distance to closest point on segment1 for each of these candidates and return the one with the minimum distance

        vec2f candidate1 = closestPointToSegment(a2, a1, b1); // Project a1 onto segment2
        vec2f candidate2 = closestPointToSegment(b2, a1, b1); // Project b1 onto segment2
        vec2f candidate3 = a1;
        vec2f candidate4 = b1;

        vec2f a2b2_src_point1 = closestPointToSegment(candidate1, a2, b2);
        vec2f a2b2_src_point2 = closestPointToSegment(candidate2, a2, b2);
        vec2f a2b2_src_point3 = closestPointToSegment(candidate3, a2, b2);
        vec2f a2b2_src_point4 = closestPointToSegment(candidate4, a2, b2);

        // Calculate squared distances
        float dist1 = OP<vec2f>::sqrDistance(candidate1, a2b2_src_point1);
        float dist2 = OP<vec2f>::sqrDistance(candidate2, a2b2_src_point2);
        float dist3 = OP<vec2f>::sqrDistance(candidate3, a2b2_src_point3);
        float dist4 = OP<vec2f>::sqrDistance(candidate4, a2b2_src_point4);

        // Find minimum distance and return corresponding point on segment2
        float minDist = dist1;
        vec2f result = candidate1;
        vec2f src_point = a2b2_src_point1;

        if (dist2 < minDist)
        {
            minDist = dist2;
            result = candidate2;
            src_point = a2b2_src_point2;
        }
        if (dist3 < minDist)
        {
            minDist = dist3;
            result = candidate3;
            src_point = a2b2_src_point3;
        }
        if (dist4 < minDist)
        {
            minDist = dist4;
            result = candidate4;
            src_point = a2b2_src_point4;
        }

        if (a2b2_src_point_output)
            *a2b2_src_point_output = src_point;
        return result;
    }

    bool Segment2D::segmentIntersectBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max)
    {
        using namespace MathCore;

        const float EPSILON = (float)1e-4;

        vec2f c = (min + max) * (float)0.5; // Box center-point
        vec2f e = max - c;                  // Box halflength extents
        vec2f m = (a + b) * (float)0.5;     // Segment midpoint
        vec2f d = b - m;                    // Segment halflength vector
        m = m - c;                          // Translate box and segment to origin
        // Try world coordinate axes as separating axes
        float adx = OP<float>::abs(d.x);
        if (OP<float>::abs(m.x) > e.x + adx)
            return false;
        float ady = OP<float>::abs(d.y);
        if (OP<float>::abs(m.y) > e.y + ady)
            return false;
        // Add in an epsilon term to counteract arithmetic errors when segment is
        // (near) parallel to a coordinate axis (see text for detail)
        adx += EPSILON;
        ady += EPSILON;
        // Try cross products of segment direction vector with coordinate axes
        // cross on xy plane
        if (OP<float>::abs(OP<vec2f>::cross_z_mag(m, d)) > e.x * ady + e.y * adx)
            return false;
        // No separating axis found; segment must be overlapping AABB
        return true;
    }

    int Segment2D::segmentIntersectBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array)
    {
        if (!segmentIntersectBox(a, b, min, max))
            return 0;

        using namespace MathCore;

        const float EPSILON = (float)1e-4;

        vec2f dir = b - a;
        float length_dir = OP<vec2f>::length(dir);
        dir *= 1.0f / length_dir;

        const float tmin_start = -EPSILON;
        float tmax_start = length_dir + EPSILON;

        float tmin = tmin_start; // set to -FLT_MAX to get first hit on line
        float tmax = tmax_start; // FLT_MAX; // set to max distance ray can travel (for segment)
        // For all three slabs
        for (int i = 0; i < 2; i++)
        {
            if (OP<float>::abs(dir[i]) < EPSILON)
            {
                // Ray is parallel to slab. No hit if origin not within slab
                if (a[i] < min[i] || a[i] > max[i])
                    return 0;
            }
            else
            {
                // Compute intersection t value of ray with near and far plane of slab
                float ood = (float)1 / dir[i];
                float t1 = (min[i] - a[i]) * ood;
                float t2 = (max[i] - a[i]) * ood;
                // Make t1 be intersection with near plane, t2 with far plane
                if (t1 > t2)
                {
                    // Swap(t1, t2);
                    float taux = t1;
                    t1 = t2;
                    t2 = taux;
                }
                // Compute the intersection of slab intersection intervals
                if (t1 > tmin)
                    tmin = t1;
                if (t2 < tmax)
                    tmax = t2;
                // Exit with no collision as soon as slab intersection becomes empty
                if (tmin > tmax)
                    return 0;
            }
        }
        // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)

        int collision_count = 0;
        if (tmin >= 0.0f)
        {
            output_array[collision_count] = a + dir * tmin;
            collision_count++;
        }

        if (tmax <= length_dir)
        {
            output_array[collision_count] = a + dir * tmax;
            collision_count++;
        }

        //*outQ = p + d * tmin;

        return collision_count;
    }


    int Segment2D::closestPointInSegmentToBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array)
    {
        using namespace MathCore;

        int rc = segmentIntersectBox(a, b, min, max, output_array);
        if (rc == 2)
            return rc;
        else if (rc == 1)
        {
            if (Box2D::isPointInside(a, min, max))
            {
                output_array[1] = a;
                return 2;
            }
            else if (Box2D::isPointInside(b, min, max))
            {
                output_array[1] = b;
                return 2;
            }
        }
        else if (rc == 0)
        {
            if (Box2D::isPointInside(a, min, max))
            {
                output_array[rc] = a;
                rc++;
            }
            if (Box2D::isPointInside(b, min, max))
            {
                output_array[rc] = b;
                rc++;
            }
            if (rc > 0)
                return rc;
        }

        // Check box corners projected onto segment
        vec2f candidate1 = closestPointToSegment(min, a, b);
        vec2f candidate2 = closestPointToSegment(vec2f(max.x, min.y), a, b);
        vec2f candidate3 = closestPointToSegment(max, a, b);
        vec2f candidate4 = closestPointToSegment(vec2f(min.x, max.y), a, b);

        vec2f box_point1 = Box2D::closestPointToBox(candidate1, min, max);
        vec2f box_point2 = Box2D::closestPointToBox(candidate2, min, max);
        vec2f box_point3 = Box2D::closestPointToBox(candidate3, min, max);
        vec2f box_point4 = Box2D::closestPointToBox(candidate4, min, max);

        float dist1 = OP<vec2f>::sqrDistance(candidate1, box_point1);
        float dist2 = OP<vec2f>::sqrDistance(candidate2, box_point2);
        float dist3 = OP<vec2f>::sqrDistance(candidate3, box_point3);
        float dist4 = OP<vec2f>::sqrDistance(candidate4, box_point4);
        
        // Also check segment endpoints projected onto box
        vec2f box_point_a = Box2D::closestPointToBox(a, min, max);
        vec2f box_point_b = Box2D::closestPointToBox(b, min, max);
        float dist_a = OP<vec2f>::sqrDistance(a, box_point_a);
        float dist_b = OP<vec2f>::sqrDistance(b, box_point_b);

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
        if (dist_a < minDist)
        {
            minDist = dist_a;
            result = a;
        }
        if (dist_b < minDist)
        {
            minDist = dist_b;
            result = b;
        }

        output_array[0] = result;

        return 1;
    }

}
