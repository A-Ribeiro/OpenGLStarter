#include "GameScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

PlayerInputState::PlayerInputState(RawInputFromDevice input_device)
{
    this->input_device = input_device;
    state = InputState_None;
}

// to load skybox, textures, cubemaps, 3DModels and setup materials
void GameScene::loadResources()
{
}
// to load the scene graph
void GameScene::loadGraph()
{
    root = Transform::CreateShared();
    root->affectComponentStart = true;

    auto main_camera = root->addChild(Transform::CreateShared("Main Camera"));
}
// to bind the resources to the current graph
void GameScene::bindResourcesToGraph()
{
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    GLRenderState *renderState = GLRenderState::Instance();

    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    {
        auto mainCamera = root->findTransformByName("Main Camera");
        camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
    }

    const auto &camera_viewport = renderWindow->CameraViewport.c_val();

    screen_custom_size.x = 1920.0f;
    screen_custom_size.y = (screen_custom_size.x * camera_viewport.h) / camera_viewport.w;

    componentCameraOrthographic->sizeX = screen_custom_size.x;
    componentCameraOrthographic->sizeY = screen_custom_size.y;

    componentCameraOrthographic->useSizeX = true;
    componentCameraOrthographic->useSizeY = true;

}

// clear all loaded scene
void GameScene::unloadAll()
{
    root = nullptr;
    camera = nullptr;
}

GameScene *GameScene::currentInstance = nullptr;

GameScene::GameScene(
    App *app,
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow),
                                                                          random32(ITKCommon::RandomDefinition<uint32_t>::randomSeed()),
                                                                          mathRandom(&random32)
{
    this->app = app;
    GameScene::currentInstance = this;
}

GameScene::~GameScene()
{
    unload();
    GameScene::currentInstance = nullptr;
}

void GameScene::draw()
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

void GameScene::onCameraViewportUpdate(const MathCore::vec2i &viewport_size)
{
    const auto &camera_viewport = renderWindow->CameraViewport.c_val();

    screen_custom_size.x = 1920.0f;
    screen_custom_size.y = (screen_custom_size.x * camera_viewport.h) / camera_viewport.w;

    auto componentCameraOrthographic = std::dynamic_pointer_cast<ComponentCameraOrthographic>(camera);

    componentCameraOrthographic->sizeX = screen_custom_size.x;
    componentCameraOrthographic->sizeY = screen_custom_size.y;

    componentCameraOrthographic->useSizeX = true;
    componentCameraOrthographic->useSizeY = true;
}

void GameScene::update(Platform::Time *elapsed)
{
}
