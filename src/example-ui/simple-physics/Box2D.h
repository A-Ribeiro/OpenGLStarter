#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace SimplePhysics
{
    class Segment2D;

    enum SegmentPointReturnType
    {
        SegmentPointReturnType_Two_Intersects = 1,
        SegmentPointReturnType_One_Intersect_One_Inside_A = 1 << 1,
        SegmentPointReturnType_One_Intersect_One_Inside_B = 1 << 2,
        SegmentPointReturnType_One_Intersect_One_Unknown = 1 << 3,
        SegmentPointReturnType_Two_Inside = 1 << 4,
        SegmentPointReturnType_One_Outside = 1 << 5,
    };

    const uint32_t SegmentPointReturnType_AnyIntersectOrInside_Bit =
        SegmentPointReturnType_Two_Intersects |
        SegmentPointReturnType_One_Intersect_One_Inside_A |
        SegmentPointReturnType_One_Intersect_One_Inside_B |
        SegmentPointReturnType_One_Intersect_One_Unknown |
        SegmentPointReturnType_Two_Inside;

    class Box2D
    {
    public:
        MathCore::vec2f min;
        MathCore::vec2f max;

        /// \brief Default constructor.
        Box2D();

        /// \brief Constructs a box with corners a and b.
        ///
        /// \param a First corner.
        /// \param b Second corner.
        Box2D(const MathCore::vec2f &a, const MathCore::vec2f &b);

        void fillVectorWithBoxPoints(std::vector<MathCore::vec2f> *points) const;
        std::vector<MathCore::vec2f> getBoxPoints() const;

        /// \brief Expands the box to include a line segment.
        ///
        /// \param a First endpoint of the segment.
        /// \param b Second endpoint of the segment.
        Box2D &wrapLine(const MathCore::vec2f &a, const MathCore::vec2f &b);
        /// \brief Expands the box to include a point.
        ///
        /// \param point The point to include.
        Box2D &wrapPoint(const MathCore::vec2f &point);
        /// \brief Expands the box to include a circle.
        ///
        /// \param center Center of the circle.
        /// \param radius Radius of the circle.
        Box2D &wrapCircle(const MathCore::vec2f &center, float radius);

        Box2D &wrapBox(const Box2D &box);

        Box2D &expand(const MathCore::vec2f &radius);

        /// \brief Sets the box to an empty state.
        Box2D &makeEmpty();
        /// \brief Checks if the box is empty.
        /// \return True if the box is empty.
        bool isEmpty() const;
        /// \brief Checks if a point is inside the box.
        ///
        /// \param point The point to check.
        /// \return True if the point is inside.
        bool isPointInside(const MathCore::vec2f &point) const;
        /// \brief Checks if this box overlaps with another box.
        ///
        /// \param other The other box.
        /// \return True if the boxes overlap.
        bool overlaps(const Box2D &other) const;

        /// \brief Checks if this box overlaps with another box.
        ///
        /// \param min2 The other box minimum corner.
        /// \param max2 The other box maximum corner.
        /// \return True if the boxes overlap.
        bool overlaps(const MathCore::vec2f &min2, const MathCore::vec2f &max2) const;

        /// \brief Returns the closest point in the box to point p.
        ///
        /// \param p The point to find the closest point to.
        /// \return Closest point in the box.
        MathCore::vec2f closestPoint(const MathCore::vec2f &p) const;
        /// \brief Checks if this box overlaps with another box.
        ///
        /// \param min2 Minimum corner of the second box.
        /// \param max2 Maximum corner of the second box.
        /// \return Closest point in the first box to the second box.
        MathCore::vec2f closestPointInBoxToBox(const MathCore::vec2f &min2, const MathCore::vec2f &max2) const;

        /// \brief Finds closest points in the box to a segment.
        ///
        /// \param segment The segment to check against.
        /// \param output_array Array to store output points.
        /// \return Number of closest points found.
        int closestPointInBoxToSegment(const Segment2D &segment, MathCore::vec2f *output_array) const;
        /// \brief Finds closest points in the box to a segment defined by endpoints.
        ///
        /// \param a First endpoint of the segment.
        /// \param b Second endpoint of the segment.
        /// \param output_array Array to store output points.
        /// \return Number of closest points found.
        int closestPointInBoxToSegment(const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output_array) const;

        /// \brief Returns the center of the box.
        /// \return Center point of the box.
        MathCore::vec2f getCenter() const;
        /// \brief Returns the size of the box.
        /// \return Size vector (width, height).
        MathCore::vec2f getSize() const;

        /// \brief Returns the closest point in a box defined by min/max to point p.
        ///
        /// \param p The point to find the closest point to.
        /// \param min Minimum corner of the box.
        /// \param max Maximum corner of the box.
        /// \return Closest point in the box.
        static MathCore::vec2f closestPointToBox(const MathCore::vec2f &p, const MathCore::vec2f &min, const MathCore::vec2f &max);
        /// \brief Checks if a point is inside a box defined by min/max.
        ///
        /// \param point The point to check.
        /// \param min Minimum corner of the box.
        /// \param max Maximum corner of the box.
        /// \return True if the point is inside.
        static bool isPointInside(const MathCore::vec2f &point, const MathCore::vec2f &min, const MathCore::vec2f &max);
        /// \brief Finds closest points in a box defined by min/max to a segment defined by endpoints.
        ///
        /// \param min Minimum corner of the box.
        /// \param max Maximum corner of the box.
        /// \param a First endpoint of the segment.
        /// \param b Second endpoint of the segment.
        /// \param output_array Array to store output points.
        /// \return Number of closest points found.
        static SegmentPointReturnType closestPointInBoxToSegment(const MathCore::vec2f &min, const MathCore::vec2f &max, const MathCore::vec2f &a, const MathCore::vec2f &b, MathCore::vec2f *output_array);
        /// \brief Checks if this box overlaps with another box.
        ///
        /// \param min1 Minimum corner of the first box.
        /// \param max1 Maximum corner of the first box.
        /// \param min2 Minimum corner of the second box.
        /// \param max2 Maximum corner of the second box.
        /// \return True if the boxes overlap.
        static bool overlaps(const MathCore::vec2f &min1, const MathCore::vec2f &max1, const MathCore::vec2f &min2, const MathCore::vec2f &max2);
        /// \brief Checks if this box overlaps with another box.
        ///
        /// \param min1 Minimum corner of the first box.
        /// \param max1 Maximum corner of the first box.
        /// \param min2 Minimum corner of the second box.
        /// \param max2 Maximum corner of the second box.
        /// \return Closest point in the first box to the second box.
        static MathCore::vec2f closestPointInBoxToBox(const MathCore::vec2f &min1, const MathCore::vec2f &max1, const MathCore::vec2f &min2, const MathCore::vec2f &max2);
    };

}