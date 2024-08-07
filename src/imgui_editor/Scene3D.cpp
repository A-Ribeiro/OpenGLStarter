#include "Scene3D.h"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

#include "App.h"

static bool ReplaceMaterial(std::shared_ptr<Transform> element, void *userData)
{
    auto material = element->findComponent<ComponentMaterial>();
    if (material != nullptr)
    {
        std::shared_ptr<ComponentMaterial> &newMaterial = *(std::shared_ptr<ComponentMaterial> *)userData;
        auto componentMaterial = element->removeComponent(material);
        element->addComponent(newMaterial);
        // little optimization
        element->makeFirstComponent(newMaterial);
    }
    return true;
}

// to load skybox, textures, cubemaps, 3DModels and setup materials
void Scene3D::loadResources()
{
}

// to load the scene graph
void Scene3D::loadGraph()
{
    root = Transform::CreateShared();
    root->setRenderWindowRegion(this->renderWindow);
    root->setName("realRoot");
}

// to bind the resources to the current graph
void Scene3D::bindResourcesToGraph()
{

    // setup camera
    {
        auto mainCamera = root->addChild(Transform::CreateShared());
        mainCamera->setName("mainCamera");
        mainCamera->setLocalPosition(vec3f(0, 0, -10.0f));
        camera = mainCamera->addNewComponent<ComponentCameraPerspective>();

        ((ComponentCameraPerspective *)camera.get())->fovDegrees = 60.0f;
        ((ComponentCameraPerspective *)camera.get())->nearPlane = 1.0f;
        ((ComponentCameraPerspective *)camera.get())->farPlane = 50.0f;

        mainCamera->lookAtLeftHanded(root);
    }
}

// clear all loaded scene
void Scene3D::unloadAll()
{
    root = nullptr;
    camera = nullptr;
}

Scene3D::Scene3D(App *app, std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow)
    : AppKit::GLEngine::SceneBase(&app->time, &app->renderPipeline, &app->resourceHelper)
{
    this->app = app;
    this->renderWindow = renderWindow;
}

Scene3D::~Scene3D()
{
    unload();
}

void Scene3D::draw()
{
    // App* app = (App*)AppKit::GLEngine::Engine::Instance()->app;

    // static EventCore::PressReleaseDetector num0;
    // num0.setState(AppKit::Window::Devices::Keyboard::isPressed( AppKit::Window::Devices::KeyCode::Num1 ));
    // static EventCore::PressReleaseDetector num1;
    // num1.setState(AppKit::Window::Devices::Keyboard::isPressed( AppKit::Window::Devices::KeyCode::Num2 ));
    // static EventCore::PressReleaseDetector num2;
    // num2.setState(AppKit::Window::Devices::Keyboard::isPressed( AppKit::Window::Devices::KeyCode::Num3 ));

    // if (num0.down)
    //     light->sun.smartSunLightCalculation(695.7 * 10.0, 150000.0, 500.0f);
    // if (num1.down)
    //     light->sun.smartSunLightCalculation(695.7 * 15.0, 150000.0,  500.0f);
    // if (num2.down)
    //     light->sun.smartSunLightCalculation(695.7 * 20.0, 150000.0,  500.0f);

    SceneBase::draw();
}

std::shared_ptr<AppKit::GLEngine::Components::ComponentCameraPerspective> Scene3D::getCamera()
{
    return std::dynamic_pointer_cast<AppKit::GLEngine::Components::ComponentCameraPerspective>(camera);
}
std::shared_ptr<AppKit::GLEngine::Transform> Scene3D::getRoot()
{
    return root;
}

void Scene3D::setCamera(std::shared_ptr<AppKit::GLEngine::Components::ComponentCameraPerspective> v)
{
    camera = v;
}

void Scene3D::setRoot(std::shared_ptr<AppKit::GLEngine::Transform> v)
{
    root = v;
}

void Scene3D::printHierarchy()
{

    std::vector<std::shared_ptr<Transform>> toVisit;
    toVisit.push_back(root);
    std::vector<std::string> spaces;
    spaces.push_back("");
    while (toVisit.size() > 0)
    {
        auto element = *toVisit.begin();
        toVisit.erase(toVisit.begin());
        std::string space = *spaces.begin();
        spaces.erase(spaces.begin());

        printf("%s- %s\n", space.c_str(), element->getName().c_str());
        space += "  ";

        toVisit.insert(toVisit.begin(), element->getChildren().begin(), element->getChildren().end());
        spaces.insert(spaces.begin(), element->getChildCount(), space);
    }
}
