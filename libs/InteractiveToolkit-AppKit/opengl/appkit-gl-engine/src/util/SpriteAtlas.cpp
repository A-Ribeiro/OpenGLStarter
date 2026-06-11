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

        std::vector<std::shared_ptr<SpriteAtlas>> SpriteAtlasGenerator::generateAtlas(const std::string &base_path,
                                                                                      const ResourceMap &resourceMap, bool sRGB, bool use_fast_positioning, int spaceBetweenSpites_px, int max_atlas_size)
        {
            std::vector<std::shared_ptr<SpriteAtlas>> result_array;

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

                int w, h, channels, depth;
                bool invertY = false;

                bool isPNG = ITKExtension::Image::PNG::isPNGFilename(path.c_str());
                bool isJPG = ITKExtension::Image::JPG::isJPGFilename(path.c_str());

                void (*closeFnc)(char *&) = nullptr;

                char *buffer = nullptr;
                if (isPNG)
                {
                    buffer = ITKExtension::Image::PNG::readPNG(path.c_str(), &w, &h, &channels, &depth, invertY);
                    closeFnc = &ITKExtension::Image::PNG::closePNG;
                }
                else if (isJPG)
                {
                    buffer = ITKExtension::Image::JPG::readJPG(path.c_str(), &w, &h, &channels, &depth, invertY);
                    closeFnc = &ITKExtension::Image::JPG::closeJPG;
                }

                if (buffer == nullptr)
                    throw std::runtime_error("Error loading texture: " + path);

                EventCore::ExecuteOnScopeEnd closeBuffer(
                    [&buffer, closeFnc]()
                    {
                        if (closeFnc)
                            closeFnc(buffer);
                    });

                if ((channels != 4 && channels != 3 && channels != 1) || depth != 8)
                    throw std::runtime_error("Invalid image format for sprite '" + name + "': expected Gray, RGB, RGBA 8-bit");

                if (channel_used == -1)
                    channel_used = channels;

                auto atlasElementFace = atlas->addElement(name, w, h);

                atlas->organizePositions(use_fast_positioning);
                if (combinedEntries.size() > 0 && (atlas->textureResolution.w > max_atlas_size || atlas->textureResolution.h > max_atlas_size))
                {
                    // flush atlas and start a new one
                    atlas->removeLastInsertedElement();
                    atlas->organizePositions(use_fast_positioning);

                    if (channel_used == 4)
                    {
                        auto rgba = atlas->createRGBA();
                        result_single->texture->uploadBufferRGBA_8888(
                            (const void *)rgba.get(),
                            atlas->textureResolution.w,
                            atlas->textureResolution.h,
                            sRGB);
                        // atlas.releaseRGBA(&rgba);
                    }
                    else if (channel_used == 3)
                    {
                        auto rgba = atlas->createRGB();
                        result_single->texture->uploadBufferRGB_888(
                            (const void *)rgba.get(),
                            atlas->textureResolution.w,
                            atlas->textureResolution.h,
                            sRGB);
                        // atlas.releaseRGBA(&rgba);
                    }
                    else if (channel_used == 1)
                    {
                        auto rgba = atlas->createGray();
                        result_single->texture->uploadBufferRed8(
                            (const void *)rgba.get(),
                            atlas->textureResolution.w,
                            atlas->textureResolution.h);
                        // atlas.releaseRGBA(&rgba);
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
                    atlasElementFace = atlas->addElement(name, w, h);

                    combinedEntries.clear();
                }

                if (channels == 4)
                    atlasElementFace->copyFromRGBABuffer((uint8_t *)buffer, w * 4);
                else if (channels == 3)
                    atlasElementFace->copyFromRGBBuffer((uint8_t *)buffer);
                else if (channels == 1)
                    atlasElementFace->copyFromGrayBuffer((uint8_t *)buffer);

                combinedEntries.push_back({name, genEntry, atlasElementFace.get()});
            }

            atlas->organizePositions(use_fast_positioning);

            if (channel_used == 4 || channel_used == -1)
            {
                auto rgba = atlas->createRGBA();
                result_single->texture->uploadBufferRGBA_8888(
                    (const void *)rgba.get(),
                    atlas->textureResolution.w,
                    atlas->textureResolution.h,
                    sRGB);
                // atlas.releaseRGBA(&rgba);
            }
            else if (channel_used == 3)
            {
                auto rgba = atlas->createRGB();
                result_single->texture->uploadBufferRGB_888(
                    (const void *)rgba.get(),
                    atlas->textureResolution.w,
                    atlas->textureResolution.h,
                    sRGB);
                // atlas.releaseRGBA(&rgba);
            }
            else if (channel_used == 1)
            {
                auto rgba = atlas->createGray();
                result_single->texture->uploadBufferRed8(
                    (const void *)rgba.get(),
                    atlas->textureResolution.w,
                    atlas->textureResolution.h);
                // atlas.releaseRGBA(&rgba);
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

    }

}