#include "ComponentUI.h"

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentUI::Type = "ComponentUI";

            ComponentUI::ComponentUI() : Component(ComponentUI::Type)
            {
                always_clone = true;
            }

            ComponentUI::~ComponentUI()
            {
            }

            std::shared_ptr<Component> ComponentUI::duplicate_ref_or_clone(bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentUI>();

                result->always_clone = this->always_clone;

                return result;
            }
            void ComponentUI::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentUI::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }
            
            void ComponentUI::Deserialize(rapidjson::Value &_value,
                                          std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                          std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                          ResourceSet &resourceSet)
            {
            }

        }
    }
}
