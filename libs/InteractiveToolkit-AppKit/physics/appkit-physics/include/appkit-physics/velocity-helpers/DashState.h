#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

#include <InteractiveToolkit/EaseCore/EaseCore.h>

namespace AppKit
{
    namespace Physics
    {
        namespace VelocityHelpers
        {

            class DashState
            {
            public:
                enum State
                {
                    None,
                    DashingRight,
                    DashingLeft,
                    RestUntilNextFrame,
                };

                static const char *stateToString(DashState::State state);

            private:
                DashState::State state;

                float total_distance;
                float total_time;
                float total_time_inv;

                float norm_time_acc;
                float last_distance_traveled;

                EventCore::Callback<float(float _start, float _end, float _lerp)> ease_eq;

                EventCore::PressReleaseDetector dash_trigger_detector;

                bool enabled;
            public:
                DashState();

                DashState::State getState() const;

                void configureDash(float distance, float time);

                void setEaseEq(const EventCore::Callback<float(float, float, float)> &ease_eq);
                void setEnableDash(bool v);

                void updateVelocity(float *velocityX, float deltaTime, bool dash_pressed_, DashState::State dash_to_apply);
                
            };

        }
    }
}