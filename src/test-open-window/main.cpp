#include <InteractiveToolkit/InteractiveToolkit.h>
// #include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>
#include <AppKit/window/Window.h>
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>
#include <iostream>

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    Platform::Thread::staticInitialization();

    auto screen_size_mm = DPI::Display::MonitorRealSizeMillimeters();
    auto screen_size_in = DPI::Display::MonitorRealSizeInches();
    auto screen_pos_pixels = DPI::Display::MonitorPositionPixels();
    auto screen_size_pixels = DPI::Display::MonitorCurrentResolutionPixels();
    auto dpif = DPI::Display::MonitorDPIf();
    auto dpii = DPI::Display::MonitorDPIi();

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
        AppKit::Window::VideoMode(screen_size_pixels.x, screen_size_pixels.y, 32)
    );


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
    
    window.setPosition( screen_pos_pixels );

    AppKit::Window::InputManager inputManager;

    inputManager.onWindowEvent.add([&](const AppKit::Window::WindowEvent &event){
        if (event.type == AppKit::Window::WindowEventType::Closed)
            window.close();
    });

    inputManager.onKeyboardEvent.add([&](const AppKit::Window::KeyboardEvent &event){
        if (event.code == AppKit::Window::Devices::KeyCode::Escape)
            window.close();
    });

    while (window.isOpen()){
        
        window.glSwapBuffers();
        window.forwardWindowEventsToInputManager(false, &inputManager);
    }

    return 0;
}
