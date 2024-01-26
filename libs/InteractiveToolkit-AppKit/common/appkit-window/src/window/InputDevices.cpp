#include <AppKit/window/InputDevices.h>
#include <AppKit/window/Window.h>

#if defined(APPKIT_WINDOW_SFML)

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
// #include <SFML/Graphics.hpp>

namespace AppKit
{

    namespace Window
    {

        namespace Devices
        {

            //
            // Joystick
            //
            const sf::Joystick::Axis joy_axis_mapping[] = {
                sf::Joystick::Axis::X,
                sf::Joystick::Axis::Y,
                sf::Joystick::Axis::Z,
                sf::Joystick::Axis::R,
                sf::Joystick::Axis::U,
                sf::Joystick::Axis::V,
                sf::Joystick::Axis::PovX,
                sf::Joystick::Axis::PovY};
            Joystick::Joystick(uint32_t _id)
            {
                id = _id;
            }
            bool Joystick::isConnected()
            {
                return sf::Joystick::isConnected(id);
            }
            float Joystick::getAxis(JoystickAxis axis)
            {
                if (!sf::Joystick::hasAxis(id, joy_axis_mapping[(int)axis]))
                    return 0.0f;
                return sf::Joystick::getAxisPosition(id, joy_axis_mapping[(int)axis]);
            }
            bool Joystick::isButtonPressed(uint32_t button)
            {
                if (button >= sf::Joystick::getButtonCount(id))
                    return false;
                return sf::Joystick::isButtonPressed(id, button);
            }
            JoystickInfo Joystick::queryJoystickInfo()
            {
                JoystickInfo result;
                memset(&result, 0, sizeof(JoystickInfo));

                sf::Joystick::Identification id_info = sf::Joystick::getIdentification(id);
                strncpy(result.name, id_info.name.toAnsiString().c_str(), 256);

                result.productId = id_info.productId;
                result.vendorId = id_info.vendorId;

                return result;
            }

            //
            // Keyboard
            //
            const sf::Keyboard::Key key_mapping[] = {
                sf::Keyboard::Key::A,
                sf::Keyboard::Key::B,
                sf::Keyboard::Key::C,
                sf::Keyboard::Key::D,
                sf::Keyboard::Key::E,
                sf::Keyboard::Key::F,
                sf::Keyboard::Key::G,
                sf::Keyboard::Key::H,
                sf::Keyboard::Key::I,
                sf::Keyboard::Key::J,
                sf::Keyboard::Key::K,
                sf::Keyboard::Key::L,
                sf::Keyboard::Key::M,
                sf::Keyboard::Key::N,
                sf::Keyboard::Key::O,
                sf::Keyboard::Key::P,
                sf::Keyboard::Key::Q,
                sf::Keyboard::Key::R,
                sf::Keyboard::Key::S,
                sf::Keyboard::Key::T,
                sf::Keyboard::Key::U,
                sf::Keyboard::Key::V,
                sf::Keyboard::Key::W,
                sf::Keyboard::Key::X,
                sf::Keyboard::Key::Y,
                sf::Keyboard::Key::Z,
                sf::Keyboard::Key::Num0,
                sf::Keyboard::Key::Num1,
                sf::Keyboard::Key::Num2,
                sf::Keyboard::Key::Num3,
                sf::Keyboard::Key::Num4,
                sf::Keyboard::Key::Num5,
                sf::Keyboard::Key::Num6,
                sf::Keyboard::Key::Num7,
                sf::Keyboard::Key::Num8,
                sf::Keyboard::Key::Num9,
                sf::Keyboard::Key::Escape,
                sf::Keyboard::Key::LControl,
                sf::Keyboard::Key::LShift,
                sf::Keyboard::Key::LAlt,
                sf::Keyboard::Key::LSystem,
                sf::Keyboard::Key::RControl,
                sf::Keyboard::Key::RShift,
                sf::Keyboard::Key::RAlt,
                sf::Keyboard::Key::RSystem,
                sf::Keyboard::Key::Menu,
                sf::Keyboard::Key::LBracket,
                sf::Keyboard::Key::RBracket,
                sf::Keyboard::Key::Semicolon,
                sf::Keyboard::Key::Comma,
                sf::Keyboard::Key::Period,
                sf::Keyboard::Key::Apostrophe,// Quote,
                sf::Keyboard::Key::Slash,
                sf::Keyboard::Key::Backslash,
                sf::Keyboard::Key::Grave,// Tilde,
                sf::Keyboard::Key::Equal,
                sf::Keyboard::Key::Hyphen,
                sf::Keyboard::Key::Space,
                sf::Keyboard::Key::Enter,
                sf::Keyboard::Key::Backspace,
                sf::Keyboard::Key::Tab,
                sf::Keyboard::Key::PageUp,
                sf::Keyboard::Key::PageDown,
                sf::Keyboard::Key::End,
                sf::Keyboard::Key::Home,
                sf::Keyboard::Key::Insert,
                sf::Keyboard::Key::Delete,
                sf::Keyboard::Key::Add,
                sf::Keyboard::Key::Subtract,
                sf::Keyboard::Key::Multiply,
                sf::Keyboard::Key::Divide,
                sf::Keyboard::Key::Left,
                sf::Keyboard::Key::Right,
                sf::Keyboard::Key::Up,
                sf::Keyboard::Key::Down,
                sf::Keyboard::Key::Numpad0,
                sf::Keyboard::Key::Numpad1,
                sf::Keyboard::Key::Numpad2,
                sf::Keyboard::Key::Numpad3,
                sf::Keyboard::Key::Numpad4,
                sf::Keyboard::Key::Numpad5,
                sf::Keyboard::Key::Numpad6,
                sf::Keyboard::Key::Numpad7,
                sf::Keyboard::Key::Numpad8,
                sf::Keyboard::Key::Numpad9,
                sf::Keyboard::Key::F1,
                sf::Keyboard::Key::F2,
                sf::Keyboard::Key::F3,
                sf::Keyboard::Key::F4,
                sf::Keyboard::Key::F5,
                sf::Keyboard::Key::F6,
                sf::Keyboard::Key::F7,
                sf::Keyboard::Key::F8,
                sf::Keyboard::Key::F9,
                sf::Keyboard::Key::F10,
                sf::Keyboard::Key::F11,
                sf::Keyboard::Key::F12,
                sf::Keyboard::Key::F13,
                sf::Keyboard::Key::F14,
                sf::Keyboard::Key::F15,
                sf::Keyboard::Key::Pause};
            bool Keyboard::isPressed(KeyCode code)
            {
                if (code == KeyCode::Unknown || code == KeyCode::Count)
                    return false;
                return sf::Keyboard::isKeyPressed(key_mapping[(int)code]);
            }

