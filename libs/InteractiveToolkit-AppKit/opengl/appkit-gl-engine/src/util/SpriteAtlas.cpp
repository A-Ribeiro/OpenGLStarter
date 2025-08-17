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
            throw std::runtime_error("Sprite not found in atlas: " + name);
        }
        const void SpriteAtlas::addSprite(const std::string &name, const Entry &entry)
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

        std::shared_ptr<SpriteAtlas> SpriteAtlasGenerator::generateAtlas(const ResourceMap &resourceMap, bool sRGB, bool use_fast_positioning, int spaceBetweenSpites_px)
        {
            std::shared_ptr<SpriteAtlas> result = std::make_shared<SpriteAtlas>();
            result->texture = std::make_shared<AppKit::OpenGL::GLTexture>();

            ITKExtension::Atlas::Atlas atlas(spaceBetweenSpites_px, spaceBetweenSpites_px);

            struct Combined_entry
            {
                const std::string &name;
                const GeneratorEntry &genEntry;
                ITKExtension::Atlas::AtlasElement *atlasElementFace;
            };
            std::vector<Combined_entry> combinedEntries;

            for (const auto &entry : entries)
            {
                const std::string &name = entry.first;
                const GeneratorEntry &genEntry = entry.second;

                auto path = resourceMap.dir.getBasePath() + genEntry.texture_path;

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

                if (channels != 4 || depth != 8)
                    throw std::runtime_error("Invalid image format for sprite '" + name + "': expected RGBA 8-bit");

                auto *atlasElementFace = atlas.addElement(name, w, h);
                atlasElementFace->copyFromRGBABuffer((uint8_t *)buffer, w * 4);

                combinedEntries.push_back({name, genEntry, atlasElementFace});
            }

            atlas.organizePositions(use_fast_positioning);

            auto rgba = atlas.createRGBA();
            result->texture->uploadBufferRGBA_8888(
                (const void *)rgba,
                atlas.textureResolution.w,
                atlas.textureResolution.h,
                sRGB);
            atlas.releaseRGBA(&rgba);

            MathCore::vec2f atlasSize_inv = 1.0f / MathCore::vec2f(atlas.textureResolution.w, atlas.textureResolution.h);

            for (const auto &item : combinedEntries)
            {

                SpriteAtlas::Entry atlasEntry;
                atlasEntry.spriteSize = MathCore::vec2f(item.atlasElementFace->rect.w, item.atlasElementFace->rect.w);
                // atlasEntry.spritePivot = item.genEntry.pivot;

                atlasEntry.uvMin = MathCore::vec2f(item.atlasElementFace->rect.x,
                                                   item.atlasElementFace->rect.y);
                atlasEntry.uvMax = atlasEntry.uvMin + atlasEntry.spriteSize;

                atlasEntry.uvMin *= atlasSize_inv;
                atlasEntry.uvMax *= atlasSize_inv;

                result->addSprite(item.name, atlasEntry);
            }

            return result;
        }

    }

}