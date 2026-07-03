#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-physics/core/Box2D.h>
#include <appkit-physics/core/Line2D.h>
#include <appkit-physics/core/Segment2D.h>

#include <InteractiveToolkit/EventCore/Event.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {

            class Structure2D;

            enum class TriggerProbeType : uint8_t
            {
                None,

                Segment,
                Box,
                Circle,
            };

            class TriggerProbe
            {
                void computeSegmentBox();

                // void computePassThroughLines(const MathCore::vec2f &pass_through_normal_hint);

                TriggerProbe(int segment_count = 0);

            public:
                TriggerProbeType type;

                std::vector<Core::Segment2D> segments;

                MathCore::vec2f offset;
                std::vector<Core::Segment2D> segments_offset_applied;

                Core::Box2D box;
                Core::Box2D box_offset_applied;

                // only valid for circle type
                float circle_radius;

                std::unordered_map<std::shared_ptr<Structure2D>, bool> active_structures;
                std::unordered_map<std::shared_ptr<Structure2D>, bool> current_structures;
                EventCore::Event<void(std::shared_ptr<Structure2D> structureTrigger)> onEnter;
                EventCore::Event<void(std::shared_ptr<Structure2D> structureTrigger)> onExit;

                void setOffset(const MathCore::vec2f &offset_);

                static std::shared_ptr<TriggerProbe> FromSegment(
                    const MathCore::vec2f &offset,
                    const Core::Segment2D &segment);

                static std::shared_ptr<TriggerProbe> FromSegmentList(
                    const MathCore::vec2f &offset,
                    const std::vector<Core::Segment2D> &segments);

                static std::shared_ptr<TriggerProbe> FromBoxCenterSize(
                    const MathCore::vec2f &offset,
                    const MathCore::vec2f &center,
                    const MathCore::vec2f &size);

                static std::shared_ptr<TriggerProbe> FromBoxMinMax(
                    const MathCore::vec2f &offset,
                    const MathCore::vec2f &min,
                    const MathCore::vec2f &max);

                static std::shared_ptr<TriggerProbe> FromCircle(
                    const MathCore::vec2f &offset,
                    const MathCore::vec2f &center,
                    float radius);

                MathCore::vec2f getCenter() const;
                MathCore::vec2f getSize() const;

                // // valid for: Box, Circle, and ClosedPolygon
                // bool checkPointInside(const MathCore::vec2f &point) const;
                // bool checkBoxOverlap(const Core::Box2D &other) const;
                // bool checkBoxOverlap(const MathCore::vec2f &min, const MathCore::vec2f &max) const;

                void startOverlapCheck();
                void checkStructureOverlap(std::shared_ptr<Structure2D> structureTrigger);
                void endOverlapCheck();

                ITK_DECLARE_CREATE_SHARED(TriggerProbe)

            };
        }
    }
}