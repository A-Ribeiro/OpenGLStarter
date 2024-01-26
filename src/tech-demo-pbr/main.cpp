#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/Thread.h>

#include "App.h"

static AppBase * CreateAppInstance() {
    App *app = new App();
    app->load();
    return app;
}

int main(int argc, char* argv[]) {
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    // initialize self referencing of the main thread.
    Platform::Thread::staticInitialization();

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    engine->initialize("Milky Way Studio", "Tech Demo: PBR", &CreateAppInstance);

    AppKit::GLEngine::EngineWindowConfig engineConfig = AppKit::GLEngine::Engine::CreateDefaultRenderingConfig();

    strcpy(engineConfig.windowConfig.windowName, "Tech Demo: PBR");

    engineConfig.glContextConfig.majorVersion = 2;
    engineConfig.glContextConfig.minorVersion = 1;

#ifdef ITK_RPI
    engineConfig.windowConfig.windowStyle = AppKit::Window::WindowStyle::FullScreen;
    engineConfig.windowConfig.videoMode = AppKit::Window::VideoMode(1280, 720);
    engineConfig.glContextConfig.antialiasingLevel = 0;
    engineConfig.glContextConfig.sRgbCapable = true;
#else
    engineConfig.windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
    engineConfig.windowConfig.videoMode = AppKit::Window::VideoMode(852, 480);

    engineConfig.glContextConfig.antialiasingLevel = 1;
    engineConfig.glContextConfig.sRgbCapable = true;
#endif

    engine->configureWindow(engineConfig);

    AppKit::Window::VideoMode vm = AppKit::Window::Window::getDesktopVideoMode();
    engine->window->setPosition(
        (MathCore::vec2i(vm.width, vm.height)
        - engine->window->getSize()) / 2
    );

    engine->mainLoop();

    return 0;
}