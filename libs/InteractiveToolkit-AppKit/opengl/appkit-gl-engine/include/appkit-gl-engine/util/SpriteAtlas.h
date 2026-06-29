#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-base/GLTexture.h>

// #include <appkit-gl-engine/Component.h>
// #include <appkit-gl-engine/Transform.h>
// #include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/ResourceMap.h>


namespace AppKit
{
    namespace GLEngine
    {
        class SpriteAtlas
        {
        public:
            std::shared_ptr<AppKit::OpenGL::GLTexture> texture;
            struct Entry
            {
                MathCore::vec2f spriteRealSize;
                MathCore::vec2f spriteCroppedSize;
                MathCore::vec2f spriteCroppedMin_OriginBottom;

                MathCore::vec2f transform_crop_vert_scale;
                MathCore::vec2f transform_crop_vert_translate;

                // MathCore::vec2f spritePivot;
                MathCore::vec2f uvMin;
                MathCore::vec2f uvMax;

                // MathCore::vec3f uvQuad[4];

                MathCore::vec2f lerpUV(float u, float v) const
                {
                    return MathCore::vec2f(
                        MathCore::OP<float>::lerp(uvMin.x, uvMax.x, u),
                        MathCore::OP<float>::lerp(uvMin.y, uvMax.y, v)
                    );
                }
            };
            std::unordered_map<std::string, Entry> sprites;

            SpriteAtlas();
            ~SpriteAtlas();

            void clear();
            bool isEmpty() const;
            bool hasSprite(const std::string &name) const;
            const Entry &getSprite(const std::string &name) const;
            void addSprite(const std::string &name, const Entry &entry);
        };

        class SpriteAtlasGenerator
        {
        public:
            struct GeneratorEntry
            {
                // std::shared_ptr<AppKit::OpenGL::GLTexture> texture;
                std::string texture_path;
                // MathCore::vec2f pivot;
            };
            std::unordered_map<std::string, GeneratorEntry> entries;

            void addEntry(const std::string &texture_path);
            // ,const MathCore::vec2f &pivot = MathCore::vec2f(0.5f, 0.5f));

            void removeEntry(const std::string &name);

            void clear();

            std::vector<std::shared_ptr<SpriteAtlas>> generateAtlas(
                const std::string &base_path,
                const ResourceMap &resourceMap, 
                bool sRGB, 
                bool use_fast_positioning = true, 
                int spaceBetweenSpites_px = 10,
                int max_atlas_size = 4096,
                bool crop_alpha = false
            );


            int estimateGeneratedAtlasCount(
                const std::string &base_path,
                const ResourceMap &resourceMap, 
                bool sRGB, 
                bool use_fast_positioning = true, 
                int spaceBetweenSpites_px = 10,
                int max_atlas_size = 4096,
                bool crop_alpha = false
            );
        };
    }

}