            //
            // Mouse
            //
            const sf::Mouse::Button mouse_btn_mapping[] = {
                sf::Mouse::Button::Left,
                sf::Mouse::Button::Right,
                sf::Mouse::Button::Middle,
                sf::Mouse::Button::Extra1,
                sf::Mouse::Button::Extra2};
            bool Mouse::isPressed(MouseButton button)
            {
                return sf::Mouse::isButtonPressed(mouse_btn_mapping[(int)button]);
            }
            MathCore::vec2i Mouse::getPosition(Window *relativeToWindow)
            {
                sf::Vector2i mouse_pos;
                if (relativeToWindow != NULL)
                    mouse_pos = sf::Mouse::getPosition(*(sf::Window *)relativeToWindow->getLibraryHandle());
                else
                    mouse_pos = sf::Mouse::getPosition();
                return MathCore::vec2i(mouse_pos.x, mouse_pos.y);
            }
            void Mouse::setPosition(const MathCore::vec2i &position, Window *relativeToWindow)
            {
                sf::Vector2i mouse_pos(position.x, position.y);
                if (relativeToWindow != NULL)
                    sf::Mouse::setPosition(mouse_pos, *(sf::Window *)relativeToWindow->getLibraryHandle());
                else
                    sf::Mouse::setPosition(mouse_pos);
            }

            //
            // Sensor
            //
            const sf::Sensor::Type sensor_mapping[] = {
                sf::Sensor::Type::Accelerometer,
                sf::Sensor::Type::Gyroscope,
                sf::Sensor::Type::Magnetometer,
                sf::Sensor::Type::Gravity,
                sf::Sensor::Type::UserAcceleration,
                sf::Sensor::Type::Orientation};
            Sensor::Sensor(SensorType _type)
            {
                type = _type;
            }
            bool Sensor::isAvailable()
            {
                return sf::Sensor::isAvailable(sensor_mapping[(int)type]);
            }
            void Sensor::setEnable(bool _enable)
            {
                sf::Sensor::setEnabled(sensor_mapping[(int)type], _enable);
            }
            MathCore::vec3f Sensor::getValue()
            {
                sf::Vector3f result = sf::Sensor::getValue(sensor_mapping[(int)type]);
                return MathCore::vec3f(result.x, result.y, result.z);
            }

            //
            // Touch
            //
            bool Touch::isPressed(uint32_t finger_id)
            {
                return sf::Touch::isDown(finger_id);
            }
            MathCore::vec2i Touch::getPosition(uint32_t finger_id, Window *relativeToWindow)
            {
                sf::Vector2i pos;
                if (relativeToWindow != NULL)
                    pos = sf::Touch::getPosition(finger_id, *(sf::Window *)relativeToWindow->getLibraryHandle());
                else
                    pos = sf::Touch::getPosition(finger_id);
                return MathCore::vec2i(pos.x, pos.y);
            }

        }
    }
}

