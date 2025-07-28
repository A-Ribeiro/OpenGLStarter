#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>
#include <InteractiveToolkit/ITKCommon/Random.h>

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
    root->affectComponentStart = true;
    root->addChild(Transform::CreateShared())->Name = "Main Camera";

    deprecated_lines_transform = root->addChild(Transform::CreateShared());
    deprecated_lines_transform->Name = "deprecated_lines_transform";

    new_line_algorithm_transform = root->addChild(Transform::CreateShared());
    new_line_algorithm_transform->Name = "new line transform";

    if (this->use3DSet)
        new_line_algorithm_transform->setLocalScale(MathCore::vec3f(50.0f, 50.0f, 50.0f) * 2.0f);
    else
        new_line_algorithm_transform->setLocalScale(MathCore::vec3f(0.5f, 0.5f, 0.5f));
}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{

    // GLRenderState *renderState = GLRenderState::Instance();

    auto mainCamera = root->findTransformByName("Main Camera");
    if (this->use3DSet)
    {
        std::shared_ptr<ComponentCameraPerspective> componentCameraOrthographic;
        camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraPerspective>();
        componentCameraOrthographic->fovDegrees = 90.0f;
        componentCameraOrthographic->nearPlane = 1.0f;
        componentCameraOrthographic->farPlane = 1500.0f;

        camera->getTransform()->setLocalPosition(MathCore::vec3f(0.0f, 0.0f, -1.0f));
    }
    else
    {
        std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
        camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
        componentCameraOrthographic->useSizeY = true;
        componentCameraOrthographic->sizeY = 1080.0f * 0.5f;
        componentCameraOrthographic->nearPlane = 50.0f;
        componentCameraOrthographic->farPlane = 150.0f;

        camera->getTransform()->setLocalPosition(MathCore::vec3f(0.0f, 0.0f, -100.0f));
    }

    

    deprecated_lines = deprecated_lines_transform->addNewComponent<ComponentColorLine>();
    deprecated_lines->color = MathCore::vec4f(1.0f, 0.0f, 1.0f, 1.0f);
    deprecated_lines->width = 5.0f;

    line_mounter = new_line_algorithm_transform->addNewComponent<ComponentLineMounter>();
    line_mounter->setLineShader(lineShader);
    line_mounter->setCamera(camera, true);
    line_mounter->meshWrapper->debugCollisionShapes = true;

    auto &bag = line_mounter->material->property_bag;
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
    angle = MathCore::OP<float>::fmod(angle + elapsed->deltaTime * 0.125f, 2.0f * MathCore::CONSTANT<float>::PI);
    auto rot = (this->use3DSet)?MathCore::GEN<MathCore::quatf>::fromEuler(angle, 0, 0):MathCore::GEN<MathCore::quatf>::fromEuler(0, 0, angle);
    // line_middle_to_half->setLocalRotation(MathCore::GEN< MathCore::quatf>::fromEuler(0, 0, angle));

    auto size = MathCore::vec2i(this->camera->viewport.w, this->camera->viewport.h);

    deprecated_lines->vertices.clear();
    if (this->use3DSet)
    {
        const MathCore::vec3f lines[24] = {
            MathCore::vec3f(-1, -1, -1),
            MathCore::vec3f(1, -1, -1),
            MathCore::vec3f(-1, -1, 1),
            MathCore::vec3f(1, -1, 1),
            MathCore::vec3f(-1, 1, -1),
            MathCore::vec3f(1, 1, -1),
            MathCore::vec3f(-1, 1, 1),
            MathCore::vec3f(1, 1, 1),
            MathCore::vec3f(-1, -1, -1),
            MathCore::vec3f(-1, 1, -1),
            MathCore::vec3f(-1, -1, 1),
            MathCore::vec3f(-1, 1, 1),
            MathCore::vec3f(1, -1, -1),
            MathCore::vec3f(1, 1, -1),
            MathCore::vec3f(1, -1, 1),
            MathCore::vec3f(1, 1, 1),
            MathCore::vec3f(-1, -1, -1),
            MathCore::vec3f(-1, -1, 1),
            MathCore::vec3f(-1, 1, -1),
            MathCore::vec3f(-1, 1, 1),
            MathCore::vec3f(1, -1, -1),
            MathCore::vec3f(1, -1, 1),
            MathCore::vec3f(1, 1, -1),
            MathCore::vec3f(1, 1, 1)};

        const auto &local_scale = new_line_algorithm_transform->getLocalScale();

        MathCore::vec3f x_offset = MathCore::vec3f(3.0f, 0.0f, 0.0f) * local_scale;
        //MathCore::vec3f z_offset = MathCore::vec3f(0.0f, 0.0f, -1.0f) * local_scale;
        MathCore::vec3f y_offset = MathCore::vec3f(0.0f, 3.0f, 0.0f) * local_scale;

        for (int x = -5; x <= 5; x++)
        {
            for (int i = 0; i < 24; i += 2)
            {
                deprecated_lines->vertices.push_back( rot * ( lines[i] * local_scale + x_offset * (float)x));
                deprecated_lines->vertices.push_back( rot * ( lines[i + 1] * local_scale + x_offset * (float)x));
            }

            if (x)
                for (int i = 0; i < 24; i += 2)
                {
                    deprecated_lines->vertices.push_back( rot * ( lines[i] * local_scale + y_offset * (float)x));
                    deprecated_lines->vertices.push_back( rot * ( lines[i + 1] * local_scale + y_offset * (float)x));
                }
        }
    }
    else
    {
        deprecated_lines->vertices.push_back(MathCore::vec3f(0, 0, 0));
        deprecated_lines->vertices.push_back(rot * MathCore::vec3f(size.width, size.height, 0) * 0.25f);

        deprecated_lines->vertices.push_back(MathCore::vec3f(0, 0, 0));
        deprecated_lines->vertices.push_back(rot * MathCore::vec3f(size.height, -size.width, 0) * 0.25f);
    }
    // deprecated_lines->syncVBODynamic();

    new_line_algorithm_transform->setLocalRotation(rot);

    bool keyPressed = false;

    auto cameraTransform = camera->getTransform();
    float speed = 500.0f;
    if (this->use3DSet)
        speed = 200.0f;
    if (Keyboard::isPressed(KeyCode::Left))
    {
        cameraTransform->setLocalPosition(
            cameraTransform->getLocalPosition() + MathCore::vec3f(-1.0f, 0.0f, 0.0f) * elapsed->deltaTime * speed);

        cameraTransform->lookAtLeftHanded(new_line_algorithm_transform,
                                          MathCore::vec3f(0.0f, 1.0f, 0.0f) // up vector
        );
    }
    else if (Keyboard::isPressed(KeyCode::Right))
    {
        cameraTransform->setLocalPosition(
            cameraTransform->getLocalPosition() + MathCore::vec3f(1.0f, 0.0f, 0.0f) * elapsed->deltaTime * speed);

        cameraTransform->lookAtLeftHanded(new_line_algorithm_transform,
                                          MathCore::vec3f(0.0f, 1.0f, 0.0f) // up vector
        );
    }

    const float angle_speed = MathCore::OP<float>::deg_2_rad(30.0f);
    if (Keyboard::isPressed(KeyCode::A))
    {
        camera_angle = MathCore::OP<float>::fmod(camera_angle - angle_speed * elapsed->deltaTime, MathCore::CONSTANT<float>::PI * 2.0f);
        auto Rotation = MathCore::GEN<MathCore::quatf>::fromEuler(0, camera_angle, 0);

        cameraTransform->setLocalPosition(Rotation * MathCore::vec3f(0, 0, -MathCore::OP<MathCore::vec3f>::length(cameraTransform->getLocalPosition())));
        cameraTransform->setLocalRotation(Rotation);
    }
    else if (Keyboard::isPressed(KeyCode::D))
    {
        camera_angle = MathCore::OP<float>::fmod(camera_angle + angle_speed * elapsed->deltaTime, MathCore::CONSTANT<float>::PI * 2.0f);
        auto Rotation = MathCore::GEN<MathCore::quatf>::fromEuler(0, camera_angle, 0);

        cameraTransform->setLocalPosition(Rotation * MathCore::vec3f(0, 0, -MathCore::OP<MathCore::vec3f>::length(cameraTransform->getLocalPosition())));
        cameraTransform->setLocalRotation(Rotation);
    }

    if (Keyboard::isPressed(KeyCode::Up))
    {
        cameraTransform->setLocalPosition(
            cameraTransform->getLocalPosition() + MathCore::vec3f(0.0f, 1.0f, 0.0f) * elapsed->deltaTime * speed);
    }
    else if (Keyboard::isPressed(KeyCode::Down))
    {
        cameraTransform->setLocalPosition(
            cameraTransform->getLocalPosition() + MathCore::vec3f(0.0f, -1.0f, 0.0f) * elapsed->deltaTime * speed);
    }

    float scale_speed = 1.0f;
    if (this->use3DSet)
        scale_speed = 10.0f;

    if (Keyboard::isPressed(KeyCode::W))
    {
        new_line_algorithm_transform->setLocalScale(
            new_line_algorithm_transform->getLocalScale() - elapsed->deltaTime * scale_speed);
    }
    else if (Keyboard::isPressed(KeyCode::S))
    {
        new_line_algorithm_transform->setLocalScale(
            new_line_algorithm_transform->getLocalScale() + elapsed->deltaTime * scale_speed);
    }
}

