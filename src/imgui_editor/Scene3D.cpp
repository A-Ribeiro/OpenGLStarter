#include "Scene3D.h"

#include <appkit-gl-engine/Components/ComponentCameraMove.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

#include "App.h"

#include "./ImGui/ImGuiManager.h"

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
    root->affectComponentStart = true;
    root->setRenderWindowRegion(this->renderWindow);
    root->setName("realRoot");
}

// to bind the resources to the current graph
void Scene3D::bindResourcesToGraph()
{

    // setup camera
    {
        mainCamera = root->addChild(Transform::CreateShared());
        mainCamera->setName("mainCamera");
        mainCamera->setLocalPosition(vec3f(0, 0, -10.0f));
        camera = mainCamera->addNewComponent<ComponentCameraPerspective>();

        ((ComponentCameraPerspective *)camera.get())->fovDegrees = 60.0f;
        ((ComponentCameraPerspective *)camera.get())->nearPlane = 1.0f;
        ((ComponentCameraPerspective *)camera.get())->farPlane = 50.0f;

        mainCamera->lookAtLeftHanded(root);
    }
}

void Scene3D::setMainCamera(std::shared_ptr<AppKit::GLEngine::Components::ComponentCamera> camera) {
    this->camera = camera;
}

void Scene3D::ensureCameraExists(){
    auto camera_perspective = mainCamera->findComponent< Components::ComponentCameraPerspective >();
    auto camera_ortho = mainCamera->findComponent< Components::ComponentCameraOrthographic>();
    if (camera_perspective == nullptr && camera_ortho == nullptr)
        bindResourcesToGraph();
}


// clear all loaded scene
void Scene3D::unloadAll()
{
    mainCamera = nullptr;
    root = nullptr;
    camera = nullptr;
}

Scene3D::Scene3D(App *app, std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow)
    : AppKit::GLEngine::SceneBase(&app->time, &app->renderPipeline, &app->resourceHelper, &app->resourceMap, renderWindow)
{
    this->app = app;
    this->renderWindow = renderWindow;

    this->renderWindow->OnUpdate.add(&Scene3D::OnUpdate, this);
}

Scene3D::~Scene3D()
{
    this->renderWindow->OnUpdate.remove(&Scene3D::OnUpdate, this);
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

void Scene3D::OnUpdate(Platform::Time* time) {
    // MathCore::vec3f pos3D = MathCore::vec3f(this->renderWindow->MousePosRelatedToCenter * this->renderWindow->windowToCameraScale, 0.0f);
    // if (cursorTransform != nullptr)
    //     cursorTransform->setLocalPosition(pos3D);

    // auto window_rect = this->renderWindow->WindowViewport.c_val();
    // auto mouse_relative_to_window = this->renderWindow->MousePos.c_val();

    // if (window_rect.contains(mouse_relative_to_window)){
    //     printf("inside...\n");
    // } else {
    //     printf("not inside...\n");
    // }

    bool inputBlocked = ImGuiManager::Instance()->shortcutManager.input_blocked;


    m_to_move.setState(Keyboard::isPressed(KeyCode::M) && !inputBlocked);
    if (m_to_move.down) {
        auto cameraMove = mainCamera->findComponent<Components::ComponentCameraMove>();
        if (cameraMove == nullptr){
            mainCamera->addComponent(Component::CreateShared<Components::ComponentCameraMove>());
            is_to_hide_mouse.setState(true);
            savedMouseCoordi = this->renderWindow->screenCenterI;
        } else {
            mainCamera->removeComponent(cameraMove);
            is_to_hide_mouse.setState(false);
        }
    }

    // right button mouse logic...
    mouse_right_btn_to_move.setState(Mouse::isPressed(MouseButton::Right) && !inputBlocked);
    if (mouse_right_btn_to_move.down) {
        
        auto window_rect = this->renderWindow->WindowViewport.c_val();
        auto mouse_relative_to_window = this->renderWindow->MousePos.c_val();
        if (window_rect.contains(mouse_relative_to_window)){
            // force camera movement
            auto cameraMove = mainCamera->findComponent<Components::ComponentCameraMove>();
            if (cameraMove == nullptr){
                mainCamera->addComponent(Component::CreateShared<Components::ComponentCameraMove>());
                is_to_hide_mouse.setState(true);
                savedMouseCoordi = this->renderWindow->iMousePosLocal;
            }
        }
    } else if (mouse_right_btn_to_move.up) {
        // force end camera movement
        auto cameraMove = mainCamera->findComponent<Components::ComponentCameraMove>();
        if (cameraMove != nullptr){
            mainCamera->removeComponent(cameraMove);
            is_to_hide_mouse.setState(false);
            this->renderWindow->forceMouseToCoord( savedMouseCoordi ); 
        }
    }

}