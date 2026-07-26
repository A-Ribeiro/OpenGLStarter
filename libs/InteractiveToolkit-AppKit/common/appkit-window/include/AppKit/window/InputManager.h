#pragma once

#include "buildFlags.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>

#include "Event.h"

#include <InteractiveToolkit/EventCore/Event.h>

namespace AppKit
{

    namespace Window
    {

        class Window;

        class InputManager
        {

        public:
            EventCore::Event<void(const AppKit::Window::WindowEvent &)> onWindowEvent;
            EventCore::Event<void(const AppKit::Window::KeyboardEvent &)> onKeyboardEvent;
            EventCore::Event<void(const AppKit::Window::MouseEvent &)> onMouseEvent;
            EventCore::Event<void(const AppKit::Window::JoystickEvent &)> onJoystickEvent;
            EventCore::Event<void(const AppKit::Window::TouchEvent &)> onTouchEvent;
            EventCore::Event<void(const AppKit::Window::SensorEvent &)> onSensorEvent;

            friend class Window;
        };

    }
}
