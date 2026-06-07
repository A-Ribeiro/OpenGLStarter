#include "ModelSmasher.h"

// #include <InteractiveToolkit/common.h>
// #include <appkit-gl-engine/util/SpriteAtlas.h>
// #include <InteractiveToolkit-Extension/model/Texture.h>

#include <InteractiveToolkit-Extension/model/ModelContainer.h>

#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>
#include <appkit-gl-engine/Components/Core/ComponentMesh.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>
#include <appkit-gl-engine/ResourceMap.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Component.h>

#include <InteractiveToolkit-Extension/encoding/Base64.h>

#include <InteractiveToolkit-Extension/image/PNG.h>
#include <InteractiveToolkit-Extension/image/JPG.h>

#include <InteractiveToolkit/ITKCommon/FileSystem/File.h>
#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>

namespace SmartImporter
{
    struct InternalData
    {
        std::unique_ptr<ITKExtension::Model::ModelContainer> container;
        std::unordered_map<const ITKExtension::Model::Geometry *, bool> geometryProcessed;

        std::unordered_map<std::string, bool> diffuseTexturesToInsertIntoAtlas;
        std::vector<std::shared_ptr<AppKit::GLEngine::SpriteAtlas>> diffuseAtlases;

        std::unordered_map<std::string, std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial>> material_uuid_to_instance;
        std::unordered_map<const ITKExtension::Model::Geometry *, std::shared_ptr<AppKit::GLEngine::Components::ComponentMesh>> geometry_ptr_to_instance;
    };

    std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> ModelSmasher::createMaterial(AppKit::GLEngine::ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info)
    {
        // bool is_opaque = mat->is_opaque();
        bool is_unlit = mat->is_unlit();
        // bool is_two_sided = mat->is_two_sided();

        if (is_unlit)
            return createUnlitMaterial(resourceMap, texture_base_path, mat, uuid_texture_info);
        else
            return createPBRMaterial(resourceMap, texture_base_path, mat, uuid_texture_info);
    }

