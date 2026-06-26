#include <appkit-gl-engine/util/SpriteAtlas.h>
#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>

#include <InteractiveToolkit/EventCore/ExecuteOnScopeEnd.h>

namespace AppKit
{
    namespace GLEngine
    {
        SpriteAtlas::SpriteAtlas() : texture(nullptr) {}
        SpriteAtlas::~SpriteAtlas() {}

        void SpriteAtlas::clear()
        {
            texture = nullptr;
            sprites.clear();
        }
        bool SpriteAtlas::isEmpty() const
        {
            return texture == nullptr || sprites.empty();
        }
        bool SpriteAtlas::hasSprite(const std::string &name) const
        {
            return sprites.find(name) != sprites.end();
        }
        const SpriteAtlas::Entry &SpriteAtlas::getSprite(const std::string &name) const
        {
            auto it = sprites.find(name);
            if (it != sprites.end())
                return it->second;
            static SpriteAtlas::Entry dummy{
                MathCore::vec2f(1, 1), // atlasEntry.spriteSize = ;
                MathCore::vec2f(0, 0), // atlasEntry.uvMin = ;
                MathCore::vec2f(0, 0), // atlasEntry.uvMax = ;
            };
            return dummy;
            // throw std::runtime_error("Sprite not found in atlas: " + name);
        }
        void SpriteAtlas::addSprite(const std::string &name, const Entry &entry)
        {
            if (sprites.find(name) != sprites.end())
                throw std::runtime_error("Sprite already exists in atlas: " + name);
            sprites[name] = entry;
        }

        void SpriteAtlasGenerator::addEntry(const std::string &texture_path)
        {
            if (entries.find(texture_path) != entries.end())
                throw std::runtime_error("Entry already exists: " + texture_path);
            entries[texture_path] = {texture_path};
        }

        void SpriteAtlasGenerator::removeEntry(const std::string &name)
        {
            auto it = entries.find(name);
            if (it != entries.end())
                entries.erase(it);
            else
                throw std::runtime_error("Entry not found: " + name);
        }

        void SpriteAtlasGenerator::clear()
        {
            entries.clear();
        }

        struct ImageBuffer
        {
            int w, h, channels, depth;
            std::unique_ptr<char, void (*)(char *)> buffer;
        };

        std::shared_ptr<ImageBuffer> smartLoadImage(const char *path)
        {
            auto imageBuffer = std::make_shared<ImageBuffer>();

            bool invertY = false;

            if (ITKExtension::Image::PNG::isPNGFilename(path))
                imageBuffer->buffer = std::unique_ptr<char, void (*)(char *)>(ITKExtension::Image::PNG::readPNG(path, &imageBuffer->w, &imageBuffer->h, &imageBuffer->channels, &imageBuffer->depth, invertY),
                                                                              [](char *ptr)
                                                                              { if(ptr) {char *aux = ptr;ITKExtension::Image::PNG::closePNG(aux); } });
            else if (ITKExtension::Image::JPG::isJPGFilename(path))
                imageBuffer->buffer = std::unique_ptr<char, void (*)(char *)>(ITKExtension::Image::JPG::readJPG(path, &imageBuffer->w, &imageBuffer->h, &imageBuffer->channels, &imageBuffer->depth, invertY),
                                                                              [](char *ptr)
                                                                              { if(ptr) {char *aux = ptr;ITKExtension::Image::JPG::closeJPG(aux); } });
            if (!imageBuffer->buffer)
                return nullptr;

            return imageBuffer;
        }

