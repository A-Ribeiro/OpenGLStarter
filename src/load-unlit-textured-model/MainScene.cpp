#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>
#include "components/ComponentGrow.h"

#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>

#include <InteractiveToolkit/ITKCommon/FileSystem/File.h>
#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>

#include <InteractiveToolkit-Extension/encoding/HexString.h>
#include <InteractiveToolkit-Extension/encoding/Base64.h>

#include <cstdlib>
#include <string>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;
using namespace ITKCommon;

#include <InteractiveToolkit-Extension/model/ModelContainer.h>

namespace SmartImporter
{
    class ModelSmasher
    {
        std::unique_ptr<ITKExtension::Model::ModelContainer> container;
        std::unordered_map<const ITKExtension::Model::Geometry *, bool> geometryProcessed;

        int textureInsertIntoAtlasBelowEqual;
        int textureAtlasMaxDimension;
        std::unordered_map<std::string, bool> texturesToInsertIntoAtlas;

        std::vector<std::shared_ptr<AppKit::GLEngine::SpriteAtlas>> generatedAtlases;

        std::unordered_map<std::string, bool> material_uuid_to_instance;

        std::string path_textures;

        ITKCommon::FileSystem::File inputFile;

        ResourceMap *resourceMap;

        std::shared_ptr<Components::ComponentMaterial> createMaterial(const ITKExtension::Model::Material *mat)
        {
            // bool is_opaque = mat->is_opaque();
            bool is_unlit = mat->is_unlit();
            // bool is_two_sided = mat->is_two_sided();

            if (is_unlit)
                return createUnlitMaterial(mat);
            else
                return createPBRMaterial(mat);
        }

        /*
        Material Float gltf.alphaCutoff: 0.500000
    Material Float opacity: 1.000000
    Material Float roughnessFactor: 1.000000
    Material Float shininess: 0.000000
    Material Float metallicFactor: 0.000000
    Material Vec4 emissive: 0.000000, 0.000000, 0.000000, 1.000000
    Material Vec4 base: 1.000000, 1.000000, 1.000000, 1.000000
    Material Vec4 diffuse: 1.000000, 1.000000, 1.000000, 1.000000
    Material Int gltf.unlit: 1
    Material Int twosided: 0
    Material Texture diffuse: stageBrick.png (Compatible with texture atlas: NO)
    Material mat_stageBrick is opaque: YES
         */

        void printMaterialInfo(const ITKExtension::Model::Material *mat, int lvl)
        {
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
                // bool compatible_with_texture_atlas = geom->is_uv_compatible_with_texture_atlas(tex.uvIndex);
                printf("%*sMaterial Texture %s: %s.%s (Wrap Mode: %s %s %s)\n", lvl * 2, "",
                       TextureTypeToStr(tex.type),
                       tex.filename.c_str(), tex.fileext.c_str(),
                       TextureMapModeToStr(tex.mapMode_s), TextureMapModeToStr(tex.mapMode_t), TextureMapModeToStr(tex.mapMode_r));
            }

            bool is_opaque = mat->is_opaque();
            bool is_unlit = mat->is_unlit();
            bool is_two_sided = mat->is_two_sided();

            printf("%*sMaterial %s is opaque: %s\n", lvl * 2, "", mat->name.c_str(), is_opaque ? "YES" : "NO");
            printf("%*sMaterial %s is unlit: %s\n", lvl * 2, "", mat->name.c_str(), is_unlit ? "YES" : "NO");
            printf("%*sMaterial %s is two-sided: %s\n", lvl * 2, "", mat->name.c_str(), is_two_sided ? "YES" : "NO");
        }

