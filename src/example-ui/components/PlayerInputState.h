#pragma once

#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>
#include <AppKit/window/InputDevices.h>

enum class RawInputFromDevice: uint8_t
{
    Keyboard,
    Joystick0,
    Joystick1,
    Joystick2,
    Joystick3,
};

enum IntInputState
{
    InputState_None = 0,
    InputState_Up = 1,
    InputState_Down = 1 << 1,
    InputState_Left = 1 << 2,
    InputState_Right = 1 << 3,
    InputState_Action = 1 << 4,
    InputState_Jump = 1 << 5,

    InputState_Start = 1 << 6,
    InputState_Select = 1 << 7,
};

enum XboxButtons
{
    Button_A  = 0,
    Button_B  = 1,
    Button_X  = 2,
    Button_Y  = 3,
    Button_LB = 4,
    Button_RB = 5,
    Button_Select = 6,
    Button_Start = 7
};

class PlayerInputState
{
    AppKit::Window::Devices::Joystick inputJoy;

public:
    RawInputFromDevice input_device;

    float x_axis;
    EventCore::PressReleaseDetector up;
    EventCore::PressReleaseDetector down;

    float y_axis;
    EventCore::PressReleaseDetector left;
    EventCore::PressReleaseDetector right;

    EventCore::PressReleaseDetector jump;
    EventCore::PressReleaseDetector action;

    EventCore::PressReleaseDetector start;
    EventCore::PressReleaseDetector select;

    IntInputState state;

    PlayerInputState(RawInputFromDevice input_device);

    void fillState();

};
