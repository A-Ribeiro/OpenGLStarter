#pragma once

#include <appkit-gl-engine/util/SpriteAtlas.h>
#include <appkit-gl-engine/Components/2d/common.h>

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
            class ComponentCamera;
            class ComponentSprite;
        }
    }

    namespace GLEngine
    {
        class SpritePool
        {

            // private constructor to force usage of CreateShared
            SpritePool();

            struct _internal_data
            {
                std::unordered_map<std::string, bool> sprite_list;
                bool try_to_keep_in_same_atlas;
            };

            std::vector<_internal_data> sprite_data;

            AppKit::GLEngine::ResourceMap *resourceMap;

        public:
            std::vector<std::shared_ptr<AppKit::GLEngine::SpriteAtlas>> atlases;

            AppKit::GLEngine::ResourceMap *getResourceMap() const;
            
            void clear();

            void addSprites(const std::vector<std::string> &sprite_list, bool try_to_keep_in_same_atlas = true);

            void generateAtlases(AppKit::GLEngine::ResourceMap *resourceMap, int maxAtlasResolution = 4096);

            std::shared_ptr<AppKit::GLEngine::SpriteAtlas> getAtlasContainingSprite(const std::string &sprite_name);

            void configureComponentSprite(
                std::shared_ptr<AppKit::GLEngine::Components::ComponentSprite> target,
                const std::string &name,
                const MathCore::vec2f &pivot,
                const MathCore::vec4f &color,
                const MathCore::vec2f &size_constraint,
                bool x_invert,
                bool y_invert = false,
                AppKit::GLEngine::Components::MeshUploadMode meshUploadMode = AppKit::GLEngine::Components::MeshUploadMode::MeshUploadMode_Dynamic_OnClone_NoModify);

            ITK_DECLARE_CREATE_SHARED(SpritePool)
        };
        
    }
}