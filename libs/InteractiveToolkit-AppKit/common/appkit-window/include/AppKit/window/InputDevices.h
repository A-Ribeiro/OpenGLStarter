#pragma once

#include "buildFlags.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>

#include "KeyCode.h"

#include <InteractiveToolkit/MathCore/vec2.h>
#include <InteractiveToolkit/MathCore/vec3.h>

namespace AppKit
{

    namespace Window
    {

        class Window;

        namespace Devices
        {

            enum class JoystickAxis : uint8_t
            {
                X,    //!< The X axis
                Y,    //!< The Y axis
                Z,    //!< The Z axis
                R,    //!< The R axis
                U,    //!< The U axis
                V,    //!< The V axis
                PovX, //!< The X axis of the point-of-view hat
                PovY, //!< The Y axis of the point-of-view hat

                Count
            };

            struct JoystickInfo
            {
                char name[256];
                uint32_t vendorId;
                uint32_t productId;
            };

            class Joystick
            {
            public:
                uint32_t id;
                Joystick(uint32_t _id);
                bool isConnected();
                float getAxis(JoystickAxis axis);
                bool isButtonPressed(uint32_t button);
                JoystickInfo queryJoystickInfo();
            };

            class Keyboard
            {
            public:
                static bool isPressed(KeyCode code);
            };

            enum class MouseButton : uint8_t
            {
                Unknown = 0xff, //!< Unhandled button
                Left = 0,
                Right,
                Middle,
                Extra1, // XButton1,
                Extra2, // XButton2,

                Count
            };

            class Mouse
            {
            public:
                static bool isPressed(MouseButton button);
                static MathCore::vec2i getPosition(Window *relativeToWindow = nullptr);
                static void setPosition(const MathCore::vec2i &position, Window *relativeToWindow = nullptr);
            };

            enum class SensorType : uint8_t
            {
                Accelerometer,    //!< Measures the raw acceleration (m/s^2)
                Gyroscope,        //!< Measures the raw rotation rates (degrees/s)
                Magnetometer,     //!< Measures the ambient magnetic field (micro-teslas)
                Gravity,          //!< Measures the direction and intensity of gravity, independent of device acceleration (m/s^2)
                UserAcceleration, //!< Measures the direction and intensity of device acceleration, independent of the gravity (m/s^2)
                Orientation,      //!< Measures the absolute 3D orientation (degrees)

                Count
            };

            class Sensor
            {
            public:
                SensorType type;
                Sensor(SensorType _type);
                bool isAvailable();
                void setEnable(bool _enable);
                MathCore::vec3f getValue();
            };

            class Touch
            {
            public:
                static bool isPressed(uint32_t finger_id);
                static MathCore::vec2i getPosition(uint32_t finger_id, Window *relativeToWindow = nullptr);
            };

        }
    }
}
