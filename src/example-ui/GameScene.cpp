#include "GameScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include "components/ComponentCameraToPlayer.h"
#include "components/ComponentPlayer.h"

#include <InteractiveToolkit/ITKCommon/Random.h>
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-engine/Components/Core/ComponentLineMounter.h>

#include "stage-generator/StageGenerator.h"

using namespace AppKit::Physics::Container;
using namespace AppKit::Physics::Core;

namespace Debug
{
    ComponentLineMounter *lineMounter = nullptr;
}

void GameScene::generateRandomStage()
{
    StageGen::StageParams params;
    params.normal_jump_height = 300.0f;
    params.double_jump_height = 200.0f;
    params.player_radius = 50.0f;
    params.stage_length_screens = 5;

    stageResult = StageGen::StageGenerator::generate(*container2D, params, random32);
    // using namespace MathCore;
    // using namespace SimplePhysics;

    // stageResult.start_point = vec2f(0.0f, 0.0f);

    // vec2f screen_size = vec2f(1920.0f, 1080.0f);
    // vec2f screen_size_2 = screen_size * 0.5f;

    // container2D->setGameArea(
    //     Box2D(vec2f(-screen_size_2.x, -screen_size_2.y),
    //           vec2f(screen_size_2.x, screen_size_2.y)));

    // container2D->static_structures.push_back(Structure2D::FromSegment(
    //     "wall", 0.5f,
    //     Segment2D(
    //         vec2f(-screen_size_2.x, screen_size_2.y),
    //         vec2f(-params.player_radius * 0.5f, -screen_size_2.y * 0.5f))));

    // container2D->static_structures.push_back(Structure2D::FromSegment(
    //     "wall", 0.5f,
    //     Segment2D(
    //         vec2f(-params.player_radius * 0.5f, -screen_size_2.y * 0.5f),
    //         vec2f(-params.player_radius * 0.5f + 100.0f, -screen_size_2.y * 0.5f))));

    // container2D->static_structures.push_back(Structure2D::FromSegment(
    //     "wall", 0.5f,
    //     Segment2D(
    //         vec2f(params.player_radius * 2.0f, screen_size_2.y),
    //         vec2f(params.player_radius * 0.5f, -screen_size_2.y * 0.5f)
    //     )));
}
// to load skybox, textures, cubemaps, 3DModels and setup materials
void GameScene::loadResources()
{
    using namespace ITKCommon;
    using namespace MathCore;

    container2D = STL_Tools::make_unique<Container2D>();

    generateRandomStage();

    container2D->buildStaticQuadtree(
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
        using namespace MathCore;
        // draw lines drawing of the game area box and static structures
        {
            // draw debug static structures
            auto debugDrawTransform = game_area_transform->addChild(Transform::CreateShared("Static Structures Debug Draw"));
            // debugDrawTransform->setLocalPosition(MathCore::vec3f(0,0,-1.0f));

            std::shared_ptr<ComponentLineMounter> line_mounter = debugDrawTransform->addNewComponent<ComponentLineMounter>();
            line_mounter->setCamera(&app->resourceMap, app->gameScene->getCamera(), true);

            auto points = container2D->game_area.getBoxPoints();

            for (size_t i = 0; i < points.size(); i++)
            {
                auto a = points[i];
                auto b = points[(i + 1) % points.size()];

                line_mounter->addLine(
                    MathCore::vec3f(a, 0),        // a
                    MathCore::vec3f(b, 0),        // b
                    3.0f,                         // thickness
                    AppKit::ui::colorFromHex("#00FF00FF") // color
                );
            }

            for (const auto &structure : container2D->getStaticStructures())
            {
                if (structure->type == StructureType::Segment)
                {
                    for (const auto &segment : structure->segments)
                    {
                        line_mounter->addLine(
                            MathCore::vec3f(segment.a, -1.0f),  // a
                            MathCore::vec3f(segment.b, -1.0f),  // b
                            3.0f,                               // thickness
                            (structure->pass_through_set)        //
                                ? AppKit::ui::colorFromHex("#0000FFFF") // color for pass-through segments
                                : AppKit::ui::colorFromHex("#FF0000FF") // color
                        );
                    }
                    if (structure->pass_through_set)
                    {
                        vec2f center = (structure->segments[0].a + structure->segments[0].b) * 0.5f;
                        // also draw the pass-through activation line
                        float dst =  Line2D::pointDistanceToLine(center, structure->pass_through_activate_line);
                        line_mounter->addCircle(
                            MathCore::vec3f(center + structure->pass_through_activate_line.normal * (50.0f - dst), -1.0f),
                            50.0f,                        // radius
                            3.0f,                         // thickness
                            AppKit::ui::colorFromHex("#ffff00FF") // color for pass-through segments
                        );

                        dst = Line2D::pointDistanceToLine(center, structure->pass_through_deactivate_line);
                        line_mounter->addCircle(
                            MathCore::vec3f(center + structure->pass_through_deactivate_line.normal * (0.0f - dst), -1.0f),
                            50.0f,                        // radius
                            3.0f,                         // thickness
                            AppKit::ui::colorFromHex("#ffff00FF") // color for pass-through segments
                        );
                    }
                }
            }
        }

        {
            auto auxDraw = game_area_transform->addChild(Transform::CreateShared("auxDraw"));
            auxDraw->setLocalPosition(MathCore::vec3f(0, 0, -100.0f));
            std::shared_ptr<ComponentLineMounter> line_mounter = auxDraw->addNewComponent<ComponentLineMounter>();
            line_mounter->setCamera(&app->resourceMap, app->gameScene->getCamera(), true);

            Debug::lineMounter = line_mounter.get();

            // // draw the quadtree nodes for debug
            // std::function<void(const SimplePhysics::QuadtreeNode *, int)> drawQuadtreeNode = [&](const SimplePhysics::QuadtreeNode *node, int depth)
            // {
            //     if (!node)
            //         return;
            //     const float draw_depth = -100.0f;
            //     auto points = node->box.getBoxPoints();
            //     for (size_t i = 0; i < points.size(); i++)
            //     {
            //         auto a = points[i];
            //         auto b = points[(i + 1) % points.size()];

            //         line_mounter->addLine(
            //             MathCore::vec3f(a, draw_depth), // a
            //             MathCore::vec3f(b, draw_depth), // b
            //             3.0f,                           // thickness
            //             ui::colorFromHex("#0000FFFF")   // color
            //         );
            //     }
            //     for (const auto &child : node->children)
            //         drawQuadtreeNode(child.get(), depth + 1);
            // };

            // drawQuadtreeNode(physicsContainer->static_quadtree->getRoot(), 0);
        }

        // draw filled quad on game area box
        {
            auto box_center = container2D->game_area.getCenter();
            auto box_size = container2D->game_area.getSize();

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
                AppKit::ui::colorFromHex("#c8e8c8FF"), // color
                MathCore::vec4f(0, 0, 0, 0),   // radius
                StrokeModeGrowOutside,         // stroke mode
                0.0f,                          // stroke thickness
                MathCore::vec4f(0, 0, 0, 0),   // stroke color
                0.0f,                          // drop shadow thickness
                MathCore::vec4f(0, 0, 0, 0),   // drop shadow color
                MeshUploadMode_Direct,         // mesh upload mode
                4);                            // segment count

            for (const auto &structure : container2D->getStaticStructures())
            {
                if (structure->type == StructureType::Box)
                {
                    auto box_center = structure->box.getCenter();
                    auto box_size = structure->box.getSize();

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
                            box_size.y),                     // size
                        (structure->pass_through_set)         //
                            ? AppKit::ui::colorFromHex("#0000ffff")  // color for pass-through structures
                            : AppKit::ui::colorFromHex("#FF0000FF"), // color
                        MathCore::vec4f(0, 0, 0, 0),         // radius
                        StrokeModeGrowOutside,               // stroke mode
                        0.0f,                                // stroke thickness
                        MathCore::vec4f(0, 0, 0, 0),         // stroke color
                        0.0f,                                // drop shadow thickness
                        MathCore::vec4f(0, 0, 0, 0),         // drop shadow color
                        MeshUploadMode_Direct,               // mesh upload mode
                        4);                                  // segment count
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

    // auto gameArea = root->findTransformByName("Game Area");
    // auto componentGameArea = gameArea->addNewComponent<ComponentGameArea>();
    // {
    //     auto ga_center = physicsContainer->game_area.getCenter();
    //     auto ga_size = physicsContainer->game_area.getSize();
    //     componentGameArea->debugDrawEnabled = true;
    //     componentGameArea->debugDrawColor = ui::colorFromHex("#00FF00FF");
    //     componentGameArea->StageArea = CollisionCore::AABB<MathCore::vec3f>(
    //         MathCore::vec3f(0.0f, 0.0f, 0.0f),
    //         MathCore::vec3f(ga_size.x, ga_size.y, 0.0f));
    //     componentGameArea->LockCameraMove = false;
    //     componentGameArea->app = app;
    // }

    auto player_0 = root->findTransformByName("Player 0");
    auto componentPlayer = player_0->addNewComponent<ComponentPlayer>();
    {
        componentPlayer->debugDrawEnabled = true;
        componentPlayer->debugDrawThickness = 2.0f;
        componentPlayer->debugDrawColor = AppKit::ui::colorFromHex("#0000ffFF");
        componentPlayer->Radius = 50.0f;
        float factor = 5.0f;
        componentPlayer->RadiusGrounded = 50.0f - factor;
        componentPlayer->OffsetGrounded = 7.0f + factor;
        componentPlayer->app = app;
        // componentPlayer->gameArea = componentGameArea;
        player_0->setLocalPosition(MathCore::vec3f(stageResult.start_point.x, stageResult.start_point.y, 0.0f));
    }

    auto componentCameraToPlayer = player_0->addNewComponent<ComponentCameraToPlayer>();
    {
        componentCameraToPlayer->app = app;
        componentCameraToPlayer->camera = componentCameraOrthographic;
        componentCameraToPlayer->player = componentPlayer;
        // componentCameraToPlayer->LockCameraMove = false;
    }
}

// clear all loaded scene
void GameScene::unloadAll()
{
    root = nullptr;
    camera = nullptr;
    container2D.reset();
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
    this->OnUpdate.add(&GameScene::update, this);
    this->app = app;
    GameScene::currentInstance = this;
}

GameScene::~GameScene()
{
    unload();
    GameScene::currentInstance = nullptr;
    this->OnUpdate.remove(&GameScene::update, this);
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
    if (elapsed->deltaTime == 0.0f)
        return;

    auto line_mounter = root->findTransformByName("auxDraw")->findComponent<ComponentLineMounter>();
    if (line_mounter)
    {
        using namespace MathCore;

        line_mounter->clear();

        // auto player_0 = root->findTransformByName("Player 0");

        // auto length = 100.0f;

        // static float rot = 0;
        // rot = OP<float>::fmod(rot + elapsed->deltaTime * 0.5f, 2.0f * CONSTANT<float>::PI);

        // auto pos = CVT<vec3f>::toVec2(player_0->getLocalPosition());
        // // auto pos_b = pos + vec2f(OP<float>::cos(rot), OP<float>::sin(rot)) * length;

        // auto segment = vec2f(OP<float>::cos(rot), OP<float>::sin(rot)) * length;

        // auto pos_c = pos + vec2f(300.0f, 0.0f);

        // auto pos_b = pos_c;

        // auto line_a = pos_c - segment * 0.5f;
        // auto line_b = pos_c + segment * 0.5f;

        // float radius = 30.0f;

        // line_mounter->addLine(
        //     vec3f(pos, -1.0f),            // a
        //     vec3f(pos_b, -1.0f),          // b
        //     3.0f,                         // thickness
        //     ui::colorFromHex("#0000ffFF") // color
        // );

        // line_mounter->addLine(
        //     vec3f(line_a, -1.0f),         // a
        //     vec3f(line_b, -1.0f),         // b
        //     3.0f,                         // thickness
        //     ui::colorFromHex("#ff00ffFF") // color
        // );

        // vec2f out_dir;
        // float t = SimplePhysics::Segment2D::circleCastIntersectsSegment(
        //     pos, pos_b,
        //     radius,
        //     line_a, line_b,
        //     &out_dir);

        // vec2f circle_pos = OP<vec2f>::lerp(pos, pos_b, t);

        // line_mounter->addCircle(
        //     vec3f(circle_pos, -1.0f),     // a
        //     radius,                       // b
        //     3.0f,                         // thickness
        //     ui::colorFromHex("#0000ffFF") // color
        // );

        // if (t < 1.0f)
        // {
        //     vec2f pt_in_line = SimplePhysics::Segment2D::closestPointToSegment(circle_pos, line_a, line_b);

        //     line_mounter->addLine(
        //         vec3f(pt_in_line, -1.0f),                    // a
        //         vec3f(pt_in_line + out_dir * length, -1.0f), // b
        //         5.0f,                                        // thickness
        //         ui::colorFromHex("#00ff00FF")                // color
        //     );
        // }

        // // for (const auto &v : Debug::dir)
        // // {
        // //     line_mounter->addLine(
        // //         vec3f(pos, -1.0f),            // a
        // //         vec3f(pos + v, -1.0f),        // b
        // //         5.0f,                         // thickness
        // //         ui::colorFromHex("#00ff00FF") // color
        // //     );
        // // }

        // // for (int i = 0; i < (int)Debug::lines.size(); i += 2)
        // // {
        // //     line_mounter->addLine(
        // //         vec3f(Debug::lines[i], -1.0f),     // a
        // //         vec3f(Debug::lines[i + 1], -1.0f), // b
        // //         4.0f,                              // thickness
        // //         ui::colorFromHex("#ffff00FF")      // color
        // //     );
        // // }
    }
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