#elif defined(APPKIT_WINDOW_GLFW)

#error "Not Implemented"

#elif defined(APPKIT_WINDOW_RPI)

#include <aRibeiroPlatform/aRibeiroPlatform.h>

#include <sstream>
#include <string>
#include <vector>
#include <queue>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

// terminal includes...
#include <termios.h>
#include <unistd.h>
#include <iostream>

#include <functional>

#include <linux/input.h>

// #include <glad/EGL/eglplatform.h>
// #include <glad/glad/egl.h>

namespace windowGLWrapper
{

    MathCore::vec2i mousePos; // hack to initialize the mouse position
    uint32_t _hack_window_width;
    uint32_t _hack_window_height;

    //
    // disable Xorg mouse input
    //
    class XInputBlockMouse
    {
    public:
        XInputBlockMouse()
        {
        }

        void disable()
        {
            // return;
            system(
                "if [ -x \"$(command -v xinput)\" ]; then "
                "xinput --disable `xinput | grep -i mouse | tr -d \" \" | cut -d\" \" -f2 | cut -d\"=\" -f2 | cut -f1`"
                "; fi;");
        }
        ~XInputBlockMouse()
        {
            system(
                "if [ -x \"$(command -v xinput)\" ]; then "
                "xinput --enable `xinput | grep -i mouse | tr -d \" \" | cut -d\" \" -f2 | cut -d\"=\" -f2 | cut -f1`"
                "; fi;");
        }
    };

    //
    // Experimental Classes to disable terminal echo & blocking
    // from: https://stackoverflow.com/questions/29973445/prevent-write-on-console-at-keypress-linux-c
    // A class for mofdifying the behavour of a terminal.
    class Terminal
    {
    public:
        typedef speed_t speed_type;

        // Initialize the terminal file descriptor and store the attributes of the terminal.
        Terminal(int fd) : m_fd(fd), m_restore(get(fd)) {}

        // Restore the orignal attributes of the terminal
        ~Terminal()
        {
            set(m_fd, m_restore, false);
        }

        Terminal(const Terminal &) = delete;
        const Terminal &operator=(const Terminal &) = delete;

        int fd() const
        {
            return m_fd;
        }
        void restore()
        {
            set(m_fd, m_restore);
        }

    protected:
        // Get attributes of a terminal
        static termios get(const int fd)
        {
            termios attributes;
            if (tcgetattr(fd, &attributes) < 0)
            {
                throw std::runtime_error("Terminal");
            }
            return attributes;
        }

        // Set attributes of a terminal
        static void set(const int fd, const termios &attributes, bool exception = true)
        {
            if (tcsetattr(fd, TCSANOW, &attributes) < 0 && exception)
            {
                throw std::runtime_error("Terminal");
            }
        }

        // Set attributes of a terminal
        static void set(const int fd, int action, const termios &attributes, bool exception = true)
        {
            if (tcsetattr(fd, action, &attributes) < 0 && exception)
            {
                throw std::runtime_error("Terminal");
            }
        }

    private:
        int m_fd;
        termios m_restore;
    };

    // A class for mofdifying the input behavour of a terminal.
    class StdInputTerminal : public Terminal
    {
    public:
        enum Attributes
        {
            Blocking = 0x01,
            Echo = 0x02
        };

        // Clear available input in std::cin
        static void clear()
        {
            termios attributes = disable_attributes(Blocking);
            while (std::cin)
                std::cin.get();
            std::cin.clear();
            set(fileno(stdin), attributes);
        }

        // Initialize with 'stdin'
        StdInputTerminal() : Terminal(fileno(stdin)) {}

        // Disable attributes specified by any combination of Attributes flags
        void disable(unsigned flags)
        {
#if NDEBUG
            disable_attributes(flags);
#endif
        }

        // Disable blocking
        void disable_blocking()
        {
            disable_attributes(Blocking);
        }

    protected:
        // Set attributes of the terminal
        static termios disable_attributes(unsigned flags)
        {
            const int fd = fileno(stdin);
            termios attributes = get(fd);
            termios a = attributes;
            if (flags & Blocking)
            {
                a.c_lflag &= ~ICANON;
                a.c_cc[VMIN] = 0;
                a.c_cc[VTIME] = 0;
            }
            if (flags & Echo)
            {
                a.c_lflag &= ~ECHO;
            }
            set(fd, a);
            return attributes;
        }
    };

    // aRibeiro::ObjectQueue<KeyboardEvent> keyboardEvents(false);

