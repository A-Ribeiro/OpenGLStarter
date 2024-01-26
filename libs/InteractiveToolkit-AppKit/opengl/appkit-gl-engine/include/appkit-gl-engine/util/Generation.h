#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
// #include <appkit-gl-engine/Component.h>
// #include <appkit-gl-engine/Transform.h>
// #include <appkit-gl-engine/Engine.h>
// #include <appkit-gl-engine/Interpolators.h>
// #include <aRibeiroData/Animation.h>

namespace AppKit
{
    namespace GLEngine
    {

        // golden ratio: (1 + sqrt(5)) / 2
        // const float PHI = 1.618033988749895f;

        class PatternPoints
        {
        public:
            // https://stackoverflow.com/questions/28567166/uniformly-distribute-x-points-inside-a-circle
            static std::vector<MathCore::vec3f> sunFlower(int n, int alpha = 0, bool geodesic = false);
            static void optimize2DAccess(std::vector<MathCore::vec3f> *points, int texturesize, bool radial);
        };

    }

}