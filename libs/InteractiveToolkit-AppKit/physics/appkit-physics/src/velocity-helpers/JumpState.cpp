#include <appkit-physics/velocity-helpers/JumpState.h>

namespace AppKit
{
    namespace Physics
    {
        namespace VelocityHelpers
        {

            namespace HeightAndTime
            {
                MathCore::vec3f IntegrateAcceleration(const MathCore::vec3f &v0, const MathCore::vec3f &acceleration, float deltaTime)
                {
                    // Use kinematic equation: height_delta = v*dt + 0.5*g*dt²
                    // This matches HeightAndTime::CalcHeightAtTime formula
                    return v0 * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;
                }

                // what is the height_delta after apply gravity during deltaTime?
                // returns the distance traveled during deltatime
                float IntegrateAcceleration(float v0, float acceleration, float deltaTime)
                {
                    // Use kinematic equation: height_delta = v*dt + 0.5*g*dt²
                    // This matches HeightAndTime::CalcHeightAtTime formula
                    return v0 * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;
                }

                float CalcVelocityToReachHeight(float height, float gravity)
                {
                    // v = sqrt(2 * g * h)
                    return MathCore::OP<float>::sqrt(2.0f * MathCore::OP<float>::abs(gravity) * height);
                }

                float CalcHeightFromVelocity(float velocity, float gravity)
                {
                    // h = v^2 / (2 * g)
                    return (velocity * velocity) / (2.0f * MathCore::OP<float>::abs(gravity));
                }

                // // only works couppled with CalcHeightFromVelocity, as it assumes the velocity -> height relationship, and the gravity is constant
                // float CalcTimeToReachHeight(float height, float gravity)
                // {
                //     // t = sqrt(2 * h / g)
                //     return MathCore::OP<float>::sqrt((2.0f * height) / MathCore::OP<float>::abs(gravity));
                // }

                float CalcTimeToReachZeroVelocity(float v0, float gravity)
                {
                    // v(t) = v0 + g*t = 0  =>  t = -v0/g = |v0/g|
                    return MathCore::OP<float>::abs(v0 / gravity);
                }

                // float CalcHeightAtTime(float initialVelocity, float gravity, float time)
                // {
                //     // d = v0 * t + 0.5 * g * t^2
                //     return initialVelocity * time + 0.5f * gravity * time * time;
                // }
            }

            const char *JumpState::stateToString(JumpState::State state)
            {
                switch (state)
                {
                case RisingBeforeNoUpImpulsion:
                    return "RisingBeforeNoUpImpulsion";
                case Grounded:
                    return "Grounded";
                case Rising:
                    return "Rising";
                case StartJump:
                    return "StartJump";
                case RisingNoUpImpulsion:
                    return "RisingButtonReleased";
                case SetVelocityZeroBeforeFalling:
                    return "SetVelocityZeroBeforeFalling";
                case Falling:
                    return "Falling";
                default:
                    return "Unknown";
                }
            }

            JumpState::JumpState()
            {
                state = Grounded;
                risingVelocity = 0.0f;
                minJumpHeight = 0.0f;
                maxJumpHeight = 0.0f;
                secondJumpHeight = 0.0f;

                rising_velocity_with_impulse = 0.0f;
                rising_velocity_without_impulse = 0.0f;
                // maxJumpHeightFinal = 0.0f;
                minJumpHeight_keep_impulse_until_height = 0.0f;
                minJumpHeight_max_height = 0.0f;
                minJumpHeight_time_to_reach_max_height_without_impulse = 0.0f;
                minJumpHeight_height_without_impulse = 0.0f;

                secondJumpHeight_keep_impulse_until_height = 0.0f;

                velocity_replacer = 0.0f;
                estimated_jump_height = 0.0f;
                time_aux = 0.0f;
                dynamic_max_height = 0.0f;
                jump_pressed = false;
                can_double_jump = true;
                double_jump_used = false;

                allow_second_jump_temporarily = false;
                is_jumping = false;
            }

            JumpState::State JumpState::getState() const { return state; }

            // set the constant up rising velocity during jump
            float JumpState::getRisingVelocity() const { return risingVelocity; }

            float JumpState::getMinJumpHeight() const { return minJumpHeight; }

            float JumpState::getMaxJumpHeight() const { return maxJumpHeight; }

            float JumpState::getSecondJumpHeight() const { return secondJumpHeight; }

            float JumpState::VelocityToReachDistance(float distance, float risingVelocity, float minJumpHeight, float maxJumpHeight, float gravity, float lrp)
            {
                float required_initial_velocity = HeightAndTime::CalcVelocityToReachHeight(minJumpHeight, gravity);
                float rising_velocity_without_impulse = MathCore::OP<float>::minimum(risingVelocity, required_initial_velocity);

                float minJumpHeight_time_to_reach_max_height_without_impulse = HeightAndTime::CalcTimeToReachZeroVelocity(rising_velocity_without_impulse, gravity);

                float minJumpHeight_height_without_impulse = HeightAndTime::CalcHeightFromVelocity(rising_velocity_without_impulse, gravity);

                float maxJumpHeight_keep_impulse_until_height = maxJumpHeight - minJumpHeight_height_without_impulse;

                float _t_keep_impulse_max_jump = maxJumpHeight_keep_impulse_until_height / rising_velocity_without_impulse;

                float _2t = minJumpHeight_time_to_reach_max_height_without_impulse * 2.0f;

                return distance / (_2t + 2.0f * _t_keep_impulse_max_jump * lrp);
            }

