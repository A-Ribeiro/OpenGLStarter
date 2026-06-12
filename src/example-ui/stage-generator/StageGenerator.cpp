#include "StageGenerator.h"
// #include "../simple-physics/PhysicsContainer.h"

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/ITKCommon/Random.h>

#include <algorithm>
#include <cmath>

#include <appkit-physics/core/Box2D.h>
#include <appkit-physics/core/Line2D.h>
#include <appkit-physics/core/Segment2D.h>


using namespace MathCore;
using namespace ITKCommon;
using namespace AppKit::Physics::Container;
using namespace AppKit::Physics::Core;

namespace StageGen
{

    // ----------------------------------------------------------------
    // Utility
    // ----------------------------------------------------------------

    bool StageGenerator::overlapsExisting(
        const std::vector<PlatformPlacement> &placed,
        const vec2f &center,
        float w,
        float h,
        float margin)
    {
        Box2D candidate(
            center - vec2f(w * 0.5f + margin, h * 0.5f + margin),
            center + vec2f(w * 0.5f + margin, h * 0.5f + margin));

        for (auto &p : placed)
        {
            Box2D existing(
                p.position - vec2f(p.width * 0.5f, p.height * 0.5f),
                p.position + vec2f(p.width * 0.5f, p.height * 0.5f));
            if (candidate.overlaps(existing))
                return true;
        }
        return false;
    }

    // ----------------------------------------------------------------
    // Floor, ceiling and walls
    // ----------------------------------------------------------------

    void StageGenerator::addFloorAndWalls(
        Container2D &container,
        const StageParams &params)
    {
        return;
        float W = params.totalWidth();
        float H = params.totalHeight();
        float margin = 10.0f;

        // Floor segment spanning the full width at y = 0
        container.addStaticStructure(
            Structure2D::FromSegment("Floor", 0.8f,
                                     Segment2D(vec2f(-margin, 0), vec2f(W + margin, 0))));

        // Ceiling
        container.addStaticStructure(
            Structure2D::FromSegment("Ceiling", 0.2f,
                                     Segment2D(vec2f(-margin, H), vec2f(W + margin, H))));

        // Left wall
        container.addStaticStructure(
            Structure2D::FromSegment("Left Wall", 0.2f,
                                     Segment2D(vec2f(0, -margin), vec2f(0, H + margin))));

        // Right wall
        container.addStaticStructure(
            Structure2D::FromSegment("Right Wall", 0.2f,
                                     Segment2D(vec2f(W, -margin), vec2f(W, H + margin))));
    }

    // ----------------------------------------------------------------
    // Start and end platforms
    // ----------------------------------------------------------------
    // Start and end platforms
    // ----------------------------------------------------------------

    void StageGenerator::addStartAndEndPlatforms(
        Container2D &container,
        const StageParams &params,
        vec2f &out_start,
        vec2f &out_end)
    {
        float W = params.totalWidth();
        float platformWidth = 200.0f;
        float platformThickness = 30.0f;
        float floorY = 0.0f;
        float platformY = floorY + platformThickness * 0.5f;

        // Start platform: near the left edge, slightly elevated
        {
            vec2f center(params.player_radius + platformWidth * 0.5f + 20.0f, platformY);
            container.addStaticStructure(
                Structure2D::FromBoxCenterSize("Start Platform", 0.8f,
                                               center, vec2f(platformWidth, platformThickness)));
            out_start = vec2f(center.x, center.y + platformThickness * 0.5f + params.player_radius);
        }

        // End platform: near the right edge, slightly elevated
        {
            vec2f center(W - params.player_radius - platformWidth * 0.5f - 20.0f, platformY);
            container.addStaticStructure(
                Structure2D::FromBoxCenterSize("End Platform", 0.8f,
                                               center, vec2f(platformWidth, platformThickness)));
            out_end = vec2f(center.x, center.y + platformThickness * 0.5f + params.player_radius);
        }
    }

    // ----------------------------------------------------------------
    // Main platform sequence generation
    // ----------------------------------------------------------------

