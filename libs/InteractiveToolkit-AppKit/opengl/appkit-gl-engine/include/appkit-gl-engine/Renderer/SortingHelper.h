#pragma once

#include <appkit-gl-engine/Components/ComponentLight.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
#include <appkit-gl-engine/Components/ComponentParticleSystem.h>

#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>
#include <InteractiveToolkit/AlgorithmCore/Sorting/RadixCountingSort.h>

namespace AppKit
{
    namespace GLEngine
    {

        enum SortingModeEnum
        {
            SortingMode_Asc,
            SortingMode_Desc,
        };

        class SortingHelper
        {

            std::vector<AlgorithmCore::Sorting::SortIndexu32> sort_u32;
            std::vector<AlgorithmCore::Sorting::SortIndexu32> sort_u32_tmp;

            std::vector<Components::ComponentParticleSystem *> particleSystem_tmp;
            std::vector<Transform *> transform_tmp;

        public:
            void sort_by_z(std::vector<Transform *> &v, SortingModeEnum mode);
            void sort_by_direction(std::vector<Transform *> &v, const MathCore::vec3f &dir, SortingModeEnum mode);
            void sort_by_direction(std::vector<Components::ComponentParticleSystem *> &v, const MathCore::vec3f &dir, SortingModeEnum mode);

            std::vector<AlgorithmCore::Sorting::SortIndexu32> &sort_by_direction(const std::vector<Components::Particle> &particles, const MathCore::vec3f &dir, SortingModeEnum mode);
        };

    }

}