#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>

// #include <InteractiveToolkit/EaseCore/EaseCore.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;
using namespace AppKit::GLEngine::Components;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources()
{

}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->addChild(Transform::CreateShared())->Name = "Main Camera";

    line_middle_to_half = root->addChild(Transform::CreateShared());
    line_middle_to_half->Name = "line_middle_to_half";

    deprecated_lines = line_middle_to_half->addNewComponent<ComponentColorLine>();
    deprecated_lines->color = MathCore::vec4f(1.0f, 0.0f, 1.0f, 1.0f);
    deprecated_lines->width = 100.0f;


}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{

    GLRenderState *renderState = GLRenderState::Instance();

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

    renderWindow->OnUpdate.add(&MainScene::update, this);
}

// clear all loaded scene
void MainScene::unloadAll()
{
    renderWindow->OnUpdate.remove(&MainScene::update, this);

    root = nullptr;
    camera = nullptr;
    deprecated_lines = nullptr;
    line_middle_to_half = nullptr;
}

void MainScene::update(Platform::Time *elapsed)
{
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
    deprecated_lines->vertices.clear();
    deprecated_lines->vertices.push_back(MathCore::vec3f(0, 0, 0));
    deprecated_lines->vertices.push_back(MathCore::vec3f(size.width, size.height, 0) * 0.25f);
    deprecated_lines->syncVBODynamic();
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
}

MainScene::~MainScene()
{
    unload();
}
