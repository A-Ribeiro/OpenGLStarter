#include "ImGuiManager.h"
#include "../InnerViewport.h"
#include "../OpenFolderDialog.h"

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>


ImGuiManager::ImGuiManager()
{
    font_half_size = NULL;
	reset_layout = true;
	innerViewport = NULL;

	memset(icons, 0, sizeof(AppKit::OpenGL::GLTexture*) * (int)IconType::Count);
}

ImGuiManager* ImGuiManager::Instance()
{
	static ImGuiManager _ImGuiManager;
	return &_ImGuiManager;
}

void ImGuiManager::Initialize(AppKit::Window::GLWindow* window,
	AppKit::Window::InputManager* inputManager,
	const std::string& base_path)
{
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
	//load all icons
	{
		icons[(int)IconType::Small_BoxNode] = AppKit::OpenGL::GLTexture::loadFromFile("./resources/boxnode.png");
		stretch[(int)IconType::Small_BoxNode] = false;

		icons[(int)IconType::Small_BoxNode_Filled] = AppKit::OpenGL::GLTexture::loadFromFile("./resources/boxnode_filled.png");
		stretch[(int)IconType::Small_BoxNode_Filled] = false;

		icons[(int)IconType::Small_Folder_Empty] = AppKit::OpenGL::GLTexture::loadFromFile("./resources/uxwing/Small_Folder_Empty.png");
		stretch[(int)IconType::Small_Folder_Empty] = false;
		icons[(int)IconType::Small_Folder_Filled] = AppKit::OpenGL::GLTexture::loadFromFile("./resources/uxwing/Small_Folder_Filled.png");
		stretch[(int)IconType::Small_Folder_Filled] = false;

		icons[(int)IconType::Big_Folder_Empty] = AppKit::OpenGL::GLTexture::loadFromFile("./resources/uxwing/Big_Folder_Empty.png");
		stretch[(int)IconType::Big_Folder_Empty] = true;
		icons[(int)IconType::Big_Folder_Filled] = AppKit::OpenGL::GLTexture::loadFromFile("./resources/uxwing/Big_Folder_Filled.png");
		stretch[(int)IconType::Big_Folder_Filled] = true;

		icons[(int)IconType::Big_File_Generic] = AppKit::OpenGL::GLTexture::loadFromFile("./resources/uxwing/Big_File_Generic.png");
		stretch[(int)IconType::Big_File_Generic] = false;


		//icons[(int)IconType::Big_Folder_Filled]->generateMipMap();
		for (auto icon : icons) {
			icon->generateMipMap();
		}
	}

	innerViewport = new InnerViewport((App*)AppKit::GLEngine::Engine::Instance()->app, true);

	this->base_path = base_path;


#if defined(GLAD_GLES2)
	const char* glsl_version = "#version 100";
#else
	const char* glsl_version = "#version 120";
#endif

	// Our state
	// show_demo_window = true;
	// show_another_window = false;
	// clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	CustomSettings::Init();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	// dont save windows positions...
	io.IniFilename = NULL;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge = true;
	// io.ConfigViewportsNoTaskBarIcon = true;
	io.MouseDoubleClickTime = 0.30f;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();
	// ImGui::StyleColorsClassic();

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.94f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	//ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_WindowGL_InitForOpenGL(WindowUserData::Create(window, inputManager));
	OPENGL_CMD(ImGui_ImplOpenGL3_Init(glsl_version));

	// float DPIScale = 3.0f;
	// ImGui::GetStyle().ScaleAllSizes(1.0f/DPIScale);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// // io.Fonts->AddFontDefault();
	// auto font = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f * this->GlobalScale);
	// // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	// // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	// // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	// // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	// IM_ASSERT(font != NULL);
	// //ImGui::PushFont(font);

    this->imGuiStyleBackup = style;
	this->applyGlobalScale();

    // render a NULL frame: force load fonts
    //  Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_WindowGL_NewFrame();
	ImGui::NewFrame();

	//
	// Initialize menus
	//
	//ImGuiMenu::Instance()->Clear();
	views.clear();
	views.push_back(hierarchy.Init());
	views.push_back(project.Init());
	views.push_back(console.Init());
	views.push_back(scene.Init());
	views.push_back(game.Init());
	views.push_back(inspector.Init());
    views.push_back(shortcutManager.Init());
    views.push_back(contextMenu.Init());
    views.push_back(dialogs.Init());

	ImGuiMenu::Instance()->AddMenu(10000,"Window/<<>>", "", nullptr);
	ImGuiMenu::Instance()->AddMenu(20000,"Window/Reset Layout", "", std::bind(&ImGuiManager::ResetLayout, this));

    ImGuiMenu::Instance()->makeLast("Window");

    // Debug
    {
        OnHover.add([](View *view, bool v){
            printf("OnHover on view: %s -> %i\n",view->type, (int)v);
        });
        OnFocus.add([](View *view, bool v){
            printf("OnFocus on view: %s -> %i\n",view->type, (int)v);
        });
    }


    ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::applyGlobalScale() {

	ImGuiStyle& style = ImGui::GetStyle();

	style = this->imGuiStyleBackup;
	style.ScaleAllSizes(this->GlobalScale);
	if (this->GlobalScale < 1.0f)
		style.MouseCursorScale = 1.0f;
	else
		style.MouseCursorScale = this->GlobalScale;

	AppKit::Window::GLWindow* window = AppKit::GLEngine::Engine::Instance()->window;
	window->setSize((MathCore::vec2f)windowResolution * this->GlobalScale);

	//AppKit::Window::VideoMode vm = AppKit::Window::Window::getDesktopVideoMode();
	window->setPosition(
		(
			//MathCore::vec2i(vm.width, vm.height)
			-window->getSize()
			) / 2
		+ mainMonitorCenter
	);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();

	auto font = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f * this->GlobalScale);
	IM_ASSERT(font != NULL);

	font_half_size = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 12.0f * this->GlobalScale);
	IM_ASSERT(font_half_size != NULL);

    // io.Fonts->Build();

    // while (!font_half_size->IsLoaded()) {
    //     printf(".");
    //     fflush(stdout);
    //     Platform::Sleep::millis(1000);
    // }
    // printf("\n");


}

