#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>
// #include "components/ComponentGrow.h"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

void save_options();


// to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    screenManager = std::make_shared<ui::ScreenManager>();
}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->affectComponentStart = true;

    auto main_camera = root->addChild(Transform::CreateShared("Main Camera"));

    root->addChild(Transform::CreateShared("ui"));
}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    GLRenderState *renderState = GLRenderState::Instance();

    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    {
        auto mainCamera = root->findTransformByName("Main Camera");
        camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
    }

    std::vector<std::unique_ptr<ui::Screen>> screens;
    screens.push_back(STL_Tools::make_unique<ui::ScreenMain>());
    screens.push_back(STL_Tools::make_unique<ui::ScreenOptions>());
    screens.push_back(STL_Tools::make_unique<ui::ScreenMessageBox>());

    screenManager->setColorPalette(ui::Pallete::Blush);
    screenManager->camera = camera;
    screenManager->load_screens(engine, resourceMap, &mathRandom, screens, MathCore::vec2i(renderWindow->CameraViewport.c_val().w, renderWindow->CameraViewport.c_val().h));
    auto ui = root->findTransformByName("ui");
    ui->addChild(screenManager->uiRoot);

    screenManager->screen<ui::ScreenMain>()->show(
        {"New Game", "Options", "Exit Game"},
        "New Game",
        [&](const std::string &option)
        {
            if (option == "New Game")
            {
                screenManager->close_all();
            }
            else if (option == "Options")
            {
                screenManager->screen<ui::ScreenOptions>()->showOptions(
                    [&](AppOptions::OptionsManager *localOptions)
                    {
                        // AppOptions::OptionsManager *appOptions = AppOptions::OptionsManager::Instance();
                        // app->appOptions->applyOptionsFrom(localOptions);

                        bool needs_save = localOptions->hasAnyChange();
                        bool needsRestart = false;
                        if (needs_save)
                        {
                            needsRestart = localOptions->hasChanged("Video", "Resolution") ||
                                           localOptions->hasChanged("Video", "WindowMode") ||
                                           localOptions->hasChanged("Video", "AntiAliasing");

                            AppOptions::OptionsManager localOptionsCopy = *localOptions;

                            screenManager->screen<ui::ScreenMessageBox>()->showMessageBox( //
                                "Content changed! Do you want to apply?",
                                {"Apply", "Discard"}, // options
                                "Apply",              // init selected
                                [this, localOptionsCopy](const std::string &option)
                                {
                                    if (option == "Apply")
                                    {
                                        *AppOptions::OptionsManager::Instance() = localOptionsCopy;
                                        save_options();
                                        screenManager->open_screen("ScreenMain");
                                    }
                                    else
                                    {
                                        screenManager->open_screen("ScreenMain");
                                    }
                                });
                        }
                        else
                        {
                            screenManager->open_screen("ScreenMain");
                        }
                    });
            }
            else if (option == "Exit Game")
            {
                AppKit::GLEngine::Engine::Instance()->app->exitApp();
            }
        });

    // renderWindow->CameraViewport.OnChange.add(EventCore::CallbackWrapper(&MainScene::resize, this));
    renderWindow->CameraViewport.forceTriggerOnChange();

    renderWindow->inputManager.onKeyboardEvent.add([&](const AppKit::Window::KeyboardEvent &evt)
                                                   {
        if (evt.type == AppKit::Window::KeyboardEventType::KeyPressed){
            if (evt.code == AppKit::Window::Devices::KeyCode::A || evt.code == AppKit::Window::Devices::KeyCode::Left)
                uiCommands.enqueue(ui::UIEvent_InputLeft);
            else if (evt.code == AppKit::Window::Devices::KeyCode::D || evt.code == AppKit::Window::Devices::KeyCode::Right)
                uiCommands.enqueue(ui::UIEvent_InputRight);
            else if (evt.code == AppKit::Window::Devices::KeyCode::W || evt.code == AppKit::Window::Devices::KeyCode::Up)
                uiCommands.enqueue(ui::UIEvent_InputUp);
            else if (evt.code == AppKit::Window::Devices::KeyCode::S || evt.code == AppKit::Window::Devices::KeyCode::Down)
                uiCommands.enqueue(ui::UIEvent_InputDown);
            else if (evt.code == AppKit::Window::Devices::KeyCode::Q || evt.code == AppKit::Window::Devices::KeyCode::LShift || evt.code == AppKit::Window::Devices::KeyCode::LControl || evt.code == AppKit::Window::Devices::KeyCode::LBracket)
                uiCommands.enqueue(ui::UIEvent_InputShoulderLeft);
            else if (evt.code == AppKit::Window::Devices::KeyCode::E || evt.code == AppKit::Window::Devices::KeyCode::RShift || evt.code == AppKit::Window::Devices::KeyCode::RControl || evt.code == AppKit::Window::Devices::KeyCode::RBracket)
                uiCommands.enqueue(ui::UIEvent_InputShoulderRight);
            else if (evt.code == AppKit::Window::Devices::KeyCode::Enter)
                uiCommands.enqueue(ui::UIEvent_InputActionEnter);
            else if (evt.code == AppKit::Window::Devices::KeyCode::Escape)
                uiCommands.enqueue(ui::UIEvent_InputActionBack);
        } });

    renderWindow->OnUpdate.add(&MainScene::update, this);
}

// clear all loaded scene
void MainScene::unloadAll()
{
    // renderWindow->CameraViewport.OnChange.remove(EventCore::CallbackWrapper(&MainScene::resize, this));
    renderWindow->OnUpdate.remove(&MainScene::update, this);

    root = nullptr;
    camera = nullptr;

    while (transformPool.size() > 0)
        transformPool.dequeue(nullptr, true);

    if (screenManager)
    {
        screenManager->close_all();
        screenManager = nullptr;
    }
}

void MainScene::update(Platform::Time *elapsed)
{
    while (uiCommands.size() > 0)
    {
        auto command = uiCommands.dequeue(nullptr, true);
        screenManager->triggerEvent(command);
    }

    screenManager->update(elapsed);
}

void MainScene::draw()
{
    auto engine = AppKit::GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(resourceMap, root, camera, true);
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void MainScene::resize(const AppKit::GLEngine::iRect &size, const AppKit::GLEngine::iRect &old_size)
{
    screenManager->resize(MathCore::vec2i(size.w, size.h));
}

MainScene::MainScene(
    App *app,
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow),
                                                                          random32(ITKCommon::RandomDefinition<uint32_t>::randomSeed()),
                                                                          mathRandom(&random32),
                                                                          transformPool(false)
{
    this->app = app;
}

MainScene::~MainScene()
{
    unload();
}