            float JumpState::LimitMaxRisingVelocity(float risingVelocity, float minJumpHeight, float gravity)
            {
                float required_initial_velocity = HeightAndTime::CalcVelocityToReachHeight(minJumpHeight, gravity);
                float rising_velocity_without_impulse = MathCore::OP<float>::minimum(risingVelocity, required_initial_velocity);

                return (rising_velocity_without_impulse < risingVelocity) ? rising_velocity_without_impulse : risingVelocity;
            }

            void JumpState::configureJump(float risingVelocity, float minJumpHeight, float maxJumpHeight, float secondJumpHeight, float gravity)
            {
                this->risingVelocity = risingVelocity;
                this->minJumpHeight = minJumpHeight;
                this->maxJumpHeight = maxJumpHeight;
                this->secondJumpHeight = MathCore::OP<float>::clamp(secondJumpHeight, minJumpHeight, maxJumpHeight);

                minJumpHeight_max_height = minJumpHeight;

                rising_velocity_with_impulse = risingVelocity;

                float required_initial_velocity = HeightAndTime::CalcVelocityToReachHeight(minJumpHeight_max_height, gravity);
                rising_velocity_without_impulse = MathCore::OP<float>::minimum(risingVelocity, required_initial_velocity);

                // if (rising_velocity_without_impulse < rising_velocity_with_impulse)
                //     rising_velocity_with_impulse = rising_velocity_without_impulse;

                minJumpHeight_height_without_impulse = HeightAndTime::CalcHeightFromVelocity(rising_velocity_without_impulse, gravity);
                minJumpHeight_keep_impulse_until_height = minJumpHeight_max_height - minJumpHeight_height_without_impulse;

                maxJumpHeight_keep_impulse_until_height = maxJumpHeight - minJumpHeight_height_without_impulse;
                secondJumpHeight_keep_impulse_until_height = secondJumpHeight - minJumpHeight_height_without_impulse;

                // minJumpHeight_time_to_reach_max_height_without_impulse = HeightAndTime::CalcTimeToReachHeight(minJumpHeight_height_without_impulse, gravity);
                minJumpHeight_time_to_reach_max_height_without_impulse = HeightAndTime::CalcTimeToReachZeroVelocity(rising_velocity_without_impulse, gravity);
            }

            void JumpState::setGrounded()
            {
                if (state == Grounded)
                    return;
                state = Grounded;
                // allow_second_jump_temporarily = false;
                is_jumping = false;
            }

            void JumpState::setFalling(bool from_any_state, bool keep_second_jump_, bool reset_is_jumping)
            {
                if (state != Grounded && !from_any_state)
                    return;
                state = Falling;
                if (!keep_second_jump_)
                    can_double_jump = false;
                if (reset_is_jumping)
                    is_jumping = false;
            }

