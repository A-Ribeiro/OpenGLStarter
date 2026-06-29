#include <appkit-gl-engine/util/SpritePool.h>

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/ResourceMap.h>

#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>
#include <appkit-gl-engine/Components/Core/ComponentMesh.h>

#include <appkit-gl-engine/Components/2d/ComponentSprite.h>

#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {
        SpritePool::SpritePool()
        {
            resourceMap = nullptr;
        }

        AppKit::GLEngine::ResourceMap *SpritePool::getResourceMap() const
        {
            return resourceMap;
        }

        void SpritePool::clear()
        {
            sprite_data.clear();
            atlases.clear();
        }

        void SpritePool::addSprites(const std::vector<std::string> &sprite_list, bool try_to_keep_in_same_atlas)
        {
            _internal_data data;
            for (const auto &sprite : sprite_list)
                data.sprite_list[sprite] = true;
            data.try_to_keep_in_same_atlas = try_to_keep_in_same_atlas;
            sprite_data.push_back(data);
        }

        void SpritePool::generateAtlases(AppKit::GLEngine::ResourceMap *resourceMap, int maxAtlasResolution, bool crop_alpha)
        {
            this->resourceMap = resourceMap;
            atlases.clear();

            auto engine = AppKit::GLEngine::Engine::Instance();

            SpriteAtlasGenerator gen;

            std::unordered_map<std::string, bool> global_sprite_list;

            // add all keep in same atlas = false first
            for (const auto &sprite_list_entry : sprite_data)
            {
                if (sprite_list_entry.try_to_keep_in_same_atlas)
                    continue;
                for (const auto &sprite : sprite_list_entry.sprite_list)
                {
                    if (global_sprite_list.find(sprite.first) != global_sprite_list.end())
                        continue;
                    global_sprite_list[sprite.first] = true;
                    gen.addEntry(sprite.first.c_str());
                }
            }

            for (const auto &sprite_list_entry : sprite_data)
            {
                if (!sprite_list_entry.try_to_keep_in_same_atlas)
                    continue;

                int atlases_count_before = gen.estimateGeneratedAtlasCount(resourceMap->dir.getBasePath(), *resourceMap, engine->sRGBCapable, true, 10, maxAtlasResolution, crop_alpha);

                for (const auto &sprite : sprite_list_entry.sprite_list)
                {
                    if (global_sprite_list.find(sprite.first) != global_sprite_list.end())
                        throw std::runtime_error("Sprite Pool Generation Names Cannot Be Duplicated: " + sprite.first);
                    global_sprite_list[sprite.first] = true;
                    gen.addEntry(sprite.first.c_str());
                }

                int atlases_count_after = gen.estimateGeneratedAtlasCount(resourceMap->dir.getBasePath(), *resourceMap, engine->sRGBCapable, true, 10, maxAtlasResolution, crop_alpha);

                if (atlases_count_before > 0 && atlases_count_after > atlases_count_before)
                {
                    // force the generation of the current sprite list into a single atlas
                    for (const auto &sprite : sprite_list_entry.sprite_list)
                        gen.removeEntry(sprite.first.c_str());

                    auto atlases_no_last_insert = gen.generateAtlas(resourceMap->dir.getBasePath(), *resourceMap, engine->sRGBCapable, true, 10, maxAtlasResolution, crop_alpha);
                    atlases.insert(atlases.end(), atlases_no_last_insert.begin(), atlases_no_last_insert.end());

                    gen.clear();
                    for (const auto &sprite : sprite_list_entry.sprite_list)
                        gen.addEntry(sprite.first.c_str());
                }
                else if (atlases_count_before == 0 && atlases_count_after > 1)
                    throw std::runtime_error("Sprite Pool Generation Failed: Sprites that are marked to be kept in the same atlas cannot fit into a single atlas. Please reduce the number of sprites or increase the maximum atlas resolution.");
            }

            auto last_atlases = gen.generateAtlas(resourceMap->dir.getBasePath(), *resourceMap, engine->sRGBCapable, true, 10, maxAtlasResolution, crop_alpha);
            atlases.insert(atlases.end(), last_atlases.begin(), last_atlases.end());

            printf("\n[SpritePool] Generated %zu sprite atlases:\n\n", atlases.size());
            for (size_t i = 0; i < atlases.size(); i++)
            {
                const auto &atlas = atlases[i];
                printf("- Atlas %zu: %d x %d, contains %zu textures:\n", i, atlas->texture->width, atlas->texture->height, atlas->sprites.size());
                for (const auto &entry : atlas->sprites)
                    printf("  - %s: uvMin (%f, %f) uvMax (%f, %f) spriteSize (%.0f, %.0f)\n",
                           entry.first.c_str(),
                           entry.second.uvMin.x, entry.second.uvMin.y,
                           entry.second.uvMax.x, entry.second.uvMax.y,
                           entry.second.spriteRealSize.x, entry.second.spriteRealSize.y);
            }
            printf("\n\n");
        }

        std::shared_ptr<AppKit::GLEngine::SpriteAtlas> SpritePool::getAtlasContainingSprite(const std::string &sprite_name)
        {
            for (const auto &atlas : atlases)
            {
                if (atlas->hasSprite(sprite_name))
                    return atlas;
            }
            return nullptr;
        }

        void SpritePool::configureComponentSprite(
            std::shared_ptr<AppKit::GLEngine::Components::ComponentSprite> target,
            const std::string &name,
            const MathCore::vec2f &pivot,
            const MathCore::vec4f &color,
            const MathCore::vec2f &size_constraint,
            bool x_invert,
            bool y_invert,
            AppKit::GLEngine::Components::MeshUploadMode meshUploadMode)
        {
            for (const auto &atlas : atlases)
            {
                if (!atlas->hasSprite(name))
                    continue;

                target->setTextureFromAtlas(
                    resourceMap,
                    atlas,
                    name,
                    pivot,
                    color,
                    size_constraint, // = MathCore::vec2f(-1, -1),
                    x_invert,        // = false,
                    y_invert,        // = false,
                    meshUploadMode   //= MeshUploadMode_Static
                );

                return;
            }

            throw std::runtime_error("Sprite not found in any atlas: " + name);
        }

    }
}