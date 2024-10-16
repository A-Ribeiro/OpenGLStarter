#pragma once

#include <InteractiveToolkit/InteractiveToolkit.h>
#include <AppKit/window/Window.h>
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

#include <appkit-gl-base/platform/PlatformGL.h> // include gl headers

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <backend/imgui_impl_window_gl.h>
#include <backend/imgui_impl_opengl3.h>

namespace ImApp
{
    struct Config
    {
        Config() : mWidth(1280), mHeight(720), mFullscreen(false)
        {
        }
        int mWidth;
        int mHeight;
        bool mFullscreen;
    };

    struct ImApp
    {
        float GlobalScale;
        MathCore::vec2i mainMonitorCenter;
        MathCore::vec2i windowResolution;

        std::shared_ptr<AppKit::Window::GLWindow> window;
        ImGuiStyle imGuiStyleBackup;

        ImApp()
        {
        }

        ~ImApp()
        {
        }

        void Init(const Config &config = Config())
        {
            // DPI Computation
            {
                int monitorDefault = 0;
                auto allMonitors = DPI::Display::QueryMonitors(&monitorDefault);

                auto selectedMonitor = &allMonitors[monitorDefault];

                auto screen_size_in = selectedMonitor->SizeInches();
                auto screen_size_pixels = selectedMonitor->SizePixels();
                auto dpii = DPI::Display::ComputeDPIi(screen_size_pixels, screen_size_in);

                // this->GlobalScale = (float)(dpii.y / 150.0f); // *selectedMonitor->scaleFactor;
                this->GlobalScale = ((float)(dpii.y + dpii.x) * 0.5f) / 96.0f; // *selectedMonitor->scaleFactor;
                if (this->GlobalScale < 1.0f)
                    this->GlobalScale = 1.0f;

                this->GlobalScale *= selectedMonitor->scaleFactor;

                mainMonitorCenter = selectedMonitor->Position() + screen_size_pixels / 2;
                windowResolution = MathCore::vec2i(config.mWidth, config.mHeight);
            }

            AppKit::Window::WindowConfig wConfig(
                "Interactive Window",
                AppKit::Window::WindowStyle::Default,
                AppKit::Window::VideoMode(windowResolution.width, windowResolution.height, 32));

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

            window = std::make_shared<AppKit::Window::GLWindow>(wConfig, glConfig);

            // glad init
            {
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
            }

            // ImGUI initialization
            {
#if defined(GLAD_GLES2)
                const char *glsl_version = "#version 100";
#else
                const char *glsl_version = "#version 120";
#endif

                // Our state
                // show_demo_window = true;
                // show_another_window = false;
                // clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                ImGuiIO &io = ImGui::GetIO();
                (void)io;
                // dont save windows positions...
                io.IniFilename = nullptr;
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
                // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
                // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
                // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
                // io.ConfigViewportsNoAutoMerge = true;
                // io.ConfigViewportsNoTaskBarIcon = true;

                // Setup Dear ImGui style
                ImGui::StyleColorsDark();
                // ImGui::StyleColorsLight();
                // ImGui::StyleColorsClassic();

                ImGuiStyle &style = ImGui::GetStyle();
                ImVec4 *colors = style.Colors;

                colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.94f);
                colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

                // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    style.WindowRounding = 0.0f;
                    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
                }

                // Setup Platform/Renderer backends
                ImGui_WindowGL_InitForOpenGL(WindowUserData::Create(window.get(), &window->inputManager));
                OPENGL_CMD(ImGui_ImplOpenGL3_Init(glsl_version));

                // Load Fonts
                // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
                // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
                // - If the file cannot be loaded, the function will return nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
                // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
                // - Read 'docs/FONTS.md' for more instructions and details.
                // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
                // auto font = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f * this->GlobalScale);
                // io.Fonts->AddFontDefault();
                // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
                // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
                // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
                // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
                // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
                // IM_ASSERT(font != nullptr);

                this->imGuiStyleBackup = ImGui::GetStyle();
            }

            // Apply Global Scale
            {

                ImGuiStyle &style = ImGui::GetStyle();

                style = this->imGuiStyleBackup;
                style.ScaleAllSizes(this->GlobalScale);
                if (this->GlobalScale < 1.0f)
                    style.MouseCursorScale = 1.0f;
                else
                    style.MouseCursorScale = this->GlobalScale;

                // AppKit::Window::GLWindow *window = AppKit::GLEngine::Engine::Instance()->window;
                window->setSize((MathCore::vec2f)windowResolution * this->GlobalScale);

                // AppKit::Window::VideoMode vm = AppKit::Window::Window::getDesktopVideoMode();
                window->setPosition(
                    (
                        // MathCore::vec2i(vm.width, vm.height)
                        -window->getSize()) /
                        2 +
                    mainMonitorCenter);

                ImGuiIO &io = ImGui::GetIO();
                io.Fonts->Clear();

                auto font = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 12.0f * this->GlobalScale);
                IM_ASSERT(font != nullptr);
            }
        }

        void NewFrame()
        {
            window->glSetActivate(true);

            // draw UI
            //  Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_WindowGL_NewFrame();
            ImGui::NewFrame();
        }

        void EndFrame()
        {
            // Rendering
            ImGui::Render();

            MathCore::vec2i windowSize = window->getSize();
            glViewport(0, 0, windowSize.width, windowSize.height);
            // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                // GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                // glfwMakeContextCurrent(backup_current_context);
                window->glSetActivate(true);
            }

            window->glSwapBuffers();
            window->forwardWindowEventsToInputManager(false, nullptr);

            auto viewport = ImGui::GetMainViewport();
            if (viewport->PlatformRequestClose)
            {
                viewport->PlatformRequestClose = false;
                window->close();
            }
        }

        void Finish()
        {
            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_WindowGL_Shutdown();
            ImGui::DestroyContext();

            // window->glSetActivate(false);
            window = nullptr;
        }

        // while loop
        bool Done()
        {
            return !window->isOpen();
        }
    };
}