        std::string computeMaterialUUID(const ITKExtension::Model::Geometry *geom, const ITKExtension::Model::Material *mat)
        {
#pragma pack(push, 1)
            struct material_bytes
            {
                vec4<float, SIMD_TYPE::NONE> diffuse;
                vec3<float, SIMD_TYPE::NONE> emission;
                float roughness;
                float metallic;
                union
                {
                    uint8_t flags;
                    struct
                    {
                        uint8_t is_unlit : 1;
                        uint8_t is_opaque : 1;
                        uint8_t is_two_sided : 1;
                        uint8_t reserved : 5;
                    };
                };
            };
#pragma pack(pop)

            material_bytes m_bytes;
            memset(&m_bytes, 0, sizeof(m_bytes));

            bool is_unlit = mat->is_unlit();
            bool is_opaque = mat->is_opaque();
            bool is_two_sided = mat->is_two_sided();

            m_bytes.is_unlit = is_unlit ? 1 : 0;
            m_bytes.is_opaque = is_opaque ? 1 : 0;
            m_bytes.is_two_sided = is_two_sided ? 1 : 0;

            auto diffuse = mat->vec4Value.find("diffuse");
            if (diffuse != mat->vec4Value.end())
                m_bytes.diffuse = diffuse->second;

            if (!is_unlit)
            {
                auto emissive = mat->vec4Value.find("emissive");
                if (emissive != mat->vec4Value.end())
                    m_bytes.emission = CVT<vec4f>::toVec3(emissive->second);

                auto roughness = mat->floatValue.find("roughnessFactor");
                if (roughness != mat->floatValue.end())
                    m_bytes.roughness = roughness->second;

                auto metallic = mat->floatValue.find("metallicFactor");
                if (metallic != mat->floatValue.end())
                    m_bytes.metallic = metallic->second;
            }

            std::vector<uint8_t> info_bytes;
            info_bytes.insert(info_bytes.end(), (uint8_t *)&m_bytes, (uint8_t *)&m_bytes + sizeof(m_bytes));

            // difuse_texture part
            auto diffuse_tex = std::find_if(mat->textures.begin(), mat->textures.end(), [](const ITKExtension::Model::Texture &tex)
                                            { return tex.type == ITKExtension::Model::TextureType::TextureType_DIFFUSE; });
            if (diffuse_tex != mat->textures.end())
            {
                std::string texture_to_use;
                if (geom->is_uv_compatible_with_texture_atlas(diffuse_tex->uvIndex))
                {
                    texture_to_use = "/atlas";
                    // in this case, search for the atlas...
                }
                else
                {
                    // in this case, create the single texture
                    texture_to_use = "/" + diffuse_tex->filename + "." + diffuse_tex->fileext;
                }

                info_bytes.insert(info_bytes.end(), (uint8_t *)texture_to_use.c_str(), (uint8_t *)texture_to_use.c_str() + sizeof(char) * texture_to_use.size());
            }
            else
            {
                const char *slash = "/";
                info_bytes.insert(info_bytes.end(), (uint8_t *)slash, (uint8_t *)slash + sizeof(char) * strlen(slash));
            }

            std::string out_string_b64;
            ITKExtension::Encoding::Base64::EncodeToString(info_bytes.data(), info_bytes.size(), &out_string_b64);

            return out_string_b64;
        }