    struct TouchSlot
    {
        int oldId;
        int id;
        MathCore::vec2i pos;

        TouchSlot()
        {
            oldId = -1;
            id = -1;
            pos.x = 0;
            pos.y = 0;
        }
    };

    void _release_at_exit();
    class KeyboardAndMouseState
    {

        void openFDs()
        {
            for (int i = 0; i < 32; i++)
            {
                std::ostringstream name;
                name << "/dev/input/event" << i;
                int temp_fd = open(name.str().c_str(), O_RDONLY | O_NONBLOCK);
                if (temp_fd < 0)
                {
                    if (errno != ENOENT)
                        std::cerr << "Error opening " << name.str() << ": " << strerror(errno) << std::endl;
                    continue;
                }

                // TODO:
                //
                // For now we blindly keep all file descriptors open... maybe try
                // to figure out if fd is a mouse or keyboard and close the rest?
                //
                // see: ioctl( fd, EVIOCGNAME( sizeof(buff), buff );
                //
                fds.push_back(temp_fd);
            }
        }
        void closeFDs()
        {
            std::vector<int>::iterator it;
            for (it = fds.begin(); it != fds.end(); it++)
                close(*it);
            fds.clear();
        }

        Devices::MouseButton toMouseButton(int c)
        {
            switch (c)
            {
            case BTN_LEFT:
                return Devices::MouseButton::Left;
            case BTN_RIGHT:
                return Devices::MouseButton::Right;
            case BTN_MIDDLE:
                return Devices::MouseButton::Middle;
            case BTN_SIDE:
                return Devices::MouseButton::XButton1;
            case BTN_EXTRA:
                return Devices::MouseButton::XButton2;

            default:
                return Devices::MouseButton::Unknown;
            }
        }