    std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> ModelSmasher::createUnlitMaterial(AppKit::GLEngine::ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info)
    {
        using namespace AppKit::GLEngine;
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

    std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> ModelSmasher::createPBRMaterial(AppKit::GLEngine::ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info)
    {
        using namespace AppKit::GLEngine;
        using namespace AppKit::GLEngine::Components;
        using namespace MathCore;

        bool is_opaque = mat->is_opaque();
        // bool is_unlit = mat->is_unlit();
        // bool is_two_sided = mat->is_two_sided();

        auto material = Component::CreateShared<Components::ComponentMaterial>();
        material->setShader(resourceMap->pbrShaderSelector);

        auto diffuse = mat->vec4Value.find("diffuse");
        if (diffuse != mat->vec4Value.end())
            material->property_bag.getProperty("albedoColor").set<vec3f>(CVT<vec4f>::toVec3(diffuse->second));

        auto emissive = mat->vec4Value.find("emissive");
        if (emissive != mat->vec4Value.end())
            material->property_bag.getProperty("emissionColor").set<vec3f>(CVT<vec4f>::toVec3(emissive->second));

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

    void ModelSmasher::printMaterialInfo(const ITKExtension::Model::Material *mat, int lvl)
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

    Material_UUID_Descriptor ModelSmasher::computeMaterialUUID(const ITKExtension::Model::Geometry *geom, const ITKExtension::Model::Material *mat)
    {
        using namespace AppKit::GLEngine;
        using namespace AppKit::GLEngine::Components;
        using namespace MathCore;

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

        // diffuse_texture part
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

                for (size_t i = 0; i < data->diffuseAtlases.size(); i++)
                {
                    if (data->diffuseAtlases[i]->hasSprite(filename))
                    {
                        texture_to_use = ITKCommon::PrintfToStdString("/atlas-%zu/%i%i", i, (uint8_t)ITKExtension::Model::TextureMapMode_Clamp, (uint8_t)ITKExtension::Model::TextureMapMode_Clamp);
                        result.atlas = data->diffuseAtlases[i];
                        result.sprite_atlas_entry = data->diffuseAtlases[i]->getSprite(filename);
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

    std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> ModelSmasher::createLineMaterial(const ITKExtension::Model::Material *mat)
    {
        using namespace AppKit::GLEngine;
        using namespace AppKit::GLEngine::Components;
        using namespace MathCore;

        // bool is_opaque = mat->is_opaque();
        // bool is_unlit = mat->is_unlit();
        // bool is_two_sided = mat->is_two_sided();

        auto material = Component::CreateShared<Components::ComponentMaterial>();
        material->setShader(resourceMap->lineShader);

        auto diffuse = mat->vec4Value.find("diffuse");
        if (diffuse != mat->vec4Value.end())
            material->property_bag.getProperty("uColor").set<vec4f>(diffuse->second);

        return material;
    }

    void ModelSmasher::getImageDimension(const char *path, int *out_w, int *out_h)
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

    void ModelSmasher::traverse_select_textures_for_atlas(const ITKExtension::Model::Node &node, int lvl)
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
                if (data->diffuseTexturesToInsertIntoAtlas.find(filename) == data->diffuseTexturesToInsertIntoAtlas.end())
                {
                    int w, h;
                    getImageDimension(full_filename.c_str(), &w, &h);
                    if (w <= textureInsertIntoAtlasBelowEqual && h <= textureInsertIntoAtlasBelowEqual)
                        data->diffuseTexturesToInsertIntoAtlas[filename] = true;
                }
            }

            data->geometryProcessed[geom] = true;
        }

        for (uint32_t child_index : node.children)
            traverse_select_textures_for_atlas(data->container->nodes[child_index], lvl + 1);
    }

    void ModelSmasher::traverse_generate_materials_and_geometries(const ITKExtension::Model::Node &node, int lvl)
    {
        using namespace AppKit::GLEngine;
        using namespace AppKit::GLEngine::Components;
        using namespace MathCore;

        ITK_ABORT(node.geometries.size() >= 2, "Node %s has %zu geometries. Max allowed is 1\n", node.name.c_str(), node.geometries.size());
        for (uint32_t gidx : node.geometries)
        {
            const ITKExtension::Model::Geometry *geom = &data->container->geometries[gidx];
            const ITKExtension::Model::Material *mat = &data->container->materials[geom->materialIndex];

            // process only triangle meshes for texture atlas compatibility, skip lines and points
            if (geom->indiceCountPerFace == 3)
            {
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
                // invert face culling order to match OpenGL's default (counter-clockwise)
                //  for (size_t i = 0; i < mesh->indices.size(); i += 3)
                //      std::swap(mesh->indices[i + 1], mesh->indices[i + 2]);
                //  if (model_dynamic_upload != 0 || model_static_upload != 0)
                //      mesh->syncVBO(model_dynamic_upload, model_static_upload);
                if (geom->bones.size() > 0)
                    mesh->bones = geom->bones;

                if (uuid_texture_info.atlas != nullptr)
                {
                    for (auto &uv : mesh->uv[0])
                        uv = vec3f(uuid_texture_info.sprite_atlas_entry.lerpUV(uv.x, uv.y), 0.0f);
                }

                data->geometry_ptr_to_instance[geom] = mesh;
            }
            else if (geom->indiceCountPerFace == 2)
            {
                // lines... use line rendering...
            }
        }

        for (uint32_t child_index : node.children)
            traverse_generate_materials_and_geometries(data->container->nodes[child_index], lvl + 1);
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ModelSmasher::traverse_generate_scene_graph(const ITKExtension::Model::Node &node, int lvl)
    {
        using namespace AppKit::GLEngine;
        using namespace AppKit::GLEngine::Components;
        using namespace MathCore;

        std::shared_ptr<Transform> result = Transform::CreateShared(node.name);
        result->setLocalPosition(node.getLocalPosition());
        result->setLocalRotation(node.getLocalRotation());
        result->setLocalScale(node.getLocalScale());

        printf("%*sNode %s:\n"
               "%*spos (%f, %f, %f)\n"
               "%*srot (%f, %f, %f, %f)\n"
               "%*sscale (%f, %f, %f)\n",
               lvl * 2, "+", node.name.c_str(),
               (lvl) * 2, "", node.getLocalPosition().x, node.getLocalPosition().y, node.getLocalPosition().z,
               (lvl) * 2, "", node.getLocalRotation().x, node.getLocalRotation().y, node.getLocalRotation().z, node.getLocalRotation().w,
               (lvl) * 2, "", node.getLocalScale().x, node.getLocalScale().y, node.getLocalScale().z);

        ITK_ABORT(node.geometries.size() >= 2, "Node %s has %zu geometries. Max allowed is 1\n", node.name.c_str(), node.geometries.size());
        for (uint32_t gidx : node.geometries)
        {
            const ITKExtension::Model::Geometry *geom = &data->container->geometries[gidx];
            const ITKExtension::Model::Material *mat = &data->container->materials[geom->materialIndex];

            // process only triangle meshes for texture atlas compatibility, skip lines and points
            if (geom->indiceCountPerFace == 3)
            {
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
            else if (geom->indiceCountPerFace == 2)
            {
                // lines... use line rendering...
            }
        }

        for (uint32_t child_index : node.children)
            result->addChild(traverse_generate_scene_graph(data->container->nodes[child_index], lvl + 1));

        return result;
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ModelSmasher::load(const char *filename,
                                                                    AppKit::GLEngine::ResourceMap *resourceMap,
                                                                    int textureInsertIntoAtlasBelowEqual, int textureAtlasMaxDimension,
                                                                    const char *path_textures_param)
    {
        using namespace AppKit::GLEngine;
        using namespace AppKit::GLEngine::Components;
        using namespace MathCore;

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

        data = std::make_shared<InternalData>();

        data->container = STL_Tools::make_unique<ITKExtension::Model::ModelContainer>();
        data->container->read(filename);

        data->geometryProcessed.clear();
        data->diffuseTexturesToInsertIntoAtlas.clear();
        traverse_select_textures_for_atlas(data->container->nodes[root_index]);

        printf("Textures to insert into atlas:\n\n");
        for (const auto &tex : data->diffuseTexturesToInsertIntoAtlas)
            printf("- %s\n", tex.first.c_str());
        printf("\n");

        SpriteAtlasGenerator gen;

        for (const auto &tex : data->diffuseTexturesToInsertIntoAtlas)
            gen.addEntry(tex.first.c_str());

        auto engine = AppKit::GLEngine::Engine::Instance();
        data->diffuseAtlases = gen.generateAtlas(path_textures, *resourceMap, engine->sRGBCapable, true, 10, this->textureAtlasMaxDimension);
        // data->generatedAtlases.clear();

        printf("\nGenerated %zu sprite atlases:\n\n", data->diffuseAtlases.size());
        for (size_t i = 0; i < data->diffuseAtlases.size(); i++)
        {
            const auto &atlas = data->diffuseAtlases[i];
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

        result->setLocalRotation(GEN<quatf>::fromEuler(0, OP<float>::deg_2_rad(180.0f), 0) * result->getLocalRotation());

        return result;
    }
}
