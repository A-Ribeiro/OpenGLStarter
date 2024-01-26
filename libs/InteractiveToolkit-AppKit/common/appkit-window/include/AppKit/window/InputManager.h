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
            EventCore::Event<void(const WindowEvent &)> onWindowEvent;
            EventCore::Event<void(const KeyboardEvent &)> onKeyboardEvent;
            EventCore::Event<void(const MouseEvent &)> onMouseEvent;
            EventCore::Event<void(const JoystickEvent &)> onJoystickEvent;
            EventCore::Event<void(const TouchEvent &)> onTouchEvent;
            EventCore::Event<void(const SensorEvent &)> onSensorEvent;

            friend class Window;
        };

    }
}
