#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>
// #include "components/ComponentGrow.h"

//
// Auto Generated: Exported Bitmaps inside the Font
//
#define Font_xbox_a u8"\U00010000"
#define Font_xbox_b u8"\U00010001"
#define Font_xbox_x u8"\U00010002"
#define Font_xbox_y u8"\U00010003"
#define Font_ps_circle_color u8"\U00010004"
#define Font_ps_cross_color u8"\U00010005"
#define Font_ps_square_color u8"\U00010006"
#define Font_ps_triangle_color u8"\U00010007"
#define Font_ps_circle_white u8"\U00010008"
#define Font_ps_cross_white u8"\U00010009"
#define Font_ps_square_white u8"\U0001000a"
#define Font_ps_triangle_white u8"\U0001000b"
#define Font_L_stick u8"\U0001000c"
#define Font_R_stick u8"\U0001000d"
#define Font_Key_arrows u8"\U0001000e"
#define Font_Key_z u8"\U0001000f"
#define Font_Key_x u8"\U00010010"
#define Font_Key_c u8"\U00010011"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

void load_options();
void save_options();
void apply_settings_to_window(const EventCore::Callback<void()> &OnAfterAppCreation);

MainScene *MainScene::currentInstance = nullptr;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    screenManager = std::make_shared<AppKit::ui::ScreenManager>();
}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared()->setRootPropertiesFromDefaultScene(this->self());
    // root->affectComponentStart = true;
    // root->setRenderWindowRegion(renderWindow);
    // root->setEventHandlerSet(this->self());

    auto main_camera = root->addChild(Transform::CreateShared("Main Camera"));

    root->addChild(Transform::CreateShared("ui"));
}

