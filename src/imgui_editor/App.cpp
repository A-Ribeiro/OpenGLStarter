#include "App.h"

#include <InteractiveToolkit/ITKCommon/Path.h>

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

#include <appkit-gl-base/platform/PlatformGL.h> // include gl headers
#include <backend/imgui_impl_window_gl.h>

#include "ImGui/ImGuiManager.h"
#include "OpenFolderDialog.h"

App::App()
{
// #if defined(ARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH)
//     serviceController.loadFromFile("/usr/local/etc/OpenMultimedia/service_list.json");
// #else
//     serviceController.loadFromFile("service_list.json");
// #endif
//     serviceController.printContent();

    // forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();

    Engine::Instance()->window->setMouseCursorVisible(false);

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate
    renderState->ClearColor = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
    renderState->FrontFace = FrontFaceCW;
    renderState->CullFace = CullFaceBack;

#ifndef ITK_RPI
    // renderState->Wireframe = WireframeBack;
    // renderState->CullFace = CullFaceNone;
#endif

    AppBase::OnGainFocus.add(&App::onGainFocus, this);
    AppBase::OnLostFocus.add(&App::onLostFocus, this);
    // AppBase::WindowSize.OnChange.add(this, &App::onWindowSizeChange);

    fade = new Fade(&time);

    fade->fadeOut(5.0f, NULL);
    time.update();

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow.setHandleWindowCloseButtonEnabled(true);
    screenRenderWindow.setViewportFromRealWindowSizeEnabled(true);
    screenRenderWindow.setEventForwardingEnabled(true);
}

void App::load()
{
    ImGuiMenu::Instance()->Clear();
    ImGuiMenu::Instance()->AddMenu("File/Open Folder", "",
        [this]() {
            std::string out;
            if (Native::OpenFolderDialog("./", &out) ){
                out = ITKCommon::Path::getAbsolutePath(out);
                printf("OpenedPath: %s\n", out.c_str());
                time.update();
            }
                
        }
    );

    ImGuiMenu::Instance()->AddMenu("File/<<>>", "", NULL);
    ImGuiMenu::Instance()->AddMenu("File/Quit", "Ctrl+Q", [this]()
        { this->exitApp(); });

    ImGuiManager::Instance()->Initialize(
        window,
        &this->screenRenderWindow.inputManager,
        ITKCommon::Path::getDocumentsPath("Milky Way Studio", "imgui_editor"));
    fade->fadeOut(0.5f, NULL);
}

App::~App()
{
    ImGuiManager::Instance()->SaveLayout();
    ImGuiManager::Instance()->Finalize();

    if (fade != NULL){
        delete fade;
        fade = NULL;
    }
    resourceHelper.finalize();
}

void App::draw()
{
    time.update();

    // set min delta time (the passed time or the time to render at 24fps)
    time.deltaTime = OP<float>::minimum(time.deltaTime, 1.0f / 24.0f);

    StartEventManager::Instance()->processAllComponentsWithTransform();

    screenRenderWindow.OnPreUpdate(&time);
    screenRenderWindow.OnUpdate(&time);
    screenRenderWindow.OnLateUpdate(&time);

    // pre process all scene graphs
    /*if (sceneJesusCross != NULL)
        sceneJesusCross->precomputeSceneGraphAndCamera();
    if (sceneGUI != NULL)
        sceneGUI->precomputeSceneGraphAndCamera();*/

    screenRenderWindow.OnAfterGraphPrecompute(&time);

    /*if (sceneJesusCross != NULL)
        sceneJesusCross->draw();
    if (sceneGUI != NULL)
        sceneGUI->draw();*/

    GLRenderState *renderState = GLRenderState::Instance();
    
    renderState->ClearColor = vec4f(0.0f, 0.0f, 0.0f, 1.0f);
    renderState->Viewport = AppKit::GLEngine::iRect(screenRenderWindow.CameraViewport.c_ptr()->w,screenRenderWindow.CameraViewport.c_ptr()->h);

    renderState->FrontFace = FrontFaceCCW;
    renderState->DepthTest = DepthTestDisabled;

    ImGuiManager::Instance()->RenderAndLogic(window, &time);

    screenRenderWindow.OnAfterOverlayDraw(&time);

    fade->draw();
    bool ctrl_pressed = Keyboard::isPressed(KeyCode::LControl) || Keyboard::isPressed(KeyCode::RControl);
    if (
        // Keyboard::isPressed(KeyCode::Escape) ||
        (ctrl_pressed && Keyboard::isPressed(KeyCode::Q)))
        exitApp();
    if (fade->isFading)
        return;
}

void App::onGainFocus()
{
    // window->setMouseCursorVisible(false);
    time.update();
}

void App::onLostFocus()
{
    // window->setMouseCursorVisible(true);
}

// void App::onWindowSizeChange(Property<MathCore::vec2i> *prop)
// {
//     GLRenderState *renderState = GLRenderState::Instance();
//     renderState->Viewport = AppKit::GLEngine::iRect(prop->value.width, prop->value.height);
// }
