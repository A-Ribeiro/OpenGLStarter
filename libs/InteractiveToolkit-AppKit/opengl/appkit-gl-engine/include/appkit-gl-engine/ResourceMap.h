#pragma once

#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLCubeMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        struct ResourceSet
        {
            std::unordered_map<uint64_t, std::shared_ptr<AppKit::OpenGL::GLTexture>> texture_map;
            std::unordered_map<uint64_t, std::shared_ptr<AppKit::OpenGL::GLCubeMap>> cubemap_map;
        };

        class ResourceMap
        {
            std::weak_ptr<ResourceMap> mSelf;

            struct Texture2DInfo
            {
                std::string relative_path;
                bool is_srgb;
                std::shared_ptr<AppKit::OpenGL::GLTexture> tex;
            };
            std::unordered_map<std::string, Texture2DInfo> texture2DMap;

            struct CubemapInfo
            {
                std::string relative_path;
                bool is_srgb;
                int maxResolution;
                std::shared_ptr<AppKit::OpenGL::GLCubeMap> cubemap;
            };
            std::unordered_map<std::string, CubemapInfo> cubemapMap;

        public:
            static inline std::shared_ptr<ResourceMap> CreateShared()
            {
                auto result = std::make_shared<ResourceMap>();
                result->mSelf = std::weak_ptr<ResourceMap>(result);
                return result;
            }

            inline std::shared_ptr<ResourceMap> self()
            {
                return ToShared(mSelf);
            }

            ITKCommon::FileSystem::Directory dir;

            void setProjectBaseFolder(const std::string &path);

            ITKCommon::FileSystem::File getFile(const std::string &relative_path);

            void clear_refcount_equals_1();
            void clear();

            void ensure_default_texture_creation();

            std::shared_ptr<AppKit::OpenGL::GLTexture> getTexture(const std::string &relative_path, bool is_srgb);

            std::shared_ptr<AppKit::OpenGL::GLCubeMap> getCubeMap(const std::string &relative_path, bool is_srgb, int maxResolution);

            void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
            void Deserialize(rapidjson::Value &_value, ResourceSet *resourceSetOutput);
        };
    }
}