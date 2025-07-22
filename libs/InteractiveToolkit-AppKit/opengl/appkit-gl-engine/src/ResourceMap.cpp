#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        void ResourceMap::setProjectBaseFolder(const std::string &path)
        {
            dir = ITKCommon::FileSystem::Directory(path);
            ITK_ABORT(!dir.isValid(), "invalid project path: %s\n", path.c_str());
        }

        ITKCommon::FileSystem::File ResourceMap::getFile(const std::string &relative_path)
        {
            auto result = ITKCommon::FileSystem::File::FromPath(dir.getBasePath() + relative_path);
            ITK_ABORT(!result.isFile, "invalid file path: %s\n", relative_path.c_str());
            return result;
        }

        void ResourceMap::clear_refcount_equals_1()
        {
            std::vector<std::string> to_remove;

            for (auto &item : texture2DMap)
            {
                if (item.second.tex.use_count() > 1 ||
                    item.second.relative_path.compare("DEFAULT_ALBEDO") == 0 ||
                    item.second.relative_path.compare("DEFAULT_NORMAL") == 0)
                    continue;
                to_remove.push_back(item.first);
            }
            for(const auto &key : to_remove)
                texture2DMap.erase(key);

            to_remove.clear();
            for (auto &item : cubemapMap)
            {
                if (item.second.cubemap.use_count() > 1)
                    continue;
                to_remove.push_back(item.first);
            }
            for(const auto &key : to_remove)
                cubemapMap.erase(key);
        }

        void ResourceMap::clear()
        {
            texture2DMap.clear();
            cubemapMap.clear();
            defaultAlbedoTexture = nullptr;
            defaultNormalTexture = nullptr;
            defaultPBRMaterial = nullptr;
        }

        void ResourceMap::ensure_default_texture_creation()
        {
            if (defaultPBRMaterial == nullptr)
            {
                defaultAlbedoTexture = getTexture("DEFAULT_ALBEDO", true);
                defaultNormalTexture = getTexture("DEFAULT_NORMAL", false);

                defaultPBRMaterial = Component::CreateShared<Components::ComponentMaterial>();
                defaultPBRMaterial->type = Components::MaterialPBR;
                defaultPBRMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
                defaultPBRMaterial->pbr.metallic = 0.0f;
                defaultPBRMaterial->pbr.roughness = 1.0f;
                defaultPBRMaterial->pbr.texAlbedo = defaultAlbedoTexture;
                defaultPBRMaterial->pbr.texNormal = defaultNormalTexture;
            }

        }

        std::shared_ptr<AppKit::OpenGL::GLTexture> ResourceMap::getTexture(const std::string &relative_path, bool is_srgb)
        {
            std::string to_query = ITKCommon::PrintfToStdString("%s:%s", (is_srgb) ? "srgb" : "linear", relative_path.c_str());

            auto tex_it = texture2DMap.find(to_query);
            if (tex_it == texture2DMap.end())
            {
                // need to load
                if (relative_path.compare("DEFAULT_ALBEDO") == 0)
                {
                    auto tex = std::make_shared<AppKit::OpenGL::GLTexture>();
                    {
                        uint32_t tex_data[16 * 16];
                        for (int i = 0; i < 16 * 16; i++)
                            tex_data[i] = 0xffffffff;
                        tex->uploadBufferRGBA_8888(tex_data, 16, 16, is_srgb);
                        tex->active(0);
                        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                        tex->deactive(0);
                    }

                    texture2DMap[to_query] = Texture2DInfo{relative_path,
                                                           is_srgb,
                                                           tex};

                    return tex;
                }
                else if (relative_path.compare("DEFAULT_NORMAL") == 0)
                {
                    auto tex = std::make_shared<AppKit::OpenGL::GLTexture>();
                    {
                        uint32_t tex_data[16 * 16];
                        for (int i = 0; i < 16 * 16; i++)
                            tex_data[i] = 0xffff7f7f;
                        tex->uploadBufferRGBA_8888(tex_data, 16, 16, false);
                        tex->active(0);
                        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                        tex->deactive(0);
                    }

                    texture2DMap[to_query] = Texture2DInfo{relative_path,
                                                           is_srgb,
                                                           tex};

                    return tex;
                }
                else
                {
                    auto file = getFile(relative_path);
                    ITK_ABORT(!file.isFile, "file not found: %s\n", relative_path.c_str());

                    std::string errorStr;
                    Platform::ObjectBuffer buffer;
                    ITK_ABORT(!file.readContentToObjectBuffer(&buffer, &errorStr), "%s", errorStr.c_str());

                    auto ogl_raw_tex = AppKit::OpenGL::GLTexture::loadFromMemory(relative_path.c_str(), (const char *)buffer.data, (int)buffer.size, false, is_srgb);
                    auto tex = std::shared_ptr<AppKit::OpenGL::GLTexture>(ogl_raw_tex);
                    tex->generateMipMap();
                    tex->setAnisioLevel(16.0f);

                    texture2DMap[to_query] = Texture2DInfo{relative_path,
                                                           is_srgb,
                                                           tex};

                    return tex;
                }
            }
            return tex_it->second.tex;
        }

        std::shared_ptr<AppKit::OpenGL::GLCubeMap> ResourceMap::getCubeMap(const std::string &relative_path, bool is_srgb, int _maxResolution)
        {
            std::string to_query = ITKCommon::PrintfToStdString("%s:%s", (is_srgb) ? "srgb" : "linear", relative_path.c_str());

            auto cube_it = cubemapMap.find(to_query);
            if (cube_it == cubemapMap.end())
            {
                // need to load
                GLint maxCubeMapSize;
                glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapSize);

                int maxResolution = _maxResolution;

                if (maxResolution > maxCubeMapSize)
                    maxResolution = maxCubeMapSize;

                auto cubeMap = std::make_shared<AppKit::OpenGL::GLCubeMap>(0, 0, 0xffffffff, maxResolution);

                std::string basePath = dir.getBasePath();
                std::string separator = std::string("/");

                if (relative_path.length() > 0)
                    cubeMap->loadFromFile(
                        basePath + relative_path + separator + std::string("negz.jpg"),
                        basePath + relative_path + separator + std::string("posz.jpg"),
                        basePath + relative_path + separator + std::string("negx.jpg"),
                        basePath + relative_path + separator + std::string("posx.jpg"),
                        basePath + relative_path + separator + std::string("negy.jpg"),
                        basePath + relative_path + separator + std::string("posy.jpg"),
                        is_srgb, true);

                cubeMap->active(0);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                cubeMap->deactive(0);

                cubemapMap[to_query] = CubemapInfo{relative_path,
                                                   is_srgb,
                                                   _maxResolution,
                                                   cubeMap};

                return cubeMap;
            }

            return cube_it->second.cubemap;
        }

        void ResourceMap::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
        {
            writer.StartObject();

            writer.String("texture_2d");
            writer.StartArray();
            for (auto &item : texture2DMap)
            {
                writer.StartObject();

                writer.String("tex_id");
                writer.Uint64((uint64_t)(uintptr_t)item.second.tex.get());

                writer.String("path");
                writer.String(item.second.relative_path.c_str());

                writer.String("is_srgb");
                writer.Bool(item.second.is_srgb);

                writer.EndObject();
            }
            writer.EndArray();

            writer.String("cubemap");
            writer.StartArray();
            for (auto &item : cubemapMap)
            {
                writer.StartObject();

                writer.String("cubemap_id");
                writer.Uint64((uint64_t)(uintptr_t)item.second.cubemap.get());

                writer.String("path");
                writer.String(item.second.relative_path.c_str());

                writer.String("is_srgb");
                writer.Bool(item.second.is_srgb);

                writer.String("max_resolution");
                writer.Int(item.second.maxResolution);

                writer.EndObject();
            }
            writer.EndArray();

            writer.EndObject();
        }
        void ResourceMap::Deserialize(rapidjson::Value &_value,
                                      ResourceSet *resourceSetOutput)
        {
            if (_value.HasMember("texture_2d") && _value["texture_2d"].IsArray())
            {
                auto &elements = _value["texture_2d"];
                for (int i = 0; i < (int)elements.Size(); i++)
                {
                    auto &element = elements[i];
                    if (!element.HasMember("tex_id") || !element["tex_id"].IsUint64())
                        continue;
                    if (!element.HasMember("path") || !element["path"].IsString())
                        continue;
                    if (!element.HasMember("is_srgb") || !element["is_srgb"].IsBool())
                        continue;
                    auto tex = getTexture(element["path"].GetString(), element["is_srgb"].GetBool());
                    resourceSetOutput->texture_map[element["tex_id"].GetUint64()] = tex;
                }
            }

            if (_value.HasMember("cubemap") && _value["cubemap"].IsArray())
            {
                auto &elements = _value["cubemap"];
                for (int i = 0; i < (int)elements.Size(); i++)
                {
                    auto &element = elements[i];
                    if (!element.HasMember("cubemap_id") || !element["cubemap_id"].IsUint64())
                        continue;
                    if (!element.HasMember("path") || !element["path"].IsString())
                        continue;
                    if (!element.HasMember("is_srgb") || !element["is_srgb"].IsBool())
                        continue;
                    if (!element.HasMember("max_resolution") || !element["max_resolution"].IsInt())
                        continue;
                    auto cubemap = getCubeMap(element["path"].GetString(), element["is_srgb"].GetBool(), element["max_resolution"].GetInt());
                    resourceSetOutput->cubemap_map[element["cubemap_id"].GetUint64()] = cubemap;
                }
            }
        }

    }
}