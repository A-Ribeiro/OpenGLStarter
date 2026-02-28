#include "GameScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include "components/ComponentGameArea.h"
#include "components/ComponentPlayer.h"
#include "simple-physics/PhysicsContainer.h"

#include <InteractiveToolkit/ITKCommon/Random.h>
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-engine/Components/Core/ComponentLineMounter.h>

void GameScene::generateRandomStage()
{
    using namespace ITKCommon;
    using namespace MathCore;
    
    auto game_area_box = SimplePhysics::Box2D(MathCore::vec2f(0.0f, 0.0f), MathCore::vec2f(1024.0f, 768.0f));
    physicsContainer->setGameArea(game_area_box);

    MathRandomExt<Random> mathRnd(Random::Instance());

    for (int i = 0; i < 10; i++)
    {

        vec2f pos_rnd = mathRnd.nextRange<vec2f>(game_area_box.min, game_area_box.max);

        float size_half = mathRnd.nextRange<float>(20.0f, 100.0f);
        vec2f segment_a = pos_rnd - vec2f(size_half, 0);
        vec2f segment_b = pos_rnd + vec2f(size_half, 0);

        auto structure = SimplePhysics::Structure2D::FromSegment(
            "Test Segment",
            0.5f,
            SimplePhysics::Segment2D(segment_a, segment_b));

        physicsContainer->static_structures.push_back(structure);
    }

    auto structure = SimplePhysics::Structure2D::FromBoxCenterSize(
        "Test Box",
        0.5f,
        vec2f(0, 0),    // center
        vec2f(100, 100) // size
    );

    physicsContainer->static_structures.push_back(structure);
}
// to load skybox, textures, cubemaps, 3DModels and setup materials
void GameScene::loadResources()
{
    using namespace ITKCommon;
    using namespace MathCore;

    physicsContainer = STL_Tools::make_unique<SimplePhysics::PhysicsContainer>();

    generateRandomStage();

    physicsContainer->buildStaticQuadtree(
        8,   // maxDepth_
        16); // minPointThresholdToSubdivide_
}
// to load the scene graph
void GameScene::loadGraph()
{
    root = Transform::CreateShared("Game Scene")->setRootPropertiesFromDefaultScene(this->self());
    // root->affectComponentStart = true;
    // root->setRenderWindowRegion(renderWindow);
    // root->setEventHandlerSet(this->self());

    auto main_camera = root->addChild(Transform::CreateShared("Main Camera"));

    auto game_area_transform = root->addChild(Transform::CreateShared("Game Area"));

    auto player_0 = game_area_transform->addChild(Transform::CreateShared("Player 0"));

    {
        using namespace AppKit::GLEngine::Components;
        // draw lines drawing of the game area box and static structures
        {
            // draw debug static structures
            auto debugDrawTransform = game_area_transform->addChild(Transform::CreateShared("Static Structures Debug Draw"));
            // debugDrawTransform->setLocalPosition(MathCore::vec3f(0,0,-1.0f));

            std::shared_ptr<ComponentLineMounter> line_mounter = debugDrawTransform->addNewComponent<ComponentLineMounter>();
            line_mounter->setCamera(&app->resourceMap, app->gameScene->getCamera(), true);

            auto points = physicsContainer->game_area.getBoxPoints();

            for (size_t i = 0; i < points.size(); i++)
            {
                auto a = points[i];
                auto b = points[(i + 1) % points.size()];

                line_mounter->addLine(
                    MathCore::vec3f(a, 0),        // a
                    MathCore::vec3f(b, 0),        // b
                    3.0f,                         // thickness
                    ui::colorFromHex("#00FF00FF") // color
                );
            }

            for (const auto &structure : physicsContainer->static_structures)
            {
                if (structure.type == SimplePhysics::StructureType::Segment)
                {
                    for (const auto &segment : structure.segments)
                    {
                        line_mounter->addLine(
                            MathCore::vec3f(segment.a, -1.0f), // a
                            MathCore::vec3f(segment.b, -1.0f), // b
                            3.0f,                              // thickness
                            ui::colorFromHex("#FF0000FF")      // color
                        );
                    }
                }
            }
        }

        // draw filled quad on game area box
        {
            auto box_center = physicsContainer->game_area.getCenter();
            auto box_size = physicsContainer->game_area.getSize();

            auto box_to_draw_transform = game_area_transform->addChild(Transform::CreateShared("DebugDrawAABB"));
            box_to_draw_transform->setLocalPosition(MathCore::vec3f(
                box_center.x,
                box_center.y,
                100.0f));
            auto rect = box_to_draw_transform->addNewComponent<ComponentRectangle>();
            rect->setQuad(
                &app->resourceMap, // use app's resource map
                MathCore::vec2f(
                    box_size.x,
                    box_size.y),               // size
                ui::colorFromHex("#c8e8c8FF"), // color
                MathCore::vec4f(0, 0, 0, 0),   // radius
                StrokeModeGrowOutside,         // stroke mode
                0.0f,                          // stroke thickness
                MathCore::vec4f(0, 0, 0, 0),   // stroke color
                0.0f,                          // drop shadow thickness
                MathCore::vec4f(0, 0, 0, 0),   // drop shadow color
                MeshUploadMode_Direct,         // mesh upload mode
                4);                            // segment count

            for (const auto &structure : physicsContainer->static_structures)
            {
                if (structure.type == SimplePhysics::StructureType::Box)
                {
                    auto box_center = structure.box.getCenter();
                    auto box_size = structure.box.getSize();

                    box_to_draw_transform = game_area_transform->addChild(Transform::CreateShared("DebugDrawAABB"));
                    box_to_draw_transform->setLocalPosition(MathCore::vec3f(
                        box_center.x,
                        box_center.y,
                        50.0f));
                    auto rect = box_to_draw_transform->addNewComponent<ComponentRectangle>();
                    rect->setQuad(
                        &app->resourceMap, // use app's resource map
                        MathCore::vec2f(
                            box_size.x,
                            box_size.y),               // size
                        ui::colorFromHex("#0000003f"), // color
                        MathCore::vec4f(0, 0, 0, 0),   // radius
                        StrokeModeGrowOutside,         // stroke mode
                        0.0f,                          // stroke thickness
                        MathCore::vec4f(0, 0, 0, 0),   // stroke color
                        0.0f,                          // drop shadow thickness
                        MathCore::vec4f(0, 0, 0, 0),   // drop shadow color
                        MeshUploadMode_Direct,         // mesh upload mode
                        4);                            // segment count
                }
            }
        }
    }
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
    physicsContainer.reset();
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
