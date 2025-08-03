#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>
#include "components/ComponentGrow.h"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources()
{
    // auto engine = AppKit::GLEngine::Engine::Instance();
    spriteShader = std::make_shared<SpriteShader>();
}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->affectComponentStart = true;

    auto main_camera = root->addChild(Transform::CreateShared());
    main_camera->Name = "Main Camera";

    // root->addChild(Transform::CreateShared())->Name = "Smoke";

    root->addChild(Transform::CreateShared())->Name = "bg";

    main_camera->addChild(Transform::CreateShared())->Name = "ui";
}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    GLRenderState *renderState = GLRenderState::Instance();

    {
        spriteNode = Transform::CreateShared("smoke");
        spriteNode->setLocalScale(MathCore::vec3f(0));
        auto componentSprite = spriteNode->addNewComponent<ComponentSprite>();
        componentSprite->setTexture(
            resourceMap, spriteShader,
            resourceMap->getTexture("resources/smoke.png", engine->sRGBCapable),
            MathCore::vec2f(0.5f, 0.5f),             // pivot
            MathCore::vec4f(1.0f, 1.0f, 1.0f, 0.4f), // color
            MathCore::vec2f(-1, 256),                // size constraint
            MeshUploadMode_Direct                    // direct draw, causes better performance on old hardware
        );
        auto componentGrow = spriteNode->addNewComponent<ComponentGrow>();
        componentGrow->app = app;
        componentGrow->transformPool = &transformPool;
        componentSprite->mesh->always_clone = true;
    }

    {
        logoNode = Transform::CreateShared("logo");
        logoNode->setLocalScale(MathCore::vec3f(0));
        auto componentSprite = logoNode->addNewComponent<ComponentSprite>();
        componentSprite->setTexture(
            resourceMap, spriteShader,
            resourceMap->getTexture("resources/opengl_logo_white.png", engine->sRGBCapable),
            MathCore::vec2f(0.5f, 0.5f),             // pivot
            MathCore::vec4f(1.0f, 1.0f, 1.0f, 0.4f), // color
            MathCore::vec2f(-1, 256),                // size constraint
            MeshUploadMode_Direct                    // direct draw, causes better performance on old hardware
        );
        auto componentGrow = logoNode->addNewComponent<ComponentGrow>();
        componentGrow->app = app;
        componentGrow->transformPool = &transformPool;
        componentSprite->mesh->always_clone = true;
    }

    // componentSprite->meshWrapper->debugCollisionShapes = true;
    // spriteNode->skip_traversing = true;
    // componentSprite->material->custom_shader_property_bag.getProperty("UseDiscard").set(true);

    bgNode = root->findTransformByName("bg");
    bgNode->setLocalPosition(MathCore::vec3f(0, 0, 10));
    bgComponentSprite = bgNode->addNewComponent<ComponentSprite>();
    bgComponentSprite->setTexture(
        resourceMap, spriteShader,
        resourceMap->getTexture("resources/Skyboxes/SanFrancisco4/posy.jpg", engine->sRGBCapable),
        MathCore::vec2f(0.5f, 0.5f),             // pivot
        MathCore::vec4f(1.0f, 1.0f, 1.0f, 1.0f), // color
        MathCore::vec2f(-1, 1024),               // size constraint
        MeshUploadMode_Static                    // static mesh
    );

    uiNode = root->findTransformByName("ui");
    // uiNode->setLocalPosition(MathCore::vec3f(0, 0, camera));

    uiComponent = uiNode->addNewComponent<ComponentUI>();
    uiComponent->Initialize(resourceMap, spriteShader);

    uiComponent->addRectangleCenterSize(
        vec2f(0, 0),                             // center
        vec2f(256, 128),                         // size
        MathCore::vec4f(0.0f, 0.0f, 1.0f, 0.4f), // color
        MathCore::vec4f(64.0f),                  // radius
        StrokeModeGrowInside,                    // stroke mode
        10.0f,                                   // stroke thickness
        MathCore::vec4f(0.0f, 0.0f, 0.8f, 0.6f), // stroke color
        80.0f,                                   // drop shadow thickness
        MathCore::vec4f(1.0f, 0.0f, 1.0f, 0.2f), // drop shadow color
        0                                        // z
    );

    uiComponent->addSprite(
        vec2f(0, 0),                             // pos
        "resources/opengl_logo_white.png",       // texture
        vec2f(0.5f),                             // pivot
        MathCore::vec4f(1.0f, 1.0f, 1.0f, 1.0f), // color
        MathCore::vec2f(256 - 16, -1),           // size constraint
        -1                                       // z
    );

    // setup renderstate

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();

    uiNode->setLocalPosition(MathCore::vec3f(0, 0, componentCameraOrthographic->nearPlane + 100.0f));

    auto rect = renderWindow->CameraViewport.c_ptr();
    resize(MathCore::vec2i(rect->w, rect->h));

    // Add AABB for all meshs...
    {
        resourceHelper->addAABBMesh(root);
    }

    renderWindow->inputManager.onKeyboardEvent.add([&](const AppKit::Window::KeyboardEvent &evt)
                                                   {
        if (evt.type == AppKit::Window::KeyboardEventType::KeyPressed &&
            evt.code == AppKit::Window::Devices::KeyCode::Space){
            auto new_element = root->addChild(spriteNode->clone(false));
            // new_element->addNewComponent<ComponentGrow>();
        } });

    renderWindow->OnUpdate.add(&MainScene::update, this);

    // initialize pool
    for (int i = 0; i < 75; i++)
    {
        transformPool.enqueue(spriteNode->clone(false));
        transformPool.enqueue(logoNode->clone(false));
    }
}

