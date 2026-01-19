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
        }
        else if (canDoubleJump && currentState == Falling)
        {
            currentState = DoubleJumping;
            *velocityY = 0.0f;
            canDoubleJump = false;
            jumpButtonHeld = true;
            currentJumpHeight = 0.0f;
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
            *velocityY = risingVelocity;
            currentJumpHeight += risingVelocity * deltaTime;
            printf("JumpState: %s, Velocity Y: %.2f, currentJumpHeight: %.2f\n", stateToString(currentState), *velocityY, currentJumpHeight);
            if (currentJumpHeight >= minJumpHeight)
            {
                if (currentJumpHeight >= maxJumpHeight || !jumpButtonHeld)
                    currentState = RisingButtonReleased;
            }
        }
        else if (currentState == DoubleJumping)
        {
            *velocityY = risingVelocity;
            currentJumpHeight += risingVelocity * deltaTime;
            printf("JumpState: %s, Velocity Y: %.2f, currentJumpHeight: %.2f\n", stateToString(currentState), *velocityY, currentJumpHeight);
            if (currentJumpHeight >= minJumpHeight)
            {
                currentState = DoubleJumpingButtonReleased;
            }
            
        }
    }
};