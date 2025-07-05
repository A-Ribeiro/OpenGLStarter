#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <memory>
#include <vector>
#include <unordered_map>

// Forward declarations
namespace AppKit {
    namespace OpenGL {
        class GLTexture;
    }
    namespace GLEngine {
        class Transform;
        class DefaultEngineShader;
        struct ResourceSet;
    }
}


namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            enum MaterialType
            {
                MaterialNone,

                MaterialUnlit,                       // implemented
                MaterialUnlitVertexColor,            // implemented
                MaterialUnlitTexture,                // implemented
                MaterialUnlitTextureVertexColor,     // implemented
                MaterialUnlitTextureVertexColorFont, // implemented

                MaterialPBR,

                MaterialCustomShader
            };

            class UnlitSetup
            {
            public:
                MathCore::vec4f color;
                std::shared_ptr<AppKit::OpenGL::GLTexture> tex;

                AppKit::GLEngine::BlendModeType blendMode;

                UnlitSetup();

                void releaseTextureReferences();

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);

                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);
            };

            class PBRSetup
            {
            public:
                MathCore::vec3f albedoColor;
                MathCore::vec3f emissionColor;

                std::shared_ptr<AppKit::OpenGL::GLTexture> texAlbedo;
                std::shared_ptr<AppKit::OpenGL::GLTexture> texNormal;
                std::shared_ptr<AppKit::OpenGL::GLTexture> texSpecular;
                std::shared_ptr<AppKit::OpenGL::GLTexture> texEmission;

                float roughness;
                float metallic;

                PBRSetup();

                void releaseTextureReferences();

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);

                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);
            };

            class ComponentMaterial : public Component
            {
            public:
                static const ComponentType Type;

                // material type
                MaterialType type;

                // parameters
                UnlitSetup unlit;
                PBRSetup pbr;

                // used for mesh skinning
                bool skin_gradient_matrix_dirty;
                std::vector<MathCore::mat4f> *skin_gradient_matrix;
                uint32_t skin_shader_matrix_size_bitflag; // skin hint to help to select shader
                
                // for custom shader only
                std::shared_ptr<DefaultEngineShader> custom_shader;
                Utils::ShaderPropertyBag custom_shader_property_bag;

                bool always_clone;

                ComponentMaterial();
                ~ComponentMaterial();

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone) override;
                
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap) override;

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) override;
                
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet) override;
            };
        }
    }

}