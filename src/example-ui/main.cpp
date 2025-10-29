#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/Thread.h>
#include <InteractiveToolkit/Platform/Process.h>
#include "util/AppOptions.h"
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

#include "App.h"

void reset_monitor_mode_to_default();

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
        auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];

        engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Borderless;
        engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(defaultMonitor.width, defaultMonitor.height);
    }
    else if (strcmp(currWindowMode, "Fullscreen") == 0)
    {
        const char *fullscreenRes = options->getGroupValueSelectedForKey("Video", "Resolution");
        int w, h;
        if (sscanf(fullscreenRes, "%ix%i", &w, &h) == 2)
        {
            engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Borderless;
            engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(w, h);
        }
        else
        {
            auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];

            options->setGroupValueSelectedForKey("Video", "WindowMode", "Window");
            currWindowMode = options->getGroupValueSelectedForKey("Video", "WindowMode");
            engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
            engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(defaultMonitor.width / 2, defaultMonitor.height / 2);
        }
    }
    else
    {
        auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];

        engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
        engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(defaultMonitor.width / 2, defaultMonitor.height / 2);
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
        []()
        {
            // This callback is called before the window is configured
            auto engine = AppKit::GLEngine::Engine::Instance();
            auto options = AppOptions::OptionsManager::Instance();

            const char *currWindowMode = options->getGroupValueSelectedForKey("Video", "WindowMode");
            if (strcmp(currWindowMode, "Fullscreen") == 0)
            {
                // set the desired resolution with the most high FPS

                const char *fullscreenRes = options->getGroupValueSelectedForKey("Video", "Resolution");
                int w, h;
                if (sscanf(fullscreenRes, "%ix%i", &w, &h) == 2)
                {

                    auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                    auto modifiableMode = defaultMonitor.getMode(w, h);
                    modifiableMode.setFrequency(options->mainMonitor_InitialMode.freqs[0]);

                    defaultMonitor.setMode(modifiableMode);
                }
            }
            else
            {
                reset_monitor_mode_to_default();
            }
        },
        [engine, options, currWindowMode, OnAfterAppCreation](AppKit::Window::GLWindow *window)
        {
            // This callback is called after the window is configured
            if (strcmp(currWindowMode, "Borderless") == 0)
            {
                engine->window->setMouseCursorVisible(false);
                auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                DPI::Display::setFullscreenAttribute(engine->window->getNativeWindowHandle(), &defaultMonitor);
            }
            else if (strcmp(currWindowMode, "Fullscreen") == 0)
            {
                engine->window->setMouseCursorVisible(false);
                auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                DPI::Display::setFullscreenAttribute(engine->window->getNativeWindowHandle(), &defaultMonitor);
            }
            else
            {
                auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                // set middle screen
                engine->window->setPosition(defaultMonitor.Position() + defaultMonitor.SizePixels() / 4);
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

void reset_monitor_mode_to_default()
{
    auto options = AppOptions::OptionsManager::Instance();
    if (!options->isInitialized())
        return;
    auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
    defaultMonitor.setMode(options->mainMonitor_InitialMode);
}

#if defined(__linux__)
#include <sys/prctl.h>

void fork_and_wait_parent_to_exit_from_child(int argc, char *argv[])
{
    pid_t parent_pid = getpid();
    pid_t pid = fork();
    if (pid < 0)
    {
        // Fork failed
        ITK_ABORT(true, "Fork failed in fork_and_wait_parent_to_exit()");
    }
    else if (pid > 0)
    {
        // Parent process
        // // Wait for the child process to complete
        // int status;
        // waitpid(pid, &status, 0);
        // // Exit the parent process
        // exit(WEXITSTATUS(status));
    }
    else
    {

        // // method 1: keep watching the parent process until it dies
        // {
        //     while (getppid() == parent_pid)
        //     {
        //         printf(".");
        //         fflush(stdout);
        //         usleep(100000);
        //     }

        //     // Parent already dead
        //     reset_monitor_mode_to_default();
        //     _exit(0);
        // }

        // method 2: use the SIGTERM on parent death

        {
            // Child process continues execution
            // Use prctl to set SIGTERM on parent death

            prctl(PR_SET_PDEATHSIG, SIGTERM);

            // Optionally, check if parent already exited before prctl
            if (getppid() != parent_pid)
            {
                // Parent already dead
                reset_monitor_mode_to_default();
                _exit(0);
            }

            // Wait for SIGTERM (sent when parent dies)
            // You can use pause() to wait for any signal
            // printf("CHILD WAITING FOR PARENT TO EXIT...\n");
            pause();

            printf("PROGRAM ENDED, BACKING THE RESOLUTION TO DEFAULT NOW\n");
            reset_monitor_mode_to_default();
            _exit(0);
        }
    }
}

#endif

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    Platform::Thread::staticInitialization();
    Platform::Signal::Set(&on_signal);

    // ITKCommon::ITKAbort::Instance()->OnAbort.add([](const char *file, int line, const char *message)
    // {
    //     reset_monitor_mode_to_default();
    // });

    load_options();

#if defined(__linux__)
    fork_and_wait_parent_to_exit_from_child(argc, argv);
#endif

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

    // reset_monitor_mode_to_default();

    return 0;
}
