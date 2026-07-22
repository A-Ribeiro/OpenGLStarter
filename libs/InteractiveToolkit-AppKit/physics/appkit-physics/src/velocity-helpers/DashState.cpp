#include <appkit-physics/velocity-helpers/DashState.h>

namespace AppKit
{
    namespace Physics
    {
        namespace VelocityHelpers
        {

            enum State
            {
                None,
                DashingRight,
                DashingLeft,
            };

            const char *DashState::stateToString(DashState::State state)
            {
                switch (state)
                {
                case State::None:
                    return "None";
                case State::DashingRight:
                    return "DashingRight";
                case State::DashingLeft:
                    return "DashingLeft";
                default:
                    return "Unknown";
                }
            }

            DashState::DashState()
            {
                state = State::None;
                enabled = true;

                ease_eq = EaseCore::Easef::none<float>; // lerp
            }

            DashState::State DashState::getState() const
            {
                return state;
            }

            void DashState::configureDash(float distance, float time)
            {
                total_distance = distance;
                total_time = time;
                total_time_inv = 1.0f / total_time;

                norm_time_acc = 0.0f;
                last_distance_traveled = 0.0f;
            }

            void DashState::setEaseEq(const EventCore::Callback<float(float, float, float)> &ease_eq)
            {
                this->ease_eq = ease_eq;
            }

            void DashState::setEnableDash(bool v)
            {
                enabled = v;
            }

            void DashState::updateVelocity(float *velocityX, float deltaTime, bool dash_pressed_, DashState::State dash_to_apply)
            {
                dash_trigger_detector.setState(dash_pressed_);

                if (enabled && state == State::None)
                {
                    if (dash_trigger_detector.down)
                    {
                        state = dash_to_apply;
                        norm_time_acc = 0;
                        last_distance_traveled = 0;
                    }
                }
                else
                    norm_time_acc += deltaTime * total_time_inv;

                if (state != State::None)
                {
                    if (state == State::RestUntilNextFrame2)
                    {
                        // previous pass was 1.0
                        // make little stop in air
                        state = State::None;
                        // *velocityX = 0.0f;
                    }
                    else
                    if (norm_time_acc == 1.0f || state == State::RestUntilNextFrame1)
                    {
                        // previous pass was 1.0
                        // make little stop in air
                        state = State::RestUntilNextFrame2;
                        *velocityX = 0.0f;
                    }
                    else
                    {
                        bool neg_result = (state == State::DashingLeft);
                        if (norm_time_acc > 1.0f)
                        {
                            state = State::RestUntilNextFrame1;
                            norm_time_acc = 1.0f;
                        }

                        float new_distance_traveled = ease_eq(0.0f, total_distance, norm_time_acc);
                        float delta = new_distance_traveled - last_distance_traveled;
                        last_distance_traveled = new_distance_traveled;

                        *velocityX = (neg_result) ? -delta / deltaTime : delta / deltaTime;
                    }
                }
            }

        }
    }
}