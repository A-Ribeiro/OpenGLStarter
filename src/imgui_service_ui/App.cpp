#include "App.h"

#include <InteractiveToolkit/ITKCommon/Path.h>

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

#include <appkit-gl-base/platform/PlatformGL.h> // include gl headers
#include <backend/imgui_impl_window_gl.h>
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>


App::App()
{
#if defined(ARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH)
    serviceController.loadFromFile("/usr/local/etc/OpenMultimedia/service_list.json");
#else
    serviceController.loadFromFile("service_list.json");
#endif

    serviceController.printContent();

    //forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();

    Engine::Instance()->window->setMouseCursorVisible(false);

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate
    renderState->ClearColor = vec4f(0.0f,0.0f,0.0f,1.0f);
    renderState->FrontFace = FrontFaceCW;
#ifndef ITK_RPI
    //renderState->Wireframe = WireframeBack;
    //renderState->CullFace = CullFaceNone;

    renderState->Wireframe = WireframeDisabled;
    renderState->CullFace = CullFaceBack;
#else
    renderState->CullFace = CullFaceBack;
#endif

    AppBase::OnGainFocus.add(&App::onGainFocus, this);
    AppBase::OnLostFocus.add(&App::onLostFocus, this);

    AppBase::screenRenderWindow.CameraViewport.OnChange.add(&App::onViewportChange, this);

    fade = new Fade(&time);

    fade->fadeOut(5.0f, nullptr);
    time.update();

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow.setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow.setViewportFromRealWindowSizeEnabled(true);
    screenRenderWindow.setEventForwardingEnabled(true);

    // DPI Computation
    {
        int monitorDefault = 0;
        auto allMonitors = DPI::Display::QueryMonitors(&monitorDefault);

        auto selectedMonitor = &allMonitors[monitorDefault];

        auto screen_size_in = selectedMonitor->SizeInches();
        auto screen_size_pixels = selectedMonitor->SizePixels();
        auto dpii = DPI::Display::ComputeDPIi(screen_size_pixels, screen_size_in);

        this->GlobalScale = (float)dpii.y / 120.0f;
        if (this->GlobalScale < 1.0f)
            this->GlobalScale = 1.0f;

        mainMonitorCenter = selectedMonitor->Position() + screen_size_pixels / 2;
        windowResolution = window->getSize();
    }
}

void App::load() {

    #if defined(GLAD_GLES2)
        const char* glsl_version = "#version 100";
    #else
        const char* glsl_version = "#version 120";
    #endif

    // Our state
    //show_demo_window = true;
    //show_another_window = false;
    //clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // dont save windows positions...
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();
	// ImGui::StyleColorsClassic();

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg]               = ImVec4(0.2f, 0.2f, 0.2f, 0.94f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_WindowGL_InitForOpenGL(WindowUserData::Create(window, &this->screenRenderWindow.inputManager));
    OPENGL_CMD(ImGui_ImplOpenGL3_Init(glsl_version));

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //auto font = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f * this->GlobalScale);
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    this->imGuiStyleBackup = ImGui::GetStyle();

    this->applyGlobalScale();

    fade->fadeOut(3.0f,nullptr);
}

void App::applyGlobalScale() {

    ImGuiStyle& style = ImGui::GetStyle();

    style = this->imGuiStyleBackup;
    style.ScaleAllSizes(this->GlobalScale);
    if (this->GlobalScale < 1.0f)
        style.MouseCursorScale = 1.0f;
    else
        style.MouseCursorScale = this->GlobalScale;

    AppKit::Window::GLWindow* window = AppKit::GLEngine::Engine::Instance()->window;
    window->setSize( (MathCore::vec2f)windowResolution * this->GlobalScale );

    //AppKit::Window::VideoMode vm = AppKit::Window::Window::getDesktopVideoMode();
    window->setPosition(
        (
            //MathCore::vec2i(vm.width, vm.height)
        - window->getSize()
        ) / 2
        + mainMonitorCenter
    );

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    auto font = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f * this->GlobalScale);
	IM_ASSERT(font != nullptr);

}


App::~App(){
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_WindowGL_Shutdown();
    ImGui::DestroyContext();

    if (fade != nullptr){
        delete fade;
        fade = nullptr;
    }
    
    resourceHelper.finalize();
}

