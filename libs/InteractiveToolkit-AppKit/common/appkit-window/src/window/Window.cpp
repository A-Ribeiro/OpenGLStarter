#include <AppKit/window/Window.h>
#include <AppKit/window/Event.h>

#if defined(APPKIT_WINDOW_SFML)

#if defined(APPKIT_WINDOW_VULKAN)
// #define GLAD_VULKAN_IMPLEMENTATION
// #include <vulkan.h>
#endif

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <InteractiveToolkit/Platform/Sleep.h>

// declared inside SFML
#if defined(_WIN32)
std::unordered_map<HWND, AppKit::Window::Window *> AppKit_Window_win32_map;
extern void (*AppKit_Window_win32_draw_on_resize_or_move)(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
#endif

namespace AppKit
{

    namespace Window
    {

#if defined(APPKIT_WINDOW_GL)

        GLWindow::GLWindow(const WindowConfig &_windowConfig, const GLContextConfig &_glContextConfig)
        {
            libraryHandle = nullptr;
            usr1Handle = nullptr;
            usr2Handle = nullptr;
            usr3Handle = nullptr;

            windowConfig = _windowConfig;
            glContextConfig = _glContextConfig;

            sf::ContextSettings contextSettings;
            contextSettings.depthBits = glContextConfig.depthBits;
            contextSettings.stencilBits = glContextConfig.stencilBits;
            contextSettings.antiAliasingLevel = glContextConfig.antialiasingLevel;
            contextSettings.sRgbCapable = glContextConfig.sRgbCapable;
            contextSettings.majorVersion = glContextConfig.majorVersion;
            contextSettings.minorVersion = glContextConfig.minorVersion;
            if (glContextConfig.coreAttribute)
                contextSettings.attributeFlags = sf::ContextSettings::Attribute::Core;
            else
                contextSettings.attributeFlags = sf::ContextSettings::Attribute::Default;

            sf::VideoMode videoMode(
                sf::Vector2u(windowConfig.videoMode.width, windowConfig.videoMode.height),
                windowConfig.videoMode.bitsPerPixel);

            const uint32_t windowStyle_mapping[] = {
                sf::Style::None,
                sf::Style::Default,
                sf::Style::None
                //                sf::Style::Fullscreen
            };

            sf::State wState = sf::State::Windowed;
            if (windowConfig.windowStyle == WindowStyle::FullScreen)
                wState = sf::State::Fullscreen;

            sf::RenderWindow *window = new sf::RenderWindow(videoMode,
                                                            windowConfig.windowName,
                                                            windowStyle_mapping[(int)windowConfig.windowStyle],
                                                            wState,
                                                            contextSettings);

            contextSettings = window->getSettings();

            // query the created context info
            glContextConfig.depthBits = contextSettings.depthBits;
            glContextConfig.stencilBits = contextSettings.stencilBits;
            glContextConfig.antialiasingLevel = contextSettings.antiAliasingLevel;
            glContextConfig.sRgbCapable = contextSettings.sRgbCapable;
            glContextConfig.majorVersion = contextSettings.majorVersion;
            glContextConfig.minorVersion = contextSettings.minorVersion;
            glContextConfig.coreAttribute = (contextSettings.attributeFlags & sf::ContextSettings::Attribute::Core) != 0;

            if (glContextConfig.majorVersion >= 3)
                glContextConfig.sRgbCapable = true;

            window->setVerticalSyncEnabled(glContextConfig.vSync);
            window->setFramerateLimit(0);
            window->setMouseCursorVisible(true);

            libraryHandle = window;
            usr1Handle = reinterpret_cast<sf::WindowBase *>(window);
            usr2Handle = reinterpret_cast<sf::RenderWindow *>(window);

#if defined(_WIN32)
            AppKit_Window_win32_map[this->getNativeWindowHandle()] = this;
            AppKit_Window_win32_draw_on_resize_or_move = &AppKit::Window::Window::_win32_draw_on_resize_or_move;
#endif
        }

        GLWindow::~GLWindow()
        {
#if defined(_WIN32)
            AppKit_Window_win32_map.erase(this->getNativeWindowHandle());
            if (AppKit_Window_win32_map.empty())
                AppKit_Window_win32_draw_on_resize_or_move = nullptr;
#endif
            sf::RenderWindow *window = static_cast<sf::RenderWindow *>(usr2Handle);
            libraryHandle = nullptr;
            if (window->isOpen())
                window->close();
            delete window;
        }

        // if there are more than one window,
        // you need to call this before issue gl commands
        bool GLWindow::glSetActivate(bool v)
        {
            sf::RenderWindow *window = static_cast<sf::RenderWindow *>(usr2Handle);
            return window->setActive(v);
        }
        void GLWindow::glSwapBuffers()
        {
            sf::RenderWindow *window = static_cast<sf::RenderWindow *>(usr2Handle);
            window->display();
        }
        void GLWindow::glSetVSync(bool vsyncOn)
        {
            sf::RenderWindow *window = static_cast<sf::RenderWindow *>(usr2Handle);
            window->setVerticalSyncEnabled(vsyncOn);
        }

        const GLContextConfig &GLWindow::getGLContextConfig() const
        {
            return glContextConfig;
        }

#endif

#if defined(APPKIT_WINDOW_VULKAN)

        VulkanWindow::VulkanWindow(const WindowConfig &_windowConfig)
        {
            libraryHandle = nullptr;
            usr1Handle = nullptr;
            usr2Handle = nullptr;
            usr3Handle = nullptr;

            windowConfig = _windowConfig;

            sf::VideoMode videoMode(
                sf::Vector2u(windowConfig.videoMode.width, windowConfig.videoMode.height),
                windowConfig.videoMode.bitsPerPixel);

            const uint32_t windowStyle_mapping[] = {
                sf::Style::None,
                sf::Style::Default,
                sf::Style::Fullscreen};

            sf::WindowBase *window = new sf::WindowBase(videoMode, windowConfig.windowName, windowStyle_mapping[(int)windowConfig.windowStyle]);

            window->setMouseCursorVisible(true);

            libraryHandle = window;
            usr1Handle = reinterpret_cast<sf::WindowBase *>(window);
        }

        VulkanWindow::~VulkanWindow()
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);

            libraryHandle = nullptr;
            if (window->isOpen())
                window->close();
            delete window;
        }

        void VulkanWindow::createVulkanSurface(
            const VkInstance &instance,
            VkSurfaceKHR &surface,
            const VkAllocationCallbacks *allocator)
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            window->createVulkanSurface(instance, surface, allocator);
        }

#endif

#if defined(_WIN32)
        void Window::_win32_draw_on_resize_or_move(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
        {
            auto it = AppKit_Window_win32_map.find(handle);
            if (it == AppKit_Window_win32_map.end())
                return;
            auto window = it->second;
            if (!window)
                return;

            switch (message)
            {
            case WM_SIZE:
            {
                if (window->resizeTimerId != 0 && window->onWin32BlockState_WindowEvent != nullptr)
                {

                    RECT rect;
                    GetClientRect(handle, &rect);

                    AppKit::Window::WindowEvent windowEventg;
                    windowEventg.window = window;
                    windowEventg.type = AppKit::Window::WindowEventType::Resized;
                    windowEventg.resized = MathCore::vec2i(rect.right - rect.left, rect.bottom - rect.top);

                    window->onWin32BlockState_WindowEvent(windowEventg);
                    // this->app->screenRenderWindow->inputManager.onWindowEvent(windowEventg);
                }
                break;
            }
            case WM_ENTERSIZEMOVE:
            {
                // Create a 60Hz timer (1000ms / 60 = ~16.67ms)
                if (window->resizeTimerId == 0)
                    window->resizeTimerId = SetTimer(handle, 1, 16, nullptr); // ~60 FPS
                break;
            }

            case WM_EXITSIZEMOVE:
            {
                // Destroy the timer
                if (window->resizeTimerId != 0)
                {
                    KillTimer(handle, window->resizeTimerId);
                    window->resizeTimerId = 0;
                }
                break;
            }

            case WM_NCLBUTTONDOWN:
            {
                if (wParam == HTCAPTION || wParam == HTSYSMENU)
                {
                    if (window->resizeTimerId == 0)
                        window->resizeTimerId = SetTimer(handle, 1, 16, nullptr); // ~60 FPS
                }
                break;
            }
            case WM_NCLBUTTONUP:
            {
                if (wParam == HTCAPTION || wParam == HTSYSMENU) // Title bar or system menu
                {
                    // Only stop timer if we're not in a size/move operation
                    // WM_EXITSIZEMOVE will handle stopping the timer in that case
                    if (window->resizeTimerId != 0)
                    {
                        // Small delay to ensure any pending operations complete
                        SetTimer(handle, 2, 100, nullptr); // 100ms delay before stopping
                    }
                }
                break;
            }

            case WM_TIMER:
            {
                if (!wParam)
                    break;
                auto window = it->second;
                // Handle the 60Hz timer during resize
                if (wParam == window->resizeTimerId)
                {
                    if (window->onWin32BlockState_WindowEvent != nullptr)
                    {
                        AppKit::Window::WindowEvent windowEventg;
                        windowEventg.window = window;
                        windowEventg.type = AppKit::Window::WindowEventType::Win32RedrawOnResize;

                        window->onWin32BlockState_WindowEvent(windowEventg);
                    }
                }
                else if (wParam == 2)
                {
                    // Delayed timer cleanup
                    KillTimer(handle, 2);
                    if (window->resizeTimerId != 0)
                    {
                        KillTimer(handle, window->resizeTimerId);
                        window->resizeTimerId = 0;
                    }
                }

                break;
            }
            }
        }
#endif

        Window::Window()
        {
            memset(key_states, 0, sizeof(bool) * (int)Devices::KeyCode::Count);
#if defined(_WIN32)
            resizeTimerId = 0;
#endif
        }

        const WindowConfig &Window::getConfig() const
        {
            return windowConfig;
        }

        std::vector<VideoMode> Window::getFullScreenVideoModes()
        {
            std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
            std::vector<VideoMode> result;
            for (auto _mode : modes)
            {
                // if (_mode.isValid())
                result.push_back(VideoMode(_mode.size.x, _mode.size.y, _mode.bitsPerPixel));
            }
            return result;
        }

        VideoMode Window::getDesktopVideoMode()
        {
            sf::VideoMode _mode = sf::VideoMode::getDesktopMode();
            return VideoMode(_mode.size.x, _mode.size.y, _mode.bitsPerPixel);
        }

        void Window::close()
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);

