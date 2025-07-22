#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources()
{
    // auto engine = AppKit::GLEngine::Engine::Instance();
    spriteShader = std::make_shared<SpriteShader>(resourceMap);

}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->addChild(Transform::CreateShared())->Name = "Main Camera";

    root->addChild(Transform::CreateShared())->Name = "bg";

    root->addChild(Transform::CreateShared())->Name = "Smoke";

}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    GLRenderState *renderState = GLRenderState::Instance();

    spriteNode = root->findTransformByName("Smoke");
    componentSprite = spriteNode->addNewComponent<ComponentSprite>();
    componentSprite->setSpriteShader(spriteShader);
    componentSprite->setTexture(
        resourceMap->getTexture("resources/smoke.png", engine->sRGBCapable),
        MathCore::vec2f(0.5f, 0.5f), // pivot
        MathCore::vec4f(1.0f, 1.0f, 1.0f, 0.4f), // color
        MathCore::vec2f(-1, -1), // size constraint
        true // static mesh
    );
    // componentSprite->meshWrapper->debugCollisionShapes = true;
    spriteNode->skip_traversing = true;
    // componentSprite->material->custom_shader_property_bag.getProperty("UseDiscard").set(true);

    bgNode = root->findTransformByName("bg");
    bgComponentSprite = bgNode->addNewComponent<ComponentSprite>();
    bgComponentSprite->setSpriteShader(spriteShader);
    bgComponentSprite->setTexture(
        resourceMap->getTexture("resources/Skyboxes/SanFrancisco4/posy.jpg", engine->sRGBCapable),
        MathCore::vec2f(0.5f, 0.5f), // pivot
        MathCore::vec4f(1.0f, 1.0f, 1.0f, 1.0f), // color
        MathCore::vec2f(-1, 1024), // size constraint
        true // static mesh
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
            //
            spriteNode->setLocalScale(MathCore::vec3f(0));
            spriteNode->skip_traversing = false;


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
    if (spriteNode->getLocalScale().x >= 1.0f)
        spriteNode->skip_traversing = true;
    else {

        if (!spriteNode->skip_traversing){
            spriteNode->setLocalScale(
                MathCore::OP<MathCore::vec3f>::move(
                    spriteNode->getLocalScale(),
                    MathCore::vec3f(1.0f, 1.0f, 1.0f),
                    elapsed->deltaTime * 5.0f
                )
            );
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
    renderPipeline->runSinglePassPipeline(root, camera, true);
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
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow)
{
    this->app = app;

    spriteShader = nullptr;

    componentSprite = nullptr;
    spriteNode = nullptr;

    bgComponentSprite = nullptr;
    bgNode = nullptr;
}

MainScene::~MainScene()
{
    unload();
}
