#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-physics/core/Box2D.h>
#include <appkit-physics/core/Line2D.h>
#include <appkit-physics/core/Segment2D.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {

            enum class StructureType : uint8_t
            {
                None,
                Segment,
                Box,
                Circle,
                ClosedPolygon
            };

            class Structure2D
            {
                void computeBox();

                void computePassThroughLines(const MathCore::vec2f &pass_through_normal_hint);

            public:
                struct QuadtreeIntegration
                {
                    using type = std::unique_ptr<Structure2D>;
                    ITK_INLINE static bool CheckBoxOverlap(const type &item, const MathCore::vec2f &min, const MathCore::vec2f &max) { return item->checkBoxOverlap(min, max); }
                    ITK_INLINE static const MathCore::vec2f &GetBoxMin(const type &item) { return item->box.min; }
                    ITK_INLINE static const MathCore::vec2f &GetBoxMax(const type &item) { return item->box.max; }
                };
                float friction;
                char tag[32];

                StructureType type;

                std::vector<Core::Segment2D> segments;

                Core::Box2D box;

                // only valid for circle type
                float circle_radius;

                bool pass_through_set;

                // bool pass_through_is_active;

                Core::Line2D pass_through_activate_line;
                Core::Line2D pass_through_deactivate_line;

                // Core::Line2D pass_through_left_bound_line;
                // Core::Line2D pass_through_right_bound_line;

                bool always_check;

                uint32_t id;

                // bool pass_through_is_inside_or_touching_left_right_bound(const MathCore::vec2f &point, float radius) const;
                bool pass_through_is_above_activation_line(const MathCore::vec2f &point, float radius) const;
                bool pass_through_is_below_or_touching_deactivation_line(const MathCore::vec2f &point, float radius) const;

                Structure2D(int segment_count = 0);

                Structure2D &setAlwaysCheck(bool value);

                static Structure2D FromSegmentPassThrough(
                    const char *tag,
                    float friction,
                    const Core::Segment2D &segment,
                    // will calculate the passthrough normal in the same direction,
                    // but with 90 degree against the segment
                    const MathCore::vec2f &pass_through_normal_hint);

                static Structure2D FromSegment(
                    const char *tag,
                    float friction,
                    const Core::Segment2D &segment);

                static Structure2D FromSegmentList(
                    const char *tag,
                    float friction,
                    const std::vector<Core::Segment2D> &segments);

                static Structure2D FromClosedPolygon(
                    const char *tag,
                    float friction,
                    const std::vector<MathCore::vec2f> &vertices);

                static Structure2D FromBoxCenterSize(
                    const char *tag,
                    float friction,
                    const MathCore::vec2f &center,
                    const MathCore::vec2f &size);

                static Structure2D FromBoxMinMax(
                    const char *tag,
                    float friction,
                    const MathCore::vec2f &min,
                    const MathCore::vec2f &max);

                static Structure2D FromCircle(
                    const char *tag,
                    float friction,
                    const MathCore::vec2f &center,
                    float radius,
                    int segment_count);

                static Structure2D FromCircleTol(
                    const char *tag,
                    float friction,
                    const MathCore::vec2f &center,
                    float radius,
                    float tolerance);

                MathCore::vec2f getCenter() const;
                MathCore::vec2f getSize() const;

                // valid for: Box, Circle, and ClosedPolygon
                bool checkPointInside(const MathCore::vec2f &point) const;
                bool checkBoxOverlap(const Core::Box2D &other) const;
                bool checkBoxOverlap(const MathCore::vec2f &min, const MathCore::vec2f &max) const;
            };
        }
    }
}