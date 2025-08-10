#include <appkit-gl-engine/ResourceMap.h>

// #include <appkit-gl-engine/shaders/SpriteShader.h>
#include <appkit-gl-engine/shaders/ShaderUnlit.h>            // UnlitPassShader
#include <appkit-gl-engine/shaders/ShaderUnlitVertexColor.h> // Unlit_vertcolor_Shader
#include <appkit-gl-engine/shaders/ShaderUnlitTexture.h>     // Unlit_tex_PassShader
#include <appkit-gl-engine/shaders/ShaderUnlitTextureAlpha.h>
// #include <appkit-gl-engine/shaders/ShaderUnlitTextureVertexColor.h> //
#include <appkit-gl-engine/shaders/ShaderUnlitTextureVertexColorAlpha.h> // Unlit_tex_vertcolor_font_PassShader
#include <appkit-gl-engine/shaders/PBRShaderSelector.h>                  //
#include <appkit-gl-engine/shaders/ShaderDepthOnly.h>

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
            printf("ResourceMap::clear_refcount_equals_1\n");

            {
                std::vector<std::string> to_remove_str;
                for (auto &item : geometryFontMap)
                {
                    if (item.second.use_count() > 1)
                        continue;
                    to_remove_str.push_back(item.first);
                }
                for (const auto &key : to_remove_str)
                    geometryFontMap.erase(key);

                to_remove_str.clear();
                for (auto &item : textureFontMap)
                {
                    if (item.second.use_count() > 1)
                        continue;
                    to_remove_str.push_back(item.first);
                }
                for (const auto &key : to_remove_str)
                    textureFontMap.erase(key);

                printf("  total loaded texture fonts: %zu\n", textureFontMap.size());
                printf("  total loaded geometry fonts: %zu\n", geometryFontMap.size());
            }

            {
                std::vector<uint64_t> to_remove_u64;
                // free sprite not used
                for (auto &item : spriteMaterialMap)
                {
                    if (item.second.use_count() > 1)
                        continue;
                    to_remove_u64.push_back(item.first);
                }
                for (const auto &key : to_remove_u64)
                    spriteMaterialMap.erase(key);
                printf("  total loaded sprite materials: %zu\n", spriteMaterialMap.size());
            }

            {
                // free texture2D not used
                std::vector<std::string> to_remove_str;
                for (auto &item : texture2DMap)
                {
                    if (item.second.tex.use_count() > 1 ||
                        item.second.relative_path.compare("DEFAULT_ALBEDO") == 0 ||
                        item.second.relative_path.compare("DEFAULT_NORMAL") == 0)
                        continue;
                    to_remove_str.push_back(item.first);
                }
                for (const auto &key : to_remove_str)
                    texture2DMap.erase(key);

                printf("  total loaded texture2D: %zu\n", texture2DMap.size());

                // free cubemap not used
                to_remove_str.clear();
                for (auto &item : cubemapMap)
                {
                    if (item.second.cubemap.use_count() > 1)
                        continue;
                    to_remove_str.push_back(item.first);
                }
                for (const auto &key : to_remove_str)
                    cubemapMap.erase(key);

                printf("  total loaded cubemaps: %zu\n", cubemapMap.size());
            }
        }

        void ResourceMap::clear()
        {
            printf("ResourceMap::clear\n");

            geometryFontMap.clear();
            textureFontMap.clear();

            spriteMaterialMap.clear();
            texture2DMap.clear();
            cubemapMap.clear();

            defaultAlbedoTexture = nullptr;
            defaultNormalTexture = nullptr;
            defaultPBRMaterial = nullptr;

            renderOnlyDepthMaterial = nullptr;
            defaultUnlitMaterial = nullptr;
            defaultUnlitVertexColorMaterial = nullptr;
            defaultUnlitAlphaMaterial = nullptr;
            defaultUnlitVertexColorAlphaMaterial = nullptr;
            defaultLineMaterial = nullptr;

            shaderUnlit = nullptr;
            shaderUnlitVertexColor = nullptr;
            shaderUnlitTexture = nullptr;
            shaderUnlitTextureAlpha = nullptr;
            shaderUnlitTextureVertexColorAlpha = nullptr;
            pbrShaderSelector = nullptr;
            shaderDepthOnly = nullptr;
            lineShader = nullptr;
        }

        void ResourceMap::ensure_default_texture_creation()
        {
            // creating default shaders
            if (shaderUnlit == nullptr)
                shaderUnlit = std::make_shared<ShaderUnlit>();
            if (shaderUnlitVertexColor == nullptr)
                shaderUnlitVertexColor = std::make_shared<ShaderUnlitVertexColor>();
            if (shaderUnlitTexture == nullptr)
                shaderUnlitTexture = std::make_shared<ShaderUnlitTexture>();
            if (shaderUnlitTextureAlpha == nullptr)
                shaderUnlitTextureAlpha = std::make_shared<ShaderUnlitTextureAlpha>();
            if (shaderUnlitTextureVertexColorAlpha == nullptr)
                shaderUnlitTextureVertexColorAlpha = std::make_shared<ShaderUnlitTextureVertexColorAlpha>();
            if (pbrShaderSelector == nullptr)
                pbrShaderSelector = std::make_shared<PBRShaderSelector>();
            if (shaderDepthOnly == nullptr)
                shaderDepthOnly = std::make_shared<ShaderDepthOnly>();
            if (lineShader == nullptr)
                lineShader = std::make_shared<LineShader>();

            if (defaultPBRMaterial == nullptr)
            {
                defaultAlbedoTexture = getTexture("DEFAULT_ALBEDO", true);
                defaultNormalTexture = getTexture("DEFAULT_NORMAL", false);

                defaultPBRMaterial = Component::CreateShared<Components::ComponentMaterial>();

                defaultPBRMaterial->setShader(pbrShaderSelector);
                // // defaultPBRMaterial->type = Components::MaterialPBR;
                // defaultPBRMaterial->shader = pbrShaderSelector;
                // defaultPBRMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
                // defaultPBRMaterial->pbr.metallic = 0.0f;
                // defaultPBRMaterial->pbr.roughness = 1.0f;
                // defaultPBRMaterial->pbr.texAlbedo = defaultAlbedoTexture;
                // defaultPBRMaterial->pbr.texNormal = defaultNormalTexture;

                defaultPBRMaterial->property_bag.getProperty("texAlbedo").set<std::shared_ptr<OpenGL::VirtualTexture>>(defaultAlbedoTexture);
                defaultPBRMaterial->property_bag.getProperty("texNormal").set<std::shared_ptr<OpenGL::VirtualTexture>>(defaultNormalTexture);
            }

            if (renderOnlyDepthMaterial == nullptr)
            {
                renderOnlyDepthMaterial = Component::CreateShared<Components::ComponentMaterial>();
                renderOnlyDepthMaterial->setShader(shaderDepthOnly);
            }

            if (defaultUnlitMaterial == nullptr)
            {
                defaultUnlitMaterial = Component::CreateShared<Components::ComponentMaterial>();
                defaultUnlitMaterial->setShader(shaderUnlit);
            }

            if (defaultUnlitVertexColorMaterial == nullptr)
            {
                defaultUnlitVertexColorMaterial = Component::CreateShared<Components::ComponentMaterial>();
                defaultUnlitVertexColorMaterial->setShader(shaderUnlitVertexColor);
            }

            if (defaultUnlitAlphaMaterial == nullptr)
            {
                defaultUnlitAlphaMaterial = Component::CreateShared<Components::ComponentMaterial>();
                defaultUnlitAlphaMaterial->setShader(shaderUnlit);
                defaultUnlitAlphaMaterial->property_bag.getProperty("BlendMode").set<int>((int)AppKit::GLEngine::BlendModeAlpha);
            }

            if (defaultUnlitVertexColorAlphaMaterial == nullptr)
            {
                defaultUnlitVertexColorAlphaMaterial = Component::CreateShared<Components::ComponentMaterial>();
                defaultUnlitVertexColorAlphaMaterial->setShader(shaderUnlitVertexColor);
                defaultUnlitVertexColorAlphaMaterial->property_bag.getProperty("BlendMode").set<int>((int)AppKit::GLEngine::BlendModeAlpha);
            }

            if (defaultLineMaterial == nullptr)
            {
                defaultLineMaterial = Component::CreateShared<Components::ComponentMaterial>();
                defaultLineMaterial->setShader(lineShader);
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

        std::shared_ptr<ResourceMap::FontResource> ResourceMap::getTextureFont(const std::string &relative_path, bool is_srgb)
        {
            std::string to_query = ITKCommon::PrintfToStdString("%s:%s", (is_srgb) ? "srgb" : "linear", relative_path.c_str());

            auto font_it = textureFontMap.find(to_query);
            if (font_it == textureFontMap.end())
            {
                auto fontResource = std::make_shared<FontResource>();
                auto fontBuilder = std::make_shared<AppKit::OpenGL::GLFont2Builder>();
                fontBuilder->load(relative_path, is_srgb);
                fontResource->fontBuilder = fontBuilder;

                fontResource->material = Component::CreateShared<Components::ComponentMaterial>();

                fontResource->material->setShader(this->shaderUnlitTextureVertexColorAlpha);
                auto tex = std::shared_ptr<AppKit::OpenGL::GLTexture>(&fontBuilder->glFont2.texture, [](AppKit::OpenGL::GLTexture *v) {});
                fontResource->material->property_bag.getProperty("uTexture").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)tex);



                textureFontMap[to_query] = fontResource;
                return fontResource;
            }
            return font_it->second;
        }
        std::shared_ptr<ResourceMap::FontResource> ResourceMap::getPolygonFont(const std::string &relative_path, float defaultSize, float max_distance_tolerance, Platform::ThreadPool *threadPool, bool is_srgb)
        {
            std::string to_query = ITKCommon::PrintfToStdString("%s:%s:%i:%i", 
                relative_path.c_str(), 
                (is_srgb) ? "srgb" : "linear", 
                (int)(MathCore::OP<float>::ceil(defaultSize) + 0.5f), 
                (int)(MathCore::OP<float>::ceil(max_distance_tolerance) + 0.5f)
            );
            auto font_it = geometryFontMap.find(to_query);
            if (font_it == geometryFontMap.end())
            {
                auto fontResourceBase = getTextureFont(relative_path, is_srgb);
                auto polygonCache = fontResourceBase->fontBuilder->createPolygonCache(
                    defaultSize, max_distance_tolerance, threadPool);
                auto fontResource = std::make_shared<FontResource>();
                fontResource->fontBuilder = fontResourceBase->fontBuilder;
                fontResource->polygonFontCache = polygonCache;
                fontResource->material = this->defaultUnlitVertexColorAlphaMaterial;
                geometryFontMap[to_query] = fontResource;
                return fontResource;
            }
            return font_it->second;
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