            void JumpState::updateVelocity(float *velocityY, float deltaTime,
                                           float gravity, bool jump_pressedp, bool can_jump,
                                           bool allow_double_jump, bool double_jump_at_any_time)
            {
                jump_trigger_detector.setState(jump_pressedp);

                if (jump_trigger_detector.down && (can_jump || allow_second_jump_temporarily))
                {
                    if (state == Grounded)
                    {
                        state = StartJump;
                        can_double_jump = true;
                        double_jump_used = false;
                        time_aux = 0.0f;
                        allow_second_jump_temporarily = false;
                    }
                    else if (allow_second_jump_temporarily || (allow_double_jump && can_double_jump && (state == Falling || double_jump_at_any_time)))
                    {
                        can_double_jump = false;
                        double_jump_used = true;
                        state = StartJump;
                        time_aux = 0.0f;
                        allow_second_jump_temporarily = false;
                    }
                }

                jump_pressed = jump_pressed && jump_trigger_detector.pressed;

                if (deltaTime <= 0.0f)
                    return;

                if (state == StartJump)
                {
                    is_jumping = true;
                    estimated_jump_height = 0.0f;
                    state = Rising;
                    velocity_replacer = rising_velocity_with_impulse;
                    *velocityY = velocity_replacer;
                    jump_pressed = true;
                }
                else if (state == RisingBeforeNoUpImpulsion)
                {
                    state = RisingNoUpImpulsion;
                    velocity_replacer = rising_velocity_without_impulse;
                    // time_aux = 0.0f;

                    float base_line_jump_height = dynamic_max_height - minJumpHeight_height_without_impulse;
                    float height_from_baseline = estimated_jump_height - base_line_jump_height;
                    if (height_from_baseline > 0.0f)
                    {
                        // Calculate remaining height to reach max
                        float remaining_height = minJumpHeight_height_without_impulse - height_from_baseline;
                        // force remaining height to be non-negative
                        if (remaining_height < 0.0f)
                            remaining_height = 0.0f;

                        velocity_replacer = HeightAndTime::CalcVelocityToReachHeight(remaining_height, gravity);
                        float time_remaining = HeightAndTime::CalcTimeToReachZeroVelocity(velocity_replacer, gravity);
                        time_aux = minJumpHeight_time_to_reach_max_height_without_impulse - time_remaining;

                        // // Time to travel remaining distance (from current position to max, ending at rest)
                        // float time_remaining = HeightAndTime::CalcTimeToReachHeight(remaining_height, gravity);
                        // // Time elapsed = total time - remaining time
                        // time_aux = minJumpHeight_time_to_reach_max_height_without_impulse - time_remaining;

                        // velocity_replacer += gravity * time_aux;
                    }
                    else
                        time_aux = 0.0f;
                }

                if (state == Rising)
                {
                    float height_delta = velocity_replacer * deltaTime;

                    float new_estimated_height = estimated_jump_height + height_delta;

                    if (double_jump_used)
                    {
                        if (new_estimated_height >= secondJumpHeight_keep_impulse_until_height)
                        {
                            state = RisingBeforeNoUpImpulsion;

                            if (new_estimated_height >= secondJumpHeight)
                            {
                                height_delta = secondJumpHeight - estimated_jump_height;
                                new_estimated_height = estimated_jump_height + height_delta;
                                velocity_replacer = height_delta / deltaTime;

                                if (velocity_replacer <= 0.0f)
                                    state = Falling;
                                else
                                    state = SetVelocityZeroBeforeFalling;
                            }

                            // height_delta = secondJumpHeight_keep_impulse_until_height - estimated_jump_height;
                            // new_estimated_height = estimated_jump_height + height_delta;
                            // velocity_replacer = height_delta / deltaTime;

                            dynamic_max_height = secondJumpHeight;
                        }
                    }
                    else
                    {
                        if (new_estimated_height >= maxJumpHeight_keep_impulse_until_height)
                        {
                            state = RisingBeforeNoUpImpulsion;
                            if (new_estimated_height >= maxJumpHeight)
                            {
                                height_delta = maxJumpHeight - estimated_jump_height;
                                new_estimated_height = estimated_jump_height + height_delta;
                                velocity_replacer = height_delta / deltaTime;

                                if (velocity_replacer <= 0.0f)
                                    state = Falling;
                                else
                                    state = SetVelocityZeroBeforeFalling;
                            }

                            // height_delta = maxJumpHeight_keep_impulse_until_height - estimated_jump_height;
                            // new_estimated_height = estimated_jump_height + height_delta;
                            // velocity_replacer = height_delta / deltaTime;

                            dynamic_max_height = maxJumpHeight;
                        }
                        else if (!jump_pressed && new_estimated_height >= minJumpHeight_keep_impulse_until_height)
                        {
                            // need to keep rising from the current position
                            state = RisingBeforeNoUpImpulsion;
                            height_delta = new_estimated_height - estimated_jump_height;
                            new_estimated_height = estimated_jump_height + height_delta;

                            velocity_replacer = height_delta / deltaTime;

                            dynamic_max_height = new_estimated_height + minJumpHeight_height_without_impulse;
                        }
                    }

                    estimated_jump_height = new_estimated_height;
                    *velocityY = velocity_replacer;
                }
                else if (state == RisingNoUpImpulsion)
                {
                    time_aux += deltaTime;

                    // what is the height_delta after apply gravity during deltaTime?
                    float height_delta = HeightAndTime::IntegrateAcceleration(velocity_replacer, gravity, deltaTime);

                    velocity_replacer += gravity * deltaTime;

                    float new_estimated_height = estimated_jump_height + height_delta;

                    if (time_aux >= minJumpHeight_time_to_reach_max_height_without_impulse)
                    {
                        state = SetVelocityZeroBeforeFalling;
                        height_delta = dynamic_max_height - estimated_jump_height;
                        new_estimated_height = estimated_jump_height + height_delta;
                    }

                    estimated_jump_height = new_estimated_height;
                    // Output effective velocity so external system gets: position += (height_delta/deltaTime) * deltaTime = height_delta
                    *velocityY = height_delta / deltaTime;

                    if (time_aux >= minJumpHeight_time_to_reach_max_height_without_impulse && *velocityY <= 0.0f)
                        state = Falling;
                }
                else if (state == SetVelocityZeroBeforeFalling)
                {
                    state = Falling;
                    *velocityY = 0.0f;
                }
            }

            void JumpState::reloadSecondJumpOneMoreTime()
            {
                allow_second_jump_temporarily = true;
            }

            bool JumpState::isJumping() const
            {
                return is_jumping;
            }
        }
    }
}