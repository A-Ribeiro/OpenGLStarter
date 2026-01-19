#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

class JumpState
{
public:
    enum State
    {
        Grounded,

        Rising,
        RisingButtonReleased,

        DoubleJumping,
        DoubleJumpingButtonReleased,

        Falling,
    };

    static const char *stateToString(State state)
    {
        switch (state)
        {
        case Grounded:
            return "Grounded";
        case Rising:
            return "Rising";
        case RisingButtonReleased:
            return "RisingButtonReleased";
        case DoubleJumping:
            return "DoubleJumping";
        case DoubleJumpingButtonReleased:
            return "DoubleJumpingButtonReleased";
        case Falling:
            return "Falling";
        default:
            return "Unknown";
        }
    }

private:
    State currentState;
    bool canDoubleJump;

    float maxJumpHeight;
    float minJumpHeight;

    float currentJumpHeight; // Track how high we've jumped so far
    bool jumpButtonHeld;     // Track if jump button is still pressed

    float risingVelocity;

    float time_to_reach_zero_velocity;
    float time_advance;

    float maxJumpHeightFinal;
    float minJumpHeightFinal;

    float initialRisingVelocity;

public:
    JumpState() : currentState(Grounded), canDoubleJump(true), maxJumpHeight(0.0f),
                  currentJumpHeight(0.0f), jumpButtonHeld(false), risingVelocity(0.0f) {}
    State getState() const { return currentState; }

    bool canJump() const
    {
        return currentState == Grounded || (canDoubleJump && currentState == Falling);
    }

    bool getCanDoubleJump() const { return canDoubleJump; }
    void setCanDoubleJump(bool value) { canDoubleJump = value; }

    // set the constant up rising velocity during jump
    void setRisingVelocity(float value) { risingVelocity = value; }
    float getRisingVelocity() const { return risingVelocity; }

    void setMinJumpHeight(float value) { minJumpHeight = value; }
    float getMinJumpHeight() const { return minJumpHeight; }

    void setMaxJumpHeight(float value) { maxJumpHeight = value; }
    float getMaxJumpHeight() const { return maxJumpHeight; }

    void computeConstants(float gravity)
    {
        // acceleration is linear and constant
        time_to_reach_zero_velocity = MathCore::OP<float>::abs(risingVelocity / gravity);

        // the velocity(d) is a quadratic function on time
        // d = v0 * t + 0.5 * gravity * t^2
        // y = x^2 0.5 g * 1/g^2 + x^2 * 1/g
        float height_rising_max = risingVelocity * time_to_reach_zero_velocity + 0.5f * gravity * time_to_reach_zero_velocity * time_to_reach_zero_velocity;

        maxJumpHeightFinal = maxJumpHeight - height_rising_max;
        minJumpHeightFinal = minJumpHeight - height_rising_max;

        initialRisingVelocity = risingVelocity;

        if (minJumpHeightFinal < 0)
        {
            // need to decrease rising velocity according the height offset
            // d = v0 * (v0/gravity) + 0.5 * gravity * (v0/gravity)^2

            // x = ± (sqrt(d) 1 g 0.816/sqrt(g)) and 1.225 sqrt(g) != 0 and g !=0

            float d = minJumpHeight;
            
            float sqrt_d = MathCore::OP<float>::sqrt(d);
            float sqrt_g = MathCore::OP<float>::sqrt(MathCore::OP<float>::abs(gravity));
            const float sqrt_2 = MathCore::OP<float>::sqrt(2.0f);

            initialRisingVelocity = sqrt_d * sqrt_g * sqrt_2;

            minJumpHeightFinal = 0;
            maxJumpHeightFinal = maxJumpHeight - d;
        }
    }

    // when hits jump button down event occurs
    void jump(float *velocityY)
    {
        if (currentState == Grounded)
        {
            currentState = Rising;
            *velocityY = 0.0f;
            canDoubleJump = true;
            jumpButtonHeld = true;
            currentJumpHeight = 0.0f;
            time_advance = 0.0f;
        }
        else if (canDoubleJump && currentState == Falling)
        {
            currentState = DoubleJumping;
            *velocityY = 0.0f;
            canDoubleJump = false;
            jumpButtonHeld = true;
            currentJumpHeight = 0.0f;
            time_advance = 0.0f;
        }
    }

    void setGrounded()
    {
        if (currentState == Grounded)
            return;
        currentState = Grounded;
        canDoubleJump = true;
        currentJumpHeight = 0.0f;
    }

    void setJumpPressed(bool pressed)
    {
        jumpButtonHeld = jumpButtonHeld && pressed;
    }

    // updates velocity until reachs max height
    void updateVelocity(float *velocityY, float deltaTime)
    {
        if (currentState == JumpState::Grounded)
        {
            *velocityY = 0.0f;
            canDoubleJump = true;
            currentJumpHeight = 0.0f;
        }
        else if (*velocityY < -0.5f)
        {
            currentState = Falling;
        }

        if (currentState == Rising)
        {
            *velocityY = initialRisingVelocity;
            currentJumpHeight += initialRisingVelocity * deltaTime;
            printf("JumpState: %s, Velocity Y: %.2f, currentJumpHeight: %.2f\n", stateToString(currentState), *velocityY, currentJumpHeight);
            if (currentJumpHeight >= minJumpHeightFinal)
            {
                if (currentJumpHeight >= maxJumpHeightFinal || !jumpButtonHeld)
                    currentState = RisingButtonReleased;
            }
        }
        else if (currentState == DoubleJumping)
        {
            *velocityY = initialRisingVelocity;
            currentJumpHeight += initialRisingVelocity * deltaTime;
            printf("JumpState: %s, Velocity Y: %.2f, currentJumpHeight: %.2f\n", stateToString(currentState), *velocityY, currentJumpHeight);
            if (currentJumpHeight >= minJumpHeightFinal)
            {
                currentState = DoubleJumpingButtonReleased;
            }
        }
    }
};