    void StageGenerator::addPlatformSequence(
        Container2D &container,
        const StageParams &params,
        MathRandomExt<Random32> &mathRnd,
        const vec2f &start,
        const vec2f &end)
    {
        float W = params.totalWidth();
        float H = params.totalHeight();
        float normalJump = params.normal_jump_height;
        float doubleJump = params.double_jump_height;
        float playerR = params.player_radius;
        float totalMaxJump = normalJump + doubleJump;

        // The reachable horizontal distance during a jump arc (approximate)
        // With horizontal speed ~600 px/s and jump time ~0.5s, horizontal reach ~300px
        // We'll use a generous max gap proportional to jump height
        float maxHorizontalGap = normalJump * 2.5f;
        float maxVerticalGap = normalJump * 0.85f;         // reachable with single jump
        float maxVerticalGapDouble = totalMaxJump * 0.85f; // reachable with double jump

        // Platform dimensions
        float minPlatformWidth = 100.0f;
        float maxPlatformWidth = 350.0f;
        float platformThickness = 20.0f;

        // Vertical band limits (keep platforms away from floor and ceiling)
        float minY = 60.0f;
        float maxY = H - 80.0f;

        // Tracked placements to avoid overlaps
        std::vector<PlatformPlacement> placed;
        float overlapMargin = playerR * 2.0f;

        // --- Generate a main traversal path ---
        // Walk from start.x to near end.x, placing platforms that the player can reach
        float cursorX = start.x + 150.0f;
        float cursorY = start.y;
        int platformIndex = 0;

        while (cursorX < end.x - 200.0f)
        {
            // Decide platform width
            float platW = mathRnd.nextRange<float>(minPlatformWidth, maxPlatformWidth);

            // Horizontal jump: place next platform within reachable horizontal distance
            float dx = mathRnd.nextRange<float>(platW * 0.5f + playerR * 2.0f, maxHorizontalGap);

            // Vertical variation: prefer staying in the lower-mid range, occasionally go higher
            float dy = 0.0f;
            float chance = mathRnd.nextRange<float>(0.0f, 1.0f);

            if (chance < 0.30f)
            {
                // Go up (single jump reachable)
                dy = mathRnd.nextRange<float>(30.0f, maxVerticalGap);
            }
            else if (chance < 0.45f)
            {
                // Go up high (needs double jump)
                dy = mathRnd.nextRange<float>(maxVerticalGap, maxVerticalGapDouble);
            }
            else if (chance < 0.70f)
            {
                // Go down
                dy = -mathRnd.nextRange<float>(30.0f, normalJump * 1.5f);
            }
            else
            {
                // Stay roughly level
                dy = mathRnd.nextRange<float>(-40.0f, 40.0f);
            }

            float newX = cursorX + dx;
            float newY = OP<float>::clamp(cursorY + dy, minY, maxY);

            vec2f center(newX, newY);

            // Check for overlap with already placed platforms
            if (!overlapsExisting(placed, center, platW, platformThickness, overlapMargin))
            {
                // Decide if this is a box or a segment platform
                bool useBox = (chance < 0.20f);

                if (useBox)
                {
                    container.addStaticStructure(
                        Structure2D::FromBoxCenterSize(
                            "Platform Box", 0.6f,
                            center,
                            vec2f(platW, platformThickness)));
                }
                else
                {
                    // Segment: a flat line at the top of where the box would be
                    float segY = center.y + platformThickness * 0.5f;
                    // mat2f rot = GEN<mat2f>::rotate(
                    //     mathRnd.nextRange<float>(OP<float>::deg_2_rad(30.0f), OP<float>::deg_2_rad(45.0f)) * OP<float>::sign(mathRnd.nextRange<float>(-1.0f, 1.0f)));
                    vec2f segA = vec2f(center.x, segY) + vec2f(-platW * 0.5f, 0);
                    vec2f segB = vec2f(center.x, segY) + vec2f(platW * 0.5f, 0);
                    container.addStaticStructure(
                        Structure2D::FromSegment("Platform Segment", 0.6f,
                                                 Segment2D(segA, segB)));
                }

                placed.push_back({center, platW, platformThickness, useBox});
                cursorX = newX;
                cursorY = newY;
                platformIndex++;
            }
            else
            {
                // Skip ahead a bit to avoid getting stuck
                cursorX += platW * 0.5f;
            }
        }

        // --- Add secondary/branch platforms for exploration ---
        int numScreens = params.stage_length_screens;
        int branchPlatforms = numScreens * 4; // ~4 extra per screen

        for (int i = 0; i < branchPlatforms; i++)
        {
            float platW = mathRnd.nextRange<float>(minPlatformWidth, maxPlatformWidth * 0.8f);

            // Pick a random existing platform as anchor
            if (placed.empty())
                break;
            int anchorIdx = (int)mathRnd.nextRange<float>(0.0f, (float)(placed.size() - 1));
            auto &anchor = placed[anchorIdx];

            float dx = mathRnd.nextRange<float>(-maxHorizontalGap, maxHorizontalGap);
            float dy = mathRnd.nextRange<float>(-maxVerticalGapDouble, maxVerticalGapDouble);

            vec2f center(
                OP<float>::clamp(anchor.position.x + dx, platW * 0.5f + 10.0f, W - platW * 0.5f - 10.0f),
                OP<float>::clamp(anchor.position.y + dy, minY, maxY));

            if (overlapsExisting(placed, center, platW, platformThickness, overlapMargin))
                continue;

            bool useBox = (mathRnd.nextRange<float>(0.0f, 1.0f) < 0.40f);

            if (useBox)
            {
                container.addStaticStructure(
                    Structure2D::FromBoxCenterSize(
                        "Branch Box", 0.5f,
                        center,
                        vec2f(platW, platformThickness)));
            }
            else
            {
                float segY = center.y + platformThickness * 0.5f;
                // mat2f rot = GEN<mat2f>::rotate(
                //     mathRnd.nextRange<float>(OP<float>::deg_2_rad(30.0f), OP<float>::deg_2_rad(45.0f)) * OP<float>::sign(mathRnd.nextRange<float>(-1.0f, 1.0f)));
                container.addStaticStructure(
                    Structure2D::FromSegment("Branch Segment", 0.5f,
                                             Segment2D(
                                                 vec2f(center.x, segY) + vec2f(-platW * 0.5f, 0),
                                                 vec2f(center.x, segY) + vec2f(platW * 0.5f, 0))));
            }

            placed.push_back({center, platW, platformThickness, useBox});
        }

        // --- Add one-way pass-through platforms ---
        // These are horizontal segments the player can jump through from below but land on from above.
        int oneWayPlatforms = numScreens * 3; // ~3 per screen
        for (int i = 0; i < oneWayPlatforms; i++)
        {
            float platW = mathRnd.nextRange<float>(minPlatformWidth, maxPlatformWidth);

            if (placed.empty())
                break;
            int anchorIdx = (int)mathRnd.nextRange<float>(0.0f, (float)(placed.size() - 1));
            auto &anchor = placed[anchorIdx];

            float dx = mathRnd.nextRange<float>(-maxHorizontalGap * 0.8f, maxHorizontalGap * 0.8f);
            // Place above the anchor so the player can jump up through them
            float dy = mathRnd.nextRange<float>(normalJump * 0.3f, normalJump * 0.9f);

            vec2f center(
                OP<float>::clamp(anchor.position.x + dx, platW * 0.5f + 10.0f, W - platW * 0.5f - 10.0f),
                OP<float>::clamp(anchor.position.y + dy, minY, maxY));

            if (overlapsExisting(placed, center, platW, platformThickness, overlapMargin))
                continue;

            // One-way platforms are always flat horizontal segments
            float segY = center.y + platformThickness * 0.5f;
            auto structure = Structure2D::FromSegmentPassThrough(
                "One-Way Platform", 0.6f,
                Segment2D(vec2f(center.x - platW * 0.5f, segY),
                          vec2f(center.x + platW * 0.5f, segY)),
                vec2f(0, 1));
            // pass through when moving upward
            container.addStaticStructure(structure);

            placed.push_back({center, platW, platformThickness, false});
        }

        // // --- Add some angled / sloped segments for variety ---
        // int slopedCount = numScreens * 2;
        // for (int i = 0; i < slopedCount; i++)
        // {
        //     if (placed.empty())
        //         break;
        //     int anchorIdx = mathRnd.random->getRange<int32_t>(0, (int32_t)(placed.size() - 1));
        //     auto &anchor = placed[anchorIdx];

        //     float slopeLen = mathRnd.nextRange<float>(80.0f, 200.0f);
        //     //float angle = mathRnd.nextRange<float>(-0.4f, 0.4f); // mild slope in radians
        //     float angle = mathRnd.nextRange<float>(OP<float>::deg_2_rad(45.0f), OP<float>::deg_2_rad(60.0f)) *
        //                   OP<float>::sign(mathRnd.nextRange<float>(-1.0f, 1.0f));

        //     float dx = mathRnd.nextRange<float>(-maxHorizontalGap * 0.7f, maxHorizontalGap * 0.7f);
        //     float dy = mathRnd.nextRange<float>(-maxVerticalGap * 0.5f, maxVerticalGap * 0.5f);

        //     vec2f slopeCenter(
        //         OP<float>::clamp(anchor.position.x + dx, slopeLen + 10.0f, W - slopeLen - 10.0f),
        //         OP<float>::clamp(anchor.position.y + dy, minY, maxY));

        //     vec2f dir(OP<float>::cos(angle), OP<float>::sin(angle));
        //     vec2f segA = slopeCenter - dir * (slopeLen * 0.5f);
        //     vec2f segB = slopeCenter + dir * (slopeLen * 0.5f);

        //     // Clamp endpoints to game area
        //     segA.y = OP<float>::clamp(segA.y, 10.0f, H - 10.0f);
        //     segB.y = OP<float>::clamp(segB.y, 10.0f, H - 10.0f);

        //     // Simple overlap check using bounding box
        //     float bw = OP<float>::abs(segB.x - segA.x);
        //     float bh = OP<float>::abs(segB.y - segA.y) + 10.0f;
        //     vec2f bCenter = (segA + segB) * 0.5f;

        //     if (!overlapsExisting(placed, bCenter, bw, bh, overlapMargin * 0.5f))
        //     {
        //         container.addStaticStructure(
        //             Structure2D::FromSegment("Slope", 0.4f, Segment2D(segA, segB)));
        //         placed.push_back({bCenter, bw, bh, false});
        //     }
        // }
    }