        std::vector<std::shared_ptr<SpriteAtlas>> SpriteAtlasGenerator::generateAtlas(const std::string &base_path,
                                                                                      const ResourceMap &resourceMap, bool sRGB, bool use_fast_positioning, int spaceBetweenSpites_px, int max_atlas_size)
        {
            std::vector<std::shared_ptr<SpriteAtlas>> result_array;

            if (entries.empty())
                return result_array;

            std::shared_ptr<SpriteAtlas> result_single = std::make_shared<SpriteAtlas>();
            result_single->texture = std::make_shared<AppKit::OpenGL::GLTexture>();

            std::unique_ptr<ITKExtension::Atlas::Atlas> atlas = STL_Tools::make_unique<ITKExtension::Atlas::Atlas>(spaceBetweenSpites_px, spaceBetweenSpites_px);

            struct Combined_entry
            {
                const std::string &name;
                const GeneratorEntry &genEntry;
                ITKExtension::Atlas::AtlasElement *atlasElementFace;
            };
            std::vector<Combined_entry> combinedEntries;

            int channel_used = -1;

            for (const auto &entry : entries)
            {
                const std::string &name = entry.first;
                const GeneratorEntry &genEntry = entry.second;

                // auto path = resourceMap.dir.getBasePath() + genEntry.texture_path;
                auto path = base_path + genEntry.texture_path;

                // int w, h, channels, depth;
                bool invertY = false;

                auto imageBuffer = smartLoadImage(path.c_str());
                if (!imageBuffer)
                    throw std::runtime_error("Failed to load image for sprite '" + name + "': " + path);

                if ((imageBuffer->channels != 4 && imageBuffer->channels != 3 && imageBuffer->channels != 1) || imageBuffer->depth != 8)
                    throw std::runtime_error("Invalid image format for sprite '" + name + "': expected Gray, RGB, RGBA 8-bit");

                if (channel_used == -1)
                    channel_used = imageBuffer->channels;

                auto atlasElementFace = atlas->addElement(name, imageBuffer->w, imageBuffer->h);

                atlas->organizePositions(use_fast_positioning);
                if (combinedEntries.size() > 0 && (atlas->textureResolution.w > max_atlas_size || atlas->textureResolution.h > max_atlas_size))
                {
                    // flush atlas and start a new one
                    atlas->removeLastInsertedElement();
                    atlas->organizePositions(use_fast_positioning);

                    if (channel_used == 4)
                    {
                        auto rgba = atlas->createRGBA();
                        result_single->texture->uploadBufferRGBA_8888((const void *)rgba.get(), atlas->textureResolution.w, atlas->textureResolution.h, sRGB);
                    }
                    else if (channel_used == 3)
                    {
                        auto rgba = atlas->createRGB();
                        result_single->texture->uploadBufferRGB_888((const void *)rgba.get(), atlas->textureResolution.w, atlas->textureResolution.h, sRGB);
                    }
                    else if (channel_used == 1)
                    {
                        auto rgba = atlas->createGray();
                        result_single->texture->uploadBufferRed8((const void *)rgba.get(), atlas->textureResolution.w, atlas->textureResolution.h);
                    }

                    MathCore::vec2f atlasSize_inv = 1.0f / MathCore::vec2f(atlas->textureResolution.w, atlas->textureResolution.h);
                    for (const auto &item : combinedEntries)
                    {
                        SpriteAtlas::Entry atlasEntry;
                        atlasEntry.spriteSize = MathCore::vec2f(item.atlasElementFace->rect.w, item.atlasElementFace->rect.h);
                        atlasEntry.uvMin = MathCore::vec2f(item.atlasElementFace->rect.x,
                                                           item.atlasElementFace->rect.y);
                        atlasEntry.uvMax = atlasEntry.uvMin + atlasEntry.spriteSize;
                        atlasEntry.uvMin *= atlasSize_inv;
                        atlasEntry.uvMax *= atlasSize_inv;
                        result_single->addSprite(item.name, atlasEntry);
                    }
                    result_array.push_back(result_single);

                    atlas = STL_Tools::make_unique<ITKExtension::Atlas::Atlas>(spaceBetweenSpites_px, spaceBetweenSpites_px);

                    result_single = std::make_shared<SpriteAtlas>();
                    result_single->texture = std::make_shared<AppKit::OpenGL::GLTexture>();
                    atlasElementFace = atlas->addElement(name, imageBuffer->w, imageBuffer->h);

                    combinedEntries.clear();
                }

                if (imageBuffer->channels == 4)
                    atlasElementFace->copyFromRGBABuffer((uint8_t *)imageBuffer->buffer.get(), imageBuffer->w * 4);
                else if (imageBuffer->channels == 3)
                    atlasElementFace->copyFromRGBBuffer((uint8_t *)imageBuffer->buffer.get());
                else if (imageBuffer->channels == 1)
                    atlasElementFace->copyFromGrayBuffer((uint8_t *)imageBuffer->buffer.get());

                combinedEntries.push_back({name, genEntry, atlasElementFace.get()});
            }

            atlas->organizePositions(use_fast_positioning);

            if (channel_used == 4 || channel_used == -1)
            {
                auto rgba = atlas->createRGBA();
                result_single->texture->uploadBufferRGBA_8888((const void *)rgba.get(), atlas->textureResolution.w, atlas->textureResolution.h, sRGB);
            }
            else if (channel_used == 3)
            {
                auto rgba = atlas->createRGB();
                result_single->texture->uploadBufferRGB_888((const void *)rgba.get(), atlas->textureResolution.w, atlas->textureResolution.h, sRGB);
            }
            else if (channel_used == 1)
            {
                auto rgba = atlas->createGray();
                result_single->texture->uploadBufferRed8((const void *)rgba.get(), atlas->textureResolution.w, atlas->textureResolution.h);
            }

            MathCore::vec2f atlasSize_inv = 1.0f / MathCore::vec2f(atlas->textureResolution.w, atlas->textureResolution.h);
            for (const auto &item : combinedEntries)
            {
                SpriteAtlas::Entry atlasEntry;
                atlasEntry.spriteSize = MathCore::vec2f(item.atlasElementFace->rect.w, item.atlasElementFace->rect.h);
                atlasEntry.uvMin = MathCore::vec2f(item.atlasElementFace->rect.x,
                                                   item.atlasElementFace->rect.y);
                atlasEntry.uvMax = atlasEntry.uvMin + atlasEntry.spriteSize;
                atlasEntry.uvMin *= atlasSize_inv;
                atlasEntry.uvMax *= atlasSize_inv;
                result_single->addSprite(item.name, atlasEntry);
            }
            result_array.push_back(result_single);

            return result_array;
        }

