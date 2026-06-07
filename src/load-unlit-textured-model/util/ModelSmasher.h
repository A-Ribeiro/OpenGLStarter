#pragma once

#include <InteractiveToolkit/common.h>
#include <appkit-gl-engine/util/SpriteAtlas.h>
#include <InteractiveToolkit-Extension/model/Texture.h>

// #include <appkit-gl-engine/Components/Core/ComponentMaterial.h>
// #include <appkit-gl-engine/Components/Core/ComponentMesh.h>
// #include <appkit-gl-engine/ResourceMap.h>
// #include <appkit-gl-engine/Transform.h>

namespace ITKExtension
{
    namespace Model
    {
        class ModelContainer;
        class Material;
        class Geometry;
        class Node;
    }
}

namespace AppKit
{
    namespace GLEngine
    {
        class Transform;
        class ResourceMap;
        namespace Components
        {
            class ComponentMaterial;
            class ComponentMesh;
        }
    }
}

namespace SmartImporter
{
    struct Material_UUID_Descriptor
    {
        std::string uuid;

        // if has sprite from atlas
        std::shared_ptr<AppKit::GLEngine::SpriteAtlas> atlas;
        std::string sprite_atlas_entry_name;
        AppKit::GLEngine::SpriteAtlas::Entry sprite_atlas_entry;

        // if needs to load texture from file
        std::string texture_path;
        ITKExtension::Model::TextureMapMode texture_s_wrap;
        ITKExtension::Model::TextureMapMode texture_t_wrap;
    };

    struct InternalData;

    class ModelSmasher
    {
        std::shared_ptr<InternalData> data;

        int textureInsertIntoAtlasBelowEqual;
        int textureAtlasMaxDimension;

        std::string path_textures;
        ITKCommon::FileSystem::File inputFile;

        AppKit::GLEngine::ResourceMap *resourceMap;

        static std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> createMaterial(AppKit::GLEngine::ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info);
        static std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> createUnlitMaterial(AppKit::GLEngine::ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info);
        static std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> createPBRMaterial(AppKit::GLEngine::ResourceMap *resourceMap, const char *texture_base_path, const ITKExtension::Model::Material *mat, const Material_UUID_Descriptor &uuid_texture_info);
        void printMaterialInfo(const ITKExtension::Model::Material *mat, int lvl);

        Material_UUID_Descriptor computeMaterialUUID(const ITKExtension::Model::Geometry *geom, const ITKExtension::Model::Material *mat);
        std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> createLineMaterial(const ITKExtension::Model::Material *mat);

        static void getImageDimension(const char *path, int *out_w, int *out_h);
        void traverse_select_textures_for_atlas(const ITKExtension::Model::Node &node, int lvl = 0);
        void traverse_generate_materials_and_geometries(const ITKExtension::Model::Node &node, int lvl = 0);
        std::shared_ptr<AppKit::GLEngine::Transform> traverse_generate_scene_graph(const ITKExtension::Model::Node &node, int lvl = 0);

    public:
        std::shared_ptr<AppKit::GLEngine::Transform> load(const char *filename,
                                        AppKit::GLEngine::ResourceMap *resourceMap,
                                        int textureInsertIntoAtlasBelowEqual = 2048, int textureAtlasMaxDimension = 4096,
                                        const char *path_textures_param = nullptr);
    };

}
