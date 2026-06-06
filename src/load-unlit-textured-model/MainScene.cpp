#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>
#include "components/ComponentGrow.h"

#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>

#include <InteractiveToolkit/ITKCommon/FileSystem/File.h>
#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>

// #include <InteractiveToolkit-Extension/encoding/HexString.h>
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
    struct Material_UUID_Descriptor
    {
        std::string uuid;

        // if has sprite from atlas
        std::shared_ptr<SpriteAtlas> atlas;
        std::string sprite_atlas_entry_name;
        AppKit::GLEngine::SpriteAtlas::Entry sprite_atlas_entry;

        // if needs to load texture from file
        std::string texture_path;
        ITKExtension::Model::TextureMapMode texture_s_wrap;
        ITKExtension::Model::TextureMapMode texture_t_wrap;
    };

    class ModelSmasher
    {
        struct InternalData
        {
            std::unique_ptr<ITKExtension::Model::ModelContainer> container;
            std::unordered_map<const ITKExtension::Model::Geometry *, bool> geometryProcessed;

            std::unordered_map<std::string, bool> texturesToInsertIntoAtlas;
            std::vector<std::shared_ptr<AppKit::GLEngine::SpriteAtlas>> generatedAtlases;

            std::unordered_map<std::string, std::shared_ptr<Components::ComponentMaterial>> material_uuid_to_instance;
            std::unordered_map<const ITKExtension::Model::Geometry *, std::shared_ptr<Components::ComponentMesh>> geometry_ptr_to_instance;
        };

        std::unique_ptr<InternalData> data;

        int textureInsertIntoAtlasBelowEqual;
        int textureAtlasMaxDimension;

        std::string path_textures;
        ITKCommon::FileSystem::File inputFile;

        ResourceMap *resourceMap;

        static std::shared_ptr<Components::ComponentMaterial> createMaterial(ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info)
        {
            // bool is_opaque = mat->is_opaque();
            bool is_unlit = mat->is_unlit();
            // bool is_two_sided = mat->is_two_sided();

            if (is_unlit)
                return createUnlitMaterial(resourceMap, texture_base_path, mat, uuid_texture_info);
            else
                return createPBRMaterial(resourceMap, texture_base_path, mat, uuid_texture_info);
        }

        static std::shared_ptr<Components::ComponentMaterial> createUnlitMaterial(ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info)
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

            if (uuid_texture_info.atlas != nullptr)
                material->property_bag.getProperty("uTexture").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)uuid_texture_info.atlas->texture);
            else if (!uuid_texture_info.texture_path.empty())
            {
                auto engine = AppKit::GLEngine::Engine::Instance();
                auto tex = resourceMap->getTexture(uuid_texture_info.texture_path, engine->sRGBCapable, texture_base_path);
                material->property_bag.getProperty("uTexture").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)tex);


                using namespace ITKExtension::Model;
                if (uuid_texture_info.texture_s_wrap == TextureMapMode_Wrap || uuid_texture_info.texture_t_wrap == TextureMapMode_Wrap)
                {
                    tex->active(0);
                    if (uuid_texture_info.texture_s_wrap == TextureMapMode_Wrap)
                        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
                    if (uuid_texture_info.texture_t_wrap == TextureMapMode_Wrap)
                        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
                    tex->deactive(0);
                }
            }

            // resourceMap->getTexture()

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

        static std::shared_ptr<Components::ComponentMaterial> createPBRMaterial(ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info)
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

            if (uuid_texture_info.atlas != nullptr)
                material->property_bag.getProperty("texAlbedo").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)uuid_texture_info.atlas->texture);
            else if (!uuid_texture_info.texture_path.empty())
            {
                auto engine = AppKit::GLEngine::Engine::Instance();
                auto tex = resourceMap->getTexture(uuid_texture_info.texture_path, engine->sRGBCapable, texture_base_path);
                material->property_bag.getProperty("texAlbedo").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)tex);
            }

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

        Material_UUID_Descriptor computeMaterialUUID(const ITKExtension::Model::Geometry *geom, const ITKExtension::Model::Material *mat)
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

            Material_UUID_Descriptor result;

            std::vector<uint8_t> info_bytes;
            info_bytes.insert(info_bytes.end(), (uint8_t *)&m_bytes, (uint8_t *)&m_bytes + sizeof(m_bytes));

            // difuse_texture part
            auto diffuse_tex = std::find_if(mat->textures.begin(), mat->textures.end(), [](const ITKExtension::Model::Texture &tex)
                                            { return tex.type == ITKExtension::Model::TextureType::TextureType_DIFFUSE; });
            std::string texture_to_use;
            if (diffuse_tex != mat->textures.end())
            {
                std::string filename = diffuse_tex->filename + "." + diffuse_tex->fileext;
                // std::string full_filename = path_textures + filename;

                if (geom->is_uv_compatible_with_texture_atlas(diffuse_tex->uvIndex))
                {
                    // in this case, search for the atlas...

                    for (size_t i = 0; i < data->generatedAtlases.size(); i++)
                    {
                        if (data->generatedAtlases[i]->hasSprite(filename))
                        {
                            texture_to_use = ITKCommon::PrintfToStdString("/atlas-%zu/%i%i", i, (uint8_t)ITKExtension::Model::TextureMapMode_Clamp, (uint8_t)ITKExtension::Model::TextureMapMode_Clamp);
                            result.atlas = data->generatedAtlases[i];
                            result.sprite_atlas_entry = data->generatedAtlases[i]->getSprite(filename);
                            result.sprite_atlas_entry_name = filename;
                            break;
                        }
                    }
                }

                if (texture_to_use.empty())
                {
                    // in this case, create the single texture
                    

                    result.texture_path = filename;
                    result.texture_s_wrap = diffuse_tex->mapMode_s;
                    result.texture_t_wrap = diffuse_tex->mapMode_t;

                    texture_to_use = ITKCommon::PrintfToStdString("/%s/%i%i", filename.c_str(), (uint8_t)diffuse_tex->mapMode_s, (uint8_t)diffuse_tex->mapMode_t);

                }
            }

            if (texture_to_use.empty())
            {
                const char *slash = "/";
                info_bytes.insert(info_bytes.end(), (uint8_t *)slash, (uint8_t *)slash + sizeof(char) * strlen(slash));
            }
            else
                info_bytes.insert(info_bytes.end(), (uint8_t *)texture_to_use.c_str(), (uint8_t *)texture_to_use.c_str() + sizeof(char) * texture_to_use.size());

            std::string out_string_b64;
            ITKExtension::Encoding::Base64::EncodeToString(info_bytes.data(), info_bytes.size(), &out_string_b64);

            result.uuid = out_string_b64;

            return result;
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

        // void traverse(const ITKExtension::Model::Node &node, int lvl = 0)
        // {
        //     auto localPosition = node.getLocalPosition();
        //     auto localScale = node.getLocalScale();
        //     auto localRotation = node.getLocalRotation();

        //     printf("%*s(%s)\n", lvl * 2, "+", node.name.c_str());

        //     for (uint32_t gidx : node.geometries)
        //     {
        //         const ITKExtension::Model::Geometry *geom = &container->geometries[gidx];
        //         const ITKExtension::Model::Material *mat = &container->materials[geom->materialIndex];

        //         if (geometryProcessed.find(geom) != geometryProcessed.end())
        //         {
        //             printf("%*sGeometry %s is a repeated geometry\n", lvl * 2, "", geom->name.c_str());
        //             continue;
        //         }

        //         if (geom->indiceCountPerFace == 2)
        //         {
        //             printf("%*sGeometry %s is a line mesh\n", lvl * 2, "", geom->name.c_str());
        //         }
        //         else if (geom->indiceCountPerFace == 3)
        //         {
        //             printf("%*sGeometry %s is a triangle mesh\n", lvl * 2, "", geom->name.c_str());

        //             printMaterialInfo(mat, lvl * 2);

        //             printf("\nChecking if geometry %s is compatible with texture atlas...\n\n", geom->name.c_str());
        //             for (const auto &tex : mat->textures)
        //             {
        //                 bool compatible_with_texture_atlas = geom->is_uv_compatible_with_texture_atlas(tex.uvIndex);
        //                 printf("%*sMaterial Texture %s: %s.%s (Wrap Mode: %s %s %s) (Compatible with texture atlas: %s)\n", lvl * 2, "",
        //                        TextureTypeToStr(tex.type),
        //                        tex.filename.c_str(), tex.fileext.c_str(),
        //                        TextureMapModeToStr(tex.mapMode_s), TextureMapModeToStr(tex.mapMode_t), TextureMapModeToStr(tex.mapMode_r),
        //                        compatible_with_texture_atlas ? "YES" : "NO");
        //             }

        //             printf("\n");
        //         }
        //         else
        //         {
        //             printf("%*sGeometry %s is a unknown mesh with %d indices per face\n", lvl * 2, "", geom->name.c_str(), geom->indiceCountPerFace);
        //             continue;
        //         }

        //         geometryProcessed[geom] = true;
        //     }

        //     for (uint32_t child_index : node.children)
        //         traverse(container->nodes[child_index], lvl + 1);
        // }

        static void getImageDimension(const char *path, int *out_w, int *out_h)
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
            ITK_ABORT(node.geometries.size() >= 2, "Node %s has %zu geometries. Max allowed is 1\n", node.name.c_str(), node.geometries.size());
            for (uint32_t gidx : node.geometries)
            {
                const ITKExtension::Model::Geometry *geom = &data->container->geometries[gidx];
                const ITKExtension::Model::Material *mat = &data->container->materials[geom->materialIndex];

                // process only triangle meshes for texture atlas compatibility, skip lines and points
                if (geom->indiceCountPerFace != 3)
                    continue;

                // skip already processed geometries (for example, if the same geometry is instanced multiple times in the scene graph)
                if (data->geometryProcessed.find(geom) != data->geometryProcessed.end())
                    continue;

                auto diffuse = std::find_if(mat->textures.begin(), mat->textures.end(), [](const ITKExtension::Model::Texture &tex)
                                            { return tex.type == ITKExtension::Model::TextureType::TextureType_DIFFUSE; });
                if (diffuse != mat->textures.end() && geom->is_uv_compatible_with_texture_atlas(diffuse->uvIndex))
                {
                    std::string filename = diffuse->filename + "." + diffuse->fileext;
                    std::string full_filename = path_textures + filename;
                    // std::string filename = path_textures + diffuse->filename + "." + diffuse->fileext;
                    if (data->texturesToInsertIntoAtlas.find(filename) == data->texturesToInsertIntoAtlas.end())
                    {
                        int w, h;
                        getImageDimension(full_filename.c_str(), &w, &h);
                        if (w <= textureInsertIntoAtlasBelowEqual && h <= textureInsertIntoAtlasBelowEqual)
                            data->texturesToInsertIntoAtlas[filename] = true;
                    }
                }

                data->geometryProcessed[geom] = true;
            }

            for (uint32_t child_index : node.children)
                traverse_select_textures_for_atlas(data->container->nodes[child_index], lvl + 1);
        }

        void traverse_generate_materials_and_geometries(const ITKExtension::Model::Node &node, int lvl = 0)
        {
            ITK_ABORT(node.geometries.size() >= 2, "Node %s has %zu geometries. Max allowed is 1\n", node.name.c_str(), node.geometries.size());
            for (uint32_t gidx : node.geometries)
            {
                const ITKExtension::Model::Geometry *geom = &data->container->geometries[gidx];
                const ITKExtension::Model::Material *mat = &data->container->materials[geom->materialIndex];

                // process only triangle meshes for texture atlas compatibility, skip lines and points
                if (geom->indiceCountPerFace != 3)
                    continue;

                // skip already processed geometries (for example, if the same geometry is instanced multiple times in the scene graph)
                if (data->geometry_ptr_to_instance.find(geom) != data->geometry_ptr_to_instance.end())
                    continue;

                auto uuid_texture_info = computeMaterialUUID(geom, mat);

                auto it_mat = data->material_uuid_to_instance.find(uuid_texture_info.uuid);
                if (it_mat == data->material_uuid_to_instance.end())
                {
                    auto material = createMaterial(resourceMap, path_textures.c_str(), mat, uuid_texture_info);
                    data->material_uuid_to_instance[uuid_texture_info.uuid] = material;

                    printf(" uuid: %s (%zu)\n", uuid_texture_info.uuid.c_str(), uuid_texture_info.uuid.size());

                    std::vector<uint8_t> out_string_b64;
                    ITKExtension::Encoding::Base64::DecodeToVector(uuid_texture_info.uuid, &out_string_b64);
                    std::string decoded_str(out_string_b64.begin() + 37, out_string_b64.end());
                    printf("     decoded path: %s\n", decoded_str.c_str());
                }

                // generate the geometry according the uuid_texture_info

                auto mesh = Component::CreateShared<Components::ComponentMesh>();
                if (geom->pos.size() > 0)
                    mesh->pos = geom->pos;
                if (geom->normals.size() > 0)
                    mesh->normals = geom->normals;
                if (geom->tangent.size() > 0)
                    mesh->tangent = geom->tangent;
                if (geom->binormal.size() > 0)
                    mesh->binormal = geom->binormal;
                for (int j = 0; j < 8; j++)
                {
                    if (geom->uv[j].size() > 0)
                        mesh->uv[j] = geom->uv[j];
                    if (geom->color[j].size() > 0)
                        mesh->color[j] = geom->color[j];
                }
                mesh->indices = geom->indice;
                //invert face culling order to match OpenGL's default (counter-clockwise)
                // for (size_t i = 0; i < mesh->indices.size(); i += 3)
                //     std::swap(mesh->indices[i + 1], mesh->indices[i + 2]);
                // if (model_dynamic_upload != 0 || model_static_upload != 0)
                //     mesh->syncVBO(model_dynamic_upload, model_static_upload);
                if (geom->bones.size() > 0)
                    mesh->bones = geom->bones;

                if (uuid_texture_info.atlas != nullptr)
                {

                    /*

                    mesh->pos.clear();
                mesh->pos.push_back(size * MathCore::vec3f(1, 1, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(1, 0, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(0, 0, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(0, 1, 0.0f));

                mesh->uv[0].clear();
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(entry.uvMax.x, entry.uvMin.y), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(entry.uvMax.x, entry.uvMax.y), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(entry.uvMin.x, entry.uvMax.y), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(entry.uvMin.x, entry.uvMin.y), 0));
                     */

                     
                    // if (uuid_texture_info.sprite_atlas_entry_name == "c.png") {
                    //     printf("    - atlas entry: %s\n", uuid_texture_info.sprite_atlas_entry_name.c_str());
                    //     vec2f uv_max = uuid_texture_info.sprite_atlas_entry.lerpUV(1.0f, 1.0f);
                    //     printf("      - uv_max: %f, %f\n", uv_max.x, uv_max.y);
                    //     vec2f uv_min = uuid_texture_info.sprite_atlas_entry.lerpUV(0.0f, 0.0f);
                    //     printf("      - uv_min: %f, %f\n", uv_min.x, uv_min.y);
                    // }
                    
                    for(auto &uv : mesh->uv[0])
                        uv = vec3f(uuid_texture_info.sprite_atlas_entry.lerpUV(uv.x, uv.y), 0.0f);

                }

                data->geometry_ptr_to_instance[geom] = mesh;
            }

            for (uint32_t child_index : node.children)
                traverse_generate_materials_and_geometries(data->container->nodes[child_index], lvl + 1);
        }


        std::shared_ptr<Transform> traverse_generate_scene_graph(const ITKExtension::Model::Node &node, int lvl = 0)
        {
            std::shared_ptr<Transform> result = Transform::CreateShared(node.name);
            result->setLocalPosition(node.getLocalPosition());
            result->setLocalRotation(node.getLocalRotation());
            result->setLocalScale(node.getLocalScale());

            // printf("%*sNode %s:\n"
            //     "%*spos (%f, %f, %f)\n"
            //     "%*srot (%f, %f, %f, %f)\n"
            //     "%*sscale (%f, %f, %f)\n", lvl * 2, "+", node.name.c_str(),
            //        (lvl) * 2, "", node.getLocalPosition().x, node.getLocalPosition().y, node.getLocalPosition().z,
            //        (lvl) * 2, "", node.getLocalRotation().x, node.getLocalRotation().y, node.getLocalRotation().z, node.getLocalRotation().w,
            //        (lvl) * 2, "", node.getLocalScale().x, node.getLocalScale().y, node.getLocalScale().z);

            ITK_ABORT(node.geometries.size() >= 2, "Node %s has %zu geometries. Max allowed is 1\n", node.name.c_str(), node.geometries.size());
            for (uint32_t gidx : node.geometries)
            {
                const ITKExtension::Model::Geometry *geom = &data->container->geometries[gidx];
                const ITKExtension::Model::Material *mat = &data->container->materials[geom->materialIndex];

                // process only triangle meshes for texture atlas compatibility, skip lines and points
                if (geom->indiceCountPerFace != 3)
                    continue;

                printf("%*sAdding geometry %s with material %s\n", lvl * 2, "", geom->name.c_str(), mat->name.c_str());

                auto uuid_texture_info = computeMaterialUUID(geom, mat);

                auto material_it = data->material_uuid_to_instance.find(uuid_texture_info.uuid);
                if (material_it == data->material_uuid_to_instance.end())
                    throw std::runtime_error(ITKCommon::PrintfToStdString("Material instance not found for material UUID '%s'", uuid_texture_info.uuid.c_str()));

                auto mesh_it = data->geometry_ptr_to_instance.find(geom);
                if (mesh_it == data->geometry_ptr_to_instance.end())
                    throw std::runtime_error(ITKCommon::PrintfToStdString("Geometry instance not found for geometry '%s'", geom->name.c_str()));

                auto material = material_it->second;
                auto mesh = mesh_it->second;

                auto meshWrapper = result->addNewComponent<Components::ComponentMeshWrapper>();
                result->addComponent(material);
                result->addComponent(mesh);
                meshWrapper->updateMeshAABB(true);

            }

            for (uint32_t child_index : node.children)
                result->addChild(traverse_generate_scene_graph(data->container->nodes[child_index], lvl + 1));

            return result;
        }

    public:
        std::shared_ptr<Transform> load(const char *filename,
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

            data = STL_Tools::make_unique<InternalData>();

            data->container = STL_Tools::make_unique<ITKExtension::Model::ModelContainer>();
            data->container->read(filename);

            data->geometryProcessed.clear();
            data->texturesToInsertIntoAtlas.clear();
            traverse_select_textures_for_atlas(data->container->nodes[root_index]);

            printf("Textures to insert into atlas:\n\n");
            for (const auto &tex : data->texturesToInsertIntoAtlas)
                printf("- %s\n", tex.first.c_str());
            printf("\n");

            SpriteAtlasGenerator gen;

            for (const auto &tex : data->texturesToInsertIntoAtlas)
                gen.addEntry(tex.first.c_str());

            auto engine = AppKit::GLEngine::Engine::Instance();
            data->generatedAtlases = gen.generateAtlas(path_textures, *resourceMap, engine->sRGBCapable, true, 10, this->textureAtlasMaxDimension);
            // data->generatedAtlases.clear();

            printf("\nGenerated %zu sprite atlases:\n\n", data->generatedAtlases.size());
            for (size_t i = 0; i < data->generatedAtlases.size(); i++)
            {
                const auto &atlas = data->generatedAtlases[i];
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
            printf("\nGenerated Materials And Geometries\n\n");

            data->material_uuid_to_instance.clear();
            data->geometry_ptr_to_instance.clear();

            traverse_generate_materials_and_geometries(data->container->nodes[root_index]);

            printf("\n\n");

            auto result = traverse_generate_scene_graph(data->container->nodes[root_index]);

            // clean up
            data.reset();

            return result;
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
        // const char *home = std::getenv("HOME");
        // std::string inputPath = std::string(home ? home : "") + "/Documents/papercat/stages_gltf/stage3_04.bams";
        // auto path = std::unique_ptr<char, decltype(&std::free)>(realpath(inputPath.c_str(), nullptr), &std::free);
        // smasher.load(path ? path.get() : inputPath.c_str(), resourceMap);
        loadedScene = smasher.load("/mnt/d/shared/papercat/stages_gltf/stage3_04.bams", resourceMap);
#else
        loadedScene = smasher.load("D:/shared/papercat/stages_gltf/stage3_04.bams", resourceMap);
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
        auto mainCamera = root->findTransformByName("Main Camera");
        {
            camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
            componentCameraOrthographic->useSizeY = true;
            componentCameraOrthographic->sizeY = 10.0f;
        }

        sceneNode = root->findTransformByName("scene");

        loadedScene->setLocalRotation(quatf());

        sceneNode->addChild(loadedScene);

        auto player_pos = sceneNode->findTransformByName("Player")->getPosition();

        mainCamera->setLocalPosition(vec3f(player_pos.x, player_pos.y, -10));

        // auto rect = renderWindow->CameraViewport.c_ptr();
        // // auto *rect = &componentCameraOrthographic->viewport;
        // resize(vec2i(rect->w, rect->h));

        // // Add AABB for all meshs...
        // {
        //     resourceHelper->addAABBMesh(root);
        // }

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