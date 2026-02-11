#pragma once

#include "Structure2D.h"

namespace SimplePhysics
{
    void Structure2D::computeBox()
    {
        box.makeEmpty();

        for (const Segment2D &segment : segments)
            box.wrapLine(segment.a, segment.b);
    }

    Structure2D::Structure2D(int segment_count) : tag{'\0'}, segments(segment_count)
    {
        friction = 0.0f;
        type = StructureType::None;
    }

    Structure2D Structure2D::FromSegment(
        const char *tag,
        float friction,
        const Segment2D &segment)
    {
        Structure2D result(1);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Segment;
        result.segments[0] = segment;

        result.computeBox();

        return result;
    }

    Structure2D Structure2D::FromSegmentList(
        const char *tag,
        float friction,
        const std::vector<Segment2D> &segments)
    {
        Structure2D result(segments.size());

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Segment;
        result.segments.assign(segments.begin(), segments.end());

        result.computeBox();

        return result;
    }

    Structure2D Structure2D::FromBoxCenterSize(
        const char *tag,
        float friction,
        const MathCore::vec2f &center,
        const MathCore::vec2f &size)
    {

        Structure2D result(4);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Box;

        MathCore::vec2f half_size = size * 0.5f;

        result.box = Box2D(center - half_size, center + half_size);

        result.segments[0] = Segment2D{result.box.min, MathCore::vec2f(result.box.max.x, result.box.min.y)};
        result.segments[1] = Segment2D{MathCore::vec2f(result.box.max.x, result.box.min.y), result.box.max};
        result.segments[2] = Segment2D{result.box.max, MathCore::vec2f(result.box.min.x, result.box.max.y)};
        result.segments[3] = Segment2D{MathCore::vec2f(result.box.min.x, result.box.max.y), result.box.min};

        return result;
    }

    Structure2D Structure2D::FromBoxMinMax(
        const char *tag,
        float friction,
        const MathCore::vec2f &min,
        const MathCore::vec2f &max)
    {

        Structure2D result(4);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Box;

        result.box = Box2D(min, max);

        result.segments[0] = Segment2D{result.box.min, MathCore::vec2f(result.box.max.x, result.box.min.y)};
        result.segments[1] = Segment2D{MathCore::vec2f(result.box.max.x, result.box.min.y), result.box.max};
        result.segments[2] = Segment2D{result.box.max, MathCore::vec2f(result.box.min.x, result.box.max.y)};
        result.segments[3] = Segment2D{MathCore::vec2f(result.box.min.x, result.box.max.y), result.box.min};

        return result;
    }

    Structure2D Structure2D::FromCircle(
        const char *tag,
        float friction,
        const MathCore::vec2f &center,
        float radius,
        int segment_count)
    {

        Structure2D result(segment_count);

        result.friction = friction;
        strncpy(result.tag, tag, sizeof(result.tag) - 1);
        result.type = StructureType::Circle;

        result.box.wrapCircle(center, radius);
        result.circle_radius = radius;

        // iterate segment count
        for (int i = 0; i < segment_count; i++)
        {
            const float _2pi = 2.0f * MathCore::CONSTANT<float>::PI;
            float angle0 = (float)(i) / segment_count * _2pi;
            float angle1 = (float)((i + 1) % segment_count) / segment_count * _2pi;

            MathCore::vec2f p1 = center + MathCore::vec2f(std::cos(angle0), std::sin(angle0)) * radius;
            MathCore::vec2f p2 = center + MathCore::vec2f(std::cos(angle1), std::sin(angle1)) * radius;

            result.segments[i] = Segment2D{p1, p2};
        }

        return result;
    }

    Structure2D Structure2D::FromCircleTol(
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

    MathCore::vec2f Structure2D::getCenter() const
    {
        return box.getCenter();
    }
    MathCore::vec2f Structure2D::getSize() const
    {
        return box.getSize();
    }

}