        Devices::KeyCode toKey(int c)
        {
            switch (c)
            {
            case KEY_ESC:
                return Devices::KeyCode::Escape;
            case KEY_1:
                return Devices::KeyCode::Num1;
            case KEY_2:
                return Devices::KeyCode::Num2;
            case KEY_3:
                return Devices::KeyCode::Num3;
            case KEY_4:
                return Devices::KeyCode::Num4;
            case KEY_5:
                return Devices::KeyCode::Num5;
            case KEY_6:
                return Devices::KeyCode::Num6;
            case KEY_7:
                return Devices::KeyCode::Num7;
            case KEY_8:
                return Devices::KeyCode::Num8;
            case KEY_9:
                return Devices::KeyCode::Num9;
            case KEY_0:
                return Devices::KeyCode::Num0;
            case KEY_MINUS:
                return Devices::KeyCode::Hyphen;
            case KEY_EQUAL:
                return Devices::KeyCode::Equal;
            case KEY_BACKSPACE:
                return Devices::KeyCode::Backspace;
            case KEY_TAB:
                return Devices::KeyCode::Tab;
            case KEY_Q:
                return Devices::KeyCode::Q;
            case KEY_W:
                return Devices::KeyCode::W;
            case KEY_E:
                return Devices::KeyCode::E;
            case KEY_R:
                return Devices::KeyCode::R;
            case KEY_T:
                return Devices::KeyCode::T;
            case KEY_Y:
                return Devices::KeyCode::Y;
            case KEY_U:
                return Devices::KeyCode::U;
            case KEY_I:
                return Devices::KeyCode::I;
            case KEY_O:
                return Devices::KeyCode::O;
            case KEY_P:
                return Devices::KeyCode::P;
            case KEY_LEFTBRACE:
                return Devices::KeyCode::LBracket;
            case KEY_RIGHTBRACE:
                return Devices::KeyCode::RBracket;
            case KEY_KPENTER:
            case KEY_ENTER:
                return Devices::KeyCode::Enter;
            case KEY_LEFTCTRL:
                return Devices::KeyCode::LControl;
            case KEY_A:
                return Devices::KeyCode::A;
            case KEY_S:
                return Devices::KeyCode::S;
            case KEY_D:
                return Devices::KeyCode::D;
            case KEY_F:
                return Devices::KeyCode::F;
            case KEY_G:
                return Devices::KeyCode::G;
            case KEY_H:
                return Devices::KeyCode::H;
            case KEY_J:
                return Devices::KeyCode::J;
            case KEY_K:
                return Devices::KeyCode::K;
            case KEY_L:
                return Devices::KeyCode::L;
            case KEY_SEMICOLON:
                return Devices::KeyCode::Semicolon;
            case KEY_APOSTROPHE:
                return Devices::KeyCode::Quote;
            case KEY_GRAVE:
                return Devices::KeyCode::Tilde;
            case KEY_LEFTSHIFT:
                return Devices::KeyCode::LShift;
            case KEY_BACKSLASH:
                return Devices::KeyCode::Backslash;
            case KEY_Z:
                return Devices::KeyCode::Z;
            case KEY_X:
                return Devices::KeyCode::X;
            case KEY_C:
                return Devices::KeyCode::C;
            case KEY_V:
                return Devices::KeyCode::V;
            case KEY_B:
                return Devices::KeyCode::B;
            case KEY_N:
                return Devices::KeyCode::N;
            case KEY_M:
                return Devices::KeyCode::M;
            case KEY_COMMA:
                return Devices::KeyCode::Comma;
            case KEY_DOT:
                return Devices::KeyCode::Period;
            case KEY_SLASH:
                return Devices::KeyCode::Slash;
            case KEY_RIGHTSHIFT:
                return Devices::KeyCode::RShift;
            case KEY_KPASTERISK:
                return Devices::KeyCode::Multiply;
            case KEY_LEFTALT:
                return Devices::KeyCode::LAlt;
            case KEY_SPACE:
                return Devices::KeyCode::Space;
            case KEY_F1:
                return Devices::KeyCode::F1;
            case KEY_F2:
                return Devices::KeyCode::F2;
            case KEY_F3:
                return Devices::KeyCode::F3;
            case KEY_F4:
                return Devices::KeyCode::F4;
            case KEY_F5:
                return Devices::KeyCode::F5;
            case KEY_F6:
                return Devices::KeyCode::F6;
            case KEY_F7:
                return Devices::KeyCode::F7;
            case KEY_F8:
                return Devices::KeyCode::F8;
            case KEY_F9:
                return Devices::KeyCode::F9;
            case KEY_F10:
                return Devices::KeyCode::F10;
            case KEY_F11:
                return Devices::KeyCode::F11;
            case KEY_F12:
                return Devices::KeyCode::F12;
            case KEY_F13:
                return Devices::KeyCode::F13;
            case KEY_F14:
                return Devices::KeyCode::F14;
            case KEY_F15:
                return Devices::KeyCode::F15;
            case KEY_KP7:
                return Devices::KeyCode::Numpad7;
            case KEY_KP8:
                return Devices::KeyCode::Numpad8;
            case KEY_KP9:
                return Devices::KeyCode::Numpad9;
            case KEY_KPMINUS:
                return Devices::KeyCode::Subtract;
            case KEY_KP4:
                return Devices::KeyCode::Numpad4;
            case KEY_KP5:
                return Devices::KeyCode::Numpad5;
            case KEY_KP6:
                return Devices::KeyCode::Numpad6;
            case KEY_KPPLUS:
                return Devices::KeyCode::Add;
            case KEY_KP1:
                return Devices::KeyCode::Numpad1;
            case KEY_KP2:
                return Devices::KeyCode::Numpad2;
            case KEY_KP3:
                return Devices::KeyCode::Numpad3;
            case KEY_KP0:
                return Devices::KeyCode::Numpad0;
            case KEY_KPDOT:
                return Devices::KeyCode::Delete;
            case KEY_RIGHTCTRL:
                return Devices::KeyCode::RControl;
            case KEY_KPSLASH:
                return Devices::KeyCode::Divide;
            case KEY_RIGHTALT:
                return Devices::KeyCode::RAlt;
            case KEY_HOME:
                return Devices::KeyCode::Home;
            case KEY_UP:
                return Devices::KeyCode::Up;
            case KEY_PAGEUP:
                return Devices::KeyCode::PageUp;
            case KEY_LEFT:
                return Devices::KeyCode::Left;
            case KEY_RIGHT:
                return Devices::KeyCode::Right;
            case KEY_END:
                return Devices::KeyCode::End;
            case KEY_DOWN:
                return Devices::KeyCode::Down;
            case KEY_PAGEDOWN:
                return Devices::KeyCode::PageDown;
            case KEY_INSERT:
                return Devices::KeyCode::Insert;
            case KEY_DELETE:
                return Devices::KeyCode::Delete;
            case KEY_PAUSE:
                return Devices::KeyCode::Pause;
            case KEY_LEFTMETA:
                return Devices::KeyCode::LSystem;
            case KEY_RIGHTMETA:
                return Devices::KeyCode::RSystem;

            case KEY_RESERVED:
            case KEY_SYSRQ:
            case KEY_CAPSLOCK:
            case KEY_NUMLOCK:
            case KEY_SCROLLLOCK:
            default:
                return Devices::KeyCode::Unknown;
            }
        }

    public:
        // aRibeiro::PlatformMutex mutex;
        StdInputTerminal inputTerminal;
        XInputBlockMouse mouseDisabler;

