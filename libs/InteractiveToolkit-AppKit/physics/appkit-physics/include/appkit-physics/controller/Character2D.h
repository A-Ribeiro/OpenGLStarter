#pragma once

#include <InteractiveToolkit/Platform/Time.h>

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>
// #include <InteractiveToolkit/Platform/Core/SmartVector.h>

#include <appkit-physics/core/Segment2D.h>
// #include <appkit-physics/core/Box2D.h>
// #include <appkit-physics/core/Line2D.h>
// #include <appkit-physics/container/Structure2D.h>
#include <appkit-physics/container/ObjectState2D.h>
#include <appkit-physics/container/ThreadState2D.h>

// #include <appkit-physics/util/Quadtree.h>
// #include <appkit-physics/util/Uuid.h>
#include <appkit-physics/velocity-helpers/JumpState.h>

#include <appkit-physics/container/TriggerProbe.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {
            class Container2D;
        }

        const int COLOR_CODE_GROUND_AXIS = 0;
        const int COLOR_CODE_GROUND_AXIS_ON_SEGMENT = 1;

        namespace Controller
        {
            class Character2D
            {
            public:
                VelocityHelpers::JumpState jumpState;

                MathCore::vec2f velocity;
                MathCore::vec2f acceleration;

                MathCore::vec2f position;

                MathCore::vec2f gravity;
                MathCore::vec2f gravity_dir;
                float gravity_mag;

                float radius;
                float radius_grounded;
                float offset_grounded;

                bool allow_double_jump;

                Core::Segment2D last_collision_segment;
                bool has_last_collision_segment;

                EventCore::PressReleaseDetector move_x_detector;

                Container::ObjectState2D object_state;

                float skin_width;

                float offset_above_activation_line;
                float offset_below_deactivation_line;


                std::unordered_map<std::string, std::shared_ptr<Container::TriggerProbe>> trigger_probes;

                Character2D();

                static Character2D fromStaticConfig(
                    float radius, float radius_grounded, float offset_grounded,
                    float jump_risingVelocity, float jump_minJumpHeight, float jump_maxJumpHeight, float jump_secondJumpHeight,
                    const MathCore::vec2f &gravity,
                    bool allow_double_jump,
                    float skin_width,
                    float offset_above_activation_line,
                    float offset_below_deactivation_line);

                void update(Container::Container2D *Container2D,
                            Container::ThreadState2D &thread_state,
                            Platform::Time *time,
                            float input_x_axis,
                            float x_axis_velocity,
                            bool jump_pressed, float max_velocity);

                // set the position and make reset velocity and acceleration, useful for teleporting the player
                void teleport(const MathCore::vec2f &position);

                EventCore::Callback<void(const MathCore::vec2f &a, const MathCore::vec2f &b, int color_code)> OnDebugDrawLine;

                ITK_DECLARE_CREATE_SHARED(Character2D)
            };
        }
    }
}