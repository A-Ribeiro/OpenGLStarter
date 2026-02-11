#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace SimplePhysics
{

    class Segment2D
    {
    public:
        MathCore::vec2f a;
        MathCore::vec2f b;

        Segment2D();
        Segment2D(const MathCore::vec2f &a, const MathCore::vec2f &b);
    };

}