        std::vector<int> fds;                            // list of open file descriptors for /dev/input
        bool keyMap[(int)Devices::KeyCode::Count];       // track whether keys are down
        bool mouseMap[(int)Devices::MouseButton::Count]; // track whether mouse buttons are down

        int touchFd;                       // file descriptor we have seen MT events on; assumes only 1
        std::vector<TouchSlot> touchSlots; // track the state of each touch "slot"
        int currentSlot;                   // which slot are we currently updating?

        bool isAltDown()
        {
            return (keyMap[(int)Devices::KeyCode::LAlt] || keyMap[(int)Devices::KeyCode::RAlt]);
        }
        bool isControlDown()
        {
            return (keyMap[(int)Devices::KeyCode::LControl] || keyMap[(int)Devices::KeyCode::RControl]);
        }
        bool isShiftDown()
        {
            return (keyMap[(int)Devices::KeyCode::LShift] || keyMap[(int)Devices::KeyCode::RShift]);
        }
        bool isSystemDown()
        {
            return (keyMap[(int)Devices::KeyCode::LSystem] || keyMap[(int)Devices::KeyCode::RSystem]);
        }

        bool isKeyPressed(const Devices::KeyCode &key) const
        {
            if ((int)key < 0 || (int)key >= (int)Devices::KeyCode::Count)
                return false;
            return keyMap[(int)key];
        }

        bool isMouseButtonPressed(const Devices::MouseButton &btn) const
        {
            if ((int)btn < 0 || (int)btn >= (int)Devices::MouseButton::Count)
                return false;
            return mouseMap[(int)btn];
        }

        TouchSlot *touchAtSlot(int idx)
        {
            if (idx >= touchSlots.size())
                touchSlots.resize(idx + 1);
            return &touchSlots[idx];
        }

        bool isTouchDown(int finger_id) const
        {
            std::vector<TouchSlot>::const_iterator it;
            for (it = touchSlots.begin(); it != touchSlots.end(); it++)
            {
                if ((*it).id == finger_id)
                    return true;
            }
            return false;
        }
        MathCore::vec2i getTouchPosition(int finger_id) const
        {
            std::vector<TouchSlot>::const_iterator it;
            for (it = touchSlots.begin(); it != touchSlots.end(); it++)
            {
                if ((*it).id == finger_id)
                    return (*it).pos;
            }
            return MathCore::vec2i(0, 0);
        }

        KeyboardAndMouseState()
        {
            memset(keyMap, 0, sizeof(bool) * (int)Devices::KeyCode::Count);
            memset(mouseMap, 0, sizeof(bool) * (int)Devices::MouseButton::Count);
            touchFd = -1;
            currentSlot = 0;

#ifdef NDEBUG
            inputTerminal.disable(StdInputTerminal::Blocking | StdInputTerminal::Echo);
            mouseDisabler.disable();
#endif
            openFDs();

            static bool added = false;
            if (!added)
            {
                atexit(&_release_at_exit);
                added = true;
            }
        }

        ~KeyboardAndMouseState()
        {
            closeFDs();
        }

        void processTouchSlots(const std::function<void(const TouchEvent &)> &OnTouchEvent)
        {
            std::vector<TouchSlot>::iterator it;
            for (it = touchSlots.begin(); it != touchSlots.end(); it++)
            {
                TouchSlot *slot = &(*it);

                TouchEvent touchEvent;
                memset(&touchEvent, 0, sizeof(TouchEvent));
                // touchEvent.type = TouchEventType::Began;
                // touchEvent.finger = event.touch.finger;
                touchEvent.position = slot->pos;

                if (slot->oldId == slot->id)
                {
                    touchEvent.type = TouchEventType::Moved;
                    touchEvent.finger = slot->id;
                    OnTouchEvent(touchEvent);
                }
                else
                {
                    if (slot->oldId != -1)
                    {
                        touchEvent.type = TouchEventType::Ended;
                        touchEvent.finger = slot->oldId;
                        OnTouchEvent(touchEvent);
                    }
                    if (slot->id != -1)
                    {
                        touchEvent.type = TouchEventType::Began;
                        touchEvent.finger = slot->id;
                        OnTouchEvent(touchEvent);
                    }
                    slot->oldId = slot->id;
                }
            }
        }

