#include "GameScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include "components/ComponentGameArea.h"
#include "components/ComponentPlayer.h"

// to load skybox, textures, cubemaps, 3DModels and setup materials
void GameScene::loadResources()
{
}
// to load the scene graph
void GameScene::loadGraph()
{
    root = Transform::CreateShared("Game Scene")->setRootPropertiesFromDefaultScene(this->self());
    // root->affectComponentStart = true;
    // root->setRenderWindowRegion(renderWindow);
    // root->setEventHandlerSet(this->self());

    auto main_camera = root->addChild(Transform::CreateShared("Main Camera"));

    auto game_area = root->addChild(Transform::CreateShared("Game Area"));

    auto player_0 = game_area->addChild(Transform::CreateShared("Player 0"));
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

    const auto &camera_viewport = renderWindow->WindowViewport.c_val();

    screen_custom_size.x = 1920.0f;
    screen_custom_size.y = (screen_custom_size.x * camera_viewport.h) / camera_viewport.w;

    componentCameraOrthographic->sizeX = screen_custom_size.x;
    componentCameraOrthographic->sizeY = screen_custom_size.y;

    componentCameraOrthographic->useSizeX = true;
    componentCameraOrthographic->useSizeY = true;

    auto gameArea = root->findTransformByName("Game Area");
    auto componentGameArea = gameArea->addNewComponent<ComponentGameArea>();
    {
        componentGameArea->debugDrawEnabled = true;
        componentGameArea->debugDrawColor = ui::colorFromHex("#00FF00FF");
        componentGameArea->StageArea = CollisionCore::AABB<MathCore::vec3f>(
            MathCore::vec3f(0.0f, 0.0f, 0.0f),
            MathCore::vec3f(600.0f, 300.0f, 0.0f));
        componentGameArea->LockCameraMove = false;
        componentGameArea->app = app;
    }

    auto player_0 = root->findTransformByName("Player 0");
    auto componentPlayer = player_0->addNewComponent<ComponentPlayer>();
    {
        componentPlayer->debugDrawEnabled = true;
        componentPlayer->debugDrawThickness = 5.0f;
        componentPlayer->debugDrawColor = ui::colorFromHex("#0000ffFF");
        componentPlayer->Radius = 50.0f;
        componentPlayer->app = app;
        componentPlayer->gameArea = componentGameArea;
        player_0->setLocalPosition(MathCore::vec3f(componentPlayer->Radius.c_val(), componentPlayer->Radius.c_val(), 0.0f));
    }
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
    const auto &camera_viewport = renderWindow->WindowViewport.c_val();

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

void GameScene::printHierarchy()
{
    struct __internal_struct
    {
        AppKit::GLEngine::Transform *node;
        int depth;
    };
    std::vector<__internal_struct> nodes;
    nodes.push_back({root.get(), 0});

    while (nodes.size() > 0)
    {
        auto node_struct = nodes.back();
        nodes.pop_back();

        printf("%*s[%s]%s\n", node_struct.depth * 4, "+", node_struct.node->skip_traversing ? "skip" : "draw", node_struct.node->getName().c_str());

        for (auto &child : STL_Tools::Reversal(node_struct.node->getChildren()))
            nodes.push_back({child.get(), node_struct.depth + 1});
    }
}
