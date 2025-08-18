#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>

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

            // ComponentMaterial implementation
            ComponentMaterial::ComponentMaterial() : Component(ComponentMaterial::Type)
            {
                skin_gradient_matrix = nullptr;
                skin_shader_matrix_size_bitflag = 0;
                skin_gradient_matrix_dirty = false;

                always_clone = false;
            }

            ComponentMaterial::~ComponentMaterial()
            {
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

            std::shared_ptr<Component> ComponentMaterial::duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentMaterial>();

                result->shader = this->shader;
                result->property_bag = this->property_bag;

                result->always_clone = this->always_clone;

                return result;
            }

            void ComponentMaterial::fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
            {
                for (auto& entry : property_bag.getProperties()) {
                    auto& item = entry.second;
                    if (item.holds<std::weak_ptr<Component>>()) {
                        auto ptr = item.get<std::weak_ptr<Component>>().lock();
                        if (componentMap.find(ptr) != componentMap.end())
                            item.set<std::weak_ptr<Component>>(componentMap[ptr]);
                    } else if (item.holds<std::weak_ptr<Transform>>()) {
                        auto ptr = item.get<std::weak_ptr<Transform>>().lock();
                        if (transformMap.find(ptr) != transformMap.end())
                            item.set<std::weak_ptr<Transform>>(transformMap[ptr]);
                    }
                }
            }

            void ComponentMaterial::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentMaterial::Type);
                writer.String("id");
                writer.Uint64((uint64_t)(uintptr_t)self().get());


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

            }
        }
    }
}

