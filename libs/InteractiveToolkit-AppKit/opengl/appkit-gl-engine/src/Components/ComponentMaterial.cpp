#include <appkit-gl-engine/Components/ComponentMaterial.h>

#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <appkit-gl-engine/ResourceMap.h>
#include <appkit-gl-engine/Serializer/SerializerUtil.h>

// #include <rapidjson/document.h>
// #include <rapidjson/writer.h>
#include <cstring>
#include <cstdio>

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentMaterial::Type = "ComponentMaterial";

            // // UnlitSetup implementation
            // UnlitSetup::UnlitSetup()
            // {
            //     blendMode = AppKit::GLEngine::BlendModeDisabled;
            //     color = MathCore::vec4f(1.0f);
            // }

            // void UnlitSetup::releaseTextureReferences()
            // {
            //     tex = nullptr;
            // }

            // void UnlitSetup::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            // {
            //     writer.StartObject();

            //     if (color != MathCore::vec4f(1.0f))
            //     {
            //         writer.String("color");
            //         SerializerUtil::write(writer, color);
            //     }

            //     if (tex != nullptr)
            //     {
            //         writer.String("tex");
            //         writer.Uint64((uint64_t)(uintptr_t)tex.get());
            //     }

            //     writer.String("blend_mode");
            //     if (blendMode == AppKit::GLEngine::BlendModeDisabled)
            //         writer.String("disabled");
            //     else if (blendMode == AppKit::GLEngine::BlendModeAlpha)
            //         writer.String("alpha");
            //     else if (blendMode == AppKit::GLEngine::BlendModeAdd)
            //         writer.String("add");
            //     else if (blendMode == AppKit::GLEngine::BlendModeAddAlpha)
            //         writer.String("add_alpha");
            //     else if (blendMode == AppKit::GLEngine::BlendModeSubtract)
            //         writer.String("subtract");
            //     else if (blendMode == AppKit::GLEngine::BlendModeSubtractAlpha)
            //         writer.String("subtract_alpha");

            //     writer.EndObject();
            // }

            // void UnlitSetup::Deserialize(rapidjson::Value &_value,
            //                  std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
            //                  std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
            //                  ResourceSet &resourceSet)
            // {
            //     if (_value.HasMember("color"))
            //         color = SerializerUtil::read<decltype(color)>(_value["color"]);
            //     else
            //         color = MathCore::vec4f(1.0f);

            //     if (_value.HasMember("tex") && _value["tex"].IsUint64())
            //     {
            //         printf("needs to query tex resource DB\n");
            //         uint64_t tex_id = _value["tex"].GetUint64();
            //         auto item = resourceSet.texture_map.find(tex_id);
            //         if (item != resourceSet.texture_map.end())
            //             tex = item->second;
            //         else
            //             tex = nullptr;
            //     }
            //     else
            //     {
            //         tex = nullptr;
            //     }

            //     if (_value.HasMember("blend_mode") && _value["blend_mode"].IsString())
            //     {
            //         auto blend_mode = _value["blend_mode"].GetString();
            //         if (strcmp(blend_mode, "disabled") == 0)
            //             blendMode = AppKit::GLEngine::BlendModeDisabled;
            //         else if (strcmp(blend_mode, "alpha") == 0)
            //             blendMode = AppKit::GLEngine::BlendModeAlpha;
            //         else if (strcmp(blend_mode, "add") == 0)
            //             blendMode = AppKit::GLEngine::BlendModeAdd;
            //         else if (strcmp(blend_mode, "add_alpha") == 0)
            //             blendMode = AppKit::GLEngine::BlendModeAddAlpha;
            //         else if (strcmp(blend_mode, "subtract") == 0)
            //             blendMode = AppKit::GLEngine::BlendModeSubtract;
            //         else if (strcmp(blend_mode, "subtract_alpha") == 0)
            //             blendMode = AppKit::GLEngine::BlendModeSubtractAlpha;
            //     }
            // }

            // // PBRSetup implementation
            // PBRSetup::PBRSetup()
            // {
            //     albedoColor = MathCore::vec3f(1.0f);
            //     emissionColor = MathCore::vec3f(0.0f); // [0.0f .. 8.0f]
            //     roughness = 1.0f;
            //     metallic = 0.0f;
            // }

            // void PBRSetup::releaseTextureReferences()
            // {
            //     texAlbedo = nullptr;
            //     texNormal = nullptr;
            //     texSpecular = nullptr;
            //     texEmission = nullptr;
            // }

            // void PBRSetup::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            // {
            //     writer.StartObject();

            //     if (albedoColor != MathCore::vec3f(1.0f))
            //     {
            //         writer.String("albedo_color");
            //         SerializerUtil::write(writer, albedoColor);
            //     }

            //     if (emissionColor != MathCore::vec3f(0.0f))
            //     {
            //         writer.String("emission_color");
            //         SerializerUtil::write(writer, emissionColor);
            //     }

            //     if (roughness != 1.0f)
            //     {
            //         writer.String("roughness");
            //         writer.Double(MathCore::CVT<float>::toDouble(roughness));
            //     }

            //     if (metallic != 0.0f)
            //     {
            //         writer.String("metallic");
            //         writer.Double(MathCore::CVT<float>::toDouble(metallic));
            //     }

            //     if (texAlbedo != nullptr)
            //     {
            //         writer.String("tex_albedo");
            //         writer.Uint64((uint64_t)(uintptr_t)texAlbedo.get());
            //     }

            //     if (texNormal != nullptr)
            //     {
            //         writer.String("tex_normal");
            //         writer.Uint64((uint64_t)(uintptr_t)texNormal.get());
            //     }

            //     if (texSpecular != nullptr)
            //     {
            //         writer.String("tex_specular");
            //         writer.Uint64((uint64_t)(uintptr_t)texSpecular.get());
            //     }

            //     if (texEmission != nullptr)
            //     {
            //         writer.String("tex_emission");
            //         writer.Uint64((uint64_t)(uintptr_t)texEmission.get());
            //     }

            //     writer.EndObject();
            // }

            // void PBRSetup::Deserialize(rapidjson::Value &_value,
            //                  std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
            //                  std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
            //                  ResourceSet &resourceSet)
            // {
            //     if (_value.HasMember("albedo_color"))
            //         albedoColor = SerializerUtil::read<decltype(albedoColor)>(_value["albedo_color"]);
            //     else
            //         albedoColor = MathCore::vec3f(1.0f);

            //     if (_value.HasMember("emission_color"))
            //         emissionColor = SerializerUtil::read<decltype(emissionColor)>(_value["emission_color"]);
            //     else
            //         emissionColor = MathCore::vec3f(0.0f);

            //     if (_value.HasMember("roughness") && _value["roughness"].IsDouble())
            //         roughness = MathCore::CVT<double>::toFloat(_value["roughness"].GetDouble());
            //     else
            //         roughness = 1.0f;

            //     if (_value.HasMember("metallic") && _value["metallic"].IsDouble())
            //         metallic = MathCore::CVT<double>::toFloat(_value["metallic"].GetDouble());
            //     else
            //         metallic = 0.0f;

            //     if (_value.HasMember("tex_albedo") && _value["tex_albedo"].IsUint64())
            //     {
            //         printf("needs to query tex resource DB\n");
            //         uint64_t tex_id = _value["tex_albedo"].GetUint64();
            //         auto item = resourceSet.texture_map.find(tex_id);
            //         if (item != resourceSet.texture_map.end())
            //             texAlbedo = item->second;
            //         else
            //             texAlbedo = nullptr;
            //     }
            //     else
            //     {
            //         texAlbedo = nullptr;
            //     }

            //     if (_value.HasMember("tex_normal") && _value["tex_normal"].IsUint64())
            //     {
            //         printf("needs to query tex resource DB\n");
            //         uint64_t tex_id = _value["tex_normal"].GetUint64();
            //         auto item = resourceSet.texture_map.find(tex_id);
            //         if (item != resourceSet.texture_map.end())
            //             texNormal = item->second;
            //         else
            //             texNormal = nullptr;
            //     }
            //     else
            //     {
            //         texNormal = nullptr;
            //     }

            //     if (_value.HasMember("tex_specular") && _value["tex_specular"].IsUint64())
            //     {
            //         printf("needs to query tex resource DB\n");
            //         uint64_t tex_id = _value["tex_specular"].GetUint64();
            //         auto item = resourceSet.texture_map.find(tex_id);
            //         if (item != resourceSet.texture_map.end())
            //             texSpecular = item->second;
            //         else
            //             texSpecular = nullptr;
            //     }
            //     else
            //     {
            //         texSpecular = nullptr;
            //     }

            //     if (_value.HasMember("tex_emission") && _value["tex_emission"].IsUint64())
            //     {
            //         printf("needs to query tex resource DB\n");
            //         uint64_t tex_id = _value["tex_emission"].GetUint64();
            //         auto item = resourceSet.texture_map.find(tex_id);
            //         if (item != resourceSet.texture_map.end())
            //             texEmission = item->second;
            //         else
            //             texEmission = nullptr;
            //     }
            //     else
            //     {
            //         texEmission = nullptr;
            //     }
            // }

            // ComponentMaterial implementation
            ComponentMaterial::ComponentMaterial() : Component(ComponentMaterial::Type)
            {
                // type = MaterialNone;

                skin_gradient_matrix = nullptr;
                skin_shader_matrix_size_bitflag = 0;
                skin_gradient_matrix_dirty = false;

                always_clone = false;
            }

            ComponentMaterial::~ComponentMaterial()
            {
                // unlit.releaseTextureReferences();
                // pbr.releaseTextureReferences();
            }

            void ComponentMaterial::setShader(std::shared_ptr<DefaultEngineShader> shader) {
                if (this->shader == shader)
                    return;
                this->shader  = shader;
                if (this->shader != nullptr)
                    this->property_bag = this->shader->createDefaultBag();
                else
                    this->property_bag.clear();
            }

            std::shared_ptr<Component> ComponentMaterial::duplicate_ref_or_clone(bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentMaterial>();

                // result->type = this->type;

                result->shader = this->shader;
                result->property_bag = this->property_bag;

                // result->unlit = this->unlit;
                // result->pbr = this->pbr;

                result->always_clone = this->always_clone;

                return result;
            }

            void ComponentMaterial::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentMaterial::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentMaterial::Type);
                writer.String("id");
                writer.Uint64((uint64_t)(uintptr_t)self().get());

                // if (type == MaterialType::MaterialUnlit)
                // { // implemented
                //     writer.String("data_type");
                //     writer.String("unlit");
                //     writer.String("data");
                //     unlit.Serialize(writer);
                // }
                // else if (type == MaterialType::MaterialUnlitTexture)
                // { // implemented
                //     writer.String("data_type");
                //     writer.String("unlit_texture");
                //     writer.String("data");
                //     unlit.Serialize(writer);
                // }
                // else if (type == MaterialType::MaterialUnlitTextureVertexColor)
                // { // implemented
                //     writer.String("data_type");
                //     writer.String("unlit_texture_vertex_color");
                //     writer.String("data");
                //     unlit.Serialize(writer);
                // }
                // else if (type == MaterialType::MaterialUnlitTextureVertexColorFont)
                // { // implemented
                //     writer.String("data_type");
                //     writer.String("unlit_texture_vertex_color_font");
                //     writer.String("data");
                //     unlit.Serialize(writer);
                // }
                // else if (type == MaterialType::MaterialPBR)
                // {
                //     writer.String("data_type");
                //     writer.String("pbr");
                //     writer.String("data");
                //     pbr.Serialize(writer);
                // }

                writer.EndObject();
            }

            void ComponentMaterial::Deserialize(rapidjson::Value &_value,
                             std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                             std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                             ResourceSet &resourceSet)
            {
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), ComponentMaterial::Type) == 0)
                    return;

                if (!_value.HasMember("data_type") || !_value["data_type"].IsString())
                    return;
                if (!_value.HasMember("data") || !_value["data"].IsObject())
                    return;

                // auto data_type = _value["data_type"].GetString();
                // if (strcmp(data_type, "unlit") == 0)
                // {
                //     type = MaterialType::MaterialUnlit;
                //     unlit.Deserialize(_value["data"], transform_map, component_map, resourceSet);
                // }
                // else if (strcmp(data_type, "unlit_texture") == 0)
                // {
                //     type = MaterialType::MaterialUnlitTexture;
                //     unlit.Deserialize(_value["data"], transform_map, component_map, resourceSet);
                // }
                // else if (strcmp(data_type, "unlit_texture_vertex_color") == 0)
                // {
                //     type = MaterialType::MaterialUnlitTextureVertexColor;
                //     unlit.Deserialize(_value["data"], transform_map, component_map, resourceSet);
                // }
                // else if (strcmp(data_type, "unlit_texture_vertex_color_font") == 0)
                // {
                //     type = MaterialType::MaterialUnlitTextureVertexColorFont;
                //     unlit.Deserialize(_value["data"], transform_map, component_map, resourceSet);
                // }
                // else if (strcmp(data_type, "pbr") == 0)
                // {
                //     type = MaterialType::MaterialPBR;
                //     pbr.Deserialize(_value["data"], transform_map, component_map, resourceSet);
                // }
            }
        }
    }
}

