#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>
#include "components/ComponentGrow.h"

#include <cstdlib>
#include <string>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

#include <InteractiveToolkit-Extension/model/ModelContainer.h>

namespace SmartImporter
{
    class ModelSmasher
    {
        std::unique_ptr<ITKExtension::Model::ModelContainer> container;
        std::unordered_map<const ITKExtension::Model::Geometry *, bool> geometryProcessed;
        ResourceMap *resourceMap;

        std::shared_ptr<Components::ComponentMaterial> createUnlitMaterial(const ITKExtension::Model::Material *mat)
        {
            using namespace AppKit::GLEngine::Components;
            auto material = Component::CreateShared<Components::ComponentMaterial>();
            material->setShader(resourceMap->shaderUnlitTextureAlpha);

            // ShaderUnlitTextureAlpha Property Bag:

            // bag.addProperty("uColor", uColor);
            // bag.addProperty("uTexture", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            // bag.addProperty("BlendMode", (int)AppKit::GLEngine::BlendModeAlpha);

            // example:
            // auto albedoColor = MathCore::CVT<MathCore::vec4f>::toVec3(it->second);
            // albedoColor = ResourceHelper::vec3ColorGammaToLinear(albedoColor);
            // material->property_bag.getProperty("albedoColor").set<MathCore::vec3f>(albedoColor);

            return material;
        }

        std::shared_ptr<Components::ComponentMaterial> createPBRMaterial(const ITKExtension::Model::Material *mat)
        {
            using namespace AppKit::GLEngine::Components;
            auto material = Component::CreateShared<Components::ComponentMaterial>();
            material->setShader(resourceMap->pbrShaderSelector);

            // PBRShaderSelector Property Bag:

            // bag.addProperty("albedoColor", MathCore::vec3f(1.0f));
            // bag.addProperty("emissionColor", MathCore::vec3f(0.0f));
            // bag.addProperty("roughness", 1.0f);
            // bag.addProperty("metallic", 0.0f);

            // bag.addProperty("texAlbedo", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            // bag.addProperty("texNormal", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            // bag.addProperty("texSpecular", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            // bag.addProperty("texEmission", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));

            // example:
            // auto albedoColor = MathCore::CVT<MathCore::vec4f>::toVec3(it->second);
            // albedoColor = ResourceHelper::vec3ColorGammaToLinear(albedoColor);
            // material->property_bag.getProperty("albedoColor").set<MathCore::vec3f>(albedoColor);

            return material;
        }

        void traverse(const ITKExtension::Model::Node &node, int lvl = 0)
        {
            auto localPosition = node.getLocalPosition();
            auto localScale = node.getLocalScale();
            auto localRotation = node.getLocalRotation();

            printf("%*s(%s)\n", lvl * 2, "+", node.name.c_str());
            // printf("%*sLocalPosition %f, %f, %f\n",
            //        lvl * 2, "",
            //        localPosition.x,
            //        localPosition.y,
            //        localPosition.z);

            // printf("%*sLocalScale %f, %f, %f\n",
            //        lvl * 2, "",
            //        localScale.x,
            //        localScale.y,
            //        localScale.z);

            // printf("%*sLocalRotation %f, %f, %f, %f\n",
            //        lvl * 2, "",
            //        localRotation.x,
            //        localRotation.y,
            //        localRotation.z,
            //        localRotation.w);

            for (uint32_t gidx : node.geometries)
            {
                const ITKExtension::Model::Geometry *geom = &container->geometries[gidx];
                const ITKExtension::Model::Material *mat = &container->materials[geom->materialIndex];

                if (geometryProcessed.find(geom) != geometryProcessed.end())
                {
                    printf("%*sGeometry %s is a repeated geometry\n", lvl * 2, "", geom->name.c_str());
                    continue;
                }

                if (geom->indiceCountPerFace == 2)
                {
                    printf("%*sGeometry %s is a line mesh\n", lvl * 2, "", geom->name.c_str());
                }
                else if (geom->indiceCountPerFace == 3)
                {
                    printf("%*sGeometry %s is a triangle mesh\n", lvl * 2, "", geom->name.c_str());

                    for (const auto &kv : mat->stringValue)
                        printf("%*sMaterial String %s: %s\n", lvl * 2, "", kv.first.c_str(), kv.second.c_str());
                    for (const auto &kv : mat->floatValue)
                        printf("%*sMaterial Float %s: %f\n", lvl * 2, "", kv.first.c_str(), kv.second);
                    for (const auto &kv : mat->vec2Value)
                        printf("%*sMaterial Vec2 %s: %f, %f\n", lvl * 2, "", kv.first.c_str(), kv.second.x, kv.second.y);
                    for (const auto &kv : mat->vec3Value)
                        printf("%*sMaterial Vec3 %s: %f, %f, %f\n", lvl * 2, "", kv.first.c_str(), kv.second.x, kv.second.y, kv.second.z);
                    for (const auto &kv : mat->vec4Value)
                        printf("%*sMaterial Vec4 %s: %f, %f, %f, %f\n", lvl * 2, "", kv.first.c_str(), kv.second.x, kv.second.y, kv.second.z, kv.second.w);
                    for (const auto &kv : mat->intValue)
                        printf("%*sMaterial Int %s: %d\n", lvl * 2, "", kv.first.c_str(), kv.second);

                    for (const auto &tex : mat->textures)
                    {
                        bool compatible_with_texture_atlas = geom->is_uv_compatible_with_texture_atlas(tex.uvIndex);
                        printf("%*sMaterial Texture %s: %s.%s (Compatible with texture atlas: %s)\n", lvl * 2, "",
                               TextureTypeToStr(tex.type),
                               tex.filename.c_str(), tex.fileext.c_str(),
                               compatible_with_texture_atlas ? "YES" : "NO");
                    }

                    bool is_opaque = mat->is_opaque();
                    bool is_unlit = mat->is_unlit();
                    bool is_two_sided = mat->is_two_sided();

                    printf("%*sMaterial %s is opaque: %s\n", lvl * 2, "", mat->name.c_str(), is_opaque ? "YES" : "NO");
                    printf("%*sMaterial %s is unlit: %s\n", lvl * 2, "", mat->name.c_str(), is_unlit ? "YES" : "NO");
                    printf("%*sMaterial %s is two-sided: %s\n", lvl * 2, "", mat->name.c_str(), is_two_sided ? "YES" : "NO");
                }
                else
                {
                    printf("%*sGeometry %s is a unknown mesh with %d indices per face\n", lvl * 2, "", geom->name.c_str(), geom->indiceCountPerFace);
                    continue;
                }

                geometryProcessed[geom] = true;
            }

            for (uint32_t child_index : node.children)
                traverse(container->nodes[child_index], lvl + 1);
        }