// clear all loaded scene
void MainScene::unloadAll()
{
    renderWindow->OnUpdate.remove(&MainScene::update, this);

    root = nullptr;
    camera = nullptr;

    spriteShader = nullptr;

    // componentSprite = nullptr;
    spriteNode = nullptr;
    logoNode = nullptr;

    bgComponentSprite = nullptr;
    bgNode = nullptr;

    while (transformPool.size() > 0)
        transformPool.dequeue(nullptr, true);

    uiComponent = nullptr;
    uiNode = nullptr;
}

void MainScene::update(Platform::Time *elapsed)
{
    static float s_angle = 0;
    s_angle = MathCore::OP<float>::fmod(s_angle + elapsed->deltaTime * 0.5f, MathCore::CONSTANT<float>::PI * 2.0f);

    camera->getTransform()->setLocalRotation(MathCore::GEN<MathCore::quatf>::fromAxisAngle(MathCore::vec3f(0, 0, 1), s_angle));

    randomNext -= elapsed->deltaTime;

    if (randomNext <= 0.0f)
    {
        randomNext = mathRandom.nextRange(0.1f, 0.3f);

        for (int i = 0; i < random32.getRange<int>(10, 50); i++)
        {
            if (transformPool.size() <= 0)
                break;
            auto new_element = root->addChild(transformPool.dequeue(nullptr, true));
            new_element->setLocalPosition(MathCore::vec3f(
                mathRandom.nextRange(-512.0f, 512.0f),
                mathRandom.nextRange(-512.0f, 512.0f),
                0));
            // new_element->addNewComponent<ComponentGrow>();
        }
    }
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

void MainScene::resize(const MathCore::vec2i &size)
{
    // fixed height of 1080 pixels
    // float new_scale = (float)size.height / 1080.0f;
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

    // spriteShader = nullptr;

    // //componentSprite = nullptr;
    // spriteNode = nullptr;
    // logoNode = nullptr;

    // bgComponentSprite = nullptr;
    // bgNode = nullptr;

    // uiComponent = nullptr;
    // uiNode = nullptr;

    randomNext = 0.0f;
}

MainScene::~MainScene()
{
    unload();
}
