#pragma once

//#include <aRibeiroCore/aRibeiroCore.h>
#include <InteractiveToolkit/Platform/Time.h>

#include <appkit-gl-engine/Component.h>
//#include <appkit-gl-engine/Transform.h>
//#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            class RotatingCross : public Component
            {
            public:
                static const ComponentType Type;

                float speed_degrees;

                RotatingCross();
                ~RotatingCross();

                void start();
                void OnUpdate(Platform::Time *time);

                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone);
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);

            };

        }

    }
}