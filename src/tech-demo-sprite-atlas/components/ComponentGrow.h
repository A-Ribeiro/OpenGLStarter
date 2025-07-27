#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            class ComponentGrow : public Component
            {

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

                void OnUpdate(Platform::Time *time);

                float lrp;

            public:
                static const ComponentType Type;

                ComponentGrow();
                ~ComponentGrow();

                void attachToTransform(std::shared_ptr<Transform> t) override;
                void detachFromTransform(std::shared_ptr<Transform> t) override;

                void start() override;

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone);
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);
            };
        }
    }
}