void App::draw() {
    time.update();

    //set min delta time (the passed time or the time to render at 24fps)
    time.deltaTime = OP<float>::minimum(time.deltaTime,1.0f/24.0f);

    StartEventManager::Instance()->processAllComponentsWithTransform();
    
    screenRenderWindow.OnPreUpdate(&time);
    screenRenderWindow.OnUpdate(&time);
    screenRenderWindow.OnLateUpdate(&time);

    // pre process all scene graphs
    /*if (sceneJesusCross != nullptr)
        sceneJesusCross->precomputeSceneGraphAndCamera();
    if (sceneGUI != nullptr)
        sceneGUI->precomputeSceneGraphAndCamera();*/

    screenRenderWindow.OnAfterGraphPrecompute(&time);

    /*if (sceneJesusCross != nullptr)
        sceneJesusCross->draw();
    if (sceneGUI != nullptr)
        sceneGUI->draw();*/

    GLRenderState *renderState = GLRenderState::Instance();
    renderState->FrontFace = FrontFaceCCW;
    renderState->DepthTest = DepthTestDisabled;

    //draw UI
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_WindowGL_NewFrame();
    ImGui::NewFrame();

    //show the service list window
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                /*
                if (ImGui::BeginMenu("Services")) {
                    if (ImGui::BeginMenu("A"))
                    {
                        if (ImGui::MenuItem("Start")) {}
                        if (ImGui::MenuItem("Stop")) {}
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("B"))
                    {
                        if (ImGui::MenuItem("Start")) {}
                        if (ImGui::MenuItem("Stop")) {}
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                */
                if (ImGui::MenuItem("Quit", "Ctrl+Q")) { exitApp(); }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        //fullscreen
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        //flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
        flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        if (ImGui::Begin("Service Controller", nullptr, flags))
        {
            static float max_line_width = 0.0f;
            
            ImGui::SetCursorPosX((-ImGui::CalcTextSize(this->serviceController.title.c_str()).x+ImGui::GetWindowSize().x)*0.5f);
            ImGui::Text("%s",this->serviceController.title.c_str());
            ImGui::Separator();
            ImGui::NewLine();

            float align_pos_x = (-max_line_width + ImGui::GetWindowSize().x) * 0.5f;

            float max_aux = 0.0f;

            this->serviceController.datamutex.lock();
            int i = 0;
            for(auto &entry : this->serviceController.entries){
                ImGui::SetCursorPosX(align_pos_x);
                entry.ImGUIDrawAndProcessLogic(i++, &time);
                max_aux = MathCore::OP<float>::maximum(max_aux, ImGui::GetItemRectMax().x - align_pos_x);
            }
            this->serviceController.datamutex.unlock();


            max_line_width = max_aux;

            // list or services...

            ImGui::NewLine();

            static float button_width = 1.0f;
            ImGui::SetCursorPosX((-button_width + ImGui::GetWindowSize().x) * 0.5f);
            if (ImGui::Button("Quit")) { exitApp(); }
            button_width = ImGui::GetItemRectSize().x;// ImGui::CalcItemWidth();
        }

        ImGui::End();

        
    }

    /*
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
    */

    // Rendering
    ImGui::Render();
    
    MathCore::vec2i windowSize = window->getSize();
    glViewport(0, 0, windowSize.width, windowSize.height);
    //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        //GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        //glfwMakeContextCurrent(backup_current_context);
        window->glSetActivate(true);
    }

    renderState->FrontFace = FrontFaceCW;
    renderState->DepthTest = DepthTestLessEqual;

    fade->draw();
    bool ctrl_pressed = Keyboard::isPressed(KeyCode::LControl) || Keyboard::isPressed(KeyCode::RControl);
    if (
        //Keyboard::isPressed(KeyCode::Escape) || 
        (ctrl_pressed && Keyboard::isPressed(KeyCode::Q))
        )
        exitApp();
    if (fade->isFading)
        return;
}

void App::onGainFocus() {
    //window->setMouseCursorVisible(false);
    time.update();
}

void App::onLostFocus() {
    //window->setMouseCursorVisible(true);
}

void App::onViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue) {
    GLRenderState *renderState = GLRenderState::Instance();
    renderState->Viewport = AppKit::GLEngine::iRect(value.w, value.h);
}
