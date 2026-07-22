#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

namespace AppKit
{
    namespace Physics
    {
        namespace VelocityHelpers
        {

            namespace HeightAndTime
            {
                // Use kinematic equation: height_delta = v*dt + 0.5*g*dt²
                MathCore::vec3f IntegrateAcceleration(const MathCore::vec3f &v0, const MathCore::vec3f &acceleration, float deltaTime);

                // what is the height_delta after apply gravity during deltaTime?
                // returns the distance traveled during deltatime
                // Use kinematic equation: height_delta = v*dt + 0.5*g*dt²
                float IntegrateAcceleration(float v0, float acceleration, float deltaTime);

                // v = sqrt(2 * g * h)
                float CalcVelocityToReachHeight(float height, float gravity);

                // h = v^2 / (2 * g)
                float CalcHeightFromVelocity(float velocity, float gravity);

                // v(t) = v0 + g*t = 0  =>  t = -v0/g = |v0/g|
                float CalcTimeToReachZeroVelocity(float v0, float gravity);
            }

            class JumpState
            {
            public:
                enum State
                {
                    Grounded,

                    StartJump,
                    Rising,

                    RisingBeforeNoUpImpulsion,
                    RisingNoUpImpulsion,

                    SetVelocityZeroBeforeFalling,

                    Falling,
                };

                static const char *stateToString(JumpState::State state);

            private:
                State state;

                // input variables
                float risingVelocity;
                float minJumpHeight;
                float maxJumpHeight;
                float secondJumpHeight;

                // variables computed after computeConstants
                float rising_velocity_with_impulse;

                // store the value of velocity needed to reach the minJumpHeight_height_without_impulse
                float rising_velocity_without_impulse;

                float minJumpHeight_keep_impulse_until_height;
                float minJumpHeight_max_height;
                float minJumpHeight_time_to_reach_max_height_without_impulse;
                float minJumpHeight_height_without_impulse;

                float maxJumpHeight_keep_impulse_until_height;
                float dynamic_max_height;

                float secondJumpHeight_keep_impulse_until_height;

                EventCore::PressReleaseDetector jump_trigger_detector;

                float velocity_replacer;
                float estimated_jump_height;
                float time_aux;
                bool jump_pressed;
                bool can_double_jump;
                bool double_jump_used;

                bool allow_second_jump_temporarily;
                bool is_jumping;

            public:
                JumpState();

                State getState() const;

                // set the constant up rising velocity during jump
                float getRisingVelocity() const;
                float getMinJumpHeight() const;
                float getMaxJumpHeight() const;
                float getSecondJumpHeight() const;

                static float VelocityToReachDistance(float distance, float risingVelocity, float minJumpHeight, float maxJumpHeight, float gravity, float lrp);
                static float LimitMaxRisingVelocity(float risingVelocity, float minJumpHeight, float gravity);

                void configureJump(float risingVelocity, float minJumpHeight, float maxJumpHeight, float secondJumpHeight, float gravity);

                void setGrounded();
                void setFalling(bool from_any_state = false, bool keep_second_jump_ = false, bool reset_is_jumping = false);

                void updateVelocity(float *velocityY, float deltaTime,
                                    float gravity, bool jump_pressedp, bool can_jump,
                                    bool allow_double_jump, bool double_jump_at_any_time = true);

                // renew second jump one more time
                void reloadSecondJumpOneMoreTime();
                void cancelReloadSecondJumpOneMoreTime();


                bool isJumping() const;
            };

        }
    }
}