// #include <glad/gl.h> // extensions here
#include <appkit-gl-base/platform/PlatformGL.h>

#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-base/font/deprecated/GLFont.h>

#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

// #include <glew/glew.h> // extensions here
// #include <glad/gl.h> // extensions here

#ifdef _WIN32

// force use the dedicated GPU to open the OpenGL Context on notebooks
// https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card
extern "C" __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
extern "C" __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 1;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined(__linux__)

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <unistd.h>
#include <dlfcn.h>

bool check_is_nvidia_or_amd_opengl()
{
    bool result = false;

    Display *dpy = XOpenDisplay(nullptr);
    int attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    XVisualInfo *vi = glXChooseVisual(dpy, 0, attribs);
    Colormap cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;
    Window win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 32, 32, 0,
                               vi->depth, InputOutput, vi->visual,
                               CWColormap | CWEventMask, &swa);
    // XMapWindow(dpy, win); // invisible window
    GLXContext glc = glXCreateContext(dpy, vi, nullptr, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);

    const GLubyte *(*custom_glGetString)(GLenum name);

    void* handle = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!handle)
        handle = dlopen("libGL.so", RTLD_LAZY | RTLD_LOCAL);
    ITK_ABORT(!handle, "Cannot open libGL.so: %s\n", dlerror());

    dlerror(); // Clear any existing error

    custom_glGetString = (const GLubyte *(*)(GLenum))dlsym(handle, "glGetString");

    char * err = dlerror();
    ITK_ABORT(err != nullptr, "Cannot find glGetString in libGL.so: %s\n", err);

// #if defined(GLAD_GLES2)
//     gladLoaderUnloadGLES2();
//     gladLoaderLoadGLES2();
// #else
//     gladLoaderUnloadGL();
//     gladLoaderLoadGL();
// #endif

    // while (true) {
    //     glClear(GL_COLOR_BUFFER_BIT);
    //     glXSwapBuffers(dpy, win);
    //     usleep(16000); // ~60 FPS
    // }

    std::string vendor_aux = ITKCommon::StringUtil::toLower(std::string((const char *)custom_glGetString(GL_VENDOR)));
    bool isNVidiaCard = ITKCommon::StringUtil::contains(vendor_aux, "nvidia");
    bool isAMDCard = ITKCommon::StringUtil::contains(vendor_aux, "amd") || ITKCommon::StringUtil::contains(vendor_aux, "radeon");
    // bool isIntelCard = ITKCommon::StringUtil::contains(vendor_aux, "intel");

    dlclose(handle);

    glXDestroyContext(dpy, glc);
    XFree(vi);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

    return isNVidiaCard || isAMDCard;
}

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

#if defined(__linux__)
        static std::string execCommand(const char *cmd)
        {
            char buffer[1024];
            std::string result;
            std::unique_ptr<FILE, int (*)(FILE *)> pipe(popen(cmd, "r"), pclose);
            if (!pipe)
                throw std::runtime_error("Falha ao executar comando.");
            while (fgets(buffer, 1024, pipe.get()) != nullptr)
                result += buffer;
            return result;
        }
#endif

        void Engine::configureWindow(const EngineWindowConfig &windowConfig,
                                     const EventCore::Callback<void(AppKit::Window::GLWindow *window)> &_OnConfigureWindowDone)
        {
            OnConfigureWindowDoneFnc = _OnConfigureWindowDone;
#if defined(__linux__)

            //             {
            //                 auto test_config = AppKit::GLEngine::Engine::CreateDefaultRenderingConfig();
            //                 test_config.windowConfig.videoMode = AppKit::Window::VideoMode(32, 32);

            //                 auto test_window_ext = STL_Tools::make_unique<AppKit::Window::GLWindow>(test_config.windowConfig, test_config.glContextConfig);

            // #if defined(GLAD_GLES2)
            //                 gladLoaderUnloadGLES2();
            //                 gladLoaderLoadGLES2();
            // #else
            //                 gladLoaderUnloadGL();
            //                 gladLoaderLoadGL();
            // #endif
            //                 std::string vendor_aux = ITKCommon::StringUtil::toLower(std::string((const char *)glGetString(GL_VENDOR)));
            //                 isNVidiaCard = ITKCommon::StringUtil::contains(vendor_aux, "nvidia");
            //                 isAMDCard = ITKCommon::StringUtil::contains(vendor_aux, "amd") || ITKCommon::StringUtil::contains(vendor_aux, "radeon");
            //                 isIntelCard = ITKCommon::StringUtil::contains(vendor_aux, "intel");
            //             }

            auto instaled_videocard = execCommand("lspci | grep VGA");
            if (!check_is_nvidia_or_amd_opengl())
            {
                if (ITKCommon::StringUtil::contains(instaled_videocard, "NVIDIA"))
                {
                    // Activate GPU NVIDIA
                    setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 1);
                    setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);
                    printf("activating NVIDIA discrete card\n");
                }
                else if (ITKCommon::StringUtil::contains(instaled_videocard, "AMD"))
                {
                    // Activate GPU AMD
                    setenv("DRI_PRIME", "1", 1);
                    printf("activating AMD discrete card\n");
                }
            }
