#pragma once

//#include <aRibeiroCore/aRibeiroCore.h>
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

// #include <appkit-gl-engine/SharedPointer/SharedPointer.h>

#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentCameraLookToNode : public Component
            {

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

                std::shared_ptr<ComponentCameraPerspective> camera;
                EventCore::PressReleaseDetector up, down;
                MathCore::vec3f euler;
                float distance_to_target;

                std::weak_ptr<Transform> targetRef;

                std::shared_ptr<Transform> getTarget();
                void setTarget(std::shared_ptr<Transform> transform);

            protected:
                void start();
                void OnLateUpdate(Platform::Time *time);
                void OnViewportChanged(const iRect &value, const iRect &oldValue);
                void OnMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue);

            public:
                EventCore::VirtualProperty<std::shared_ptr<Transform>> Target;

                float zoomSpeed;
                float angleSpeed;

                static const ComponentType Type;
                ComponentCameraLookToNode();
                ~ComponentCameraLookToNode();

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