        void ProcessInputEvents(
            const std::function<void(const WindowEvent &)> &OnWindowEvent,
            const std::function<void(const KeyboardEvent &)> &OnKeyboardEvent,
            const std::function<void(const MouseEvent &)> &OnMouseEvent,
            const std::function<void(const TouchEvent &)> &OnTouchEvent)
        {

            // Check all the open file descriptors for the next event
            //
            std::vector<int>::iterator it;
            for (it = fds.begin(); it != fds.end(); it++)
            {
                struct input_event ie;
                int rd = read(*it, &ie, sizeof(struct input_event));
                while (rd > 0)
                {
                    if (ie.type == EV_KEY)
                    {
                        Devices::MouseButton mb = toMouseButton(ie.code);
                        if (mb != Devices::MouseButton::Unknown)
                        {
                            mouseMap[(int)mb] = ie.value;

                            MouseEvent mouseEvent;
                            memset(&mouseEvent, 0, sizeof(MouseEvent));
                            mouseEvent.type = ie.value ? MouseEventType::ButtonPressed : MouseEventType::ButtonReleased;
                            mouseEvent.position = mousePos;
                            mouseEvent.button = mb;

                            OnMouseEvent(mouseEvent);
                        }
                        else
                        {
                            Devices::KeyCode kb = toKey(ie.code);

                            int special = 0;
                            if (kb == Devices::KeyCode::Delete ||
                                kb == Devices::KeyCode::Backspace)
                                special = (kb == Devices::KeyCode::Delete) ? 127 : 8;

                            if (ie.value == 2)
                            {
                                // key repeat events
                                //
                                if (special)
                                {
                                    WindowEvent windowEvent;
                                    memset(&windowEvent, 0, sizeof(WindowEvent));
                                    // windowEvent.window = this;
                                    windowEvent.type = WindowEventType::TextEntered;
                                    windowEvent.textEntered = special;
                                    OnWindowEvent(windowEvent);
                                }
                            }
                            else if (kb != Devices::KeyCode::Unknown)
                            {

                                keyMap[(int)kb] = ie.value;

                                KeyboardEvent keyboardEvent;
                                memset(&keyboardEvent, 0, sizeof(KeyboardEvent));
                                keyboardEvent.type = ie.value ? KeyboardEventType::KeyPressed : KeyboardEventType::KeyReleased;
                                keyboardEvent.code = kb;
                                keyboardEvent.alt = isAltDown();
                                keyboardEvent.control = isControlDown();
                                keyboardEvent.shift = isShiftDown();
                                keyboardEvent.system = isSystemDown();

                                if (special && ie.value)
                                {
                                    WindowEvent windowEvent;
                                    memset(&windowEvent, 0, sizeof(WindowEvent));
                                    // windowEvent.window = this;
                                    windowEvent.type = WindowEventType::TextEntered;
                                    windowEvent.textEntered = special;
                                    OnWindowEvent(windowEvent);
                                }
                            }
                        }
                    }
                    else if (ie.type == EV_REL)
                    {
                        int oldValue;
                        switch (ie.code)
                        {
                        case REL_X:
                            oldValue = mousePos.x;
                            mousePos.x += ie.value;
                            if (mousePos.x < 0)
                                mousePos.x = 0;
                            else if (mousePos.x >= _hack_window_width)
                                mousePos.x = _hack_window_width - 1;
                            if (oldValue != mousePos.x)
                            {
                                MouseEvent mouseEvent;
                                memset(&mouseEvent, 0, sizeof(MouseEvent));
                                mouseEvent.type = MouseEventType::Moved;
                                mouseEvent.position = mousePos;
                                OnMouseEvent(mouseEvent);
                            }
                            break;
                        case REL_Y:
                            oldValue = mousePos.y;
                            mousePos.y += ie.value;
                            if (mousePos.y < 0)
                                mousePos.y = 0;
                            else if (mousePos.y >= _hack_window_height)
                                mousePos.y = _hack_window_height - 1;
                            if (oldValue != mousePos.y)
                            {
                                MouseEvent mouseEvent;
                                memset(&mouseEvent, 0, sizeof(MouseEvent));
                                mouseEvent.type = MouseEventType::Moved;
                                mouseEvent.position = mousePos;
                                OnMouseEvent(mouseEvent);
                            }
                            break;
                        case REL_WHEEL:
                            MouseEvent mouseEvent;
                            memset(&mouseEvent, 0, sizeof(MouseEvent));
                            //    mouseEvent.type = MouseEventType::HorizontalWheelScrolled;
                            mouseEvent.type = MouseEventType::VerticalWheelScrolled;
                            mouseEvent.position = mousePos;
                            mouseEvent.wheelDelta = ie.value;
                            OnMouseEvent(mouseEvent);
                            break;
                        }
                    }
                    else if (ie.type == EV_ABS)
                    {
                        switch (ie.code)
                        {
                        case ABS_MT_SLOT:
                            currentSlot = ie.value;
                            touchFd = *it;
                            break;
                        case ABS_MT_TRACKING_ID:
                            touchAtSlot(currentSlot)->id = ie.value;
                            touchFd = *it;
                            break;
                        case ABS_MT_POSITION_X:
                            touchAtSlot(currentSlot)->pos.x = ie.value;
                            touchFd = *it;
                            break;
                        case ABS_MT_POSITION_Y:
                            touchAtSlot(currentSlot)->pos.y = ie.value;
                            touchFd = *it;
                            break;
                        }
                    }
                    else if (ie.type == EV_SYN && ie.code == SYN_REPORT && *it == touchFd)
                    {
                        // This pushes events directly to the queue, because it
                        // can generate more than one event.
                        processTouchSlots(OnTouchEvent);
                    }
                    rd = read(*it, &ie, sizeof(struct input_event));
                }

                if ((rd < 0) && (errno != EAGAIN))
                    std::cerr << " Error: " << strerror(errno) << std::endl;
            }

            // Finally check if there is a Text event on stdin
            //
            struct termios newt, oldt;
            tcgetattr(STDIN_FILENO, &oldt);

            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);

            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;

