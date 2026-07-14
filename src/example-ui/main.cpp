#define ASAN_CONFIG_OPENGL
// #define ASAN_CONFIG_CUDA
#include <InteractiveToolkit-Extension/asan-config/all.inl>
#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/Thread.h>
#include <InteractiveToolkit/Platform/Process.h>

#include <appkit-ui/util/AppOptions.h>

#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

#include "App.h"

#include <appkit-ui/util/AppTemplate.h>

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
        //     AppKit::ui::AppTemplate::Instance()->reset_monitor_mode_to_default();
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
                AppKit::ui::AppTemplate::Instance()->reset_monitor_mode_to_default();
                _exit(0);
            }

            // Wait for SIGTERM (sent when parent dies)
            // You can use pause() to wait for any signal
            // printf("CHILD WAITING FOR PARENT TO EXIT...\n");
            pause();

            printf("PROGRAM ENDED, BACKING THE RESOLUTION TO DEFAULT NOW\n");
            AppKit::ui::AppTemplate::Instance()->reset_monitor_mode_to_default();
            _exit(0);
        }
    }
}

#endif

const char *company_name = "OpenGLStarter";
const char *app_name = "Example - InteractiveToolkit AppKit UI";

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));
    Platform::Thread::staticInitialization();
    Platform::Signal::Set(&on_signal);

    // ITKCommon::ITKAbort::Instance()->OnAbort.add([](const char *file, int line, const char *message)
    // {
    //     AppKit::ui::AppTemplate::Instance()->reset_monitor_mode_to_default();
    // });

    auto appTemplate = AppKit::ui::AppTemplate::Instance();
    appTemplate->configure(
        {{"Control",
          {
              {"Input", {"Steam 1", "Keyboard", "Steam 1 + Keyboard"}, "Steam 1 + Keyboard"},
              {"Movement", {"Fluid", "Legacy"}, "Fluid"},
          }},
         {"Audio",
          {
              {"EffectsVolume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100"},
              {"MusicVolume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100"},
          }},
         {"Video",
          {
              {"WindowMode", {"Window", "Borderless", "Fullscreen"}, "Window"},
              {"Resolution", {}, ""},
              {"Aspect", {"16:9", "16:10"}, "16:9"},
              {"AntiAliasing", {"MSAA", "OFF"}, "MSAA"},
              {"VSync", {"ON", "OFF"}, "ON"},
              {"OpenGLAntiStutter", {"ON", "OFF"}, "ON"},
          }},
         {"Extra",
          {
              {"Language", {"English", "Português (BR)"}, "English"},
              {"ColorScheme", {"Blush", "Purple", "Orange", "Green", "Blue", "Dark"}, "Blush"},
              {"ButtonAppearance", {"Bend Up", "Bend Down", "Round", "Tip Front", "Tip Back", "Tip Up", "Tip Down", "Square"}, "Bend Up"},
              {"UiSize", {"Extra Small", "Small", "Medium", "Large", "Extra Large"}, "Medium"},
              {"MeshCrusher", {"ON", "OFF"}, "OFF"},
              {"Particles", {"Low", "Medium", "High", "Ultra"}, "High"},
              {"OnGameStats", {"OFF", "FPS"}, "OFF"},
          }}},
        [](Platform::ObjectBuffer *out_optionsDataRaw)
        {
            // read options binary data from file or stream
            auto optionsPath = ITKCommon::Path::getSaveGamePath(company_name, app_name) + ITKCommon::PATH_SEPARATOR + "options.bin";
            ITKCommon::FileSystem::File::FromPath(optionsPath).readContentToObjectBuffer(out_optionsDataRaw);
        },
        [](const Platform::ObjectBuffer &in_optionsDataRaw)
        {
            // write options binary data to file or stream
            auto optionsPath = ITKCommon::Path::getSaveGamePath(company_name, app_name) + ITKCommon::PATH_SEPARATOR + "options.bin";
            ITKCommon::FileSystem::File::FromPath(optionsPath).writeContentFromObjectBuffer(&in_optionsDataRaw);
        });

    appTemplate->load_options();

#if defined(__linux__)
    fork_and_wait_parent_to_exit_from_child(argc, argv);
#endif

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    engine->initialize(company_name, app_name, &CreateAppInstance);

    appTemplate->apply_settings_to_window(nullptr);

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

    // AppKit::ui::AppTemplate::Instance()->reset_monitor_mode_to_default();

    return 0;
}
