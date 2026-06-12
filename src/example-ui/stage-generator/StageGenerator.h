#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/ITKCommon/Random.h>
#include <vector>

#include <appkit-physics/container/Container2D.h>

// namespace SimplePhysics
// {
//     class PhysicsContainer;
// }

namespace StageGen
{

    struct StageParams
    {
        float normal_jump_height = 300.0f;
        float double_jump_height = 200.0f;
        float player_radius = 50.0f;
        int stage_length_screens = 5; // number of 1920x1080 screen blocks

        // derived helpers
        float screenWidth() const { return 1920.0f; }
        float screenHeight() const { return 1080.0f; }
        float totalWidth() const { return screenWidth() * stage_length_screens; }
        float totalHeight() const { return screenHeight(); }
    };

    struct StageResult
    {
        MathCore::vec2f start_point;
        MathCore::vec2f end_point;
    };

    class StageGenerator
    {
    public:
        /// Generates a random platformer stage and populates the physicsContainer.
        /// Returns the start and end points for the player.
        static StageResult generate(
            AppKit::Physics::Container::Container2D &container,
            const StageParams &params,
            ITKCommon::Random32 &rng);

    private:
        struct PlatformPlacement
        {
            MathCore::vec2f position; // center of the platform
            float width;
            float height;
            bool is_box; // true = box, false = segment
        };

        static void addFloorAndWalls(
            AppKit::Physics::Container::Container2D &container,
            const StageParams &params);

        static void addStartAndEndPlatforms(
            AppKit::Physics::Container::Container2D &container,
            const StageParams &params,
            MathCore::vec2f &out_start,
            MathCore::vec2f &out_end);

        static void addPlatformSequence(
            AppKit::Physics::Container::Container2D &container,
            const StageParams &params,
            MathCore::MathRandomExt<ITKCommon::Random32> &mathRnd,
            const MathCore::vec2f &start,
            const MathCore::vec2f &end);

        static void addTestPlatformSequence(
            AppKit::Physics::Container::Container2D &container,
            const StageParams &params,
            MathCore::MathRandomExt<ITKCommon::Random32> &mathRnd,
            const MathCore::vec2f &start,
            const MathCore::vec2f &end);

        static void addDecorativeBoxes(
            AppKit::Physics::Container::Container2D &container,
            const StageParams &params,
            MathCore::MathRandomExt<ITKCommon::Random32> &mathRnd);

        static bool overlapsExisting(
            const std::vector<PlatformPlacement> &placed,
            const MathCore::vec2f &center,
            float w,
            float h,
            float margin);
    };

}