            unsigned char c = 0;

            fd_set rdfs;
            FD_ZERO(&rdfs);
            FD_SET(STDIN_FILENO, &rdfs);
            int sel = select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);

            if (sel > 0 && FD_ISSET(STDIN_FILENO, &rdfs))
                read(STDIN_FILENO, &c, 1);

            if ((c == 127) || (c == 8)) // Suppress 127 (DEL) to 8 (BACKSPACE)
                c = 0;
            else if (c == 27) // ESC
            {
                // Suppress ANSI escape sequences
                //
                FD_ZERO(&rdfs);
                FD_SET(STDIN_FILENO, &rdfs);
                sel = select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
                if (sel > 0 && FD_ISSET(STDIN_FILENO, &rdfs))
                {
                    unsigned char buff[16];
                    int rd = read(STDIN_FILENO, buff, 16);
                    c = 0;
                }
            }

            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

            if (c > 0)
            {
                WindowEvent windowEvent;
                memset(&windowEvent, 0, sizeof(WindowEvent));
                // windowEvent.window = this;
                windowEvent.type = WindowEventType::TextEntered;
                windowEvent.textEntered = c;

                OnWindowEvent(windowEvent);
            }
        }
    };

    KeyboardAndMouseState *linux_fd_monitoring = new KeyboardAndMouseState();

    void _release_at_exit()
    {
        if (linux_fd_monitoring != NULL)
        {
            delete linux_fd_monitoring);
        }
    }

    void __internal__inputProcessing(

        const std::function<void(const WindowEvent &)> &OnWindowEvent,
        const std::function<void(const KeyboardEvent &)> &OnKeyboardEvent,
        const std::function<void(const MouseEvent &)> &OnMouseEvent,
        const std::function<void(const TouchEvent &)> &OnTouchEvent)
    {
        linux_fd_monitoring->ProcessInputEvents(OnWindowEvent, OnKeyboardEvent, OnMouseEvent, OnTouchEvent);
    }

    namespace Devices
    {

        //
        // Joystick
        //
        Joystick::Joystick(uint32_t _id)
        {
            id = _id;
        }
        bool Joystick::isConnected()
        {
            return false;
        }
        float Joystick::getAxis(JoystickAxis axis)
        {
            return 0.0f;
        }
        bool Joystick::isButtonPressed(uint32_t button)
        {
            return false;
        }
        JoystickInfo Joystick::queryJoystickInfo()
        {
            JoystickInfo result;
            memset(&result, 0, sizeof(JoystickInfo));
            return result;
        }

        //
        // Keyboard
        //
        bool Keyboard::isPressed(KeyCode code)
        {
            return linux_fd_monitoring->isKeyPressed(code);
        }

        //
        // Mouse
        //
        bool Mouse::isPressed(MouseButton button)
        {
            return linux_fd_monitoring->isMouseButtonPressed(button);
        }
        MathCore::vec2i Mouse::getPosition(Window *relativeToWindow)
        {
            return mousePos;
        }
        void Mouse::setPosition(const MathCore::vec2i &position, Window *relativeToWindow)
        {
            mousePos = position;
        }

        //
        // Sensor
        //
        Sensor::Sensor(SensorType _type)
        {
            type = _type;
        }
        bool Sensor::isAvailable()
        {
            return false;
        }
        void Sensor::setEnable(bool _enable)
        {
        }
        MathCore::vec3f Sensor::getValue()
        {
            return MathCore::vec3f();
        }

        //
        // Touch
        //
        bool Touch::isPressed(uint32_t finger_id)
        {
            return linux_fd_monitoring->isTouchDown(finger_id);
        }
        MathCore::vec2i Touch::getPosition(uint32_t finger_id, Window *relativeToWindow)
        {
            return linux_fd_monitoring->getTouchPosition(finger_id);
        }

    }
}

#else

#error "No Window Wrapper Specified (Defined)"

#endif
