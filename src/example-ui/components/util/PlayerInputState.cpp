#include "PlayerInputState.h"


PlayerInputState::PlayerInputState(RawInputFromDevice input_device):inputJoy(0)
{
    this->input_device = input_device;
    state = InputState_None;
    if (input_device != RawInputFromDevice::Keyboard)
        inputJoy = AppKit::Window::Devices::Joystick((int)input_device - (int)RawInputFromDevice::Joystick0);
}

void PlayerInputState::fillState()
{
    if (input_device == RawInputFromDevice::Keyboard)
    {

        up.setState(AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::W) ||
                    AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::Up));
        down.setState(AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::S) ||
                      AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::Down));
        left.setState(AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::A) ||
                      AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::Left));
        right.setState(AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::D) ||
                       AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::Right));

        jump.setState(AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::Space));
        action.setState(AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::Enter));

        start.setState(AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::Escape));
        select.setState(AppKit::Window::Devices::Keyboard::isPressed(AppKit::Window::Devices::KeyCode::Tab));

        x_axis = 0.0f;
        if (left.pressed)
            x_axis -= 1.0f;
        if (right.pressed)
            x_axis += 1.0f;

        y_axis = 0.0f;
        if (up.pressed)
            y_axis += 1.0f;
        if (down.pressed)
            y_axis -= 1.0f;
    }
    else
    {
        x_axis = inputJoy.getAxis(AppKit::Window::Devices::JoystickAxis::X);
        y_axis = inputJoy.getAxis(AppKit::Window::Devices::JoystickAxis::Y);

        up.setState(y_axis > 0.5f);
        down.setState(y_axis < -0.5f);
        left.setState(x_axis < -0.5f);
        right.setState(x_axis > 0.5f);

        jump.setState(inputJoy.isButtonPressed(XboxButtons::Button_A));
        action.setState(inputJoy.isButtonPressed(XboxButtons::Button_B));

        start.setState(inputJoy.isButtonPressed(XboxButtons::Button_Start));
        select.setState(inputJoy.isButtonPressed(XboxButtons::Button_Select));
    }
    state = InputState_None;
    if (up.pressed)
        state = (IntInputState)(state | InputState_Up);
    if (down.pressed)
        state = (IntInputState)(state | InputState_Down);
    if (left.pressed)
        state = (IntInputState)(state | InputState_Left);
    if (right.pressed)
        state = (IntInputState)(state | InputState_Right);
    if (action.pressed)
        state = (IntInputState)(state | InputState_Action);
    if (jump.pressed)
        state = (IntInputState)(state | InputState_Jump);
    if (start.pressed)
        state = (IntInputState)(state | InputState_Start);
    if (select.pressed)
        state = (IntInputState)(state | InputState_Select);
}