    void StageGenerator::addTestPlatformSequence(
        Container2D &container,
        const StageParams &params,
        MathRandomExt<Random32> &mathRnd,
        const vec2f &start,
        const vec2f &end)
    {
        float W = params.totalWidth();
        float H = params.totalHeight();
        float normalJump = params.normal_jump_height;
        float doubleJump = params.double_jump_height;
        float playerR = params.player_radius;
        float totalMaxJump = normalJump + doubleJump;

        // The reachable horizontal distance during a jump arc (approximate)
        // With horizontal speed ~600 px/s and jump time ~0.5s, horizontal reach ~300px
        // We'll use a generous max gap proportional to jump height
        float maxHorizontalGap = normalJump * 2.5f;
        float maxVerticalGap = normalJump * 0.85f;         // reachable with single jump
        float maxVerticalGapDouble = totalMaxJump * 0.85f; // reachable with double jump

        // Platform dimensions
        float minPlatformWidth = 100.0f;
        float maxPlatformWidth = 350.0f;
        float platformThickness = 20.0f;

        // Vertical band limits (keep platforms away from floor and ceiling)
        float minY = 60.0f;
        float maxY = H - 80.0f;

        // Tracked placements to avoid overlaps
        std::vector<PlatformPlacement> placed;
        float overlapMargin = playerR * 2.0f;

        // --- Generate a main traversal path ---
        // Walk from start.x to near end.x, placing platforms that the player can reach
        float cursorX = start.x + 150.0f;
        float cursorY = start.y;
        int platformIndex = 0;

        float dy_increment = -50.0f;

        while (cursorX < end.x - 200.0f)
        {
            // Decide platform width
            float platW = mathRnd.nextRange<float>(minPlatformWidth, maxPlatformWidth);

            // Horizontal jump: place next platform within reachable horizontal distance
            float dx = mathRnd.nextRange<float>(platW * 0.5f + playerR * 2.0f, maxHorizontalGap);

            // Vertical variation: prefer staying in the lower-mid range, occasionally go higher
            float dy = 0.0f;
            // float chance = mathRnd.nextRange<float>(0.0f, 1.0f);
            dy = dy_increment;
            dy_increment += 5.0f; // steadily increase vertical gap to test jumps

            float newX = cursorX + dx;
            float newY = OP<float>::clamp(cursorY + dy, minY, maxY);

            vec2f center(newX, newY);

            // Check for overlap with already placed platforms
            if (!overlapsExisting(placed, center, platW, platformThickness, overlapMargin))
            {
                container.addStaticStructure(
                    Structure2D::FromBoxCenterSize(
                        "Platform Box", 0.6f,
                        center,
                        vec2f(platW, platformThickness)));
                placed.push_back({center, platW, platformThickness, true});
                cursorX = newX;
                cursorY = newY;
                platformIndex++;
            }
            else
            {
                // Skip ahead a bit to avoid getting stuck
                cursorX += platW * 0.5f;
            }
        }
    }