        int SpriteAtlasGenerator::estimateGeneratedAtlasCount(
            const std::string &base_path,
            const ResourceMap &resourceMap,
            bool sRGB,
            bool use_fast_positioning,
            int spaceBetweenSpites_px,
            int max_atlas_size)
        {
            std::vector<std::shared_ptr<SpriteAtlas>> result_array;

            if (entries.empty())
                return 0;

            int count_atlas = 0;

            std::unique_ptr<ITKExtension::Atlas::Atlas> atlas = STL_Tools::make_unique<ITKExtension::Atlas::Atlas>(spaceBetweenSpites_px, spaceBetweenSpites_px);

            struct Combined_entry
            {
                const std::string &name;
                const GeneratorEntry &genEntry;
                ITKExtension::Atlas::AtlasElement *atlasElementFace;
            };
            std::vector<Combined_entry> combinedEntries;

            int channel_used = -1;

            for (const auto &entry : entries)
            {
                const std::string &name = entry.first;
                const GeneratorEntry &genEntry = entry.second;

                auto path = base_path + genEntry.texture_path;

                bool invertY = false;

                auto imageBuffer = smartLoadImage(path.c_str());
                if (!imageBuffer)
                    throw std::runtime_error("Failed to load image for sprite '" + name + "': " + path);

                if ((imageBuffer->channels != 4 && imageBuffer->channels != 3 && imageBuffer->channels != 1) || imageBuffer->depth != 8)
                    throw std::runtime_error("Invalid image format for sprite '" + name + "': expected Gray, RGB, RGBA 8-bit");

                if (channel_used == -1)
                    channel_used = imageBuffer->channels;

                auto atlasElementFace = atlas->addElement(name, imageBuffer->w, imageBuffer->h);

                atlas->organizePositions(use_fast_positioning);
                if (combinedEntries.size() > 0 && (atlas->textureResolution.w > max_atlas_size || atlas->textureResolution.h > max_atlas_size))
                {
                    // flush atlas and start a new one
                    atlas->removeLastInsertedElement();
                    atlas->organizePositions(use_fast_positioning);
                    count_atlas++;

                    atlas = STL_Tools::make_unique<ITKExtension::Atlas::Atlas>(spaceBetweenSpites_px, spaceBetweenSpites_px);

                    atlasElementFace = atlas->addElement(name, imageBuffer->w, imageBuffer->h);
                    combinedEntries.clear();
                }

                combinedEntries.push_back({name, genEntry, atlasElementFace.get()});
            }

            atlas->organizePositions(use_fast_positioning);
            count_atlas++;

            return count_atlas;
        }

    }

}