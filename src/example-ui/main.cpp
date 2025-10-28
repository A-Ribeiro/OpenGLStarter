#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/Thread.h>
#include <InteractiveToolkit/Platform/Process.h>
#include "util/AppOptions.h"
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

#include "App.h"

static AppBase *CreateAppInstance()
{
    App *app = new App();
    app->load();
    return app;
}

void on_signal(int)
{
    if (AppKit::GLEngine::Engine::Instance()->app != nullptr)
        AppKit::GLEngine::Engine::Instance()->app->exitApp();
}

void load_options()
{
    auto options = AppOptions::OptionsManager::Instance();
    options->initializeDefaults();

    Platform::ObjectBuffer optionsDataRaw;
    auto optionsPath = ITKCommon::Path::getSaveGamePath("OpenGLStarter", "Options") + ITKCommon::PATH_SEPARATOR + "options.bin";
    ITKCommon::FileSystem::File::FromPath(optionsPath).readContentToObjectBuffer(&optionsDataRaw);
    options->loadOptionsFromBuffer(optionsDataRaw);
}

void save_options()
{
    auto options = AppOptions::OptionsManager::Instance();

    Platform::ObjectBuffer optionsDataRaw;
    options->saveOptionsToBuffer(&optionsDataRaw);

    auto optionsPath = ITKCommon::Path::getSaveGamePath("OpenGLStarter", "Options") + ITKCommon::PATH_SEPARATOR + "options.bin";
    ITKCommon::FileSystem::File::FromPath(optionsPath).writeContentFromObjectBuffer(&optionsDataRaw);
}

void apply_window_options_to_engine(AppKit::GLEngine::EngineWindowConfig *engineConfig, const EventCore::Callback<void()> &OnAfterAppCreation)
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    auto options = AppOptions::OptionsManager::Instance();

    const char *currWindowMode = options->getGroupValueSelectedForKey("Video", "WindowMode");
    if (strcmp(currWindowMode, "Borderless") == 0)
    {
        engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Borderless;
        engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(options->mainMonitorResolution.width, options->mainMonitorResolution.height);
    }
    else if (strcmp(currWindowMode, "Fullscreen") == 0)
    {
        const char *fullscreenRes = options->getGroupValueSelectedForKey("Video", "Resolution");
        int w = 1280;
        int h = 720;
        if (sscanf(fullscreenRes, "%ix%i", &w, &h) == 2)
        {
            engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::FullScreen;
            engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(w, h);
        }
        else
        {
            options->setGroupValueSelectedForKey("Video", "WindowMode", "Window");
            currWindowMode = options->getGroupValueSelectedForKey("Video", "WindowMode");
            engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
            engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(options->mainMonitorResolution.width / 2, options->mainMonitorResolution.height / 2);
        }
    }
    else
    {
        engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
        engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(options->mainMonitorResolution.width / 2, options->mainMonitorResolution.height / 2);
    }

    {
        const char *aaMode = options->getGroupValueSelectedForKey("Video", "AntiAliasing");
        if (strcmp(aaMode, "MSAA") == 0)
            engineConfig->glContextConfig.antialiasingLevel = 2;
        else
            engineConfig->glContextConfig.antialiasingLevel = 0;
    }

    {
        const char *vsyncMode = options->getGroupValueSelectedForKey("Video", "VSync");
        if (strcmp(vsyncMode, "ON") == 0)
            engineConfig->glContextConfig.vSync = true;
        else
            engineConfig->glContextConfig.vSync = false;
    }

    engine->configureWindow( //
        *engineConfig,
        [engine, options, currWindowMode, OnAfterAppCreation](AppKit::Window::GLWindow *window)
        {
            if (strcmp(currWindowMode, "Borderless") == 0)
            {
                engine->window->setMouseCursorVisible(false);
                // set fullscreen attributes
                int monitorDefault = 0;
                auto allMonitors = DPI::Display::QueryMonitors(&monitorDefault);
                DPI::Display::setFullscreenAttribute(engine->window->getNativeWindowHandle(), &allMonitors[monitorDefault]);
            }
            else if (strcmp(currWindowMode, "Fullscreen") == 0)
            {
            }
            else
            {
                // set middle screen
                engine->window->setPosition(options->mainMonitorPosition + options->mainMonitorResolution / 4);
            }
            if (OnAfterAppCreation != nullptr)
                OnAfterAppCreation();
        });
}

void apply_settings_to_window(const EventCore::Callback<void()> &OnAfterAppCreation)
{
    AppKit::GLEngine::EngineWindowConfig engineConfig = AppKit::GLEngine::Engine::CreateDefaultRenderingConfig();

    strcpy(engineConfig.windowConfig.windowName, "Opening");

    engineConfig.glContextConfig.majorVersion = 2;
    engineConfig.glContextConfig.minorVersion = 1;

    engineConfig.glContextConfig.sRgbCapable = false;

    apply_window_options_to_engine(&engineConfig, OnAfterAppCreation);
}

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    Platform::Thread::staticInitialization();
    Platform::Signal::Set(&on_signal);

    load_options();

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    engine->initialize("Alessandro Ribeiro", "Opening", &CreateAppInstance);

    apply_settings_to_window(nullptr);

    // AppKit::GLEngine::EngineWindowConfig engineConfig = AppKit::GLEngine::Engine::CreateDefaultRenderingConfig();

    // strcpy(engineConfig.windowConfig.windowName, "Opening");

    // engineConfig.glContextConfig.majorVersion = 2;
    // engineConfig.glContextConfig.minorVersion = 1;

    // // #if (!defined(__APPLE__)) && (defined(ITK_RPI) || defined(NDEBUG))
    // //     engineConfig.windowConfig.windowStyle = AppKit::Window::WindowStyle::FullScreen;
    // //     engineConfig.windowConfig.videoMode = AppKit::Window::Window::getDesktopVideoMode();//AppKit::Window::VideoMode(1280, 720);

    // //     engineConfig.glContextConfig.antialiasingLevel = 0;
    // //     engineConfig.glContextConfig.sRgbCapable = false;

    // //     engine->configureWindow(engineConfig);
    // // #else
    // // engineConfig.windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
    // // engineConfig.windowConfig.videoMode = AppKit::Window::VideoMode(1024, 768);

    // // engineConfig.glContextConfig.antialiasingLevel = 0;
    // engineConfig.glContextConfig.sRgbCapable = false;

    // apply_window_options_to_engine(&engineConfig, nullptr);

    // // engine->configureWindow(engineConfig);

    // // AppKit::Window::VideoMode vm = AppKit::Window::Window::getDesktopVideoMode();
    // // engine->window->setPosition(
    // //     (MathCore::vec2i(vm.width, vm.height) - engine->window->getSize()) / 2);

    // // #endif

    engine->mainLoop();

    return 0;
}
