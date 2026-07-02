
#include <appkit-physics/container/Structure2D.h>

#include <appkit-physics/util/Quadtree.h>

#include <InteractiveToolkit/AlgorithmCore/Polygon/Polygon2D.h>
#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {
            void Structure2D::computeBox()
            {
                box.makeEmpty();

                for (const Core::Segment2D &segment : segments)
                    box.wrapLine(segment.a, segment.b);
            }

            void Structure2D::computePassThroughLines(const MathCore::vec2f &pass_through_normal_hint)
            {
                using namespace MathCore;
                pass_through_set = true;

                vec2f segment_ab = segments[0].b - segments[0].a;
                vec2f normal = OP<vec2f>::cross_z_up(segment_ab);
                normal = OP<vec2f>::normalize(normal);
                normal = (OP<vec2f>::dot(normal, pass_through_normal_hint) >= 0) ? normal : -normal;

                const float deactivate_threshold_px = 2.0f;

                box.expand(vec2f(deactivate_threshold_px));

                vec2f point_in_line = segments[0].a;

                pass_through_activate_line = Core::Line2D::FromPointNormal(point_in_line, normal);
                // pass_through_deactivate_line = Line2D::FromPointNormal(point_in_line - deactivate_threshold_px * normal, -normal);
                pass_through_deactivate_line = Core::Line2D::FromPointNormal(point_in_line, -normal);

                // vec2f x_axis_positive = OP<vec2f>::cross_z_down(normal);
                // vec2f x_axis_negative = -x_axis_positive;

                // if (OP<vec2f>::dot(x_axis_positive, segment_ab) < 0)
                //     std::swap(segments[0].a, segments[0].b);
                // // segment ab is in the same direction of x_axis_positive
                // // a is the left point and b is the right point

                // pass_through_left_bound_line = Line2D::FromPointNormal(segments[0].a, x_axis_positive);
                // pass_through_right_bound_line = Line2D::FromPointNormal(segments[0].b, x_axis_negative);
            }

            // bool Structure2D::pass_through_is_inside_or_touching_left_right_bound(const MathCore::vec2f &point, float radius) const
            // {
            //     float dst = Line2D::pointDistanceToLine(point, pass_through_left_bound_line);
            //     if (dst < -radius)
            //         return false;
            //     dst = Line2D::pointDistanceToLine(point, pass_through_right_bound_line);
            //     if (dst < -radius)
            //         return false;
            //     return true;
            // }

            bool Structure2D::pass_through_is_above_activation_line(const MathCore::vec2f &point, float radius, float offset_above) const
            {
                float dst = Core::Line2D::pointDistanceToLine(point, pass_through_activate_line);
                // printf("is_above: %d\n", (int)(dst > radius));
                return dst > radius + offset_above;
            }

            bool Structure2D::pass_through_is_below_or_touching_deactivation_line(const MathCore::vec2f &point, float radius, float offset_below) const
            {
                float dst = Core::Line2D::pointDistanceToLine(point, pass_through_deactivate_line);
                // printf("is_below: %d\n", (int)(dst > 0));
                return dst > -radius + offset_below;
            }

            Structure2D::Structure2D(int segment_count) : tag{'\0'}, segments(segment_count)
            {
                id = STRUCTURE2D_ID_INVALID;
                friction = 0.0f;
                type = StructureType::None;
                pass_through_set = false;
                always_check = false;
                // pass_through_is_active = true;
            }

            // bool Structure2D::shouldPassThrough(const MathCore::vec2f &move_direction) const
            // {
            //     using namespace MathCore;
            //     if (OP<vec2f>::sqrLength(pass_through_direction) < 1e-12f)
            //         return false;
            //     return OP<vec2f>::dot(move_direction, pass_through_direction) > 0.0f;
            // }

            std::shared_ptr<Structure2D> Structure2D::setAlwaysCheck(bool value)
            {
                always_check = value;
                return self<Structure2D>();
            }

            std::shared_ptr<Structure2D> Structure2D::FromSegmentPassThrough(
                const char *tag,
                float friction,
                const Core::Segment2D &segment,
                // will calculate the passthrough normal in the same direction,
                // but with 90 degree against the segment
                const MathCore::vec2f &pass_through_normal_hint)
            {
                auto result = Structure2D::CreateShared(1);

                result->friction = friction;
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::Segment;
                result->segments[0] = segment;

                result->computeBox();

                result->computePassThroughLines(pass_through_normal_hint);

                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromSegment(
                const char *tag,
                float friction,
                const Core::Segment2D &segment)
            {
                auto result = Structure2D::CreateShared(1);

                result->friction = friction;
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::Segment;
                result->segments[0] = segment;

                result->computeBox();

                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromSegmentList(
                const char *tag,
                float friction,
                const std::vector<Core::Segment2D> &segments)
            {
                auto result = Structure2D::CreateShared((int)segments.size());

                result->friction = friction;
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::Segment;
                result->segments.assign(segments.begin(), segments.end());

                result->computeBox();

                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromClosedPolygon(
                const char *tag,
                float friction,
                const std::vector<MathCore::vec2f> &vertices)
            {
                auto result = Structure2D::CreateShared((int)vertices.size());

                result->friction = friction;
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::ClosedPolygon;

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
                        result->segments[i] = Core::Segment2D(p1, p2);
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
                        result->segments[i] = Core::Segment2D(p1, p2);
                    }
                }

                result->computeBox();

                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromBoxCenterSize(
                const char *tag,
                float friction,
                const MathCore::vec2f &center,
                const MathCore::vec2f &size)
            {

                auto result = Structure2D::CreateShared(4);

                result->friction = friction;
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::Box;

                MathCore::vec2f half_size = size * 0.5f;

                result->box = Core::Box2D(center - half_size, center + half_size);

                result->segments[0] = Core::Segment2D(result->box.min, MathCore::vec2f(result->box.max.x, result->box.min.y));
                result->segments[1] = Core::Segment2D(MathCore::vec2f(result->box.max.x, result->box.min.y), result->box.max);
                result->segments[2] = Core::Segment2D(result->box.max, MathCore::vec2f(result->box.min.x, result->box.max.y));
                result->segments[3] = Core::Segment2D(MathCore::vec2f(result->box.min.x, result->box.max.y), result->box.min);

                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromBoxMinMax(
                const char *tag,
                float friction,
                const MathCore::vec2f &min,
                const MathCore::vec2f &max)
            {

                auto result = Structure2D::CreateShared(4);

                result->friction = friction;
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::Box;

                result->box = Core::Box2D(min, max);

                result->segments[0] = Core::Segment2D(result->box.min, MathCore::vec2f(result->box.max.x, result->box.min.y));
                result->segments[1] = Core::Segment2D(MathCore::vec2f(result->box.max.x, result->box.min.y), result->box.max);
                result->segments[2] = Core::Segment2D(result->box.max, MathCore::vec2f(result->box.min.x, result->box.max.y));
                result->segments[3] = Core::Segment2D(MathCore::vec2f(result->box.min.x, result->box.max.y), result->box.min);

                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromCircle(
                const char *tag,
                float friction,
                const MathCore::vec2f &center,
                float radius,
                int segment_count)
            {

                auto result = Structure2D::CreateShared(segment_count);

                result->friction = friction;
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::Circle;

                result->box.wrapCircle(center, radius);
                result->circle_radius = radius;

                // iterate segment count
                for (int i = 0; i < segment_count; i++)
                {
                    const float _2pi = 2.0f * MathCore::CONSTANT<float>::PI;
                    float angle0 = (float)(i) / segment_count * _2pi;
                    float angle1 = (float)((i + 1) % segment_count) / segment_count * _2pi;

                    MathCore::vec2f p1 = center + MathCore::vec2f(std::cos(angle0), std::sin(angle0)) * radius;
                    MathCore::vec2f p2 = center + MathCore::vec2f(std::cos(angle1), std::sin(angle1)) * radius;

                    result->segments[i] = Core::Segment2D(p1, p2);
                }

                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromCircleTol(
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

            std::shared_ptr<Structure2D> Structure2D::FromSegmentTrigger(
                const char *tag,
                const Core::Segment2D &segment)
            {
                auto result = Structure2D::CreateShared(1);
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::SegmentTrigger;
                result->segments[0] = segment;
                result->computeBox();
                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromSegmentListTrigger(
                const char *tag,
                const std::vector<Core::Segment2D> &segments)
            {
                auto result = Structure2D::CreateShared((int)segments.size());
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::SegmentTrigger;
                result->segments.assign(segments.begin(), segments.end());
                result->computeBox();
                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromBoxCenterSizeTrigger(
                const char *tag,
                const MathCore::vec2f &center,
                const MathCore::vec2f &size)
            {

                auto result = Structure2D::CreateShared(0);
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::BoxTrigger;
                MathCore::vec2f half_size = size * 0.5f;
                result->box = Core::Box2D(center - half_size, center + half_size);
                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromBoxMinMaxTrigger(
                const char *tag,
                const MathCore::vec2f &min,
                const MathCore::vec2f &max)
            {
                auto result = Structure2D::CreateShared(0);
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::BoxTrigger;
                result->box = Core::Box2D(min, max);
                return result;
            }

            std::shared_ptr<Structure2D> Structure2D::FromCircleTrigger(
                const char *tag,
                const MathCore::vec2f &center,
                float radius)
            {
                auto result = Structure2D::CreateShared(0);
                snprintf(result->tag, 32, "%s", tag);
                result->type = StructureType::CircleTrigger;
                result->box.wrapCircle(center, radius);
                result->circle_radius = radius;
                return result;
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

            bool Structure2D::checkBoxOverlap(const Core::Box2D &other) const
            {
                return checkBoxOverlap(other.min, other.max);
            }

            bool Structure2D::checkBoxOverlap(const MathCore::vec2f &min, const MathCore::vec2f &max) const
            {
                if (!Util::QuadtreeNode::box_overlaps(box.min, box.max, min, max))
                    return false;

                if (type == StructureType::Box || type == StructureType::BoxTrigger || pass_through_set)
                {
                    return true;
                }
                else if (type == StructureType::Circle || type == StructureType::CircleTrigger)
                {
                    // check if the closest point in the box to the circle center is within the circle radius
                    MathCore::vec2f circle_center = box.getCenter();
                    MathCore::vec2f closest_point = Core::Box2D::closestPointToBox(circle_center, min, max);
                    return MathCore::OP<MathCore::vec2f>::sqrDistance(closest_point, circle_center) <= (circle_radius * circle_radius);
                }
                else if (type == StructureType::ClosedPolygon || type == StructureType::Segment || type == StructureType::SegmentTrigger)
                {
                    // check if any of the polygon edges intersect with the box
                    for (const Core::Segment2D &segment : segments)
                    {
                        auto segment_box = Core::Box2D(segment.a, segment.b);
                        if (!Util::QuadtreeNode::box_overlaps(segment_box.min, segment_box.max, min, max))
                            continue; // skip segments that are completely outside the box
                        if (segment.intersectsBox(min, max))
                            return true;
                    }
                }
                return false;
            }
        }
    }
}
#if defined(_WIN32)
#pragma warning(pop)
#endif
