
#include <appkit-physics/container/TriggerProbe.h>
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
            void TriggerProbe::computeSegmentBox()
            {
                box.makeEmpty();

                for (const Core::Segment2D &segment : segments)
                    box.wrapLine(segment.a, segment.b);
            }

            TriggerProbe::TriggerProbe(int segment_count) : segments(segment_count)
            {
                type = TriggerProbeType::None;
            }

            void TriggerProbe::setOffset(const MathCore::vec2f &offset_)
            {
                offset = offset_;
                setCharacterOffset(character_offset);
                // if (type == TriggerProbeType::Segment)
                // {
                //     segments_offset_applied.clear();
                //     segments_offset_applied.reserve(segments.size());
                //     for (const Core::Segment2D &segment : segments)
                //         segments_offset_applied.push_back(Core::Segment2D(segment.a + offset, segment.b + offset));
                // }
                
                // if (type == TriggerProbeType::Segment || type == TriggerProbeType::Box || type == TriggerProbeType::Circle)
                // {
                //     box_offset_applied = box;
                //     box_offset_applied.min += offset;
                //     box_offset_applied.max += offset;
                // }
            }

            void TriggerProbe::setCharacterOffset(const MathCore::vec2f &offset_)
            {
                character_offset = offset_;

                auto total_offset = offset + character_offset;

                if (type == TriggerProbeType::Segment)
                {
                    segments_offset_applied.clear();
                    segments_offset_applied.reserve(segments.size());
                    for (const Core::Segment2D &segment : segments)
                        segments_offset_applied.push_back(Core::Segment2D(segment.a + total_offset, segment.b + total_offset));
                }
                
                if (type == TriggerProbeType::Segment || type == TriggerProbeType::Box || type == TriggerProbeType::Circle)
                {
                    box_offset_applied = box;
                    box_offset_applied.min += total_offset;
                    box_offset_applied.max += total_offset;
                }
            }

            std::shared_ptr<TriggerProbe> TriggerProbe::FromSegment(
                const MathCore::vec2f &offset,
                const Core::Segment2D &segment)
            {
                auto result = TriggerProbe::CreateShared(1);

                result->type = TriggerProbeType::Segment;
                result->segments[0] = segment;
                result->computeSegmentBox();
                result->setOffset(offset);

                return result;
            }

            std::shared_ptr<TriggerProbe> TriggerProbe::FromSegmentList(
                const MathCore::vec2f &offset,
                const std::vector<Core::Segment2D> &segments)
            {
                auto result = TriggerProbe::CreateShared((int)segments.size());

                result->type = TriggerProbeType::Segment;
                result->segments.assign(segments.begin(), segments.end());
                result->computeSegmentBox();
                result->setOffset(offset);

                return result;
            }

            std::shared_ptr<TriggerProbe> TriggerProbe::FromBoxCenterSize(
                const MathCore::vec2f &offset,
                const MathCore::vec2f &center,
                const MathCore::vec2f &size)
            {
                auto result = TriggerProbe::CreateShared(0);

                result->type = TriggerProbeType::Box;
                MathCore::vec2f half_size = size * 0.5f;
                result->box = Core::Box2D(center - half_size, center + half_size);
                result->setOffset(offset);

                return result;
            }

            std::shared_ptr<TriggerProbe> TriggerProbe::FromBoxMinMax(
                const MathCore::vec2f &offset,
                const MathCore::vec2f &min,
                const MathCore::vec2f &max)
            {
                auto result = TriggerProbe::CreateShared(0);

                result->setOffset(offset);
                result->type = TriggerProbeType::Box;
                result->box = Core::Box2D(min, max);
                result->setOffset(offset);

                return result;
            }

            std::shared_ptr<TriggerProbe> TriggerProbe::FromCircle(
                const MathCore::vec2f &offset,
                const MathCore::vec2f &center,
                float radius)
            {
                auto result = TriggerProbe::CreateShared(0);

                result->type = TriggerProbeType::Circle;
                result->box.wrapCircle(center, radius);
                result->circle_radius = radius;
                result->setOffset(offset);

                return result;
            }

            MathCore::vec2f TriggerProbe::getCenter() const
            {
                return box.getCenter();
            }
            MathCore::vec2f TriggerProbe::getSize() const
            {
                return box.getSize();
            }

            void TriggerProbe::startOverlapCheck() {
                // active_structures.clear();
                // current_structures.clear();
            }

            void TriggerProbe::endOverlapCheck() {
                // for (const auto &pair : active_structures) {
                //     if (pair.second) {
                //         onExit(pair.first);
                //     }
                // }
            }

            void TriggerProbe::checkStructureOverlap(std::shared_ptr<Structure2D> structureTrigger)
            {
                // if (type == StructureType::Box)
                // {
                //     return box.isPointInside(point);
                // }
                // else if (type == StructureType::Circle)
                // {
                //     return MathCore::OP<MathCore::vec2f>::sqrDistance(box.getCenter(), point) <= (circle_radius * circle_radius);
                // }
                // else if (type == StructureType::ClosedPolygon)
                // {
                //     if (!box.isPointInside(point))
                //         return false;

                //     bool inside = false;
                //     size_t count = segments.size();
                //     if (count < 3)
                //         return false; // A polygon must have at least 3 points
                //     size_t prev_i = count - 1;
                //     for (size_t i = 0; i < count; i++)
                //     {
                //         const MathCore::vec2f &pt_curr = segments[i].a;
                //         const MathCore::vec2f &pt_prev = segments[prev_i].a;
                //         // check if the point is within the y-bounds of the polygon edge
                //         if ((pt_curr.y > point.y) != (pt_prev.y > point.y))
                //         {
                //             if (pt_curr.x > point.x && pt_prev.x > point.x)
                //             {
                //                 // both points are to the right of the point
                //                 // the edge is always to the right of the point
                //                 inside = !inside;
                //             }
                //             else
                //             {
                //                 // if pt_curr is at bottom
                //                 // if side is positive, the point is to the left of the edge (edge is at right)
                //                 // if side is negative, the point is to the right of the edge (edge is at left)

                //                 // if pt_prev is at bottom
                //                 // the side will be reversed
                //                 float side = MathCore::OP<MathCore::vec2f>::orientation(pt_curr, pt_prev, point);

                //                 // if prev is at bottom
                //                 // flip the side
                //                 if (pt_prev.y < pt_curr.y)
                //                     side = -side;

                //                 if (side > 0)
                //                     inside = !inside;
                //             }
                //         }
                //         prev_i = i;
                //     }
                //     return inside;
                // }
                // return false;
            }

        }
    }
}
#if defined(_WIN32)
#pragma warning(pop)
#endif
