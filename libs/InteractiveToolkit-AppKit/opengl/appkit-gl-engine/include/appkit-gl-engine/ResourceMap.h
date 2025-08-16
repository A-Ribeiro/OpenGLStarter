#pragma once

#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/ToShared.h>
#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLCubeMap.h>
#include <appkit-gl-engine/Serializer/SerializerUtil.h>
#include <appkit-gl-base/font/GLFont2Builder.h>

namespace AppKit
{
    namespace GLEngine
    {

        // class SpriteShader;
        class ShaderUnlit;
        class ShaderUnlitVertexColor;
        class ShaderUnlitTexture;
        class ShaderUnlitTextureAlpha;
        class ShaderUnlitTextureVertexColorAlpha;
        class PBRShaderSelector;
        class ShaderDepthOnly;
        class LineShader;

        namespace Components
        {
            class ComponentMaterial;
        }

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
            struct FontResource
            {
                std::shared_ptr<AppKit::OpenGL::GLFont2Builder> fontBuilder;
                std::shared_ptr<AppKit::OpenGL::GLFont2PolygonCache> polygonFontCache;
                std::shared_ptr<Components::ComponentMaterial> material;
                std::shared_ptr<Components::ComponentMaterial> material_mask;
            };

        private:
            std::unordered_map<std::string, std::shared_ptr<FontResource>> textureFontMap;
            std::unordered_map<std::string, std::shared_ptr<FontResource>> geometryFontMap;

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
            void clear(); // each scene call this on unload

            void ensure_default_texture_creation();

            std::shared_ptr<AppKit::OpenGL::GLTexture> getTexture(const std::string &relative_path, bool is_srgb);

            std::shared_ptr<AppKit::OpenGL::GLCubeMap> getCubeMap(const std::string &relative_path, bool is_srgb, int maxResolution);

            std::shared_ptr<FontResource> getTextureFont(const std::string &relative_path, bool is_srgb);
            std::shared_ptr<FontResource> getPolygonFont(const std::string &relative_path, float defaultSize, float max_distance_tolerance, Platform::ThreadPool *threadPool, bool is_srgb);

            void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
            void Deserialize(rapidjson::Value &_value, ResourceSet *resourceSetOutput);

            std::shared_ptr<AppKit::OpenGL::GLTexture> defaultAlbedoTexture;
            std::shared_ptr<AppKit::OpenGL::GLTexture> defaultNormalTexture;
            std::shared_ptr<Components::ComponentMaterial> defaultPBRMaterial;

            std::shared_ptr<Components::ComponentMaterial> renderOnlyDepthMaterial;
            std::shared_ptr<Components::ComponentMaterial> defaultUnlitMaterial;
            std::shared_ptr<Components::ComponentMaterial> defaultUnlitAlphaMaterial;
            std::shared_ptr<Components::ComponentMaterial> defaultUnlitVertexColorMaterial;
            std::shared_ptr<Components::ComponentMaterial> defaultUnlitVertexColorAlphaMaterial;
            std::shared_ptr<Components::ComponentMaterial> defaultLineMaterial;

            std::unordered_map<uint64_t, std::shared_ptr<Components::ComponentMaterial>> spriteMaterialMap;

            // default shaders
            // std::shared_ptr<SpriteShader> spriteShader;
            std::shared_ptr<ShaderUnlit> shaderUnlit;
            std::shared_ptr<ShaderUnlitVertexColor> shaderUnlitVertexColor;
            std::shared_ptr<ShaderUnlitTexture> shaderUnlitTexture;
            std::shared_ptr<ShaderUnlitTextureAlpha> shaderUnlitTextureAlpha;
            std::shared_ptr<ShaderUnlitTextureVertexColorAlpha> shaderUnlitTextureVertexColorAlpha;
            std::shared_ptr<ShaderDepthOnly> shaderDepthOnly;
            std::shared_ptr<PBRShaderSelector> pbrShaderSelector;
            std::shared_ptr<LineShader> lineShader;
        };
    }
}

// need to include component material after using its forward declaration
#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>

#include <appkit-gl-engine/shaders/ShaderUnlit.h>             // UnlitPassShader
#include <appkit-gl-engine/shaders/ShaderUnlitVertexColor.h>  // Unlit_vertcolor_Shader
#include <appkit-gl-engine/shaders/ShaderUnlitTexture.h>      // Unlit_tex_PassShader
#include <appkit-gl-engine/shaders/ShaderUnlitTextureAlpha.h> // Unlit_tex_PassShader
// #include <appkit-gl-engine/shaders/ShaderUnlitTextureVertexColor.h> //
#include <appkit-gl-engine/shaders/ShaderUnlitTextureVertexColorAlpha.h> // Unlit_tex_vertcolor_font_PassShader
#include <appkit-gl-engine/shaders/PBRShaderSelector.h>                  //
#include <appkit-gl-engine/shaders/ShaderDepthOnly.h>                    // DepthOnlyShader
#include <appkit-gl-engine/shaders/LineShader.h>