    public:
        void load(const char *filename, ResourceMap *resourceMap)
        {
            const uint32_t root_index = 0;

            this->resourceMap = resourceMap;

            container = STL_Tools::make_unique<ITKExtension::Model::ModelContainer>();
            container->read(filename);

            traverse(container->nodes[root_index]);
        }
    };

}

namespace Scenes
{

    // to load skybox, textures, cubemaps, 3DModels and setup materials
    void MainScene::loadResources()
    {
        // auto engine = AppKit::GLEngine::Engine::Instance();

        // SpriteAtlasGenerator gen;

        // gen.addEntry("resources/smoke.png");
        // gen.addEntry("resources/opengl_logo_white.png");

        // auto engine = AppKit::GLEngine::Engine::Instance();
        // spriteAtlas = gen.generateAtlas(*resourceMap, engine->sRGBCapable, true, 10)[0];

        // /mnt/d/shared/papercat/stages_gltf/stage3_04.bams

        // Texture_Guard[0] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__diffuse.jpg", true && engine->sRGBCapable);
        // Texture_Guard[1] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__normal.jpg", false);
        // Texture_Guard[2] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__specular.jpg", false);

        // Model_Palace = resourceHelper->createTransformFromModel("resources/palace/colonnato.min.bams", resourceMap, resourceMap->defaultPBRMaterial);

        // return Basof2ToResource::loadAndConvert(path.c_str(), resourceMap, defaultPBRMaterial, nullptr, model_dynamic_upload, model_static_upload);

        SmartImporter::ModelSmasher smasher;
#if defined(__linux__)
        const char *home = std::getenv("HOME");
        std::string inputPath = std::string(home ? home : "") + "/Documents/papercat/stages_gltf/stage3_04.bams";
        auto path = std::unique_ptr<char, decltype(&std::free)>(realpath(inputPath.c_str(), nullptr), &std::free);
        smasher.load(path ? path.get() : inputPath.c_str());
#else
        smasher.load("/mnt/d/shared/papercat/stages_gltf/stage3_04.bams");
#endif
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
        {
            auto mainCamera = root->findTransformByName("Main Camera");

            mainCamera->setLocalPosition(vec3f(0, 0, -10));

            camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
            componentCameraOrthographic->sizeY = 5;
        }

        sceneNode = root->findTransformByName("scene");

        auto rect = renderWindow->CameraViewport.c_ptr();
        resize(vec2i(rect->w, rect->h));

        // Add AABB for all meshs...
        {
            resourceHelper->addAABBMesh(root);
        }

        this->OnUpdate.add(&MainScene::update, this);
    }

    // clear all loaded scene
    void MainScene::unloadAll()
    {
        this->OnUpdate.remove(&MainScene::update, this);

        root = nullptr;
        camera = nullptr;

        sceneNode = nullptr;
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