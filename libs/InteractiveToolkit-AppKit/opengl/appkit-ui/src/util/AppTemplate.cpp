#include <appkit-ui/util/AppTemplate.h>

#include <appkit-gl-engine/Engine.h>
#include <appkit-ui/util/AppOptions.h>

namespace AppKit
{
    namespace ui
    {

        void AppTemplate::configure(
            const std::vector<OptionsGroup> &options_template,
            const EventCore::Callback<void(Platform::ObjectBuffer *out_optionsDataRaw)> &read_options_raw_data,
            const EventCore::Callback<void(const Platform::ObjectBuffer &in_optionsDataRaw)> &write_options_raw_data)
        {
            this->options_template = options_template;
            this->read_options_raw_data = read_options_raw_data;
            this->write_options_raw_data = write_options_raw_data;
        }

        void AppTemplate::load_options()
        {
            if (this->read_options_raw_data == nullptr)
                return;

            auto options = AppKit::ui::OptionsManager::Instance();
            options->initializeDefaults(this->options_template);

            Platform::ObjectBuffer optionsDataRaw;

            this->read_options_raw_data(&optionsDataRaw);

            // auto optionsPath = ITKCommon::Path::getSaveGamePath(this->company_name, this->app_name) + ITKCommon::PATH_SEPARATOR + "options.bin";
            // ITKCommon::FileSystem::File::FromPath(optionsPath).readContentToObjectBuffer(&optionsDataRaw);

            options->loadOptionsFromBuffer(optionsDataRaw);
        }

        void AppTemplate::save_options()
        {
            if (this->write_options_raw_data == nullptr)
                return;

            auto options = AppKit::ui::OptionsManager::Instance();

            Platform::ObjectBuffer optionsDataRaw;
            options->saveOptionsToBuffer(&optionsDataRaw);

            // auto optionsPath = ITKCommon::Path::getSaveGamePath(this->company_name, this->app_name) + ITKCommon::PATH_SEPARATOR + "options.bin";
            // ITKCommon::FileSystem::File::FromPath(optionsPath).writeContentFromObjectBuffer(&optionsDataRaw);

            this->write_options_raw_data(optionsDataRaw);
        }

        void AppTemplate::apply_settings_to_window(const EventCore::Callback<void()> &OnAfterAppCreation)
        {
            auto engine = AppKit::GLEngine::Engine::Instance();
            AppKit::GLEngine::EngineWindowConfig engineConfig = AppKit::GLEngine::Engine::CreateDefaultRenderingConfig();

            strcpy(engineConfig.windowConfig.windowName, engine->gameName.c_str());

            engineConfig.glContextConfig.majorVersion = 2;
            engineConfig.glContextConfig.minorVersion = 1;

            engineConfig.glContextConfig.sRgbCapable = false;

            apply_window_options_to_engine(&engineConfig, OnAfterAppCreation);
        }

        void AppTemplate::reset_monitor_mode_to_default()
        {
            auto options = AppKit::ui::OptionsManager::Instance();
            if (!options->isInitialized())
                return;
            auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
            defaultMonitor.setMode(options->mainMonitor_InitialMode);
        }

        AppTemplate *AppTemplate::Instance()
        {
            static AppTemplate instance;
            return &instance;
        }

        void AppTemplate::apply_window_options_to_engine(AppKit::GLEngine::EngineWindowConfig *engineConfig, const EventCore::Callback<void()> &OnAfterAppCreation)
        {
            auto engine = AppKit::GLEngine::Engine::Instance();

            auto options = AppKit::ui::OptionsManager::Instance();

            std::string currWindowMode = options->getGroupValueSelectedForKey("Video", "WindowMode");
            if (currWindowMode == "Borderless")
            {
                auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];

                engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Borderless;
                engineConfig->windowConfig.videoMode = AppKit::Window::VideoMode(defaultMonitor.width, defaultMonitor.height);
            }
            else if (currWindowMode == "Fullscreen")
            {
                const std::string &fullscreenRes = options->getGroupValueSelectedForKey("Video", "Resolution");
                int w, h;
                if (sscanf(fullscreenRes.c_str(), "%ix%i", &w, &h) == 2)
                {
#if defined(__linux__)
                    engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::Borderless;
#elif defined(_WIN32)
                    engineConfig->windowConfig.windowStyle = AppKit::Window::WindowStyle::FullScreen;
#endif
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
                const std::string &aaMode = options->getGroupValueSelectedForKey("Video", "AntiAliasing");
                if (aaMode == "MSAA")
                    engineConfig->glContextConfig.antialiasingLevel = 2;
                else
                    engineConfig->glContextConfig.antialiasingLevel = 0;
            }

            {
                const std::string &vsyncMode = options->getGroupValueSelectedForKey("Video", "VSync");
                if (vsyncMode == "ON")
                    engineConfig->glContextConfig.vSync = true;
                else
                    engineConfig->glContextConfig.vSync = false;
            }

            engine->configureWindow( //
                *engineConfig,
                []()
                {
#if defined(__linux__)
                    // This callback is called before the window is configured
                    auto engine = AppKit::GLEngine::Engine::Instance();
                    auto options = AppKit::ui::OptionsManager::Instance();

                    std::string currWindowMode = options->getGroupValueSelectedForKey("Video", "WindowMode");
                    if (currWindowMode == "Fullscreen")
                    {
                        // set the desired resolution with the most high FPS
                        const std::string &fullscreenRes = options->getGroupValueSelectedForKey("Video", "Resolution");
                        int w, h;
                        if (sscanf(fullscreenRes.c_str(), "%ix%i", &w, &h) == 2)
                        {

                            auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                            auto modifiableMode = defaultMonitor.getMode(w, h);
                            modifiableMode.setFrequency(options->mainMonitor_InitialMode.freqs[0]);

                            defaultMonitor.setMode(modifiableMode);
                        }
                    }
                    else
                        AppKit::ui::AppTemplate::Instance()->reset_monitor_mode_to_default();

#endif
                },
                [engine, options, currWindowMode, OnAfterAppCreation](AppKit::Window::GLWindow *window)
                {
                    // This callback is called after the window is configured
                    if (currWindowMode == "Borderless")
                    {
                        engine->window->setMouseCursorVisible(false);
                        auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                        DPI::Display::setFullscreenAttribute(engine->window->getNativeWindowHandle(), &defaultMonitor);
                    }
                    else if (currWindowMode == "Fullscreen")
                    {
#if defined(__linux__)
                        engine->window->setMouseCursorVisible(false);
                        auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                        DPI::Display::setFullscreenAttribute(engine->window->getNativeWindowHandle(), &defaultMonitor);
#elif defined(_WIN32)
                        // windows fullscreen window seems to work fine
                        engine->window->setMouseCursorVisible(false);
#endif
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

    }
}