#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace SimplePhysics
{
    class Box2D;

    class Segment2D
    {
    public:
        MathCore::vec2f a;
        MathCore::vec2f b;

        /// \brief Default constructor.
        Segment2D();
        /// \brief Constructs a segment with endpoints a and b.
        ///
        /// \param a First endpoint.
        /// \param b Second endpoint.
        Segment2D(const MathCore::vec2f &a, const MathCore::vec2f &b);

        /// \brief Returns the closest point on this segment to point p.
        ///
        /// \param p The point to find the closest point to.
        /// \return Closest point on the segment.
        MathCore::vec2f closestPoint(const MathCore::vec2f &p) const;
        /// \brief Finds closest points from this segment to a box.
        ///
        /// \param box The box to check against.
        /// \param output_array Array to store output points.
        /// \return Number of closest points found.
        int closestPointInSegmentToBox(const Box2D &box, MathCore::vec2f *output_array) const;
        /// \brief Finds closest points from this segment to a box defined by min/max.
        ///
        /// \param min Minimum corner of the box.
        /// \param max Maximum corner of the box.
        /// \param output_array Array to store output points.
        /// \return Number of closest points found.
        int closestPointInSegmentToBox(const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array) const;

        /// \brief Returns the closest point on this segment to another segment.
        ///
        /// \param segment The other segment.
        /// \param other_src_point_output Optional output for closest point on the other segment.
        /// \return Closest point on this segment.
        MathCore::vec2f closestPointInSegmentToSegment(const Segment2D &segment, MathCore::vec2f *other_src_point_output = nullptr) const;

        /// \brief Returns the closest point on this segment to another segment defined by endpoints.
        ///
        /// \param a First endpoint of the other segment.
        /// \param b Second endpoint of the other segment.
        /// \param other_src_point_output Optional output for closest point on the other segment.
        /// \return Closest point on this segment.
        MathCore::vec2f closestPointInSegmentToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *other_src_point_output = nullptr) const;

        /// \brief Checks if this segment intersects a box.
        ///
        /// \param box The box to check.
        /// \return True if intersection occurs.
        bool intersectsBox(const Box2D &box) const;
        /// \brief Checks if this segment intersects a box defined by min/max.
        ///
        /// \param min Minimum corner of the box.
        /// \param max Maximum corner of the box.
        /// \return True if intersection occurs.
        bool intersectsBox(const MathCore::vec2f &min, const MathCore::vec2f &max) const;

        /// \brief Checks if this segment intersects another segment.
        ///
        /// \param segment The other segment.
        /// \return True if intersection occurs.
        bool intersects(const Segment2D &segment) const;
        /// \brief Checks if this segment intersects another segment defined by endpoints.
        ///
        /// \param a First endpoint of the other segment.
        /// \param b Second endpoint of the other segment.
        /// \return True if intersection occurs.
        bool intersects(const MathCore::vec2f &a, const MathCore::vec2f &b) const;

        /// \brief Finds intersection point with another segment.
        ///
        /// \param segment The other segment.
        /// \param output Output intersection point.
        /// \return True if intersection occurs.
        bool intersectionPoint(const Segment2D &segment, MathCore::vec2f *output) const;
        /// \brief Finds intersection point with another segment defined by endpoints.
        ///
        /// \param a First endpoint of the other segment.
        /// \param b Second endpoint of the other segment.
        /// \param output Output intersection point.
        /// \return True if intersection occurs.
        bool intersectionPoint(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output) const;

        /// \brief Returns the closest point on segment ab to point p.
        ///
        /// \param p The point to find the closest point to.
        /// \param a First endpoint of the segment.
        /// \param b Second endpoint of the segment.
        /// \return Closest point on the segment.
        static MathCore::vec2f closestPointToSegment(const MathCore::vec2f &p, const MathCore::vec2f &a, const MathCore::vec2f &b);

        /// \brief Returns the closest point on segment a2b2 to segment a1b1, and optionally the closest point on a1b1.
        ///
        /// \param a1 First endpoint of segment 1.
        /// \param b1 Second endpoint of segment 1.
        /// \param a2 First endpoint of segment 2.
        /// \param b2 Second endpoint of segment 2.
        /// \param a2b2_src_point_output Optional output for closest point on segment 2.
        /// \return Closest point on segment 1.

        static MathCore::vec2f closestPointSegmentToSegment(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2, MathCore::vec2f *a2b2_src_point_output = nullptr);
        /// \brief Checks if two segments intersect.
        ///
        /// \param a1 First endpoint of segment 1.
        /// \param b1 Second endpoint of segment 1.
        /// \param a2 First endpoint of segment 2.
        /// \param b2 Second endpoint of segment 2.
        /// \return True if intersection occurs.
        static bool segmentsIntersect(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2);
        /// \brief Finds intersection point between two segments.
        ///
        /// \param a1 First endpoint of segment 1.
        /// \param b1 Second endpoint of segment 1.
        /// \param a2 First endpoint of segment 2.
        /// \param b2 Second endpoint of segment 2.
        /// \param output Output intersection point.
        /// \return True if intersection occurs.
        static bool segmentsIntersectionPoint(const MathCore::vec2f &a1, const MathCore::vec2f &b1, const MathCore::vec2f &a2, const MathCore::vec2f &b2, MathCore::vec2f *output);
        /// \brief Checks if a segment intersects a box defined by min/max.
        ///
        /// \param a First endpoint of the segment.
        /// \param b Second endpoint of the segment.
        /// \param min Minimum corner of the box.
        /// \param max Maximum corner of the box.
        /// \return True if intersection occurs.
        static bool segmentIntersectBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max);

        /// \brief Checks intersection between a segment and a box, returns intersection details.
        ///
        /// \param a First endpoint of the segment.
        /// \param b Second endpoint of the segment.
        /// \param min Minimum corner of the box.
        /// \param max Maximum corner of the box.
        /// \param output_array Output intersection points.
        /// \return 1 if one endpoint inside, 2 if both outside but intersection, 0 if no intersection.
        static int segmentIntersectBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array);

        /// \brief Finds closest points from a segment to a box defined by min/max.
        ///
        /// \param a First endpoint of the segment.
        /// \param b Second endpoint of the segment.
        /// \param min Minimum corner of the box.
        /// \param max Maximum corner of the box.
        /// \param output_array Array to store output points.
        /// \return Number of closest points found.
        static int closestPointInSegmentToBox(const MathCore::vec2f &a, const MathCore::vec2f &b, const MathCore::vec2f &min, const MathCore::vec2f &max, MathCore::vec2f *output_array);
    };

}