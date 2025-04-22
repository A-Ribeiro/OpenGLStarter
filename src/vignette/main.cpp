#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/Thread.h>
#include <InteractiveToolkit/Platform/Process.h>

#include "App.h"

static AppBase * CreateAppInstance() {
    App *app = new App();
    app->load();
    return app;
}

void on_signal(int) {
    if (AppKit::GLEngine::Engine::Instance()->app != nullptr)
        AppKit::GLEngine::Engine::Instance()->app->exitApp();
}

int main(int argc, char* argv[]) {
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    Platform::Thread::staticInitialization();
    Platform::Signal::Set(&on_signal);

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    engine->initialize("Alessandro Ribeiro", "Vignette", &CreateAppInstance);

    AppKit::GLEngine::EngineWindowConfig engineConfig = AppKit::GLEngine::Engine::CreateDefaultRenderingConfig();

    strcpy(engineConfig.windowConfig.windowName, "Vignette");

    engineConfig.glContextConfig.majorVersion = 2;
    engineConfig.glContextConfig.minorVersion = 1;

// #if (!defined(__APPLE__)) && (defined(ITK_RPI) || defined(NDEBUG))
//     engineConfig.windowConfig.windowStyle = AppKit::Window::WindowStyle::FullScreen;
//     engineConfig.windowConfig.videoMode = AppKit::Window::Window::getDesktopVideoMode();//AppKit::Window::VideoMode(1280, 720);

//     engineConfig.glContextConfig.antialiasingLevel = 0;
//     engineConfig.glContextConfig.sRgbCapable = false;

//     engine->configureWindow(engineConfig);
// #else
    engineConfig.windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
    engineConfig.windowConfig.videoMode = AppKit::Window::VideoMode(1920, 1080);

    engineConfig.glContextConfig.antialiasingLevel = 0;
    engineConfig.glContextConfig.sRgbCapable = false;

    engine->configureWindow(engineConfig);

    AppKit::Window::VideoMode vm = AppKit::Window::Window::getDesktopVideoMode();
    engine->window->setPosition(
        (MathCore::vec2i(vm.width, vm.height)
        - engine->window->getSize()) / 2
    );

// #endif

    engine->mainLoop();

    return 0;
}
