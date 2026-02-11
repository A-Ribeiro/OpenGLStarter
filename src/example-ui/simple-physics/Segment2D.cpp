#pragma once

#include "Segment2D.h"

namespace SimplePhysics
{

    Segment2D::Segment2D()
    {
    }

    Segment2D::Segment2D(const MathCore::vec2f &a, const MathCore::vec2f &b)
    {
        this->a = a;
        this->b = b;
    }
}