#if defined(_WIN32)
            // Clean up resize timer if it exists
            if (resizeTimerId != 0 && window->isOpen())
            {
                KillTimer(this->getNativeWindowHandle(), resizeTimerId);
                resizeTimerId = 0;
            }
#endif

            window->close();
        }
        bool Window::isOpen()
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            return window->isOpen();
        }

        bool Window::hasFocus()
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            return window->hasFocus();
        }
        void Window::requestFocus()
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            window->requestFocus();
        }

        const Devices::KeyCode reverse_key_mapping[int(sf::Keyboard::Scan::LaunchMediaSelect) + 1] = {
            Devices::KeyCode::A,         // A       = 0,  //!< Keyboard a and A key
            Devices::KeyCode::B,         // B       = 1,  //!< Keyboard b and B key
            Devices::KeyCode::C,         // C       = 2,  //!< Keyboard c and C key
            Devices::KeyCode::D,         // D       = 3,  //!< Keyboard d and D key
            Devices::KeyCode::E,         // E       = 4,  //!< Keyboard e and E key
            Devices::KeyCode::F,         // F       = 5,  //!< Keyboard f and F key
            Devices::KeyCode::G,         // G       = 6,  //!< Keyboard g and G key
            Devices::KeyCode::H,         // H       = 7,  //!< Keyboard h and H key
            Devices::KeyCode::I,         // I       = 8,  //!< Keyboard i and I key
            Devices::KeyCode::J,         // J       = 9,  //!< Keyboard j and J key
            Devices::KeyCode::K,         // K       = 10, //!< Keyboard k and K key
            Devices::KeyCode::L,         // L       = 11, //!< Keyboard l and L key
            Devices::KeyCode::M,         // M       = 12, //!< Keyboard m and M key
            Devices::KeyCode::N,         // N       = 13, //!< Keyboard n and N key
            Devices::KeyCode::O,         // O       = 14, //!< Keyboard o and O key
            Devices::KeyCode::P,         // P       = 15, //!< Keyboard p and P key
            Devices::KeyCode::Q,         // Q       = 16, //!< Keyboard q and Q key
            Devices::KeyCode::R,         // R       = 17, //!< Keyboard r and R key
            Devices::KeyCode::S,         // S       = 18, //!< Keyboard s and S key
            Devices::KeyCode::T,         // T       = 19, //!< Keyboard t and T key
            Devices::KeyCode::U,         // U       = 20, //!< Keyboard u and U key
            Devices::KeyCode::V,         // V       = 21, //!< Keyboard v and V key
            Devices::KeyCode::W,         // W       = 22, //!< Keyboard w and W key
            Devices::KeyCode::X,         // X       = 23, //!< Keyboard x and X key
            Devices::KeyCode::Y,         // Y       = 24, //!< Keyboard y and Y key
            Devices::KeyCode::Z,         // Z       = 25, //!< Keyboard z and Z key
            Devices::KeyCode::Num1,      // Num1 = 26, //!< Keyboard 1 and ! key
            Devices::KeyCode::Num2,      // Num2 = 27, //!< Keyboard 2 and @ key
            Devices::KeyCode::Num3,      // Num3 = 28, //!< Keyboard 3 and # key
            Devices::KeyCode::Num4,      // Num4 = 29, //!< Keyboard 4 and $ key
            Devices::KeyCode::Num5,      // Num5 = 30, //!< Keyboard 5 and % key
            Devices::KeyCode::Num6,      // Num6 = 31, //!< Keyboard 6 and ^ key
            Devices::KeyCode::Num7,      // Num7 = 32, //!< Keyboard 7 and & key
            Devices::KeyCode::Num8,      // Num8 = 33, //!< Keyboard 8 and * key
            Devices::KeyCode::Num9,      // Num9 = 34, //!< Keyboard 9 and ) key
            Devices::KeyCode::Num0,      // Num0 = 35, //!< Keyboard 0 and ) key
            Devices::KeyCode::Enter,     // Enter = 36, //!< Keyboard Enter/Return key
            Devices::KeyCode::Escape,    // Escape = 37, //!< Keyboard Escape key
            Devices::KeyCode::Backspace, // Backspace = 38, //!< Keyboard Backspace key
            Devices::KeyCode::Tab,       // Tab = 39, //!< Keyboard Tab key
            Devices::KeyCode::Space,     // Space = 40, //!< Keyboard Space key
            Devices::KeyCode::Hyphen,    // Hyphen = 41, //!< Keyboard - and _ key
            Devices::KeyCode::Equal,     // Equal = 42, //!< Keyboard = and + key
            Devices::KeyCode::LBracket,  //!< Keyboard [ and { key
            Devices::KeyCode::RBracket,  //!< Keyboard ] and } key
            // For US keyboards mapped to key 29 (Microsoft Keyboard Scan Code Specification)
            // For Non-US keyboards mapped to key 42 (Microsoft Keyboard Scan Code Specification)
            // Typical language mappings: Belg:£µ` FrCa:<>} Dan:*' Dutch:`´ Fren:µ* Ger:'# Ital:§ù LatAm:[}` Nor:*@ Span:ç} Swed:*' Swiss:$£} UK:~# Brazil:}]
            Devices::KeyCode::Backslash,  // Backslash,      //!< Keyboard \ and | key OR various keys for Non-US keyboards
            Devices::KeyCode::Semicolon,  // Semicolon,      //!< Keyboard ; and : key
            Devices::KeyCode::Apostrophe, // Apostrophe,     //!< Keyboard ' and " key
            Devices::KeyCode::Grave,      // Grave,          //!< Keyboard ` and ~ key
            Devices::KeyCode::Comma,      // Comma,          //!< Keyboard , and < key
            Devices::KeyCode::Period,     // Period,         //!< Keyboard . and > key
            Devices::KeyCode::Slash,      // Slash,          //!< Keyboard / and ? key
            Devices::KeyCode::F1,         // F1,             //!< Keyboard F1 key
            Devices::KeyCode::F2,         // F2,             //!< Keyboard F2 key
            Devices::KeyCode::F3,         // F3,             //!< Keyboard F3 key
            Devices::KeyCode::F4,         // F4,             //!< Keyboard F4 key
            Devices::KeyCode::F5,         // F5,             //!< Keyboard F5 key
            Devices::KeyCode::F6,         // F6,             //!< Keyboard F6 key
            Devices::KeyCode::F7,         // F7,             //!< Keyboard F7 key
            Devices::KeyCode::F8,         // F8,             //!< Keyboard F8 key
            Devices::KeyCode::F9,         // F9,             //!< Keyboard F9 key
            Devices::KeyCode::F10,        // F10,            //!< Keyboard F10 key
            Devices::KeyCode::F11,        // F11,            //!< Keyboard F11 key
            Devices::KeyCode::F12,        // F12,            //!< Keyboard F12 key
            Devices::KeyCode::F13,        // F13,            //!< Keyboard F13 key
            Devices::KeyCode::F14,        // F14,            //!< Keyboard F14 key
            Devices::KeyCode::F15,        // F15,            //!< Keyboard F15 key
            Devices::KeyCode::Unknown,    // F16,            //!< Keyboard F16 key
            Devices::KeyCode::Unknown,    // F17,            //!< Keyboard F17 key
            Devices::KeyCode::Unknown,    // F18,            //!< Keyboard F18 key
            Devices::KeyCode::Unknown,    // F19,            //!< Keyboard F19 key
            Devices::KeyCode::Unknown,    // F20,            //!< Keyboard F20 key
            Devices::KeyCode::Unknown,    // F21,            //!< Keyboard F21 key
            Devices::KeyCode::Unknown,    // F22,            //!< Keyboard F22 key
            Devices::KeyCode::Unknown,    // F23,            //!< Keyboard F23 key
            Devices::KeyCode::Unknown,    // F24,            //!< Keyboard F24 key
            Devices::KeyCode::Unknown,    // CapsLock,       //!< Keyboard Caps %Lock key
            Devices::KeyCode::Unknown,    // PrintScreen,    //!< Keyboard Print Screen key
            Devices::KeyCode::Unknown,    // ScrollLock,     //!< Keyboard Scroll %Lock key
            Devices::KeyCode::Pause,      // Pause,          //!< Keyboard Pause key
            Devices::KeyCode::Insert,     // Insert,         //!< Keyboard Insert key
            Devices::KeyCode::Home,       // Home,           //!< Keyboard Home key
            Devices::KeyCode::PageUp,     // PageUp,         //!< Keyboard Page Up key
            Devices::KeyCode::Delete,     // Delete,         //!< Keyboard Delete Forward key
            Devices::KeyCode::End,        // End,            //!< Keyboard End key
            Devices::KeyCode::PageDown,   // PageDown,       //!< Keyboard Page Down key
            Devices::KeyCode::Right,      // Right,          //!< Keyboard Right Arrow key
            Devices::KeyCode::Left,       // Left,           //!< Keyboard Left Arrow key
            Devices::KeyCode::Down,       // Down,           //!< Keyboard Down Arrow key
            Devices::KeyCode::Up,         // Up,             //!< Keyboard Up Arrow key
            Devices::KeyCode::Unknown,    // NumLock,        //!< Keypad Num %Lock and Clear key
            Devices::KeyCode::Divide,     // NumpadDivide,   //!< Keypad / key
            Devices::KeyCode::Multiply,   // NumpadMultiply, //!< Keypad * key
            Devices::KeyCode::Subtract,   // NumpadMinus,    //!< Keypad - key
            Devices::KeyCode::Add,        // NumpadPlus,     //!< Keypad + key
            Devices::KeyCode::Equal,      // NumpadEqual,    //!< keypad = key
            Devices::KeyCode::Enter,      // NumpadEnter,    //!< Keypad Enter/Return key
            Devices::KeyCode::Comma,      // NumpadDecimal,  //!< Keypad . and Delete key
            Devices::KeyCode::Numpad1,    // Numpad1,        //!< Keypad 1 and End key
            Devices::KeyCode::Numpad2,    // Numpad2,        //!< Keypad 2 and Down Arrow key
            Devices::KeyCode::Numpad3,    // Numpad3,        //!< Keypad 3 and Page Down key
            Devices::KeyCode::Numpad4,    // Numpad4,        //!< Keypad 4 and Left Arrow key
            Devices::KeyCode::Numpad5,    // Numpad5,        //!< Keypad 5 key
            Devices::KeyCode::Numpad6,    // Numpad6,        //!< Keypad 6 and Right Arrow key
            Devices::KeyCode::Numpad7,    // Numpad7,        //!< Keypad 7 and Home key
            Devices::KeyCode::Numpad8,    // Numpad8,        //!< Keypad 8 and Up Arrow key
            Devices::KeyCode::Numpad9,    // Numpad9,        //!< Keypad 9 and Page Up key
            Devices::KeyCode::Numpad0,    // Numpad0,        //!< Keypad 0 and Insert key
            // For US keyboards doesn't exist
            // For Non-US keyboards mapped to key 45 (Microsoft Keyboard Scan Code Specification)
            // Typical language mappings: Belg:<\> FrCa:«°» Dan:<\> Dutch:]|[ Fren:<> Ger:<|> Ital:<> LatAm:<> Nor:<> Span:<> Swed:<|> Swiss:<\> UK:\| Brazil: \|.
            Devices::KeyCode::Backslash, // NonUsBackslash,     //!< Keyboard Non-US \ and | key
            Devices::KeyCode::Unknown,   // Application,        //!< Keyboard Application key
            Devices::KeyCode::Unknown,   // Execute,            //!< Keyboard Execute key
            Devices::KeyCode::Unknown,   // ModeChange,         //!< Keyboard Mode Change key
            Devices::KeyCode::Unknown,   // Help,               //!< Keyboard Help key
            Devices::KeyCode::Menu,      // Menu,               //!< Keyboard Menu key
            Devices::KeyCode::Unknown,   // Select,             //!< Keyboard Select key
            Devices::KeyCode::Unknown,   // Redo,               //!< Keyboard Redo key
            Devices::KeyCode::Unknown,   // Undo,               //!< Keyboard Undo key
            Devices::KeyCode::Unknown,   // Cut,                //!< Keyboard Cut key
            Devices::KeyCode::Unknown,   // Copy,               //!< Keyboard Copy key
            Devices::KeyCode::Unknown,   // Paste,              //!< Keyboard Paste key
            Devices::KeyCode::Unknown,   // VolumeMute,         //!< Keyboard Volume Mute key
            Devices::KeyCode::Unknown,   // VolumeUp,           //!< Keyboard Volume Up key
            Devices::KeyCode::Unknown,   // VolumeDown,         //!< Keyboard Volume Down key
            Devices::KeyCode::Unknown,   // MediaPlayPause,     //!< Keyboard Media Play Pause key
            Devices::KeyCode::Unknown,   // MediaStop,          //!< Keyboard Media Stop key
            Devices::KeyCode::Unknown,   // MediaNextTrack,     //!< Keyboard Media Next Track key
            Devices::KeyCode::Unknown,   // MediaPreviousTrack, //!< Keyboard Media Previous Track key
            Devices::KeyCode::LControl,  // LControl,           //!< Keyboard Left Control key
            Devices::KeyCode::LShift,    // LShift,             //!< Keyboard Left Shift key
            Devices::KeyCode::LAlt,      // LAlt,               //!< Keyboard Left Alt key
            Devices::KeyCode::LSystem,   // LSystem,            //!< Keyboard Left System key
            Devices::KeyCode::RControl,  // RControl,           //!< Keyboard Right Control key
            Devices::KeyCode::RShift,    // RShift,             //!< Keyboard Right Shift key
            Devices::KeyCode::RAlt,      // RAlt,               //!< Keyboard Right Alt key
            Devices::KeyCode::RSystem,   // RSystem,            //!< Keyboard Right System key
            Devices::KeyCode::Unknown,   // Back,               //!< Keyboard Back key
            Devices::KeyCode::Unknown,   // Forward,            //!< Keyboard Forward key
            Devices::KeyCode::Unknown,   // Refresh,            //!< Keyboard Refresh key
            Devices::KeyCode::Unknown,   // Stop,               //!< Keyboard Stop key
            Devices::KeyCode::Unknown,   // Search,             //!< Keyboard Search key
            Devices::KeyCode::Unknown,   // Favorites,          //!< Keyboard Favorites key
            Devices::KeyCode::Unknown,   // HomePage,           //!< Keyboard Home Page key
            Devices::KeyCode::Unknown,   // LaunchApplication1, //!< Keyboard Launch Application 1 key
            Devices::KeyCode::Unknown,   // LaunchApplication2, //!< Keyboard Launch Application 2 key
            Devices::KeyCode::Unknown,   // LaunchMail,         //!< Keyboard Launch Mail key
            Devices::KeyCode::Unknown    // LaunchMediaSelect,  //!< Keyboard Launch Media Select key
        };

        const Devices::MouseButton reverse_mouse_btn_mapping[] = {
            Devices::MouseButton::Left,
            Devices::MouseButton::Right,
            Devices::MouseButton::Middle,
            Devices::MouseButton::Extra1,
            Devices::MouseButton::Extra2};

        const Devices::JoystickAxis reverse_joystick_axis_mapping[] = {
            Devices::JoystickAxis::X,
            Devices::JoystickAxis::Y,
            Devices::JoystickAxis::Z,
            Devices::JoystickAxis::R,
            Devices::JoystickAxis::U,
            Devices::JoystickAxis::V,
            Devices::JoystickAxis::PovX,
            Devices::JoystickAxis::PovY};

        const SensorEventType reverse_sensor_type_mapping[] = {
            Devices::SensorType::Accelerometer,
            Devices::SensorType::Gyroscope,
            Devices::SensorType::Magnetometer,
            Devices::SensorType::Gravity,
            Devices::SensorType::UserAcceleration,
            Devices::SensorType::Orientation};

        // TODO: poll window event and feed the InputManager
        void Window::forwardWindowEventsToInputManager(bool alwaysDraw, InputManager *customInputManager)
        {
            InputManager *targetInputManager = customInputManager;
            if (targetInputManager == nullptr)
                targetInputManager = &this->inputManager;

            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);

            // reset only mouse event, avoid call it several times in the loop...
            mouseEventg.type = MouseEventType::None;

            std::optional<sf::Event> event_reader;
            while (window->isOpen() && (event_reader = window->pollEvent()))
            {
                const sf::Event &event = event_reader.value();

                // in case of window is not focused, we still need to process the events
                if (!alwaysDraw && !window->hasFocus() && !event.is<sf::Event::Closed>() && !event.is<sf::Event::Resized>())
                    continue;

                if (event.is<sf::Event::Closed>())
                { //!< The window requested to be closed (no data)
                    // WindowEvent windowEvent;
                    // memset(&windowEvent, 0, sizeof(WindowEvent));
                    // windowEvent.window = this;
                    // windowEvent.type = WindowEventType::Closed;
                    // targetInputManager->onWindowEvent(windowEvent);
                    windowEventg.window = this;
                    windowEventg.type = WindowEventType::Closed;
                    targetInputManager->onWindowEvent(windowEventg);
                    // break;
                }
                else if (event.is<sf::Event::Resized>())
                { //!< The window was resized (data in event.size)
                    auto value = event.getIf<sf::Event::Resized>();
                    // WindowEvent windowEvent;
                    // memset(&windowEvent, 0, sizeof(WindowEvent));
                    // windowEvent.window = this;
                    // windowEvent.type = WindowEventType::Resized;
                    // windowEvent.resized = MathCore::vec2i(value->size.x, value->size.y);
                    // targetInputManager->onWindowEvent(windowEvent);
                    windowEventg.window = this;
                    windowEventg.type = WindowEventType::Resized;
                    windowEventg.resized = MathCore::vec2i(value->size.x, value->size.y);
                    targetInputManager->onWindowEvent(windowEventg);
                    // break;
                }
                else if (event.is<sf::Event::FocusLost>())
                { //!< The window lost the focus (no data)
                    // WindowEvent windowEvent;
                    // memset(&windowEvent, 0, sizeof(WindowEvent));
                    // windowEvent.window = this;
                    // windowEvent.type = WindowEventType::LostFocus;
                    // targetInputManager->onWindowEvent(windowEvent);
                    windowEventg.window = this;
                    windowEventg.type = WindowEventType::LostFocus;
                    targetInputManager->onWindowEvent(windowEventg);
                    // break;
                }
                else if (event.is<sf::Event::FocusGained>())
                { //!< The window gained the focus (no data)
                    // WindowEvent windowEvent;
                    // memset(&windowEvent, 0, sizeof(WindowEvent));
                    // windowEvent.window = this;
                    // windowEvent.type = WindowEventType::GainedFocus;
                    // targetInputManager->onWindowEvent(windowEvent);
                    windowEventg.window = this;
                    windowEventg.type = WindowEventType::GainedFocus;
                    targetInputManager->onWindowEvent(windowEventg);
                    // break;
                }
                else if (event.is<sf::Event::TextEntered>())
                { //!< A character was entered (data in event.text)
                    auto value = event.getIf<sf::Event::TextEntered>();
                    // WindowEvent windowEvent;
                    // memset(&windowEvent, 0, sizeof(WindowEvent));
                    // windowEvent.window = this;
                    // windowEvent.type = WindowEventType::TextEntered;
                    // windowEvent.textEntered = value->unicode;
                    // targetInputManager->onWindowEvent(windowEvent);
                    windowEventg.window = this;
                    windowEventg.type = WindowEventType::TextEntered;
                    windowEventg.textEntered = value->unicode;
                    targetInputManager->onWindowEvent(windowEventg);
                    // break;
                }
                else if (event.is<sf::Event::KeyPressed>())
                { //!< A key was pressed (data in event.key)
                    auto value = event.getIf<sf::Event::KeyPressed>();
                    if ((int)value->code < 0 || (int)value->scancode > int(sf::Keyboard::Scan::LaunchMediaSelect))
                        continue;
                    // KeyboardEvent keyboardEvent;
                    // memset(&keyboardEvent, 0, sizeof(KeyboardEvent));
                    // keyboardEvent.type = KeyboardEventType::KeyPressed;
                    // keyboardEvent.code = reverse_key_mapping[(int)value->code];
                    // keyboardEvent.alt = value->alt;
                    // keyboardEvent.control = value->control;
                    // keyboardEvent.shift = value->shift;
                    // keyboardEvent.system = value->system;
                    // targetInputManager->onKeyboardEvent(keyboardEvent);

                    keyboardEventg.code = reverse_key_mapping[(int)value->scancode];
                    if (key_states[(int)keyboardEventg.code])
                        continue;

                    key_states[(int)keyboardEventg.code] = true;

                    keyboardEventg.type = KeyboardEventType::KeyPressed;
                    keyboardEventg.alt = value->alt;
                    keyboardEventg.control = value->control;
                    keyboardEventg.shift = value->shift;
                    keyboardEventg.system = value->system;
                    targetInputManager->onKeyboardEvent(keyboardEventg);
                    // break;
                }
                else if (event.is<sf::Event::KeyReleased>())
                { //!< A key was released (data in event.key)
                    auto value = event.getIf<sf::Event::KeyReleased>();
                    if ((int)value->code < 0 || (int)value->scancode >= int(sf::Keyboard::Scan::LaunchMediaSelect))
                        continue;
                    // KeyboardEvent keyboardEvent;
                    // memset(&keyboardEvent, 0, sizeof(KeyboardEvent));
                    // keyboardEvent.type = KeyboardEventType::KeyReleased;
                    // keyboardEvent.code = reverse_key_mapping[(int)value->code];
                    // keyboardEvent.alt = value->alt;
                    // keyboardEvent.control = value->control;
                    // keyboardEvent.shift = value->shift;
                    // keyboardEvent.system = value->system;
                    // targetInputManager->onKeyboardEvent(keyboardEvent);

                    keyboardEventg.code = reverse_key_mapping[(int)value->scancode];

                    if (!key_states[(int)keyboardEventg.code])
                        continue;

                    key_states[(int)keyboardEventg.code] = false;

                    keyboardEventg.type = KeyboardEventType::KeyReleased;
                    keyboardEventg.alt = value->alt;
                    keyboardEventg.control = value->control;
                    keyboardEventg.shift = value->shift;
                    keyboardEventg.system = value->system;
                    targetInputManager->onKeyboardEvent(keyboardEventg);
                    // break;
                }
                else if (event.is<sf::Event::MouseWheelScrolled>())
                { //!< The mouse wheel was scrolled (data in event.mouseWheelScroll)
                    if (mouseEventg.type == MouseEventType::Moved)
                    {
                        targetInputManager->onMouseEvent(mouseEventg);
                        // mouseEventg.type = MouseEventType::None;
                    }
                    auto value = event.getIf<sf::Event::MouseWheelScrolled>();
                    // MouseEvent mouseEvent;
                    // memset(&mouseEvent, 0, sizeof(MouseEvent));
                    // if (value->wheel == sf::Mouse::Wheel::Horizontal)
                    //     mouseEvent.type = MouseEventType::HorizontalWheelScrolled;
                    // else
                    //     mouseEvent.type = MouseEventType::VerticalWheelScrolled;
                    // mouseEvent.position = MathCore::vec2i(value->position.x, value->position.y);
                    // mouseEvent.wheelDelta = value->delta;
                    // targetInputManager->onMouseEvent(mouseEvent);
                    if (value->wheel == sf::Mouse::Wheel::Horizontal)
                        mouseEventg.type = MouseEventType::HorizontalWheelScrolled;
                    else
                        mouseEventg.type = MouseEventType::VerticalWheelScrolled;
                    mouseEventg.position = MathCore::vec2i(value->position.x, value->position.y);
                    mouseEventg.wheelDelta = value->delta;
                    targetInputManager->onMouseEvent(mouseEventg);
                    // break;
                }
                else if (event.is<sf::Event::MouseButtonPressed>())
                { //!< A mouse button was pressed (data in event.mouseButton)
                    auto value = event.getIf<sf::Event::MouseButtonPressed>();
                    if ((int)value->button >= (int)Devices::MouseButton::Count)
                        continue;
                    if (mouseEventg.type == MouseEventType::Moved)
                    {
                        targetInputManager->onMouseEvent(mouseEventg);
                        // mouseEventg.type = MouseEventType::None;
                    }
                    // MouseEvent mouseEvent;
                    // memset(&mouseEvent, 0, sizeof(MouseEvent));
                    // mouseEvent.type = MouseEventType::ButtonPressed;
                    // mouseEvent.position = MathCore::vec2i(value->position.x, value->position.y);
                    // mouseEvent.button = reverse_mouse_btn_mapping[(int)value->button];
                    // targetInputManager->onMouseEvent(mouseEvent);
                    mouseEventg.type = MouseEventType::ButtonPressed;
                    mouseEventg.position = MathCore::vec2i(value->position.x, value->position.y);
                    mouseEventg.button = reverse_mouse_btn_mapping[(int)value->button];
                    targetInputManager->onMouseEvent(mouseEventg);
                    // break;
                }
                else if (event.is<sf::Event::MouseButtonReleased>())
                { //!< A mouse button was released (data in event.mouseButton)
                    auto value = event.getIf<sf::Event::MouseButtonReleased>();
                    if ((int)value->button >= (int)Devices::MouseButton::Count)
                        continue;
                    if (mouseEventg.type == MouseEventType::Moved)
                    {
                        targetInputManager->onMouseEvent(mouseEventg);
                        // mouseEventg.type = MouseEventType::None;
                    }
                    // MouseEvent mouseEvent;
                    // memset(&mouseEvent, 0, sizeof(MouseEvent));
                    // mouseEvent.type = MouseEventType::ButtonReleased;
                    // mouseEvent.position = MathCore::vec2i(value->position.x, value->position.y);
                    // mouseEvent.button = reverse_mouse_btn_mapping[(int)value->button];
                    // targetInputManager->onMouseEvent(mouseEvent);
                    mouseEventg.type = MouseEventType::ButtonReleased;
                    mouseEventg.position = MathCore::vec2i(value->position.x, value->position.y);
                    mouseEventg.button = reverse_mouse_btn_mapping[(int)value->button];
                    targetInputManager->onMouseEvent(mouseEventg);
                    // break;
                }
                else if (event.is<sf::Event::MouseMoved>())
                { //!< The mouse cursor moved (data in event.mouseMove)
                    auto value = event.getIf<sf::Event::MouseMoved>();
                    // MouseEvent mouseEvent;
                    // memset(&mouseEvent, 0, sizeof(MouseEvent));
                    // mouseEvent.type = MouseEventType::Moved;
                    // mouseEvent.position = MathCore::vec2i(value->position.x, value->position.y);
                    // targetInputManager->onMouseEvent(mouseEvent);
                    // if (mouseEventg.type == MouseEventType::None ||
                    //     mouseEventg.type == MouseEventType::Moved){
                    mouseEventg.type = MouseEventType::Moved;
                    mouseEventg.position = MathCore::vec2i(value->position.x, value->position.y);
                    //}

                    // break;
                }
                else if (event.is<sf::Event::MouseEntered>())
                { //!< The mouse cursor entered the area of the window (no data)
                    // WindowEvent windowEvent;
                    // memset(&windowEvent, 0, sizeof(WindowEvent));
                    // windowEvent.window = this;
                    // windowEvent.type = WindowEventType::MouseEntered;
                    // targetInputManager->onWindowEvent(windowEvent);
                    windowEventg.window = this;
                    windowEventg.type = WindowEventType::MouseEntered;
                    targetInputManager->onWindowEvent(windowEventg);
                    // break;
                }
                else if (event.is<sf::Event::MouseLeft>())
                { //!< The mouse cursor left the area of the window (no data)
                    // WindowEvent windowEvent;
                    // memset(&windowEvent, 0, sizeof(WindowEvent));
                    // windowEvent.window = this;
                    // windowEvent.type = WindowEventType::MouseLeft;
                    // targetInputManager->onWindowEvent(windowEvent);
                    windowEventg.window = this;
                    windowEventg.type = WindowEventType::MouseLeft;
                    targetInputManager->onWindowEvent(windowEventg);
                    // break;
                }
                else if (event.is<sf::Event::JoystickButtonPressed>())
                { //!< A joystick button was pressed (data in event.joystickButton)
                    auto value = event.getIf<sf::Event::JoystickButtonPressed>();
                    // JoystickEvent joystickEvent;
                    // memset(&joystickEvent, 0, sizeof(JoystickEvent));
                    // joystickEvent.type = JoystickEventButtonPressed;
                    // joystickEvent.joystickId = value->joystickId;
                    // joystickEvent.button = value->button;
                    // targetInputManager->onJoystickEvent(joystickEvent);
                    joystickEventg.type = JoystickEventButtonPressed;
                    joystickEventg.joystickId = value->joystickId;
                    joystickEventg.button = value->button;
                    targetInputManager->onJoystickEvent(joystickEventg);
                    // break;
                }
                else if (event.is<sf::Event::JoystickButtonReleased>())
                { //!< A joystick button was released (data in event.joystickButton)
                    auto value = event.getIf<sf::Event::JoystickButtonReleased>();
                    // JoystickEvent joystickEvent;
                    // memset(&joystickEvent, 0, sizeof(JoystickEvent));
                    // joystickEvent.type = JoystickEventButtonReleased;
                    // joystickEvent.joystickId = value->joystickId;
                    // joystickEvent.button = value->button;
                    // targetInputManager->onJoystickEvent(joystickEvent);
                    joystickEventg.type = JoystickEventButtonReleased;
                    joystickEventg.joystickId = value->joystickId;
                    joystickEventg.button = value->button;
                    targetInputManager->onJoystickEvent(joystickEventg);
                    // break;
                }
                else if (event.is<sf::Event::JoystickMoved>())
                { //!< The joystick moved along an axis (data in event.joystickMove)
                    auto value = event.getIf<sf::Event::JoystickMoved>();
                    if ((int)value->axis >= (int)Devices::JoystickAxis::Count)
                        continue;
                    // JoystickEvent joystickEvent;
                    // memset(&joystickEvent, 0, sizeof(JoystickEvent));
                    // joystickEvent.type = JoystickEventMoved;
                    // joystickEvent.joystickId = value->joystickId;
                    // joystickEvent.move.axis = reverse_joystick_axis_mapping[(int)value->axis];
                    // joystickEvent.move.value = value->position;
                    // targetInputManager->onJoystickEvent(joystickEvent);
                    joystickEventg.type = JoystickEventMoved;
                    joystickEventg.joystickId = value->joystickId;
                    joystickEventg.move.axis = reverse_joystick_axis_mapping[(int)value->axis];
                    joystickEventg.move.value = value->position;
                    targetInputManager->onJoystickEvent(joystickEventg);
                    // break;
                }
                else if (event.is<sf::Event::JoystickConnected>())
                { //!< A joystick was connected (data in event.joystickConnect)
                    auto value = event.getIf<sf::Event::JoystickConnected>();
                    // JoystickEvent joystickEvent;
                    // memset(&joystickEvent, 0, sizeof(JoystickEvent));
                    // joystickEvent.type = JoystickEventConnected;
                    // joystickEvent.joystickId = value->joystickId;
                    // targetInputManager->onJoystickEvent(joystickEvent);
                    joystickEventg.type = JoystickEventConnected;
                    joystickEventg.joystickId = value->joystickId;
                    targetInputManager->onJoystickEvent(joystickEventg);
                    // break;
                }
                else if (event.is<sf::Event::JoystickDisconnected>())
                { //!< A joystick was disconnected (data in event.joystickConnect)
                    auto value = event.getIf<sf::Event::JoystickDisconnected>();
                    // JoystickEvent joystickEvent;
                    // memset(&joystickEvent, 0, sizeof(JoystickEvent));
                    // joystickEvent.type = JoystickEventDisconnected;
                    // joystickEvent.joystickId = value->joystickId;
                    // targetInputManager->onJoystickEvent(joystickEvent);
                    joystickEventg.type = JoystickEventDisconnected;
                    joystickEventg.joystickId = value->joystickId;
                    targetInputManager->onJoystickEvent(joystickEventg);
                    // break;
                }
                else if (event.is<sf::Event::TouchBegan>())
                { //!< A touch event began (data in event.touch)
                    auto value = event.getIf<sf::Event::TouchBegan>();
                    // TouchEvent touchEvent;
                    // memset(&touchEvent, 0, sizeof(TouchEvent));
                    // touchEvent.type = TouchEventType::Began;
                    // touchEvent.finger = value->finger;
                    // touchEvent.position = MathCore::vec2i(value->position.x, value->position.y);
                    // targetInputManager->onTouchEvent(touchEvent);
                    touchEventg.type = TouchEventType::Began;
                    touchEventg.finger = value->finger;
                    touchEventg.position = MathCore::vec2i(value->position.x, value->position.y);
                    targetInputManager->onTouchEvent(touchEventg);
                    // break;
                }
                else if (event.is<sf::Event::TouchMoved>())
                { //!< A touch moved (data in event.touch)
                    auto value = event.getIf<sf::Event::TouchMoved>();
                    // TouchEvent touchEvent;
                    // memset(&touchEvent, 0, sizeof(TouchEvent));
                    // touchEvent.type = TouchEventType::Moved;
                    // touchEvent.finger = value->finger;
                    // touchEvent.position = MathCore::vec2i(value->position.x, value->position.y);
                    // targetInputManager->onTouchEvent(touchEvent);
                    touchEventg.type = TouchEventType::Moved;
                    touchEventg.finger = value->finger;
                    touchEventg.position = MathCore::vec2i(value->position.x, value->position.y);
                    targetInputManager->onTouchEvent(touchEventg);
                    // break;
                }
                else if (event.is<sf::Event::TouchEnded>())
                { //!< A touch event ended (data in event.touch)
                    auto value = event.getIf<sf::Event::TouchEnded>();
                    // TouchEvent touchEvent;
                    // memset(&touchEvent, 0, sizeof(TouchEvent));
                    // touchEvent.type = TouchEventType::Ended;
                    // touchEvent.finger = value->finger;
                    // touchEvent.position = MathCore::vec2i(value->position.x, value->position.y);
                    // targetInputManager->onTouchEvent(touchEvent);
                    touchEventg.type = TouchEventType::Ended;
                    touchEventg.finger = value->finger;
                    touchEventg.position = MathCore::vec2i(value->position.x, value->position.y);
                    targetInputManager->onTouchEvent(touchEventg);
                    // break;
                }
                else if (event.is<sf::Event::SensorChanged>())
                { //!< A sensor value changed (data in event.sensor)
                    auto value = event.getIf<sf::Event::SensorChanged>();
                    if ((int)value->type >= (int)Devices::SensorType::Count)
                        continue;
                    // SensorEvent sensorEvent;
                    // memset(&sensorEvent, 0, sizeof(SensorEvent));
                    // sensorEvent.type = reverse_sensor_type_mapping[(int)value->type];
                    // sensorEvent.value = MathCore::vec3f(value->value.x, value->value.y, value->value.z);
                    // targetInputManager->onSensorEvent(sensorEvent);
                    sensorEventg.type = reverse_sensor_type_mapping[(int)value->type];
                    sensorEventg.value = MathCore::vec3f(value->value.x, value->value.y, value->value.z);
                    targetInputManager->onSensorEvent(sensorEventg);
                    // break;
                }
                else if (event.is<sf::Event::MouseMovedRaw>())
                {
                }
                else
                {
                    printf("[Window] Event not recognized.\n");
                    // break;
                }
            }
            if (mouseEventg.type == MouseEventType::Moved)
            {
                targetInputManager->onMouseEvent(mouseEventg);
                // mouseEventg.type = MouseEventType::None;
            }
        }

        MathCore::vec2i Window::getPosition() const
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            sf::Vector2i pos = window->getPosition();
            return MathCore::vec2i(pos.x, pos.y);
        }
        void Window::setPosition(const MathCore::vec2i &position)
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            window->setPosition(sf::Vector2i(position.x, position.y));
        }

        MathCore::vec2i Window::getSize() const
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            sf::Vector2u size = window->getSize();
            return MathCore::vec2i(size.x, size.y);
        }
        void Window::setSize(const MathCore::vec2i &size)
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            window->setSize(sf::Vector2u(size.width, size.height));
        }

        void Window::setVisible(bool visible)
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            window->setVisible(visible);
        }
        void Window::setMouseCursorVisible(bool visible)
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            window->setMouseCursorVisible(visible);
        }
        void Window::setLockMouseCursorInside(bool lock)
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            window->setMouseCursorGrabbed(lock);
        }

        NativeWindowHandleType Window::getNativeWindowHandle()
        {
            sf::WindowBase *window = static_cast<sf::WindowBase *>(usr1Handle);
            return (NativeWindowHandleType)window->getNativeHandle();
        }

        void *Window::getLibraryHandle()
        {
            return libraryHandle;
        }
    }
}

