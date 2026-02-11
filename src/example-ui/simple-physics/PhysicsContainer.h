#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

class Segment2D
{
public:
    MathCore::vec2f a;
    MathCore::vec2f b;
};

enum class StructureType : uint8_t
{
    None,
    Segment,
    Box,
    Circle
};

class Structure
{
    void computeBox()
    {
        box_min = MathCore::vec2f(MathCore::FloatTypeInfo<float>::max);
        box_max = MathCore::vec2f(-MathCore::FloatTypeInfo<float>::max);

        for (const Segment2D &segment : segments)
        {
            box_min = MathCore::OP<MathCore::vec2f>::minimum(box_min, segment.a);
            box_max = MathCore::OP<MathCore::vec2f>::maximum(box_max, segment.a);

            box_min = MathCore::OP<MathCore::vec2f>::minimum(box_min, segment.b);
            box_max = MathCore::OP<MathCore::vec2f>::maximum(box_max, segment.b);
        }
    }

public:
    float friction;
    char tag[32];

    StructureType type;

    std::vector<Segment2D> segments;

    MathCore::vec2f box_min;
    MathCore::vec2f box_max;

    // only valid for circle type
    float circle_radius;

    Structure(int segment_count = 0) : tag{'\0'}, segments(segment_count)
    {
        friction = 0.0f;
        type = StructureType::None;
    }

    static Structure FromSegment(
        const char *tag,
        float friction,
        const Segment2D &segment)
    {
        Structure result(1);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Segment;
        result.segments[0] = segment;

        result.computeBox();

        return result;
    }


    static Structure FromSegmentList(
        const char *tag,
        float friction,
        const std::vector<Segment2D> &segments)
    {
        Structure result(segments.size());

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Segment;
        result.segments.assign(segments.begin(), segments.end());

        result.computeBox();

        return result;
    }

    static Structure FromBoxCenterSize(
        const char *tag,
        float friction,
        const MathCore::vec2f &center,
        const MathCore::vec2f &size)
    {

        Structure result(4);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Box;

        MathCore::vec2f half_size = size * 0.5f;

        result.box_min = center - half_size;
        result.box_max = center + half_size;

        result.segments[0] = Segment2D{result.box_min, MathCore::vec2f(result.box_max.x, result.box_min.y)};
        result.segments[1] = Segment2D{MathCore::vec2f(result.box_max.x, result.box_min.y), result.box_max};
        result.segments[2] = Segment2D{result.box_max, MathCore::vec2f(result.box_min.x, result.box_max.y)};
        result.segments[3] = Segment2D{MathCore::vec2f(result.box_min.x, result.box_max.y), result.box_min};

        return result;
    }

    static Structure FromBoxMinMax(
        const char *tag,
        float friction,
        const MathCore::vec2f &min,
        const MathCore::vec2f &max)
    {

        Structure result(4);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Box;

        result.box_min = MathCore::OP<MathCore::vec2f>::minimum(min, max);
        result.box_max = MathCore::OP<MathCore::vec2f>::maximum(min, max);

        result.segments[0] = Segment2D{result.box_min, MathCore::vec2f(result.box_max.x, result.box_min.y)};
        result.segments[1] = Segment2D{MathCore::vec2f(result.box_max.x, result.box_min.y), result.box_max};
        result.segments[2] = Segment2D{result.box_max, MathCore::vec2f(result.box_min.x, result.box_max.y)};
        result.segments[3] = Segment2D{MathCore::vec2f(result.box_min.x, result.box_max.y), result.box_min};

        return result;
    }

    static Structure FromCircle(
        const char *tag,
        float friction,
        const MathCore::vec2f &center,
        float radius,
        int segment_count = 32)
    {

        Structure result(segment_count);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Circle;

        MathCore::vec2f half_size = MathCore::vec2f(radius, radius);

        result.box_min = center - half_size;
        result.box_max = center + half_size;

        result.circle_radius = radius;

        // iterate segment count
        for (int i = 0; i < segment_count; i++)
        {
            const float _2pi = 2.0f * MathCore::CONSTANT<float>::PI;
            float angle0 = (float)(i / segment_count) * _2pi;
            float angle1 = (float)(((i + 1) % segment_count) / segment_count) * _2pi;

            MathCore::vec2f p1 = center + MathCore::vec2f(std::cos(angle0), std::sin(angle0)) * radius;
            MathCore::vec2f p2 = center + MathCore::vec2f(std::cos(angle1), std::sin(angle1)) * radius;

            result.segments[i] = Segment2D{p1, p2};
        }

        return result;
    }

    MathCore::vec2f getCenter() const
    {
        return (box_min + box_max) * 0.5f;
    }
    MathCore::vec2f getSize() const
    {
        return box_max - box_min;
    }
};

class PhysicsContainer
{
public:
    std::vector<Segment2D> static_structures;

    std::vector<Segment2D> dynamic_structures;
};