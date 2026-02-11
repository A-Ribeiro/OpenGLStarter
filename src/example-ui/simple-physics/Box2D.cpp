#pragma once

#include "Box2D.h"

namespace SimplePhysics
{

    Box2D::Box2D()
    {
        makeEmpty();
    }

    Box2D::Box2D(const MathCore::vec2f &a, const MathCore::vec2f &b)
    {
        min = MathCore::OP<MathCore::vec2f>::minimum(a, b);
        max = MathCore::OP<MathCore::vec2f>::maximum(a, b);
    }

    void Box2D::wrapLine(const MathCore::vec2f &a, const MathCore::vec2f &b)
    {
        min = MathCore::OP<MathCore::vec2f>::minimum(a, b);
        max = MathCore::OP<MathCore::vec2f>::maximum(a, b);
    }

    void Box2D::wrapPoint(const MathCore::vec2f &point)
    {
        min = MathCore::OP<MathCore::vec2f>::minimum(min, point);
        max = MathCore::OP<MathCore::vec2f>::maximum(max, point);
    }

    void Box2D::wrapCircle(const MathCore::vec2f &center, float radius)
    {
        MathCore::vec2f point_min = center - MathCore::vec2f(radius, radius);
        MathCore::vec2f point_max = center + MathCore::vec2f(radius, radius);
        wrapPoint(point_min);
        wrapPoint(point_max);
    }

    void Box2D::makeEmpty()
    {
        min = MathCore::vec2f(MathCore::FloatTypeInfo<float>::max);
        max = MathCore::vec2f(-MathCore::FloatTypeInfo<float>::max);
    }

    MathCore::vec2f Box2D::getCenter() const
    {
        return (min + max) * 0.5f;
    }
    MathCore::vec2f Box2D::getSize() const
    {
        return max - min;
    }

}