#elif defined(APPKIT_WINDOW_GLFW)

#error "Not Implemented"

#elif defined(APPKIT_WINDOW_RPI)

#include <stdio.h>
// #include <filesystem>
#include <iostream>

#include <EGL/eglplatform.h>
// #include <glad/EGL/eglplatform.h>

/* C++ / C typecast macros for special EGL handle values */
#if defined(__cplusplus)
#define EGL_CAST(type, value) (static_cast<type>(value))
#else
#define EGL_CAST(type, value) ((type)(value))
#endif

// #include <glad/glad/egl.h>
#include <EGL/egl.h>

#include <bcm_host.h>
#include <functional>

namespace windowGLWrapper
{

    extern MathCore::vec2i mousePos; // hack to initialize the mouse position
    extern uint32_t _hack_window_width;
    extern uint32_t _hack_window_height;

    static void initialize_bcm_host()
    {
        static bool initialized = false;
        if (!initialized)
        {
            bcm_host_init();
            initialized = true;
        }
    }

#ifdef NDEBUG
#define eglCheck(expr) expr
#else
#define eglCheck(expr)                                            \
    do                                                            \
    {                                                             \
        expr;                                                     \
        AppKit::Window::eglCheckError(__FILE__, __LINE__, #expr); \
    } while (false)
    static void eglCheckError(const std::string &file, unsigned int line, const char *expression)
    {
        // Obtain information about the success or failure of the most recent EGL
        // function called in the current thread
        EGLint errorCode = eglGetError();

        if (errorCode != EGL_SUCCESS)
        {
            std::string error = "unknown error";
            std::string description = "no description";

            // Decode the error code returned
            switch (errorCode)
            {
            case EGL_NOT_INITIALIZED:
            {
                error = "EGL_NOT_INITIALIZED";
                description = "EGL is not initialized, or could not be initialized, for the specified display";
                break;
            }

            case EGL_BAD_ACCESS:
            {
                error = "EGL_BAD_ACCESS";
                description = "EGL cannot access a requested resource (for example, a context is bound in another thread)";
                break;
            }

            case EGL_BAD_ALLOC:
            {
                error = "EGL_BAD_ALLOC";
                description = "EGL failed to allocate resources for the requested operation";
                break;
            }
            case EGL_BAD_ATTRIBUTE:
            {
                error = "EGL_BAD_ATTRIBUTE";
                description = "an unrecognized attribute or attribute value was passed in an attribute list";
                break;
            }

            case EGL_BAD_CONTEXT:
            {
                error = "EGL_BAD_CONTEXT";
                description = "an EGLContext argument does not name a valid EGLContext";
                break;
            }

            case EGL_BAD_CONFIG:
            {
                error = "EGL_BAD_CONFIG";
                description = "an EGLConfig argument does not name a valid EGLConfig";
                break;
            }

            case EGL_BAD_CURRENT_SURFACE:
            {
                error = "EGL_BAD_CURRENT_SURFACE";
                description = "the current surface of the calling thread is a window, pbuffer, or pixmap that is no longer valid";
                break;
            }

            case EGL_BAD_DISPLAY:
            {
                error = "EGL_BAD_DISPLAY";
                description = "an EGLDisplay argument does not name a valid EGLDisplay; or, EGL is not initialized on the specified EGLDisplay";
                break;
            }

            case EGL_BAD_SURFACE:
            {
                error = "EGL_BAD_SURFACE";
                description = "an EGLSurface argument does not name a valid surface (window, pbuffer, or pixmap) configured for rendering";
                break;
            }

            case EGL_BAD_MATCH:
            {
                error = "EGL_BAD_MATCH";
                description = "arguments are inconsistent; for example, an otherwise valid context requires buffers (e.g. depth or stencil) not allocated by an otherwise valid surface";
                break;
            }

            case EGL_BAD_PARAMETER:
            {
                error = "EGL_BAD_PARAMETER";
                description = "one or more argument values are invalid";
                break;
            }

            case EGL_BAD_NATIVE_PIXMAP:
            {
                error = "EGL_BAD_NATIVE_PIXMAP";
                description = "an EGLNativePixmapType argument does not refer to a valid native pixmap";
                break;
            }

            case EGL_BAD_NATIVE_WINDOW:
            {
                error = "EGL_BAD_NATIVE_WINDOW";
                description = "an EGLNativeWindowType argument does not refer to a valid native window";
                break;
            }

            case EGL_CONTEXT_LOST:
            {
                error = "EGL_CONTEXT_LOST";
                description = "a power management event has occurred. The application must destroy all contexts and reinitialize client API state and objects to continue rendering";
                break;
            }
            }

            // Log the error
            std::cerr << "An internal EGL call failed in "
                      << file << " (" << line << ") : "
                      << "\nExpression:\n   " << expression
                      << "\nError description:\n   " << error << "\n   " << description << '\n'
                      << std::endl;
        }
    }
#endif

    class EGLWrapper
    {
    public:
        // EGLDisplay display;

        EGLDisplay m_display; //!< The internal EGL display
        EGLContext m_context; //!< The internal EGL context
        EGLSurface m_surface; //!< The internal EGL surface
        EGLConfig m_config;   //!< The internal EGL config

        WindowConfig *windowConfig;

        EGLWrapper(const EGLNativeWindowType &nativeWindow, WindowConfig *windowConfig)
        {

            m_display = EGL_NO_DISPLAY;
            m_context = EGL_NO_CONTEXT;
            m_surface = EGL_NO_SURFACE;
            m_config = nullptr;

            this->windowConfig = windowConfig;
            // m_display = display;

            // gladLoaderUnloadEGL();
            // gladLoaderLoadEGL(EGL_NO_DISPLAY);//load eglGetDisplay

            eglCheck(m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY));
            int major, minor;
            eglCheck(eglInitialize(m_display, &major, &minor));

            printf("EGL Loaded: %i.%i\n", major, minor);

            // gladLoaderUnloadEGL();
            // gladLoaderLoadEGL(m_display);//load ext

            m_config = getBestConfig();
            updateWindowConfig();

            createContext();
            createSurface(nativeWindow);
            makeCurrent(true);
        }

        ~EGLWrapper()
        {
            // Deactivate the current context
            EGLContext currentContext = EGL_NO_CONTEXT;
            eglCheck(currentContext = eglGetCurrentContext());

            if (currentContext == m_context)
                eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));