        std::shared_ptr<Components::ComponentMaterial> createUnlitMaterial(const ITKExtension::Model::Material *mat)
        {
            using namespace AppKit::GLEngine::Components;

            bool is_opaque = mat->is_opaque();
            // bool is_unlit = mat->is_unlit();
            // bool is_two_sided = mat->is_two_sided();

            auto material = Component::CreateShared<Components::ComponentMaterial>();
            if (is_opaque)
                material->setShader(resourceMap->shaderUnlitTexture);
            else
                material->setShader(resourceMap->shaderUnlitTextureAlpha);

            auto diffuse = mat->vec4Value.find("diffuse");
            if (diffuse != mat->vec4Value.end())
                material->property_bag.getProperty("uColor").set<MathCore::vec4f>(diffuse->second);

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

            bool is_opaque = mat->is_opaque();
            // bool is_unlit = mat->is_unlit();
            // bool is_two_sided = mat->is_two_sided();

            auto material = Component::CreateShared<Components::ComponentMaterial>();
            material->setShader(resourceMap->pbrShaderSelector);

            auto diffuse = mat->vec4Value.find("diffuse");
            if (diffuse != mat->vec4Value.end())
                material->property_bag.getProperty("albedoColor").set<MathCore::vec3f>(CVT<vec4f>::toVec3(diffuse->second));

            auto emissive = mat->vec4Value.find("emissive");
            if (emissive != mat->vec4Value.end())
                material->property_bag.getProperty("emissionColor").set<MathCore::vec3f>(CVT<vec4f>::toVec3(emissive->second));

            auto roughness = mat->floatValue.find("roughnessFactor");
            if (roughness != mat->floatValue.end())
                material->property_bag.getProperty("roughness").set<float>(roughness->second);

            auto metallic = mat->floatValue.find("metallicFactor");
            if (metallic != mat->floatValue.end())
                material->property_bag.getProperty("metallic").set<float>(metallic->second);

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

        std::shared_ptr<Components::ComponentMaterial> createLineMaterial(const ITKExtension::Model::Material *mat)
        {
            using namespace AppKit::GLEngine::Components;

            // bool is_opaque = mat->is_opaque();
            // bool is_unlit = mat->is_unlit();
            // bool is_two_sided = mat->is_two_sided();

            auto material = Component::CreateShared<Components::ComponentMaterial>();
            material->setShader(resourceMap->lineShader);

            auto diffuse = mat->vec4Value.find("diffuse");
            if (diffuse != mat->vec4Value.end())
                material->property_bag.getProperty("uColor").set<MathCore::vec4f>(diffuse->second);

            return material;
        }

        void traverse(const ITKExtension::Model::Node &node, int lvl = 0)
        {
            auto localPosition = node.getLocalPosition();
            auto localScale = node.getLocalScale();
            auto localRotation = node.getLocalRotation();

            printf("%*s(%s)\n", lvl * 2, "+", node.name.c_str());

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

                    printMaterialInfo(mat, lvl * 2);

                    printf("\nChecking if geometry %s is compatible with texture atlas...\n\n", geom->name.c_str());
                    for (const auto &tex : mat->textures)
                    {
                        bool compatible_with_texture_atlas = geom->is_uv_compatible_with_texture_atlas(tex.uvIndex);
                        printf("%*sMaterial Texture %s: %s.%s (Wrap Mode: %s %s %s) (Compatible with texture atlas: %s)\n", lvl * 2, "",
                               TextureTypeToStr(tex.type),
                               tex.filename.c_str(), tex.fileext.c_str(),
                               TextureMapModeToStr(tex.mapMode_s), TextureMapModeToStr(tex.mapMode_t), TextureMapModeToStr(tex.mapMode_r),
                               compatible_with_texture_atlas ? "YES" : "NO");
                    }

                    printf("\n");
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

        void getImageDimension(const char *path, int *out_w, int *out_h)
        {
            int w, h, channels, depth;
            bool invertY = false;

            std::unique_ptr<char, void (*)(char *)> buffer(nullptr, [](char *ptr) {});

            if (ITKExtension::Image::PNG::isPNGFilename(path))
                buffer = std::unique_ptr<char, void (*)(char *)>(ITKExtension::Image::PNG::readPNG(path, &w, &h, &channels, &depth, invertY),
                                                                 [](char *ptr)
                                                                 { if(ptr) {char *aux = ptr;ITKExtension::Image::PNG::closePNG(aux); } });
            else if (ITKExtension::Image::JPG::isJPGFilename(path))
                buffer = std::unique_ptr<char, void (*)(char *)>(ITKExtension::Image::JPG::readJPG(path, &w, &h, &channels, &depth, invertY),
                                                                 [](char *ptr)
                                                                 { if(ptr) {char *aux = ptr;ITKExtension::Image::JPG::closeJPG(aux); } });

            if (buffer == nullptr)
                throw std::runtime_error(ITKCommon::PrintfToStdString("Error loading texture: %s", path));

            if (channels != 4 || depth != 8)
                throw std::runtime_error(ITKCommon::PrintfToStdString("Invalid image format for texture '%s': expected RGBA 8-bit", path));

            *out_w = w;
            *out_h = h;
        }

        void traverse_select_textures_for_atlas(const ITKExtension::Model::Node &node, int lvl = 0)
        {
            for (uint32_t gidx : node.geometries)
            {
                const ITKExtension::Model::Geometry *geom = &container->geometries[gidx];
                const ITKExtension::Model::Material *mat = &container->materials[geom->materialIndex];

                // process only triangle meshes for texture atlas compatibility, skip lines and points
                if (geom->indiceCountPerFace != 3)
                    continue;

                // skip already processed geometries (for example, if the same geometry is instanced multiple times in the scene graph)
                if (geometryProcessed.find(geom) != geometryProcessed.end())
                    continue;

                auto diffuse = std::find_if(mat->textures.begin(), mat->textures.end(), [](const ITKExtension::Model::Texture &tex)
                                            { return tex.type == ITKExtension::Model::TextureType::TextureType_DIFFUSE; });
                if (diffuse != mat->textures.end() && geom->is_uv_compatible_with_texture_atlas(diffuse->uvIndex))
                {
                    std::string filename = path_textures + diffuse->filename + "." + diffuse->fileext;
                    if (texturesToInsertIntoAtlas.find(filename) == texturesToInsertIntoAtlas.end())
                    {
                        int w, h;
                        getImageDimension(filename.c_str(), &w, &h);
                        if (w <= textureInsertIntoAtlasBelowEqual && h <= textureInsertIntoAtlasBelowEqual)
                            texturesToInsertIntoAtlas[filename] = true;
                    }
                }

                geometryProcessed[geom] = true;
            }

            for (uint32_t child_index : node.children)
                traverse_select_textures_for_atlas(container->nodes[child_index], lvl + 1);
        }

        void traverse_generate_materials_and_geometries(const ITKExtension::Model::Node &node, int lvl = 0)
        {

            for (uint32_t gidx : node.geometries)
            {
                const ITKExtension::Model::Geometry *geom = &container->geometries[gidx];
                const ITKExtension::Model::Material *mat = &container->materials[geom->materialIndex];

                // process only triangle meshes for texture atlas compatibility, skip lines and points
                if (geom->indiceCountPerFace != 3)
                    continue;

                // skip already processed geometries (for example, if the same geometry is instanced multiple times in the scene graph)
                if (geometryProcessed.find(geom) != geometryProcessed.end())
                    continue;

                auto uuid = computeMaterialUUID(geom, mat);
                printf(" uuid: %s (%zu)\n", uuid.c_str(), uuid.size());

                std::vector<uint8_t> out_string_b64;
                ITKExtension::Encoding::Base64::DecodeToVector(uuid, &out_string_b64);
                std::string decoded_str(out_string_b64.begin() + 37, out_string_b64.end());

                printf("     decoded path: %s\n", decoded_str.c_str());

                geometryProcessed[geom] = true;
            }

            for (uint32_t child_index : node.children)
                traverse_generate_materials_and_geometries(container->nodes[child_index], lvl + 1);
        }

    public:
        void load(const char *filename,
                  ResourceMap *resourceMap,
                  int textureInsertIntoAtlasBelowEqual = 2048, int textureAtlasMaxDimension = 4096,
                  const char *path_textures_param = nullptr)
        {
            inputFile = ITKCommon::FileSystem::File::FromPath(filename);

            if (!inputFile.isFile)
                throw std::runtime_error(ITKCommon::PrintfToStdString("Input file does not exist: %s", filename));

            const uint32_t root_index = 0;

            this->resourceMap = resourceMap;
            this->textureAtlasMaxDimension = textureAtlasMaxDimension;
            if (path_textures_param)
            {
                auto directory = ITKCommon::FileSystem::Directory(path_textures_param);
                if (!directory)
                    throw std::runtime_error(ITKCommon::PrintfToStdString("Invalid texture directory: %s", path_textures_param));
                this->path_textures = directory.getBasePath();
            }
            else
                this->path_textures = inputFile.base_path;
            this->textureInsertIntoAtlasBelowEqual = textureInsertIntoAtlasBelowEqual;

            container = STL_Tools::make_unique<ITKExtension::Model::ModelContainer>();
            container->read(filename);

            geometryProcessed.clear();
            texturesToInsertIntoAtlas.clear();
            traverse_select_textures_for_atlas(container->nodes[root_index]);

            printf("Textures to insert into atlas:\n\n");
            for (const auto &tex : texturesToInsertIntoAtlas)
                printf("- %s\n", tex.first.c_str());
            printf("\n");

            SpriteAtlasGenerator gen;

            for (const auto &tex : texturesToInsertIntoAtlas)
                gen.addEntry(tex.first.c_str());

            auto engine = AppKit::GLEngine::Engine::Instance();
            generatedAtlases = gen.generateAtlas("", *resourceMap, engine->sRGBCapable, true, 10);

            printf("\nGenerated %zu sprite atlases:\n\n", generatedAtlases.size());
            for (size_t i = 0; i < generatedAtlases.size(); i++)
            {
                const auto &atlas = generatedAtlases[i];
                printf("- Atlas %zu: %d x %d, contains %zu textures:\n", i, atlas->texture->width, atlas->texture->height, atlas->sprites.size());
                for (const auto &entry : atlas->sprites)
                    printf("  - %s: uvMin (%f, %f) uvMax (%f, %f) spriteSize (%.0f, %.0f)\n",
                           entry.first.c_str(),
                           entry.second.uvMin.x, entry.second.uvMin.y,
                           entry.second.uvMax.x, entry.second.uvMax.y,
                           entry.second.spriteSize.x, entry.second.spriteSize.y);
            }
            printf("\n\n");

            // generate materials and process geometries
            geometryProcessed.clear();

            printf("\nGenerated Materials And Geometries\n\n");

            traverse_generate_materials_and_geometries(container->nodes[root_index]);

            printf("\n\n");


            Engine::Instance()->app->exitApp();
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
#if defined(__linux__) && 0
        const char *home = std::getenv("HOME");
        std::string inputPath = std::string(home ? home : "") + "/Documents/papercat/stages_gltf/stage3_04.bams";
        auto path = std::unique_ptr<char, decltype(&std::free)>(realpath(inputPath.c_str(), nullptr), &std::free);
        smasher.load(path ? path.get() : inputPath.c_str(), resourceMap);
#else
        smasher.load("/mnt/d/shared/papercat/stages_gltf/stage3_04.bams", resourceMap);
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