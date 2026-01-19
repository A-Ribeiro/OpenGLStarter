#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

namespace HeightAndTime
{
    static MathCore::vec3f IntegrateAcceleration(const MathCore::vec3f &v0, const MathCore::vec3f &acceleration, float deltaTime)
    {
        // Use kinematic equation: height_delta = v*dt + 0.5*g*dt²
        // This matches HeightAndTime::CalcHeightAtTime formula
        return v0 * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;
    }

    // what is the height_delta after apply gravity during deltaTime?
    // returns the distance traveled during deltatime
    static float IntegrateAcceleration(float v0, float acceleration, float deltaTime)
    {
        // Use kinematic equation: height_delta = v*dt + 0.5*g*dt²
        // This matches HeightAndTime::CalcHeightAtTime formula
        return v0 * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;
    }

    static float CalcVelocityToReachHeight(float height, float gravity)
    {
        // v = sqrt(2 * g * h)
        return MathCore::OP<float>::sqrt(2.0f * MathCore::OP<float>::abs(gravity) * height);
    }

    static float CalcHeightFromVelocity(float velocity, float gravity)
    {
        // h = v^2 / (2 * g)
        return (velocity * velocity) / (2.0f * MathCore::OP<float>::abs(gravity));
    }

    static float CalcTimeToReachHeight(float height, float gravity)
    {
        // t = sqrt(2 * h / g)
        return MathCore::OP<float>::sqrt((2.0f * height) / MathCore::OP<float>::abs(gravity));
    }

    static float CalcTimeToReachVelocity(float velocity, float gravity)
    {
        // t = v / g
        return MathCore::OP<float>::abs(velocity / gravity);
    }

    static float CalcHeightAtTime(float initialVelocity, float gravity, float time)
    {
        // d = v0 * t + 0.5 * g * t^2
        return initialVelocity * time + 0.5f * gravity * time * time;
    }
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

    static const char *stateToString(State state)
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

private:
    State state;

    // input variables
    float risingVelocity;
    // float maxJumpHeight;
    float minJumpHeight;

    // variables computed after computeConstants
    float rising_velocity_with_impulse;
    
    // store the value of velocity needed to reach the minJumpHeight_height_without_impulse
    float rising_velocity_without_impulse;

    // float maxJumpHeightFinal;
    float minJumpHeight_keep_impulse_until_height;
    float minJumpHeight_max_height;
    float minJumpHeight_time_to_reach_max_height_without_impulse;
    float minJumpHeight_height_without_impulse;

    EventCore::PressReleaseDetector jump_trigger_detector;

    float velocity_replacer;
    float estimated_jump_height;
    float time_aux;

    float initial_pos;
public:
    JumpState()
    {
        state = Grounded;
        risingVelocity = 0.0f;
        // maxJumpHeight = 0.0f;
        minJumpHeight = 0.0f;

        rising_velocity_with_impulse = 0.0f;
        rising_velocity_without_impulse = 0.0f;
        // maxJumpHeightFinal = 0.0f;
        minJumpHeight_keep_impulse_until_height = 0.0f;
        minJumpHeight_max_height = 0.0f;
        minJumpHeight_time_to_reach_max_height_without_impulse = 0.0f;
        minJumpHeight_height_without_impulse = 0.0f;
    }
    State getState() const { return state; }

    bool canJump() const
    {
        return state == Grounded; // || (canDoubleJump && currentState == Falling);
    }

    // set the constant up rising velocity during jump
    void setRisingVelocity(float value) { risingVelocity = value; }
    float getRisingVelocity() const { return risingVelocity; }

    void setMinJumpHeight(float value) { minJumpHeight = value; }
    float getMinJumpHeight() const { return minJumpHeight; }

    void setMaxJumpHeight(float value) { /* maxJumpHeight = value; */ }
    float getMaxJumpHeight() const { return 0.0f; /* return maxJumpHeight; */ }

    void computeConstants(float gravity)
    {
        minJumpHeight_max_height = minJumpHeight;

        rising_velocity_with_impulse = risingVelocity;

        float required_initial_velocity = HeightAndTime::CalcVelocityToReachHeight(minJumpHeight_max_height, gravity);
        rising_velocity_without_impulse = MathCore::OP<float>::minimum(risingVelocity, required_initial_velocity);

        minJumpHeight_height_without_impulse = HeightAndTime::CalcHeightFromVelocity(rising_velocity_without_impulse, gravity);
        minJumpHeight_keep_impulse_until_height = minJumpHeight_max_height - minJumpHeight_height_without_impulse;

        minJumpHeight_time_to_reach_max_height_without_impulse = HeightAndTime::CalcTimeToReachHeight(minJumpHeight_height_without_impulse, gravity);
    }

    void setGrounded()
    {
        if (state == Grounded)
            return;
        state = Grounded;
    }

    bool isJumping() const
    {
        return state == Rising || state == RisingNoUpImpulsion || state == RisingBeforeNoUpImpulsion;
    }

    void update(float *velocityY, float deltaTime, float gravity, bool jump_pressed, float *posY)
    {
        jump_trigger_detector.setState(jump_pressed);

        if (jump_trigger_detector.down && state == Grounded)
            state = StartJump;

        if (deltaTime <= 0.0f)
            return;

        if (state == StartJump)
        {
            estimated_jump_height = 0.0f;
            state = Rising;
            velocity_replacer = rising_velocity_with_impulse;
            *velocityY = velocity_replacer;
            initial_pos = *posY;
        }
        else if (state == RisingBeforeNoUpImpulsion)
        {
            state = RisingNoUpImpulsion;
            velocity_replacer = rising_velocity_without_impulse;
            time_aux = 0.0f;

            printf("[RisingBeforeNoUpImpulsion] \n");
        }

        if (state == Rising)
        {
            float height_delta = velocity_replacer * deltaTime;

            float new_estimated_height = estimated_jump_height + height_delta;

            if (new_estimated_height >= minJumpHeight_keep_impulse_until_height)
            {
                state = RisingBeforeNoUpImpulsion;
                height_delta = minJumpHeight_keep_impulse_until_height - estimated_jump_height;
                new_estimated_height = estimated_jump_height + height_delta;

                velocity_replacer = height_delta / deltaTime;
            }

            estimated_jump_height = new_estimated_height;
            *velocityY = velocity_replacer;

            printf("[Rising] new_estimated_height: %f keep_impulse_until: %f\n", new_estimated_height, minJumpHeight_keep_impulse_until_height);
            printf("                      position after apply velocity: %f\n", (*posY) - initial_pos + (*velocityY) * deltaTime);
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
                height_delta = minJumpHeight_max_height - estimated_jump_height;
                new_estimated_height = estimated_jump_height + height_delta;
                printf("[RisingNoUpImpulsion] (reach top) estimated_jump_height: %f max_height: %f\n", estimated_jump_height, minJumpHeight_max_height);
            }

            estimated_jump_height = new_estimated_height;
            // Output effective velocity so external system gets: position += (height_delta/deltaTime) * deltaTime = height_delta
            *velocityY = height_delta / deltaTime;

            printf("[RisingNoUpImpulsion] new_estimated_height: %f keep_impulse_until: %f\n", new_estimated_height, minJumpHeight_max_height);
            printf("                      position after apply velocity: %f\n", (*posY) - initial_pos + (*velocityY) * deltaTime);
            
        }
        else if (state == SetVelocityZeroBeforeFalling)
        {
            state = Falling;
            *velocityY = 0.0f;

            printf("[SetVelocityZeroBeforeFalling] \n");
        }
    }

};