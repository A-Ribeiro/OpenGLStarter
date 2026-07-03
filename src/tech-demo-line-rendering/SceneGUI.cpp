#include "SceneGUI.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

const AppKit::GLEngine::SceneBaseType SceneGUI::Type = "SceneGUI";

// to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneGUI::loadResources()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    fontBuilder.load("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);
}
// to load the scene graph
void SceneGUI::loadGraph()
{
    // root = Transform::CreateShared();
    // root->affectComponentStart = true;
    root = Transform::CreateShared()->setRootPropertiesFromDefaultScene(this->self());

    auto t = root->addChild(Transform::CreateShared());
    t->Name = "Main Camera";

}
// to bind the resources to the current graph
void SceneGUI::bindResourcesToGraph()
{

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
    componentCameraOrthographic->useSizeY = true;
    componentCameraOrthographic->sizeY = 720.0f;

    button = new Button(
        0,                           // _position,
        false,                       // _left,
        "button_change_scene",       //_id,
        "View in 3D",                //_text,
        componentCameraOrthographic, // camera,
        &fontBuilder,                 //_fontBuilder
        resourceMap,
        this->renderWindow, this->self()
    );

    root->addChild(button->getTransform());


    // Add AABB for all meshs...
    {
        // root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }
}

// clear all loaded scene
void SceneGUI::unloadAll()
{

    // ResourceHelper::releaseTransformRecursive(&root);
    root = nullptr;
    camera = nullptr;

    if (button != nullptr)
    {
        delete button;
        button = nullptr;
    }
}

void SceneGUI::draw()
{
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    if (button != nullptr)
    {
        auto pos = MathCore::vec3f(renderWindow->MousePosRelatedToCenter_OriginBottom * renderWindow->windowToCameraScale, 0.0f);

        if (this->camera != nullptr)
        {
            auto ortho = std::dynamic_pointer_cast<ComponentCameraOrthographic>(camera);
            if (ortho->useSizeY)
            {
                float factor = ortho->sizeY / (float)ortho->projectionAreaSizePx.height;
                pos *= factor;
            }
        }
        button->update(pos);
    }

    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);

    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(resourceMap, root, camera, false);

    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void SceneGUI::resize(const MathCore::vec2f &size)
{
    if (button != nullptr)
        button->resize();
}

SceneGUI::SceneGUI(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow, SceneGUI::Type)
{
    button = nullptr;
}

SceneGUI::~SceneGUI()
{
    unload();
}
