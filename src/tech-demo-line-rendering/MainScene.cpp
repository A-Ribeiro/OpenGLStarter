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
    lineShader = std::make_shared<LineShader>();
}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->addChild(Transform::CreateShared())->Name = "Main Camera";

    deprecated_lines_transform = root->addChild(Transform::CreateShared());
    deprecated_lines_transform->Name = "deprecated_lines_transform";

    new_line_algorithm_transform = root->addChild(Transform::CreateShared());
    new_line_algorithm_transform->Name = "new line transform";

    new_line_algorithm_transform->setLocalScale(MathCore::vec3f(0.5f, 0.5f, 0.5f));

}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{

    // GLRenderState *renderState = GLRenderState::Instance();

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
    componentCameraOrthographic->useSizeY = true;
    componentCameraOrthographic->sizeY = 1080.0f * 0.5f;

    deprecated_lines = deprecated_lines_transform->addNewComponent<ComponentColorLine>();
    deprecated_lines->color = MathCore::vec4f(1.0f, 0.0f, 1.0f, 1.0f);
    deprecated_lines->width = 5.0f;

    line_mounter = new_line_algorithm_transform->addNewComponent<ComponentLineMounter>();
    line_mounter->setLineShader(lineShader);
    line_mounter->setCamera(camera);
    line_mounter->meshWrapper->debugCollisionShapes = true;

    auto &bag = line_mounter->material->custom_shader_property_bag;
    // bag.getProperty("uColor").set(MathCore::vec4f(1.0f, 0.0f, 0.0f, 1.0f));
    bag.getProperty("uAntialias").set(1.0f);

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
    deprecated_lines_transform = nullptr;

    new_line_algorithm_transform = nullptr;
    line_mounter = nullptr;

    lineShader = nullptr;
}

void MainScene::update(Platform::Time *elapsed)
{
    static float angle = 0.0f;
    angle = MathCore::OP<float>::fmod(angle + elapsed->deltaTime * 0.125f, 2.0f * MathCore::CONSTANT<float>::PI);
    auto rot = MathCore::GEN< MathCore::quatf>::fromEuler(0, 0, angle);
    //line_middle_to_half->setLocalRotation(MathCore::GEN< MathCore::quatf>::fromEuler(0, 0, angle));

    auto size = MathCore::vec2i(this->camera->viewport.w, this->camera->viewport.h);

    deprecated_lines->vertices.clear();
    deprecated_lines->vertices.push_back(MathCore::vec3f(0, 0, 0));
    deprecated_lines->vertices.push_back(rot * MathCore::vec3f(size.width, size.height, 0) * 0.25f);

    deprecated_lines->vertices.push_back(MathCore::vec3f(0, 0, 0));
    deprecated_lines->vertices.push_back(rot * MathCore::vec3f(size.height, -size.width, 0) * 0.25f);
    // deprecated_lines->syncVBODynamic();


    new_line_algorithm_transform->setLocalRotation( MathCore::OP<MathCore::quatf>::conjugate(rot));

    bool keyPressed = false;

    auto cameraTransform = camera->getTransform();
    const float speed = 500.0f;
    if (Keyboard::isPressed(KeyCode::Left)){
        cameraTransform->setLocalPosition(
            cameraTransform->getLocalPosition() + MathCore::vec3f(-1.0f, 0.0f, 0.0f) * elapsed->deltaTime * speed
        );
    } else if (Keyboard::isPressed(KeyCode::Right)){
        cameraTransform->setLocalPosition(
            cameraTransform->getLocalPosition() + MathCore::vec3f(1.0f, 0.0f, 0.0f) * elapsed->deltaTime * speed
        );
    }
    if (Keyboard::isPressed(KeyCode::Up)){
        cameraTransform->setLocalPosition(
            cameraTransform->getLocalPosition() + MathCore::vec3f(0.0f, 1.0f, 0.0f) * elapsed->deltaTime * speed
        );
    } else if (Keyboard::isPressed(KeyCode::Down)){
        cameraTransform->setLocalPosition(
            cameraTransform->getLocalPosition() + MathCore::vec3f(0.0f, -1.0f, 0.0f) * elapsed->deltaTime * speed
        );
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
    /*deprecated_lines->vertices.clear();
    deprecated_lines->vertices.push_back(MathCore::vec3f(0, 0, 0));
    deprecated_lines->vertices.push_back(MathCore::vec3f(size.width, size.height, 0) * 0.25f);
    deprecated_lines->syncVBODynamic();*/

    line_mounter->clear();

    line_mounter->addLine(
        MathCore::vec3f(0, 0, 0),
        MathCore::vec3f(0, size.height, 0) * 0.25f,
        5.0f,
        MathCore::CVT<MathCore::vec4f>::sRGBToLinear(
            MathCore::vec4f(1.0f, 0.0f, 0.0f, 1.0f)
        )
    );

    line_mounter->addLine(
        MathCore::vec3f(0, 0, 0),
        MathCore::vec3f(size.width, size.height, 0) * 0.25f,
        15.0f,
        MathCore::CVT<MathCore::vec4f>::sRGBToLinear(
            MathCore::vec4f(0.0f, 1.0f, 0.0f, 1.0f)
        )
    );

    line_mounter->addLine(
        MathCore::vec3f(0, 0, 0),
        MathCore::vec3f(size.width, 0, 0) * 0.25f,
        10.0f,
        MathCore::CVT<MathCore::vec4f>::sRGBToLinear(
            MathCore::vec4f(0.0f, 0.0f, 1.0f, 1.0f)
        )
    );

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
