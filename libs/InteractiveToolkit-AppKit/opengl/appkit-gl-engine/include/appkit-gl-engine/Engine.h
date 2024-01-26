#pragma once

#if defined(_WIN32)
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4309)
#pragma warning(disable : 4018)
#pragma warning(disable : 4305)
#endif

// #include <aRibeiroCore/aRibeiroCore.h>
// #include <appkit-gl-base/opengl-wrapper.h>
// #include <appkit-gl-engine/Component.h>
// #include <appkit-gl-engine/AppBase.h>
// #include <appkit-gl-engine/ReferenceCounter.h>
// #include <window-gl-wrapper/window-gl-wrapper.h>

// #include <functional>

#ifdef _MSC_VER
#ifdef NDEBUG
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#else
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif
#endif

#include <InteractiveToolkit/EventCore/Callback.h>

#include <AppKit/window/WindowConfig.h>

#include <appkit-gl-engine/AppBase/AppBase.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/util/ReferenceCounter.h>

#include <appkit-gl-base/GLTexture.h>

namespace AppKit
{
    namespace GLEngine
    {

        struct EngineWindowConfig {
            AppKit::Window::WindowConfig windowConfig;
            AppKit::Window::GLContextConfig glContextConfig;
        };

        class Engine
        {

            EngineWindowConfig changeWindowConfig;

            bool setNewWindowConfig;

            EventCore::Callback<AppBase *(void)> OnCreateInstanceFnc;

            Engine();

            void clear();

        public:
            std::string companyName;
            std::string gameName;

            AppBase *app;
            AppKit::Window::GLWindow *window;

            ReferenceCounter<AppKit::OpenGL::GLTexture *> textureReferenceCounter;
            ReferenceCounter<AppKit::GLEngine::Component *> componentReferenceCounter;

            // after initialization this variable will hold the sRGB information about the buffer
            bool sRGBCapable;

            bool isNVidiaCard;
            bool isAMDCard;
            bool isIntelCard;

            bool alwaysDraw;

            static Engine *Instance();

            void initialize(const std::string &_companyName, const std::string &_gameName,
                            const EventCore::Callback<AppBase *(void)> &_OnCreateInstanceFnc);
            void configureWindow(
                const EngineWindowConfig &windowConfig
            );

            std::vector<AppKit::Window::VideoMode> getResolutionList() const;

            void mainLoop();

            static EngineWindowConfig CreateDefaultRenderingConfig()
            {
                EngineWindowConfig result;

                strcpy(result.windowConfig.windowName, "Interactive Window");
                result.windowConfig.windowStyle = AppKit::Window::WindowStyle::Default;
                result.windowConfig.videoMode = AppKit::Window::VideoMode(1280, 720, 24);

                result.glContextConfig.vSync = true;

                // GL Context Settings
                result.glContextConfig.depthBits = 24;
                result.glContextConfig.stencilBits = 0;
                result.glContextConfig.antialiasingLevel = 0;
                result.glContextConfig.sRgbCapable = true;
                result.glContextConfig.coreAttribute = false;

                result.glContextConfig.majorVersion = 2;
                result.glContextConfig.minorVersion = 1;

                return result;
            }
        };

    }

}