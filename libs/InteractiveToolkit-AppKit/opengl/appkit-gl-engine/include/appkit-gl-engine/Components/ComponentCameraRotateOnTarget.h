#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

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

            class ComponentCameraRotateOnTarget : public Component
            {

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

                float distanceRotation;
                MathCore::vec3f euler;
                MathCore::vec2f lastPosition;
                bool mouseMoving;

                std::weak_ptr<Transform> targetRef;

                std::shared_ptr<Transform> getTarget();
                void setTarget(std::shared_ptr<Transform> _transform);

            protected:
                void start();
                // void OnMouseUp(sf::Mouse::Button button, const MathCore::vec2f &pos);
                // void OnMouseDown(sf::Mouse::Button button, const MathCore::vec2f &pos);

                void onMouseEvent(const AppKit::Window::MouseEvent &mouseEvent);
                void OnNormalizedMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue);

            public:
                EventCore::VirtualProperty<std::shared_ptr<Transform>> Target;

                float speedAngle;
                bool enabled;

                float rotation_x_deg_min;
                float rotation_x_deg_max;

                static const ComponentType Type;

                ComponentCameraRotateOnTarget();
                ~ComponentCameraRotateOnTarget();

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone);
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value, std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map, std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map);

            };

        }
    }

}