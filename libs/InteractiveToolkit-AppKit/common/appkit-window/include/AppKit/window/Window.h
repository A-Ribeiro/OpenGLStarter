#pragma once

#include "buildFlags.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>

#include "WindowConfig.h"
#include "InputManager.h"

#include <InteractiveToolkit/MathCore/vec2.h>

namespace AppKit
{

    namespace Window
    {

#if defined(_WIN32)
        // Window handle is HWND (HWND__*) or (HWND) on Windows
        typedef HWND__ *NativeWindowHandleType;
#elif defined(__linux__)
        // Window handle is Window (unsigned long) on Unix - X11
        typedef unsigned long NativeWindowHandleType;
#elif defined(__APPLE__)
        // Window handle is NSWindow or NSView (void*) on Mac OS X - Cocoa
        typedef void *NativeWindowHandleType;
#else
        typedef void *NativeWindowHandleType;
#endif

        class Window
        {
        protected:
            // cached event structures
            WindowEvent windowEventg;
            KeyboardEvent keyboardEventg;
            MouseEvent mouseEventg;
            JoystickEvent joystickEventg;
            TouchEvent touchEventg;
            SensorEvent sensorEventg;

            bool key_states[(int)Devices::KeyCode::Count];

        protected:
            void *libraryHandle;

            // user defined opaque handlers for GLWindow or VulkanWindow use
            void *usr1Handle;
            void *usr2Handle;
            void *usr3Handle;

            WindowConfig windowConfig;

            Window();

#if defined(_WIN32)
            MathCore::vec2i resizeTimerId_last_size;
            UINT_PTR resizeTimerId;
            static void _win32_draw_on_resize_or_move(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
#endif

        public:

            //allow redrawing when is resizing or moving the window
            EventCore::Callback<void(const WindowEvent&)> onWin32BlockState_WindowEvent;

            virtual ~Window() = default;

            InputManager inputManager;

            const WindowConfig &getConfig() const;

            static std::vector<VideoMode> getFullScreenVideoModes();
            static VideoMode getDesktopVideoMode();

            void close();
            bool isOpen();

            bool hasFocus();
            void requestFocus();

            // TODO: poll window event and feed the InputManager
            void forwardWindowEventsToInputManager(bool alwaysDraw = false, InputManager *customInputManager = nullptr);

            MathCore::vec2i getPosition() const;
            void setPosition(const MathCore::vec2i &position);

            MathCore::vec2i getSize() const;
            void setSize(const MathCore::vec2i &size);

            void setVisible(bool visible);
            void setMouseCursorVisible(bool visible);
            void setLockMouseCursorInside(bool lock);

            // return the SFML, GLTF, OR Custom Window Library Handle
            void *getLibraryHandle();

            // HWND on win32
            NativeWindowHandleType getNativeWindowHandle();
        };

#if defined(APPKIT_WINDOW_GL)

        class GLWindow : public Window
        {

            GLContextConfig glContextConfig;

        public:
            GLWindow(const WindowConfig &windowConfig, const GLContextConfig &glContextConfig);

            ~GLWindow();

            const GLContextConfig &getGLContextConfig() const;

            // if there are more than one window,
            // you need to call this before issue gl commands
            bool glSetActivate(bool v);
            void glSwapBuffers();
            void glSetVSync(bool vsyncOn);
        };

#endif

#if defined(APPKIT_WINDOW_VULKAN)

        // #define GLAD_VULKAN_IMPLEMENTATION
        // #include <vulkan.h>
        typedef void *VkInstance;
        typedef void *VkSurfaceKHR;
        typedef void *VkAllocationCallbacks;

        class VulkanWindow : public Window
        {

        public:
            VulkanWindow(const WindowConfig &windowConfig);

            // need headers vulkan
            void createVulkanSurface(
                const VkInstance &instance, VkSurfaceKHR &surface, const VkAllocationCallbacks *allocator = nullptr);

            ~VulkanWindow();
        };

#endif

    }
}
