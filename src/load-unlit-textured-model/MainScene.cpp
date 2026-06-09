#include "MainScene.h"
#include "App.h"

#include "util/ModelSmasher.h"
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

// #include <InteractiveToolkit/EaseCore/EaseCore.h>
#include "components/ComponentGrow.h"
#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>


#include <cstdlib>
#include <string>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;
using namespace ITKCommon;

namespace Scenes
{

    // to load skybox, textures, cubemaps, 3DModels and setup materials
    void MainScene::loadResources()
    {
    }
    // to load the scene graph
    void MainScene::loadGraph()
    {
        root = Transform::CreateShared()->setRootPropertiesFromDefaultScene(this->self());

        auto main_camera = root->addChild(Transform::CreateShared("Main Camera"));

        root->addChild(Transform::CreateShared("scene"));
    }

    // to bind the resources to the current graph
    void MainScene::bindResourcesToGraph()
    {
        auto engine = AppKit::GLEngine::Engine::Instance();

        GLRenderState *renderState = GLRenderState::Instance();

        std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
        auto mainCamera = root->findTransformByName("Main Camera");
        {
            camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
            componentCameraOrthographic->useSizeY = true;
            componentCameraOrthographic->sizeY = 10.0f;
        }

        {
            SmartImporter::ModelSmasher smasher;
#if defined(__linux__)
            // FileSystem::Directory("/mnt/wslg/runtime-dir").isValid()
            if (ITKCommon::Path::isDirectory("/mnt/wslg/runtime-dir"))
            {
                // wsl environment
                loadedScene = smasher.load("/mnt/d/shared/papercat/stages_gltf/stage3_04.bams", resourceMap, camera);
            }
            else
            {
                // real linux environment
                const char *home = std::getenv("HOME");
                std::string inputPath = std::string(home ? home : "") + "/Documents/papercat/stages_gltf/stage3_04.bams";
                auto path = std::unique_ptr<char, decltype(&std::free)>(realpath(inputPath.c_str(), nullptr), &std::free);
                loadedScene = smasher.load(path ? path.get() : inputPath.c_str(), resourceMap, camera);
            }

#else
            loadedScene = smasher.load("D:/shared/papercat/stages_gltf/stage3_04.bams", resourceMap, camera);
#endif
        }

        sceneNode = root->findTransformByName("scene");

        sceneNode->addChild(loadedScene);

        auto player_pos = sceneNode->findTransformByName("Player")->getPosition();

        mainCamera->setLocalPosition(vec3f(player_pos.x, player_pos.y, -10));

        auto rect = renderWindow->CameraViewport.c_ptr();
        resize(vec2i(rect->w, rect->h));

        this->OnUpdate.add(&MainScene::update, this);
    }

    // clear all loaded scene
    void MainScene::unloadAll()
    {
        this->OnUpdate.remove(&MainScene::update, this);

        root = nullptr;
        camera = nullptr;

        sceneNode = nullptr;
        loadedScene = nullptr;
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
        renderPipeline->runSinglePassPipeline(resourceMap, root, camera, true, OrthographicFilter_UsingAABB, &app->threadPool);
        if (engine->sRGBCapable)
            glEnable(GL_FRAMEBUFFER_SRGB);
    }

    void MainScene::resize(const vec2i &size)
    {
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
    }

    MainScene::~MainScene()
    {
        unload();
    }

}