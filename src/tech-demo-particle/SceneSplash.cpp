#include "SceneSplash.h"

// #include <appkit-gl-engine/mini-gl-engine.h>

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneSplash::loadResources()
{

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    Milky_512_512 = resourceHelper->createTextureFromFile("resources/opengl_logo_white.png", true && engine->sRGBCapable);

    //ReferenceCounter<AppKit::OpenGL::GLTexture *> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    //texRefCount->add(Milky_512_512);
}
// to load the scene graph
void SceneSplash::loadGraph()
{
    // root = Transform::CreateShared();
    // root->affectComponentStart = true;
    root = Transform::CreateShared()->setRootPropertiesFromDefaultScene(this->self());

    auto t = root->addChild(Transform::CreateShared());
    t->Name = "Main Camera";
    //    t->LocalPosition = MathCore::vec3f(0,0,0);

    t = root->addChild(Transform::CreateShared());
    t->Name = "Sprite";
}
// to bind the resources to the current graph
void SceneSplash::bindResourcesToGraph()
{

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate
    renderState->ClearColor = vec4f(0.0f, 0.0f, 0.0f, 1.0f);

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();

    componentCameraOrthographic->useSizeY = true;
    componentCameraOrthographic->sizeY = 512.0f * 2.5f;

    auto spriteTransform = root->findTransformByName("Sprite");
    // spriteTransform->LocalRotation = quatFromEuler(MathCore::OP<float>::deg_2_rad(-90.0f), 0, 0);

    auto material = spriteTransform->addNewComponent<ComponentMaterial>();
    spriteTransform->addComponent(ComponentMesh::createPlaneXY(512.0f, 512.0f));

    //ReferenceCounter<AppKit::OpenGL::GLTexture *> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    material->setShader(resourceMap->shaderUnlitTextureAlpha);
    //material->property_bag.getProperty("BlendMode").set((int)AppKit::GLEngine::BlendModeAlpha);
    material->property_bag.getProperty("uTexture").set<std::shared_ptr<AppKit::OpenGL::VirtualTexture>>(Milky_512_512);

    // material->type = MaterialUnlitTexture;
    // material->unlit.color = vec4f(1.0f);
    // material->unlit.blendMode = BlendModeAlpha;
    // material->unlit.tex = Milky_512_512;

    // Add AABB for all meshs...
    {
        // root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }
}

// clear all loaded scene
void SceneSplash::unloadAll()
{

    //ResourceHelper::releaseTransformRecursive(&root);
    root = nullptr;
    camera = nullptr;

    // ReferenceCounter<AppKit::OpenGL::GLTexture *> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;
    // if (Milky_512_512 != nullptr)
    // {
    //     texRefCount->removeNoDelete(Milky_512_512);
    //     delete Milky_512_512;
    //     Milky_512_512 = nullptr;
    // }
    Milky_512_512 = nullptr;
}

// AppKit::OpenGL::GLTexture *Milky_512_512;

SceneSplash::SceneSplash(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow)
{

    Milky_512_512 = nullptr;
}

SceneSplash::~SceneSplash()
{
    unload();
}
