#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace SimplePhysics
{

    class Box2D
    {
    public:
        Box2D();

        Box2D(const MathCore::vec2f &a, const MathCore::vec2f &b);
        
        void wrapLine(const MathCore::vec2f &a, const MathCore::vec2f &b);
        void wrapPoint(const MathCore::vec2f &point);
        void wrapCircle(const MathCore::vec2f &center, float radius);
        void makeEmpty();

        MathCore::vec2f getCenter() const;
        MathCore::vec2f getSize() const;

        MathCore::vec2f min;
        MathCore::vec2f max;
    };

}