    // ----------------------------------------------------------------
    // Decorative box obstacles
    // ----------------------------------------------------------------

    void StageGenerator::addDecorativeBoxes(
        Container2D &container,
        const StageParams &params,
        MathRandomExt<Random32> &mathRnd)
    {
        return;
        float W = params.totalWidth();
        int numScreens = params.stage_length_screens;
        int numBoxes = numScreens * 2; // ~2 obstacle boxes per screen

        for (int i = 0; i < numBoxes; i++)
        {
            float boxSize = mathRnd.nextRange<float>(40.0f, 120.0f);
            float x = mathRnd.nextRange<float>(boxSize * 0.5f + 50.0f, W - boxSize * 0.5f - 50.0f);
            // Place on the floor
            float y = boxSize * 0.5f;

            container.addStaticStructure(
                Structure2D::FromBoxCenterSize(
                    "Obstacle Box", 0.5f,
                    vec2f(x, y),
                    vec2f(boxSize, boxSize)));
        }
    }

    // ----------------------------------------------------------------
    // Main entry point
    // ----------------------------------------------------------------

    StageResult StageGenerator::generate(
        Container2D &container,
        const StageParams &params,
        Random32 &rng)
    {
        // rng.setSeed(1);
        MathRandomExt<Random32> mathRnd(&rng);

        float W = params.totalWidth();
        float H = params.totalHeight();

        // 1. Set game area
        auto game_area_box = Box2D(vec2f(0.0f, 0.0f), vec2f(W, H));
        
        container.setGameArea(game_area_box);

        // add game area boundary segments for collision (optional, since we have floor/walls, but good for safety)
        {
            float normalJump = params.normal_jump_height;
            float doubleJump = params.double_jump_height;
            float playerR = params.player_radius;
            float totalMaxJump = normalJump + doubleJump;

            container.addStaticStructure(
                Structure2D::FromSegment("Floor", 0.8f,
                                         Segment2D(vec2f(0, 0), vec2f(W, 0))));
            container.addStaticStructure(
                Structure2D::FromSegment("Wall", 1.0f,
                                         Segment2D(vec2f(0, 0), vec2f(0, H + totalMaxJump * 10.0f))).setAlwaysCheck(true));
            container.addStaticStructure(
                Structure2D::FromSegment("Wall", 1.0f,
                                         Segment2D(vec2f(W, 0), vec2f(W, H + totalMaxJump * 10.0f))).setAlwaysCheck(true));
        }

        // // 2. Floor, ceiling, walls
        // addFloorAndWalls(container, params);

        // // 3. Start and end platforms
        vec2f startPt, endPt;
        addStartAndEndPlatforms(container, params, startPt, endPt);

        // // addTestPlatformSequence(container, params, mathRnd, startPt, endPt);

        // 4. Main platform sequence + branches + slopes
        addPlatformSequence(container, params, mathRnd, startPt, endPt);

        // 5. Decorative box obstacles on the floor
        addDecorativeBoxes(container, params, mathRnd);


        // float start_x = 0.0f + 500.0f;
        // float end_x = 100.0f + 500.0f;
        // container.addStaticStructure(
        //                 Structure2D::FromBoxCenterSize(
        //                     "Platform Box", 0.6f,
        //                     vec2f((start_x+end_x)*0.5f, 10.0f),
        //                     vec2f((end_x-start_x), 20.0f)));

        return StageResult{startPt, endPt};
    }

}
