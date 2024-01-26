#include <InteractiveToolkit/InteractiveToolkit.h>
// #include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>
#include <AppKit/window/Window.h>

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    Platform::Thread::staticInitialization();

    printf("Hello World\n");

    AppKit::Window::WindowConfig wConfig(
        "Interactive Window",
        AppKit::Window::WindowStyle::Default,
        AppKit::Window::VideoMode(1280, 720, 32)
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
