#pragma once

#include "buildFlags.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>

#include "KeyCode.h"
#include "InputDevices.h"

#include <InteractiveToolkit/MathCore/vec2.h>
#include <InteractiveToolkit/MathCore/vec3.h>

namespace AppKit
{

    namespace Window
    {

        class Window;

        enum class WindowEventType : uint8_t
        {
            Closed,
            Resized,
            LostFocus,
            GainedFocus,

            // window related events
            TextEntered,
            MouseEntered,
            MouseLeft
        };

        struct WindowEvent
        {
            Window *window;
            WindowEventType type;
            // union
            // {
                MathCore::vec2i resized;
                uint32_t textEntered; // utf32 unicode char
            //};
        };

        // BEGIN_DECLARE_DELEGATE(WindowEventDelegate, const WindowEvent &event)
        // CALL_PATTERN(event) END_DECLARE_DELEGATE;

        enum class KeyboardEventType : uint8_t
        {
            KeyPressed,
            KeyReleased
        };

        struct KeyboardEvent
        {
            KeyboardEventType type;
            Devices::KeyCode code;
            bool alt;
            bool control;
            bool shift;
            bool system;
        };

        // BEGIN_DECLARE_DELEGATE(KeyboardEventDelegate, const KeyboardEvent &event)
        // CALL_PATTERN(event) END_DECLARE_DELEGATE;

        enum class MouseEventType : uint8_t
        {
            None = 0,
            VerticalWheelScrolled,
            HorizontalWheelScrolled,
            ButtonPressed,
            ButtonReleased,
            Moved
        };

        struct MouseEvent
        {
            MouseEventType type;
            MathCore::vec2i position;
            union
            {
                float wheelDelta;
                Devices::MouseButton button;
            };
        };

        // BEGIN_DECLARE_DELEGATE(MouseEventDelegate, const MouseEvent &event)
        // CALL_PATTERN(event) END_DECLARE_DELEGATE;

        enum JoystickEventType
        {
            JoystickEventButtonPressed,
            JoystickEventButtonReleased,
            JoystickEventMoved,
            JoystickEventConnected,
            JoystickEventDisconnected
        };

        struct JoystickMove
        {
            Devices::JoystickAxis axis;
            float value;
        };

        struct JoystickEvent
        {
            JoystickEventType type;
            uint32_t joystickId;
            union
            {
                uint32_t button;
                JoystickMove move;
            };
        };

        // BEGIN_DECLARE_DELEGATE(JoystickEventDelegate, const JoystickEvent &event)
        // CALL_PATTERN(event) END_DECLARE_DELEGATE;

        /*
            struct TouchEvent2i {
                int x;
                int y;
                static TouchEvent2i Create(int x, int y) {
                    TouchEvent2i result;
                    result.x = x;
                    result.y = y;
                    return result;
                }
            };
            */

        enum class TouchEventType : uint8_t
        {
            Began,
            Moved,
            Ended
        };

        struct TouchEvent
        {
            TouchEventType type;
            unsigned int finger;
            MathCore::vec2i position;
        };

        // BEGIN_DECLARE_DELEGATE(TouchEventDelegate, const TouchEvent &event)
        // CALL_PATTERN(event) END_DECLARE_DELEGATE;

        typedef Devices::SensorType SensorEventType;

        struct SensorEvent
        {
            SensorEventType type;
            MathCore::vec3f value;
        };

        // BEGIN_DECLARE_DELEGATE(SensorEventDelegate, const SensorEvent &event)
        // CALL_PATTERN(event) END_DECLARE_DELEGATE;

    }
}