#endif

            if (app != nullptr)
            {
                changeWindowConfig = windowConfig;
                setNewWindowConfig = true;
                return;
            }

            window = new AppKit::Window::GLWindow(windowConfig.windowConfig, windowConfig.glContextConfig);
            window->onWin32BlockState_WindowEvent = EventCore::CallbackWrapper(&Engine::onWin32BlockState_WindowEvent, this);

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

            if (!windowConfig.glContextConfig.sRgbCapable && sRGBCapable)
            {
                sRGBCapable = false;
                window->glSetActivate(true);
                glDisable(GL_FRAMEBUFFER_SRGB);
            }

            GLRenderState::Instance()->initialize(window->getSize().width, window->getSize().height);

            std::string vendor = ITKCommon::StringUtil::toLower(std::string((const char *)glGetString(GL_VENDOR)));
            // std::string renderer = std::string((const char*)glGetString(GL_RENDERER));
            printf("Card: %s\n", vendor.c_str());
            isNVidiaCard = ITKCommon::StringUtil::contains(vendor, "nvidia");
            isAMDCard = ITKCommon::StringUtil::contains(vendor, "amd") || ITKCommon::StringUtil::contains(vendor, "radeon");
            isIntelCard = ITKCommon::StringUtil::contains(vendor, "intel");

#if defined(__linux__)
            if (!isNVidiaCard && !isAMDCard)
            {
                if (ITKCommon::StringUtil::contains(instaled_videocard, "NVIDIA"))
                {
                    // Activate GPU NVIDIA
                    unsetenv("__NV_PRIME_RENDER_OFFLOAD");
                    unsetenv("__GLX_VENDOR_LIBRARY_NAME");
                }
                else if (ITKCommon::StringUtil::contains(instaled_videocard, "AMD"))
                {
                    // Activate GPU AMD
                    unsetenv("DRI_PRIME");
                }
            }
#endif

            app = OnCreateInstanceFnc();
            if (OnConfigureWindowDoneFnc != nullptr)
                OnConfigureWindowDoneFnc(window);
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
                window->onWin32BlockState_WindowEvent = nullptr;
                window->close();
                delete window;
                window = nullptr;
            }
        }

        void Engine::onWin32BlockState_WindowEvent(const AppKit::Window::WindowEvent &evt)
        {
            if (!window)
                return;
            if (evt.type == AppKit::Window::WindowEventType::Win32RedrawOnResize)
            {
                window->glSetActivate(true);
                app->draw();
                window->glSwapBuffers();
                // force terminate all GL commands after swap buffers...
                //  reduces a bit of stuttering
                glFinish();
                return;
            }
            // only handles the resize and redraw...
            AppKit::Window::InputManager *inputManager = (app != nullptr)
                                                             ? &app->screenRenderWindow->inputManager
                                                             : &window->inputManager;
            inputManager->onWindowEvent(evt);
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
                    inputManager = &app->screenRenderWindow->inputManager;
                window->forwardWindowEventsToInputManager(alwaysDraw, inputManager);

                if (app != nullptr && app->canExitApplication)
                    clear();

                // resize, or change resolution, or turn on AA... etc...
                if (setNewWindowConfig)
                {
                    setNewWindowConfig = false;
                    clear();
                    configureWindow(changeWindowConfig, OnConfigureWindowDoneFnc);
                }
            }
        }

    }
}
