#include <InteractiveToolkit/InteractiveToolkit.h>
// #include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>
#include <AppKit/window/Window.h>
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>
#include <iostream>

// #include <SFML/Window.hpp>
// #include <SFML/Graphics.hpp>

#include "wayland_query_monitors.h"

int main(int argc, char *argv[])
{
    main_2();
    return 0;


    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    Platform::Thread::staticInitialization();

    int monitorDefault = 0;
    auto allMonitors = DPI::Display::QueryMonitors(&monitorDefault);

    for (const auto &monitor : allMonitors)
    {
        std::cout << "Monitor: " << monitor.name << "(port: " << monitor.port << ")" << std::endl;
        std::cout << "    Primary: " << monitor.primary << std::endl;
        std::cout << "    ScaleFactor: " << monitor.scaleFactor << std::endl;
        std::cout << "    Pos(pixels): " << monitor.x << ", " << monitor.y << std::endl;
        std::cout << "    Size(pixels): " << monitor.width << ", " << monitor.height << std::endl;
        std::cout << "    Size(mm): " << monitor.mwidth << ", " << monitor.mheight << std::endl;
        std::cout << "    Current Mode: " << monitor.modes[monitor.current_mode_index].width
                  << ", " << monitor.modes[monitor.current_mode_index].height
                  << " @" << monitor.modes[monitor.current_mode_index].freqs[monitor.current_freq_index] << std::endl;
        std::cout << "    Prefered Modes: " << std::endl;

        for (const auto &preferedMode : monitor.getPreferedModes())
        {
            std::cout << "        " << preferedMode.width
                      << ", " << preferedMode.height
                      << " @" << preferedMode.freqs[0] << std::endl;
        }

        std::cout << "    Modes: " << std::endl;
        for (const auto &mode : monitor.modes)
        {
            std::cout << "        " << mode.width << ", " << mode.height;
            for (const auto &freq : mode.freqs)
                std::cout << " @" << freq;
            std::cout << std::endl;
        }
    }

    auto selectedMonitor = &allMonitors[monitorDefault];

    auto screen_size_mm = selectedMonitor->SizeMillimeters();
    auto screen_size_in = selectedMonitor->SizeInches();
    auto screen_pos_pixels = selectedMonitor->Position();
    auto screen_size_pixels = selectedMonitor->SizePixels();
    auto dpif = DPI::Display::ComputeDPIf(screen_size_pixels, screen_size_in);
    auto dpii = DPI::Display::ComputeDPIi(screen_size_pixels, screen_size_in);

    std::cout << "Size(mm): " << screen_size_mm.x << ", " << screen_size_mm.y << std::endl;
    std::cout << "Size(in): " << screen_size_in.x << ", " << screen_size_in.y << std::endl;
    std::cout << "CurrPos(pixels): " << screen_pos_pixels.x << ", " << screen_pos_pixels.y << std::endl;
    std::cout << "CurrRes(pixels): " << screen_size_pixels.x << ", " << screen_size_pixels.y << std::endl;
    std::cout << "DPIf: " << dpif.x << ", " << dpif.y << std::endl;
    std::cout << "DPIi: " << dpii.x << ", " << dpii.y << std::endl;

    printf("Hello World\n");

    AppKit::Window::WindowConfig wConfig(
        "Interactive Window",
        AppKit::Window::WindowStyle::Borderless,
        AppKit::Window::VideoMode(selectedMonitor->SizePixels().x, selectedMonitor->SizePixels().y, 32));

    AppKit::Window::GLContextConfig glConfig;
    glConfig.vSync = true;

    // GL Context Settings
    glConfig.depthBits = 24;
    glConfig.stencilBits = 0;
    glConfig.antialiasingLevel = 0;
    glConfig.sRgbCapable = true;
    glConfig.coreAttribute = false;

    glConfig.majorVersion = 2;
    glConfig.minorVersion = 1;

    AppKit::Window::GLWindow window(wConfig, glConfig);

    DPI::Display::setFullscreenAttribute(window.getNativeWindowHandle(), selectedMonitor);

    AppKit::Window::InputManager inputManager;

    inputManager.onWindowEvent.add([&](const AppKit::Window::WindowEvent &event)
                                   {
        if (event.type == AppKit::Window::WindowEventType::Closed)
            window.close(); });

    inputManager.onKeyboardEvent.add( //
        [&](const AppKit::Window::KeyboardEvent &event)
        {
            if (event.code == AppKit::Window::Devices::KeyCode::Escape)
                window.close();
            if (event.type == AppKit::Window::KeyboardEventType::KeyPressed &&
                event.code == AppKit::Window::Devices::KeyCode::F)
            {

                // sf::RenderWindow *_window = reinterpret_cast<sf::RenderWindow *>(window.getLibraryHandle());
                // auto pos = _window->getPosition();
                // std::cout << "pos: " << pos.x << ", " << pos.y << std::endl;
                // auto size = _window->getSize();
                // std::cout << "size: " << size.x << ", " << size.y << std::endl;

                // window.setVisible(false);

                // // DPI::Display::setFullscreenAttribute2(window.getNativeWindowHandle(), selectedMonitor->Position());
                // // window.setPosition(selectedMonitor->Position());

                // // DPI::Display::setFullscreenAttribute(window.getNativeWindowHandle(), selectedMonitor->Position());

                // // _window->setPosition(sf::Vector2i(32,32));
                // //_window->requestFocus();

                // // window.setSize(MathCore::vec2i(100, 100));
                // window.setVisible(true);

                // DPI::Display::setFullscreenAttribute(window.getNativeWindowHandle(), selectedMonitor);

                // // window.setLockMouseCursorInside(true);

                // // DPI::Display::setFullscreenAttribute(window.getNativeWindowHandle(), selectedMonitor->Position());

                // pos = _window->getPosition();
                // std::cout << "pos: " << pos.x << ", " << pos.y << std::endl;
                // size = _window->getSize();
                // std::cout << "size: " << size.x << ", " << size.y << std::endl;
            }
            // DPI::Display::setFullscreenAttribute(window.getNativeWindowHandle(), selectedMonitor->Position());
        });

    while (window.isOpen())
    {

        window.glSwapBuffers();
        window.forwardWindowEventsToInputManager(false, &inputManager);
    }

    return 0;
}
