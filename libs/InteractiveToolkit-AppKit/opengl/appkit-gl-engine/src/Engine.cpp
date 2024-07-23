//#include <glad/gl.h> // extensions here
#include <appkit-gl-base/platform/PlatformGL.h>

#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-base/font/deprecated/GLFont.h>

#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

// #include <glew/glew.h> // extensions here
// #include <glad/gl.h> // extensions here

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace AppKit
{
    namespace GLEngine
    {

        Engine *Engine::Instance()
        {
            static Engine result;
            return &result;
        }

        Engine::Engine()
        {
            OnCreateInstanceFnc = nullptr;
            app = nullptr;
            window = nullptr;
            setNewWindowConfig = false;
            alwaysDraw = false;
        }

        void Engine::initialize(const std::string &_companyName, const std::string &_gameName,
                            const EventCore::Callback<AppBase *(void)> &_OnCreateInstanceFnc)
        {

            ITK_ABORT(OnCreateInstanceFnc != nullptr, "Engine Already Initialized.\n");

            OnCreateInstanceFnc = _OnCreateInstanceFnc;
            app = nullptr;
            window = nullptr;
            companyName = _companyName;
            gameName = _gameName;
        }

        std::vector<AppKit::Window::VideoMode> Engine::getResolutionList() const
        {
            return AppKit::Window::Window::getFullScreenVideoModes();
        }

        void Engine::configureWindow(const EngineWindowConfig &windowConfig)
        {

            if (app != nullptr)
            {
                changeWindowConfig = windowConfig;
                setNewWindowConfig = true;
                return;
            }

            window = new AppKit::Window::GLWindow(windowConfig.windowConfig, windowConfig.glContextConfig);

            // set first level of the event managers...
            // window->inputManager.onWindowEvent.add(this, &Engine::onWindowEvent);
            // window->inputManager.onKeyboardEvent.add(this, &Engine::onKeyboardEvent);
            // window->inputManager.onMouseEvent.add(this, &Engine::onMouseEvent);
            // window->inputManager.onJoystickEvent.add(this, &Engine::onJoystickEvent);
            // window->inputManager.onTouchEvent.add(this, &Engine::onTouchEvent);
            // window->inputManager.onSensorEvent.add(this, &Engine::onSensorEvent);

            //
            // get config after window creation
            //
            sRGBCapable = window->getGLContextConfig().sRgbCapable;

//
// Check hardware capabilities
//
// glewInit();
#if defined(GLAD_GLES2)
            gladLoaderUnloadGLES2();
            gladLoaderLoadGLES2();
            ITK_ABORT(glClearColor == nullptr, "Function not loaded...\n");

            char *lang_ver = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
            printf("GL_SHADING_LANGUAGE_VERSION: %s\n", lang_ver);

            char *ext_list = (char *)glGetString(GL_EXTENSIONS);
            printf("GL_EXTENSIONS: %s\n", ext_list);

#else
            gladLoaderUnloadGL();
            gladLoaderLoadGL();
#endif

            GLRenderState::Instance()->initialize(window->getSize().width, window->getSize().height);

            std::string vendor = ITKCommon::StringUtil::toLower(std::string((const char *)glGetString(GL_VENDOR)));
            // std::string renderer = std::string((const char*)glGetString(GL_RENDERER));
            printf("Card: %s\n", vendor.c_str());
            isNVidiaCard = ITKCommon::StringUtil::contains(vendor, "nvidia");
            isAMDCard = ITKCommon::StringUtil::contains(vendor, "amd") || ITKCommon::StringUtil::contains(vendor, "radeon");
            isIntelCard = ITKCommon::StringUtil::contains(vendor, "intel");

            app = OnCreateInstanceFnc();
        }

        void Engine::clear()
        {
            if (app != nullptr)
            {
                delete app;
                app = nullptr;
            }

            GLRenderState::Instance()->releaseResources();
            AppKit::OpenGL::GLFont::releaseSharedResources();

            // cannot setnullptr and delete app... some components need app access when deleting
            // delete app);

            if (window != nullptr)
            {
                window->close();
                delete window;
                window = nullptr;
            }
        }

        void Engine::mainLoop()
        {
            EventCore::PressReleaseDetector focusChange;

            while (window != nullptr && window->isOpen())
            {
                focusChange.setState(window->hasFocus());

                if (focusChange.down)
                {
                    app->OnGainFocus();
                }
                else if (focusChange.up)
                {
                    app->OnLostFocus();
                }

                if (alwaysDraw)
                {
                    window->glSetActivate(true);
                    app->draw();
                    // glClearColor(0,0,0,1);
                    // glClear(GL_COLOR_BUFFER_BIT);
                    window->glSwapBuffers();

                    // force terminate all GL commands after swap buffers...
                    //  reduces a bit of stuttering
                    glFinish();

                    if (!focusChange.pressed)
                    {
                        // avoid linux 100% CPU usage
                        Platform::Sleep::millis(1000 / 24);
                    }
                }
                else if (focusChange.pressed)
                {
                    window->glSetActivate(true);
                    app->draw();
                    // glClearColor(0,0,0,1);
                    // glClear(GL_COLOR_BUFFER_BIT);
                    window->glSwapBuffers();

                    // force terminate all GL commands after swap buffers...
                    //  reduces a bit of stuttering
                    glFinish();
                }
                else
                {
                    // avoid linux 100% CPU usage
                    Platform::Sleep::millis(100);
                }

                AppKit::Window::InputManager *inputManager = nullptr;
                if (app != nullptr)
                    inputManager = &app->screenRenderWindow.inputManager;
                window->forwardWindowEventsToInputManager(alwaysDraw, inputManager);

                if (app != nullptr && app->canExitApplication)
                    clear();

                // resize, or change resolution, or turn on AA... etc...
                if (setNewWindowConfig)
                {
                    setNewWindowConfig = false;
                    clear();
                    configureWindow(changeWindowConfig);
                }
            }
        }

    }
}