void MainScene::draw()
{
    auto engine = AppKit::GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestLessEqual;
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

    if (this->use3DSet)
    {

        auto color = MathCore::CVT<MathCore::vec4f>::sRGBToLinear(
            MathCore::vec4f(0.0f, 1.0f, 0.0f, 1.0f));

        const MathCore::vec3f lines[24] = {
            MathCore::vec3f(-1, -1, -1),
            MathCore::vec3f(1, -1, -1),
            MathCore::vec3f(-1, -1, 1),
            MathCore::vec3f(1, -1, 1),
            MathCore::vec3f(-1, 1, -1),
            MathCore::vec3f(1, 1, -1),
            MathCore::vec3f(-1, 1, 1),
            MathCore::vec3f(1, 1, 1),
            MathCore::vec3f(-1, -1, -1),
            MathCore::vec3f(-1, 1, -1),
            MathCore::vec3f(-1, -1, 1),
            MathCore::vec3f(-1, 1, 1),
            MathCore::vec3f(1, -1, -1),
            MathCore::vec3f(1, 1, -1),
            MathCore::vec3f(1, -1, 1),
            MathCore::vec3f(1, 1, 1),
            MathCore::vec3f(-1, -1, -1),
            MathCore::vec3f(-1, -1, 1),
            MathCore::vec3f(-1, 1, -1),
            MathCore::vec3f(-1, 1, 1),
            MathCore::vec3f(1, -1, -1),
            MathCore::vec3f(1, -1, 1),
            MathCore::vec3f(1, 1, -1),
            MathCore::vec3f(1, 1, 1)};

        MathCore::vec3f x_offset = MathCore::vec3f(3.0f, 0.0f, 0.0f);
        MathCore::vec3f y_offset = MathCore::vec3f(0.0f, 3.0f, 0.0f);
        // MathCore::vec3f z_offset = MathCore::vec3f(0.0f, 0.0f, -1.0f);
        for (int x = -5; x <= 5; x++)
        {
            color = mathRnd.next<MathCore::vec4f>();
            color.a = 1.0f;
            for (int i = 0; i < 24; i += 2)
            {
                line_mounter->addLine(
                    lines[i] + x_offset * (float)x,
                    lines[i + 1] + x_offset * (float)x,
                    50.0f,
                    color);

                if (x)
                    line_mounter->addLine(
                        lines[i] + y_offset * (float)x,
                        lines[i + 1] + y_offset * (float)x,
                        50.0f,
                        color);
            }
        }
    }
    else
    {
        line_mounter->addLine(
            MathCore::vec3f(0, 0, 0),
            MathCore::vec3f(0, size.height, 0) * 0.25f,
            5.0f,
            MathCore::CVT<MathCore::vec4f>::sRGBToLinear(
                MathCore::vec4f(1.0f, 0.0f, 0.0f, 1.0f)));

        line_mounter->addLine(
            MathCore::vec3f(0, 0, 0),
            MathCore::vec3f(size.width, size.height, 0) * 0.25f,
            15.0f,
            MathCore::CVT<MathCore::vec4f>::sRGBToLinear(
                MathCore::vec4f(0.0f, 1.0f, 0.0f, 1.0f)));

        line_mounter->addLine(
            MathCore::vec3f(0, 0, 0),
            MathCore::vec3f(size.width, 0, 0) * 0.25f,
            10.0f,
            MathCore::CVT<MathCore::vec4f>::sRGBToLinear(
                MathCore::vec4f(0.0f, 0.0f, 1.0f, 1.0f)));
    }
}

MainScene::MainScene(
    App *app,
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow,
    bool _3d) : 
    AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow),
    mathRnd(ITKCommon::Random::Instance())
{
    this->app = app;
    this->use3DSet = _3d;

    angle = 0.0f;
    camera_angle = 0.0f;
}

MainScene::~MainScene()
{
    unload();
}