void ImGuiManager::Finalize()
{
	if (innerViewport != NULL) {
		delete innerViewport;
		innerViewport = NULL;
	}

	for (auto& tex : icons) {
		if (tex != NULL) {
			delete tex;
			tex = NULL;
		}
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_WindowGL_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::SaveLayout()
{
	// save layout
	auto path = base_path + ITKCommon::PATH_SEPARATOR + "layout.ini";
	size_t settings_size = 0;
	const char* settings = ImGui::SaveIniSettingsToMemory(&settings_size);
	FILE* out = fopen(path.c_str(), "wb");
	if (out)
	{
		printf("Saving layout...\n");
		fwrite(settings, sizeof(char), settings_size, out);
		fclose(out);
	}
}

void ImGuiManager::LoadLayout()
{
	// load layout
	auto path = base_path + ITKCommon::PATH_SEPARATOR + "layout.ini";
	std::vector<char> settings;
	FILE* in = fopen(path.c_str(), "rb");
	if (in)
	{
		printf("Loading saved layout...\n");
		fseek(in, 0, SEEK_END);
		int32_t size = (int32_t)ftell(in);
		settings.resize(size + 1);
		fseek(in, 0, SEEK_SET);
		uint32_t readed_size = (uint32_t)fread(&settings[0], sizeof(uint8_t), size, in);
		fclose(in);
		ImGui::LoadIniSettingsFromMemory(&settings[0]);
	}
}

void ImGuiManager::DeleteLayout()
{
	auto path = base_path + ITKCommon::PATH_SEPARATOR + "layout.ini";
	remove(path.c_str());
}

void ImGuiManager::ResetLayout()
{
	DeleteLayout();
	reset_layout = true;
	for (auto& view : views)
		view->active = true;

	CustomSettings::Instance()->reset();

	// static ImGuiStyle savedStyle = ImGui::GetStyle();

	// ImGui::GetStyle() = savedStyle;

	// float DPIScale = 1.5f;
	// ImGui::GetStyle().ScaleAllSizes(DPIScale);

	// ImGui::GetIO().FontGlobalScale = DPIScale;

	//ImGui::GetMainViewport()->DpiScale = DPIScale;

}

void ImGuiManager::RenderAndLogic(AppKit::Window::GLWindow* window, Platform::Time* time)
{

	AppKit::GLEngine::GLRenderState* renderState = AppKit::GLEngine::GLRenderState::Instance();
	renderState->FrontFace = AppKit::GLEngine::FrontFaceCCW;
	renderState->DepthTest = AppKit::GLEngine::DepthTestDisabled;

	MathCore::vec2i windowSize = window->getSize();
	renderState->Viewport = iRect(windowSize.width, windowSize.height);

	// draw UI

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = MathCore::OP<float>::maximum(time->unscaledDeltaTime, 1.0f / 1000.0f); // set the time elapsed since the previous frame (in seconds)

	//  Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_WindowGL_NewFrame();

	//float DPIScale = 1.25f;

	//io.DisplaySize = ImVec2((float)windowSize.width, (float)windowSize.height) / DPIScale;
	//io.DisplayFramebufferScale = ImVec2(1.0f,1.0f) * DPIScale;

	ImGui::NewFrame();

	ImGuiMenu::Instance()->RenderAndLogic();

	// fullscreen
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings;
	// flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
	flags |= ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("FullScreen Window", NULL, flags))
	{
		// display_order.push_back(ImGui::GetCurrentWindow());

		// setup dockspace
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr || reset_layout)
		{
			printf("Creating dockspace\n");

			// Clear out existing layout
			ImGui::DockBuilderRemoveNode(dockspace_id);
			// Add empty node
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
			// Main node should cover entire window
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
			// ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);
			//  Build dock layout
			ImGuiID center = dockspace_id;
			ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, nullptr, &center);
			ImGuiID down = ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.25f, nullptr, &center);
			ImGuiID left = ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.25f, nullptr, &center);

			// ImGuiID rightDown;
			// ImGuiID rightUp = ImGui::DockBuilderSplitNode(right, ImGuiDir_Up, 0.65f, nullptr, &rightDown);

			ImGui::DockBuilderDockWindow("Hierarchy", left);

			ImGui::DockBuilderDockWindow("Project", down);
			ImGui::DockBuilderDockWindow("Console", down);

			ImGui::DockBuilderDockWindow("Scene", center);
			ImGui::DockBuilderDockWindow("Game", center);

			ImGui::DockBuilderDockWindow("Inspector", right);

			// dock some more windows ...
			// ImGui::DockBuilderGetNode(down)->SelectedTabId = ImHashStr("Project");//ImGui::FindWindowByName("Project")->ID;//ImHashStr("#TAB",0,ImHashStr("Project"));

			// printf("%u\n", ImGui::DockBuilderGetNode(down)->SelectedTabId);

			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None; // ImGuiDockNodeFlags_NoSplit;
		ImGui::DockSpace(dockspace_id, { 0, 0 }, dockspace_flags);
		ImGui::BringWindowToDisplayBack(ImGui::GetCurrentWindow());
	}
	ImGui::End();
	ImGui::PopStyleVar();

	for (auto& view : views)
	{
		view->RenderAndLogic();
	}

	if (reset_layout)
	{
		reset_layout = false;

		ImGui::FocusWindow(ImGui::FindWindowByName("Project"));
		ImGui::FocusWindow(ImGui::FindWindowByName("Scene"));

		ImGuiManager::Instance()->LoadLayout();
	}

	// Rendering
	ImGui::Render();

	//MathCore::vec2i windowSize = window->getSize();
	//glViewport(0, 0, windowSize.width, windowSize.height);
	// glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	// glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	//ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		// glfwMakeContextCurrent(backup_current_context);
		window->glSetActivate(true);
	}

	renderState->FrontFace = AppKit::GLEngine::FrontFaceCW;
	renderState->DepthTest = AppKit::GLEngine::DepthTestLessEqual;


    PostAction();
    PostAction.clear();
}