            // Destroy context
            if (m_context != EGL_NO_CONTEXT)
            {
                eglCheck(eglDestroyContext(m_display, m_context));
                m_context = EGL_NO_CONTEXT;
            }

            // Destroy surface
            if (m_surface != EGL_NO_SURFACE)
            {
                eglCheck(eglDestroySurface(m_display, m_surface));
                m_surface = EGL_NO_CONTEXT;
            }
        }

        bool makeCurrent(bool current)
        {
            if (m_surface == EGL_NO_SURFACE)
                return false;
            EGLBoolean result = EGL_FALSE;
            if (current)
                eglCheck(result = eglMakeCurrent(m_display, m_surface, m_surface, m_context));
            else
                eglCheck(result = eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
            return (result != EGL_FALSE);
        }

        void swapBuffers()
        {
            if (m_surface != EGL_NO_SURFACE)
                eglCheck(eglSwapBuffers(m_display, m_surface));
        }

        void swapInterval(EGLint interval)
        {
            eglCheck(eglSwapInterval(m_display, interval));
        }

        void createContext()
        {
            const EGLint contextVersion[] = {
                // request a context using Open GL ES 2.0
                EGL_CONTEXT_CLIENT_VERSION, 2,
                // EGL_CONTEXT_CLIENT_VERSION, 1,
                EGL_NONE};
            EGLContext toShared = EGL_NO_CONTEXT;
            // Create EGL context
            eglCheck(m_context = eglCreateContext(m_display, m_config, toShared, contextVersion));
        }

        void createSurface(const EGLNativeWindowType &window)
        {
            eglCheck(m_surface = eglCreateWindowSurface(m_display, m_config, window, nullptr));
        }

        void destroySurface()
        {
            // Ensure that this context is no longer active since our surface is going to be destroyed
            makeCurrent(false);

            eglCheck(eglDestroySurface(m_display, m_surface));
            m_surface = EGL_NO_SURFACE;
        }

        EGLConfig getBestConfig()
        {
            // Set our video settings constraint
            const EGLint attributes[] = {
                EGL_BUFFER_SIZE, static_cast<EGLint>(windowConfig->videoMode.bitsPerPixel),
                EGL_DEPTH_SIZE, static_cast<EGLint>(windowConfig->depthBits),
                EGL_STENCIL_SIZE, static_cast<EGLint>(windowConfig->stencilBits),
                EGL_SAMPLE_BUFFERS, windowConfig->antialiasingLevel ? 1 : 0,
                EGL_SAMPLES, static_cast<EGLint>(windowConfig->antialiasingLevel),
                // EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                // request a context using Open GL ES 2.0
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                // EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,

                // not supported on RPI
                //                EGL_COLORSPACE, windowConfig->sRgbCapable?EGL_COLORSPACE_sRGB :EGL_COLORSPACE_LINEAR ,

                EGL_NONE

            };

            EGLint configCount;
            EGLConfig configs[1];

            // Ask EGL for the best config matching our video settings
            eglCheck(eglChooseConfig(m_display, attributes, configs, 1, &configCount));

            // TODO: This should check EGL_CONFORMANT and pick the first conformant configuration.
            return configs[0];
        }

        void updateWindowConfig()
        {
            EGLBoolean result = EGL_FALSE;
            EGLint tmp = 0;

            // Update the internal context settings with the current config
            eglCheck(result = eglGetConfigAttrib(m_display, m_config, EGL_DEPTH_SIZE, &tmp));

            if (result == EGL_FALSE)
                std::cerr << "Failed to retrieve EGL_DEPTH_SIZE" << std::endl;

            windowConfig->depthBits = static_cast<unsigned int>(tmp);

            eglCheck(result = eglGetConfigAttrib(m_display, m_config, EGL_STENCIL_SIZE, &tmp));

            if (result == EGL_FALSE)
                std::cerr << "Failed to retrieve EGL_STENCIL_SIZE" << std::endl;

            windowConfig->stencilBits = static_cast<unsigned int>(tmp);

            eglCheck(result = eglGetConfigAttrib(m_display, m_config, EGL_SAMPLES, &tmp));

            if (result == EGL_FALSE)
                std::cerr << "Failed to retrieve EGL_SAMPLES" << std::endl;

            windowConfig->antialiasingLevel = static_cast<unsigned int>(tmp);

            /*
            eglCheck(result = eglGetConfigAttrib(m_display, m_config, EGL_COLORSPACE, &tmp));
            if (result == EGL_FALSE)
                std::cerr << "Failed to retrieve EGL_COLORSPACE" << std::endl;
            windowConfig->sRgbCapable = tmp == EGL_COLORSPACE_sRGB;
            if (!windowConfig->sRgbCapable)
                printf("%x<<<---\n",tmp);
            */

            // for RPI 3, sRGB is not supported...
            windowConfig->sRgbCapable = false;

            windowConfig->majorVersion = 1;
            windowConfig->minorVersion = 1;
            windowConfig->coreAttribute = false;
        }
    };

    class DispManX
    {
    public:
        DISPMANX_DISPLAY_HANDLE_T m_display;
        EGL_DISPMANX_WINDOW_T m_nativeWindow;

        WindowConfig *windowConfig;

        EGLWrapper *egl;

        DispManX(WindowConfig *windowConfig)
        {
            egl = nullptr;
            this->windowConfig = windowConfig;
            initialize_bcm_host();

            // Disable alpha to prevent app looking composed on whatever dispman
            // is showing - lifted from SDL source: video/raspberry/SDL_rpivideo.c
            //
            VC_DISPMANX_ALPHA_T dispman_alpha;
            dispman_alpha.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
            dispman_alpha.opacity = 0xFF;
            dispman_alpha.mask = 0;

            // create an EGL window surface
            uint32_t screen_width;
            uint32_t screen_height;
            graphics_get_display_size(0 /* LCD */, &screen_width, &screen_height);

            VC_RECT_T dst_rect;
            if (windowConfig->windowStyle == WindowStyle::FullScreen)
            {
                // fullscreen scale the src rect (stretch both width and height)
                dst_rect.x = 0;
                dst_rect.y = 0;
                dst_rect.width = screen_width;
                dst_rect.height = screen_height;
            }
            else
            {
                // draw centered screen on the display
                dst_rect.x = (screen_width - windowConfig->videoMode.width) >> 1;   // 0;
                dst_rect.y = (screen_height - windowConfig->videoMode.height) >> 1; // 0;
                dst_rect.width = windowConfig->videoMode.width;
                dst_rect.height = windowConfig->videoMode.height;
            }

            _hack_window_width = windowConfig->videoMode.width;
            _hack_window_height = windowConfig->videoMode.height;
            mousePos = MathCore::vec2i(_hack_window_width >> 1, _hack_window_height >> 1);

            VC_RECT_T src_rect;
            src_rect.x = 0;
            src_rect.y = 0;
            // src_rect.width = 0;
            // src_rect.height = 0;
            src_rect.width = windowConfig->videoMode.width << 16;
            src_rect.height = windowConfig->videoMode.height << 16;

            m_display = vc_dispmanx_display_open(0 /* LCD */);

            DISPMANX_UPDATE_HANDLE_T dispman_update;
            dispman_update = vc_dispmanx_update_start(0);

            DISPMANX_ELEMENT_HANDLE_T dispman_element;
            dispman_element = vc_dispmanx_element_add(dispman_update, m_display,
                                                      0 /*layer*/, &dst_rect, 0 /*src*/,
                                                      &src_rect, DISPMANX_PROTECTION_NONE, &dispman_alpha, 0 /*clamp*/, DISPMANX_NO_ROTATE);
            vc_dispmanx_update_submit_sync(dispman_update);

            memset(&m_nativeWindow, 0, sizeof(EGL_DISPMANX_WINDOW_T));

            m_nativeWindow.element = dispman_element;
            m_nativeWindow.width = windowConfig->videoMode.width;
            m_nativeWindow.height = windowConfig->videoMode.height;

            egl = new EGLWrapper((EGLNativeWindowType)&m_nativeWindow, windowConfig);
        }

        ~DispManX()
        {
            delete egl;
            egl = nullptr;

            if (m_nativeWindow.element)
            {
                DISPMANX_UPDATE_HANDLE_T dispman_update;
                dispman_update = vc_dispmanx_update_start(0);
                vc_dispmanx_element_remove(dispman_update, m_nativeWindow.element);
                vc_dispmanx_update_submit_sync(dispman_update);
            }

            if (m_display)
                vc_dispmanx_display_close(m_display);
        }
    };

    static int GCD(int a, int b)
    {
        int Remainder;
        while (b != 0)
        {
            Remainder = a % b;
            a = b;
            b = Remainder;
        }
        return a;
    }

    Window::Window(const WindowConfig &_windowConfig)
    {
        libraryHandle = nullptr;

        windowConfig = _windowConfig;

        libraryHandle = new DispManX(&windowConfig);

        DispManX *dispmanx = static_cast<DispManX *>(libraryHandle);
        dispmanx->egl->swapInterval(windowConfig.vSync ? 1 : 0);
        // window->setFramerateLimit(0);
        // window->setMouseCursorVisible(true);

        printf("Window Initialization OK\n");
    }

    Window::~Window()
    {
        DispManX *dispmanx = static_cast<DispManX *>(libraryHandle);
        libraryHandle = nullptr;
        // if (window->isOpen())
        // window->close();
        delete dispmanx;

        printf("Window Destroyed...\n");
    }

    const WindowConfig &Window::getConfig() const
    {
        return windowConfig;
    }

    std::vector<VideoMode> Window::getFullScreenVideoModes()
    {

        VideoMode desktopMode = Window::getDesktopVideoMode();

        std::vector<VideoMode> modes;
        // modes.push_back(getDesktopMode());

        int gcd = GCD(desktopMode.width, desktopMode.height);
        int aspectx = desktopMode.width / gcd;
        int aspecty = desktopMode.height / gcd;

        printf("[VideoModeImpl] aspect: %i:%i\n", aspectx, aspecty);
        printf("[VideoModeImpl] Fullscreen Resolutions:\n");

        VideoMode vm = modes[0];
        if (aspectx == 4 && aspecty == 3)
        {
            int res[] =
                {
                    6400, 4800,
                    4096, 3072,
                    3200, 2400,
                    2800, 2100,
                    // 2732, 2048,
                    2560, 1920,
                    2304, 1728,
                    2048, 1536,
                    1920, 1440,
                    1856, 1392,
                    1792, 1344,
                    1600, 1200,
                    1440, 1080,
                    1400, 1050,
                    1280, 960,
                    1152, 864,
                    // 1120, 832,
                    1024, 768,
                    960, 720,
                    832, 624,
                    800, 600,
                    640, 480,
                    512, 384,
                    400, 300,
                    384, 288,
                    320, 240,
                    256, 192,
                    160, 120};

            printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
            for (int i = 0; i < sizeof(res) / sizeof(int); i += 2)
            {
                vm.width = res[i];
                vm.height = res[i + 1];
                if (vm.width < modes[modes.size() - 1].width)
                {
                    modes.push_back(vm);
                    printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
                }
            }
        }
        else if (aspectx == 16 && aspecty == 9)
        {
            // https://en.wikipedia.org/wiki/16:9_aspect_ratio
            int res[] =
                {
                    7680, 4320, // 8K UHD
                    5120, 2880, // 5k
                    4096, 2304,
                    3840, 2160, // 4K UHD
                    3200, 1800, // QHD+
                    2880, 1620,
                    2560, 1440, // QHD
                    2048, 1152,
                    1920, 1080, // fullhd
                    1600, 900,  // HD+
                    // 1366, 768, // WXGA
                    1280, 720, // HD
                    1024, 576,
                    960, 540, // qHD
                    // 848, 480,
                    800, 450,
                    768, 432,
                    640, 360, // nHD
                    // 426, 240,
                    256, 144 // YouTube 144p
                };

            printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
            for (int i = 0; i < sizeof(res) / sizeof(int); i += 2)
            {
                vm.width = res[i];
                vm.height = res[i + 1];
                if (vm.width < modes[modes.size() - 1].width)
                {
                    modes.push_back(vm);
                    printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
                }
            }
        }
        else if (aspectx == 8 && aspecty == 5) // 16:10
        {
            // https://en.wikipedia.org/wiki/16:10_aspect_ratio
            int res[] =
                {
                    2560, 1600, // WQXGA
                    1920, 1200, // WUXGA
                    1680, 1050, // WSXGA+
                    1440, 900,  // WXGA+
                    1280, 800,  // WXGA
                    1024, 640,
                    640, 400,
                    320, 200,
                    160, 100};

            printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
            for (int i = 0; i < sizeof(res) / sizeof(int); i += 2)
            {
                vm.width = res[i];
                vm.height = res[i + 1];
                if (vm.width < modes[modes.size() - 1].width)
                {
                    modes.push_back(vm);
                    printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
                }
            }
        }

        return modes;
    }

    VideoMode Window::getDesktopVideoMode()
    {
        initialize_bcm_host();

        uint32_t width(0), height(0);
        graphics_get_display_size(0 /* LCD */, &width, &height);
        return VideoMode(width, height);
    }

    void Window::close()
    {
    }
    bool Window::isOpen()
    {
        return libraryHandle != nullptr;
    }

    bool Window::hasFocus()
    {
        return true;
    }
    void Window::requestFocus()
    {
    }

    // if there are more than one window,
    // you need to call this before issue gl commands
    bool Window::glSetActivate(bool v)
    {
        DispManX *dispmanx = static_cast<DispManX *>(libraryHandle);
        return dispmanx->egl->makeCurrent(v);
    }
    void Window::glSwapBuffers()
    {
        // need to use the egl swap buffers
        DispManX *dispmanx = static_cast<DispManX *>(libraryHandle);
        dispmanx->egl->swapBuffers();
    }
    void Window::glSetVSync(bool vsyncOn)
    {
        windowConfig.vSync = vsyncOn;

        DispManX *dispmanx = static_cast<DispManX *>(libraryHandle);
        dispmanx->egl->swapInterval(windowConfig.vSync ? 1 : 0);
    }

    // TODO: poll window event and feed the InputManager
    void __internal__inputProcessing(
        const std::function<void(const WindowEvent &)> &OnWindowEvent,
        const std::function<void(const KeyboardEvent &)> &OnKeyboardEvent,
        const std::function<void(const MouseEvent &)> &OnMouseEvent,
        const std::function<void(const TouchEvent &)> &OnTouchEvent);

    void Window::forwardWindowEventsToInputManager(bool alwaysDraw, InputManager *customInputManager)
    {
        InputManager *targetInputManager = customInputManager;
        if (targetInputManager == nullptr)
            targetInputManager = &this->inputManager;

        Window *thiz = this;
        __internal__inputProcessing(
            [thiz, targetInputManager](const WindowEvent &v)
            {
                WindowEvent evt = v;
                evt.window = thiz;
                targetInputManager->onWindowEvent(evt);
            },
            [thiz, targetInputManager](const KeyboardEvent &evt)
            {
                targetInputManager->onKeyboardEvent(evt);
            },
            [thiz, targetInputManager](const MouseEvent &evt)
            {
                targetInputManager->onMouseEvent(evt);
            },
            [thiz, targetInputManager](const TouchEvent &evt)
            {
                targetInputManager->onTouchEvent(evt);
            });
    }

    MathCore::vec2i Window::getPosition() const
    {
        return MathCore::vec2i(0, 0);
    }
    void Window::setPosition(const MathCore::vec2i &position)
    {
    }

    MathCore::vec2i Window::getSize() const
    {
        DispManX *dispmanx = static_cast<DispManX *>(libraryHandle);
        return MathCore::vec2i(dispmanx->m_nativeWindow.width, dispmanx->m_nativeWindow.height);
    }
    void Window::setSize(const MathCore::vec2i &size)
    {
    }

    void Window::setVisible(bool visible)
    {
    }
    void Window::setMouseCursorVisible(bool visible)
    {
    }
    void Window::setLockMouseCursorInside(bool lock)
    {
    }

    NativeWindowHandleType Window::getNativeWindowHandle()
    {
        DispManX *dispmanx = static_cast<DispManX *>(libraryHandle);
        return (NativeWindowHandleType)&dispmanx->m_nativeWindow;
    }

    void *Window::getLibraryHandle()
    {
        return libraryHandle;
    }
}

#else

#error "No Window Wrapper Specified (Defined)"

#endif