void MainScene::applySettingsChanges()
{
    AppKit::ui::OptionsManager *options = AppKit::ui::OptionsManager::Instance();

    {
        const std::string &buttonAppearance = options->getGroupValueSelectedForKey("Extra", "ButtonAppearance");
        const std::string &colorScheme = options->getGroupValueSelectedForKey("Extra", "ColorScheme");
        if (colorScheme == "Blush")
            screenManager->setColorPalette(AppKit::ui::Pallete::Blush.setAppearance(buttonAppearance.c_str()));
        else if (colorScheme == "Purple")
            screenManager->setColorPalette(AppKit::ui::Pallete::Purple.setAppearance(buttonAppearance.c_str()));
        else if (colorScheme == "Orange")
            screenManager->setColorPalette(AppKit::ui::Pallete::Orange.setAppearance(buttonAppearance.c_str()));
        else if (colorScheme == "Green")
            screenManager->setColorPalette(AppKit::ui::Pallete::Green.setAppearance(buttonAppearance.c_str()));
        else if (colorScheme == "Blue")
            screenManager->setColorPalette(AppKit::ui::Pallete::Blue.setAppearance(buttonAppearance.c_str()));
        else if (colorScheme == "Dark")
            screenManager->setColorPalette(AppKit::ui::Pallete::Dark.setAppearance(buttonAppearance.c_str()));
    }

    // handle viewport

    // renderWindow->CameraViewport
    //    renderWindow->viewportScaleFactor = 0.5f;
    // renderWindow->forceViewportFromRealWindowSize();
    MathCore::vec2i size = app->window->getSize();
    // renderWindow->setWindowViewport(AppKit::GLEngine::iRect(0, 0, size.width, size.height));
    onWindowResized(size);
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

    std::vector<std::unique_ptr<AppKit::ui::Screen>> screens;
    screens.push_back(STL_Tools::make_unique<AppKit::ui::ScreenMain>());
    screens.push_back(STL_Tools::make_unique<AppKit::ui::ScreenOptions>());
    screens.push_back(STL_Tools::make_unique<AppKit::ui::ScreenMessageBox>());
    screens.push_back(STL_Tools::make_unique<AppKit::ui::ScreenHUD>());

    applySettingsChanges();

    screenManager->camera = camera;
    screenManager->load_screens(engine, resourceMap, &mathRandom, screens, MathCore::vec2i(renderWindow->CameraViewport.c_val().w, renderWindow->CameraViewport.c_val().h));
    auto ui = root->findTransformByName("ui");
    ui->addChild(screenManager->uiRoot);
    screenManager->screen<AppKit::ui::ScreenHUD>()->inGameDialog.setSpriteAvatars({
        "resources/look_to_right_normal.png",
        "resources/look_to_right_smile.png",
        "resources/look_to_left_normal.png",
        "resources/look_to_center_normal.png",
    });

    screenManager->screen<AppKit::ui::ScreenMain>()->show(
        {"New Game", "Options", "Exit Game"},
        "New Game",
        [&](const std::string &option)
        {
            if (option == "New Game")
            {
                // screenManager->close_all();

                screenManager->open_screen("ScreenHUD");

                app->createGameScene();

                // // screenManager->screen<AppKit::ui::ScreenHUD>()->inGameDialog.showDialog(
                // //     AppKit::ui::DialogAppearModeType_Scroll,
                // //     0.5f,
                // //     "resources/look_to_right_normal.png",
                // //     AppKit::ui::DialogTextModeType_CharAppear,
                // //     "Welcome to the <b>Example UI</b> demo!\nThis is a simple dialog box with <i>rich text</i> support.\n\nPress the "
                // //     "{push;lineHeight:0.8;faceColor:ffffffff;size:40.0;}" Font_Key_z "{pop;}"
                // //     " button to proceed.",
                // //     "{push;lineHeight:0.6;faceColor:ffffffff;size:60.0;}" Font_Key_z "{pop;}",
                // //     [&]()
                // //     {
                // //         // Dialog appeared callback
                // //     },
                // //     [&]()
                // //     {
                // //         // Dialog continue pressed
                // //         screenManager->screen<ui::ScreenHUD>()->inGameDialog.hideDialog(
                // //             ui::DialogAppearModeType_Scroll,
                // //             [&]()
                // //             {
                // //                 screenManager->open_screen("ScreenMain");
                // //                 // screenManager->close_all();
                // //                 // Dialog disapear callback
                // //             });
                // //     });

                // screenManager->screen<AppKit::ui::ScreenHUD>()->inGameDialog.smartShowDialog(
                //     AppKit::ui::DialogAppearModeType_Scroll, // appear_mode
                //     AppKit::ui::DialogAppearModeType_Scroll, // disappear_mode

                //     "{push;lineHeight:0.6;faceColor:ffffffff;size:60.0;}" Font_Key_z "{pop;}",

                //     {
                //         {
                //             0.0f,
                //             "resources/look_to_right_normal.png",
                //             false,
                //             AppKit::ui::DialogTextModeType_CharAppear,
                //             "Big Moves Causes Big Wins.",
                //         },
                //         {
                //             0.1f,
                //             "resources/look_to_right_smile.png",
                //             false,
                //             AppKit::ui::DialogTextModeType_CharAppear,
                //             "Big Moves Causes Big Wins!\nNew line here.",
                //         },
                //         {
                //             1.0f,
                //             "resources/look_to_left_normal.png",
                //             false,
                //             AppKit::ui::DialogTextModeType_CharAppear,
                //             "Big Moves Causes Big Wins.",
                //         },
                //         {
                //             -1.0f,
                //             "NO_IMAGE",
                //             false,
                //             AppKit::ui::DialogTextModeType_CharAppear,
                //             "He deeply thoughts...",
                //         },
                //         {
                //             0.5f,
                //             "resources/look_to_center_normal.png",
                //             false,
                //             AppKit::ui::DialogTextModeType_CharAppear,
                //             "Shure it does!",
                //         },
                //         {
                //             0.9f,
                //             "resources/look_to_right_smile.png",
                //             true,
                //             AppKit::ui::DialogTextModeType_CharAppear,
                //             "Other Side!\nNew line here.",
                //         },
                //         {
                //             1.0f,
                //             "resources/look_to_right_normal.png",
                //             true,
                //             AppKit::ui::DialogTextModeType_CharAppear,
                //             "Dialog Finished.",
                //         },
                //         {
                //             -1.0f,
                //             "NO_IMAGE",
                //             false,
                //             AppKit::ui::DialogTextModeType_CharAppear,
                //             "ending...",
                //         },
                //     },
                //     [&]()
                //     {
                //         // Dialog ended callback
                //         screenManager->open_screen("ScreenMain");
                //     });
            }
            else if (option == "Options")
            {
                screenManager->screen<AppKit::ui::ScreenOptions>()->showOptions(
                    [&](AppKit::ui::OptionsManager *localOptions)
                    {
                        // AppOptions::OptionsManager *appOptions = AppOptions::OptionsManager::Instance();
                        // app->appOptions->applyOptionsFrom(localOptions);

                        bool needs_save = localOptions->hasAnyChange();
                        bool needsRestart = false;
                        if (needs_save)
                        {
                            needsRestart = localOptions->hasChanged("Video", "Resolution") ||
                                           localOptions->hasChanged("Video", "WindowMode") ||
                                           localOptions->hasChanged("Video", "AntiAliasing") ||
                                           localOptions->hasChanged("Extra", "ButtonAppearance");

                            AppKit::ui::OptionsManager localOptionsCopy = *localOptions;

                            screenManager->screen<AppKit::ui::ScreenMessageBox>()->showMessageBox( //
                                "Do you want to apply the new settings?",
                                {"Apply", "Discard"}, // options
                                "Apply",              // init selected
                                [this, localOptionsCopy, needsRestart](const std::string &option)
                                {
                                    if (option == "Apply")
                                    {
                                        *AppKit::ui::OptionsManager::Instance() = localOptionsCopy;
                                        if (needsRestart)
                                            app->executeOnMainThread.enqueue([this]()
                                                                             { apply_settings_to_window(&MainScene::comes_from_app_recreation); });
                                        else
                                            app->executeOnMainThread.enqueue([this]()
                                                                             {
                                                                         save_options();

                                                                         // apply global settings...
                                                                         app->applySettingsChanges();

                                                                         screenManager->open_screen("ScreenMain"); });
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

    // renderWindow->inputManager.onWindowEvent.add(&MainScene::window_event, this);
    //  renderWindow->CameraViewport.OnChange.add(EventCore::CallbackWrapper(&MainScene::resize, this));
    //  renderWindow->WindowViewport.forceTriggerOnChange();
    //  renderWindow->CameraViewport.forceTriggerOnChange();

    // auto windowSize = app->window->getSize();
    // onWindowResized(MathCore::vec2i(windowSize.x, windowSize.y));

    renderWindow->inputManager.onKeyboardEvent.add([&](const AppKit::Window::KeyboardEvent &evt)
                                                   {
        if (evt.type == AppKit::Window::KeyboardEventType::KeyPressed){
            if (evt.code == AppKit::Window::Devices::KeyCode::A || evt.code == AppKit::Window::Devices::KeyCode::Left)
                uiCommands.enqueue(AppKit::ui::UIEvent_InputLeft);
            else if (evt.code == AppKit::Window::Devices::KeyCode::D || evt.code == AppKit::Window::Devices::KeyCode::Right)
                uiCommands.enqueue(AppKit::ui::UIEvent_InputRight);
            else if (evt.code == AppKit::Window::Devices::KeyCode::W || evt.code == AppKit::Window::Devices::KeyCode::Up)
                uiCommands.enqueue(AppKit::ui::UIEvent_InputUp);
            else if (evt.code == AppKit::Window::Devices::KeyCode::S || evt.code == AppKit::Window::Devices::KeyCode::Down)
                uiCommands.enqueue(AppKit::ui::UIEvent_InputDown);
            else if (evt.code == AppKit::Window::Devices::KeyCode::Q || evt.code == AppKit::Window::Devices::KeyCode::LShift || evt.code == AppKit::Window::Devices::KeyCode::LControl || evt.code == AppKit::Window::Devices::KeyCode::LBracket)
                uiCommands.enqueue(AppKit::ui::UIEvent_InputShoulderLeft);
            else if (evt.code == AppKit::Window::Devices::KeyCode::E || evt.code == AppKit::Window::Devices::KeyCode::RShift || evt.code == AppKit::Window::Devices::KeyCode::RControl || evt.code == AppKit::Window::Devices::KeyCode::RBracket)
                uiCommands.enqueue(AppKit::ui::UIEvent_InputShoulderRight);
            else if (evt.code == AppKit::Window::Devices::KeyCode::Enter)
                uiCommands.enqueue(AppKit::ui::UIEvent_InputActionEnter);
            else if (evt.code == AppKit::Window::Devices::KeyCode::Escape)
                uiCommands.enqueue(AppKit::ui::UIEvent_InputActionBack);
        } });

    this->OnUpdate.add(&MainScene::update, this);
}

// clear all loaded scene
void MainScene::unloadAll()
{
    // renderWindow->inputManager.onWindowEvent.remove(&MainScene::window_event, this);
    //  renderWindow->CameraViewport.OnChange.remove(EventCore::CallbackWrapper(&MainScene::resize, this));
    this->OnUpdate.remove(&MainScene::update, this);

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
    bool HUD_visible = screenManager->screenIsVisible<AppKit::ui::ScreenHUD>();
    while (uiCommands.size() > 0)
    {
        auto command = uiCommands.dequeue(nullptr, true);

        screenManager->triggerEvent(command);

        if (HUD_visible)
        {
            if (command == AppKit::ui::UIEvent_InputActionEnter)
            {
                screenManager->screen<AppKit::ui::ScreenHUD>()->inGameDialog.pressContinue();
            }
            else if (command == AppKit::ui::UIEvent_InputActionBack)
            {
                // Hide HUD on back
                screenManager->screen<AppKit::ui::ScreenHUD>()->inGameDialog.hideDialog(
                    AppKit::ui::DialogAppearModeType_Scroll,
                    [&]()
                    {
                        app->freeGameScene();
                        screenManager->open_screen("ScreenMain");
                    });
            }
        }
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
    renderPipeline->runSinglePassPipeline(resourceMap, root, camera, false, OrthographicFilter_UsingAABB, &app->threadPool);
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void MainScene::onWindowResized(const MathCore::vec2i &new_size)
{

    MathCore::vec2i target_size;

    MathCore::vec2i ui_screen_size = MathCore::vec2i(1920, 1080);

    AppKit::ui::OptionsManager *options = AppKit::ui::OptionsManager::Instance();
    {
        const std::string &uiSize = options->getGroupValueSelectedForKey("Extra", "UiSize");
        printf("UI Size Selected: %s\n", uiSize.c_str());
        if (uiSize == "Extra Small")
            ui_screen_size.x = 3840;
        else if (uiSize == "Small")
            ui_screen_size.x = 2560;
        else if (uiSize == "Medium")
            ui_screen_size.x = 1920;
        else if (uiSize == "Large")
            ui_screen_size.x = 1280;
        else if (uiSize == "Extra Large")
            ui_screen_size.x = 1024;

        const std::string &videoAspect = options->getGroupValueSelectedForKey("Video", "Aspect");

        float target_aspect = 16.0f / 9.0f;

        if (videoAspect == "16:9")
        {
            target_aspect = 16.0f / 9.0f;
            ui_screen_size.y = (9 * ui_screen_size.x) / 16;
        }
        else if (videoAspect == "16:10")
        {
            target_aspect = 16.0f / 10.0f;
            ui_screen_size.y = (10 * ui_screen_size.x) / 16;
        }

        printf("UI Size Resolution: %ix%i\n", ui_screen_size.x, ui_screen_size.y);

        float window_aspect = (float)new_size.width / (float)new_size.height;

        if (target_aspect >= window_aspect)
        {
            // fit width
            target_size.x = new_size.x;
            target_size.y = (int)((float)new_size.x / target_aspect + 0.5f);

            renderWindow->viewportScaleFactor = (float)target_size.x / (float)ui_screen_size.x;
        }
        else
        {
            // fit height
            target_size.y = new_size.y;
            target_size.x = (int)((float)new_size.y * target_aspect + 0.5f);

            renderWindow->viewportScaleFactor = (float)target_size.y / (float)ui_screen_size.y;
        }
    }

    MathCore::vec2i centering_aux = (new_size - target_size) / 2;
    auto new_viewport = AppKit::GLEngine::iRect(centering_aux.x, centering_aux.y, target_size.x, target_size.y);
    if (renderWindow->WindowViewport != new_viewport)
        renderWindow->WindowViewport = new_viewport;
    else
        renderWindow->WindowViewport.forceTriggerOnChange();

    auto cameraViewport = renderWindow->CameraViewport.c_val();
    screenManager->resize(MathCore::vec2i(cameraViewport.w, cameraViewport.h));

    auto windowViewport = renderWindow->WindowViewport.c_val();

    GLRenderState *renderState = GLRenderState::Instance();
    renderState->Viewport = AppKit::GLEngine::iRect(
        windowViewport.x,
        new_size.height - 1 - (windowViewport.h - 1 + windowViewport.y),
        windowViewport.w,
        windowViewport.h);
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
    MainScene::currentInstance = this;
}

MainScene::~MainScene()
{
    unload();
    MainScene::currentInstance = nullptr;
}

void MainScene::comes_from_app_recreation()
{
    MainScene::currentInstance->screenManager->screen<AppKit::ui::ScreenMessageBox>()->showMessageBox( //
        "Keep this settings?",
        {"Keep", "Roll Back"}, // options
        "Keep",                // init selected
        [](const std::string &option)
        {
            if (option == "Keep")
            {
                save_options();
                MainScene::currentInstance->screenManager->pop_screen();
            }
            else
            {
                load_options();
                apply_settings_to_window(nullptr);
            }
        });
}