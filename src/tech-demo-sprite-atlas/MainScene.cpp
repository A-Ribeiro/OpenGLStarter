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

    root->addChild(Transform::CreateShared())->Name = "Main Camera";

    // root->addChild(Transform::CreateShared())->Name = "Smoke";

    root->addChild(Transform::CreateShared())->Name = "bg";
}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    GLRenderState *renderState = GLRenderState::Instance();

    spriteNode = Transform::CreateShared("smoke");
    spriteNode->setLocalScale(MathCore::vec3f(0));
    componentSprite = spriteNode->addNewComponent<ComponentSprite>();
    componentSprite->setTexture(
        resourceMap, spriteShader,
        resourceMap->getTexture("resources/smoke.png", engine->sRGBCapable),
        MathCore::vec2f(0.5f, 0.5f),             // pivot
        MathCore::vec4f(1.0f, 1.0f, 1.0f, 0.4f), // color
        MathCore::vec2f(-1, 256),                // size constraint
        MeshUploadMode_Dynamic                   // dynamic mesh
    );
    spriteNode->addNewComponent<ComponentGrow>()->app = app;

    componentSprite->mesh->always_clone = true;

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

    // setup renderstate

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();

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
}

// clear all loaded scene
void MainScene::unloadAll()
{
    renderWindow->OnUpdate.remove(&MainScene::update, this);

    root = nullptr;
    camera = nullptr;

    spriteShader = nullptr;

    componentSprite = nullptr;
    spriteNode = nullptr;

    bgComponentSprite = nullptr;
    bgNode = nullptr;
}

void MainScene::update(Platform::Time *elapsed)
{
    camera->getTransform()->setLocalRotation(
        camera->getTransform()->getLocalRotation() *
        MathCore::GEN<MathCore::quatf>::fromAxisAngle(MathCore::vec3f(0, 0, 1), elapsed->deltaTime * 0.5f));

    randomNext -= elapsed->deltaTime;

    if (randomNext <= 0.0f)
    {
        randomNext = mathRandom.nextRange(0.1f, 0.3f);

        for (int i = 0; i < random32.getRange<int>(10, 50); i++)
        {
            auto new_element = root->addChild(spriteNode->clone(false));
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
    renderPipeline->runSinglePassPipeline(resourceMap,root, camera, true);
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
                                                                          mathRandom(&random32)
{
    this->app = app;

    spriteShader = nullptr;

    componentSprite = nullptr;
    spriteNode = nullptr;

    bgComponentSprite = nullptr;
    bgNode = nullptr;

    randomNext = 0.0f;
}

MainScene::~MainScene()
{
    unload();
}
