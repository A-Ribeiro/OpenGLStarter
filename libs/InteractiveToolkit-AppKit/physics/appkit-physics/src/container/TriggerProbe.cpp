
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

            void TriggerProbe::startOverlapCheck()
            {
                // active_structures.clear();
                current_structures.clear();
            }

            void TriggerProbe::endOverlapCheck()
            {
                // for active_structures not in current_structures
                for (auto it = active_structures.begin(); it != active_structures.end(); ++it)
                {
                    auto structure_ptr = it->first;
                    if (current_structures.find(structure_ptr) == current_structures.end())
                    {
                        // not found, trigger exit
                        onExit(structure_ptr);
                    }
                }

                // for current_structures not in active_structures
                for (auto it = current_structures.begin(); it != current_structures.end(); ++it)
                {
                    auto structure_ptr = it->first;
                    if (active_structures.find(structure_ptr) == active_structures.end())
                    {
                        // not found, trigger enter
                        onEnter(structure_ptr);
                    }
                }

                std::swap(active_structures, current_structures);
            }

            void TriggerProbe::checkStructureOverlap(std::shared_ptr<Structure2D> structureTrigger)
            {
                if (structureTrigger->type == StructureType::BoxTrigger)
                {
                    if (type == TriggerProbeType::Box)
                    {
                        if (box_offset_applied.overlaps(structureTrigger->box))
                            current_structures[structureTrigger] = true;
                    }
                    else if (type == TriggerProbeType::Circle)
                    {
                        auto center = box_offset_applied.getCenter();
                        auto structure_closes_pt_box = structureTrigger->box.closestPoint(center);
                        float dst_to_center = MathCore::OP<MathCore::vec2f>::sqrDistance(structure_closes_pt_box, center);
                        if (dst_to_center <= (circle_radius * circle_radius))
                            current_structures[structureTrigger] = true;
                    }
                    else if (type == TriggerProbeType::Segment)
                    {
                        const auto &struct_box = structureTrigger->box;
                        for (const Core::Segment2D &probe_segment : segments_offset_applied)
                        {
                            if (probe_segment.intersectsBox(struct_box))
                            {
                                current_structures[structureTrigger] = true;
                                break;
                            }
                        }
                    }
                }
                else if (structureTrigger->type == StructureType::CircleTrigger)
                {
                    if (type == TriggerProbeType::Box)
                    {
                        auto structure_center = structureTrigger->box.getCenter();
                        float structure_radius = structureTrigger->circle_radius;
                        auto closes_pt_box = box_offset_applied.closestPoint(structure_center);
                        float structure_dst_to_center = MathCore::OP<MathCore::vec2f>::sqrDistance(closes_pt_box, structure_center);

                        if (structure_dst_to_center <= (structure_radius * structure_radius))
                            current_structures[structureTrigger] = true;
                    }
                    else if (type == TriggerProbeType::Circle)
                    {
                        auto structure_center = structureTrigger->box.getCenter();
                        float structure_radius = structureTrigger->circle_radius;
                        auto center = box_offset_applied.getCenter();
                        float total_radius = structure_radius + circle_radius;

                        float dst_center_to_center = MathCore::OP<MathCore::vec2f>::sqrDistance(structure_center, center);
                        if (dst_center_to_center <= (total_radius * total_radius))
                            current_structures[structureTrigger] = true;
                    }
                    else if (type == TriggerProbeType::Segment)
                    {
                        auto structure_center = structureTrigger->box.getCenter();
                        float structure_radius = structureTrigger->circle_radius;
                        float structure_radius_sq = structure_radius * structure_radius;
                        for (const Core::Segment2D &probe_segment : segments_offset_applied)
                        {
                            auto closest_pt = probe_segment.closestPoint(structure_center);
                            float dst_to_center = MathCore::OP<MathCore::vec2f>::sqrDistance(closest_pt, structure_center);
                            if (dst_to_center <= structure_radius_sq)
                            {
                                current_structures[structureTrigger] = true;
                                break;
                            }
                        }
                    }
                }
                else if (structureTrigger->type == StructureType::SegmentTrigger)
                {
                    if (type == TriggerProbeType::Box)
                    {
                        for (const Core::Segment2D &struct_segment : structureTrigger->segments)
                        {
                            if (struct_segment.intersectsBox(box_offset_applied))
                            {
                                current_structures[structureTrigger] = true;
                                break;
                            }
                        }
                    }
                    else if (type == TriggerProbeType::Circle)
                    {
                        auto center = box_offset_applied.getCenter();
                        float circle_radius_sq = circle_radius * circle_radius;

                        for (const Core::Segment2D &struct_segment : structureTrigger->segments)
                        {
                            auto closest_pt = struct_segment.closestPoint(center);
                            float dst_to_center = MathCore::OP<MathCore::vec2f>::sqrDistance(closest_pt, center);
                            if (dst_to_center <= circle_radius_sq)
                            {
                                current_structures[structureTrigger] = true;
                                break;
                            }
                        }
                    }
                    else if (type == TriggerProbeType::Segment)
                    {
                        for (const Core::Segment2D &probe_segment : segments_offset_applied)
                        {
                            for (const Core::Segment2D &struct_segment : structureTrigger->segments)
                            {
                                if (probe_segment.intersects(struct_segment))
                                {
                                    current_structures[structureTrigger] = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

        }
    }
}
#if defined(_WIN32)
#pragma warning(pop)
#endif
