#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include "Segment2D.h"
#include "Box2D.h"
#include "Structure2D.h"

namespace SimplePhysics
{
    class PhysicsContainer
    {
    public:
        std::vector<Structure2D> static_structures;
        std::vector<Structure2D> dynamic_structures;
    };

}