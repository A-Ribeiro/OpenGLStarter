#pragma once

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

            class ComponentCameraMove : public Component
            {

                std::weak_ptr<ComponentCameraPerspective> cameraRef;
                
                EventCore::PressReleaseDetector left, right, up, down;
                MathCore::vec3f euler;

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

            protected:
                void start();
                void OnLateUpdate(Platform::Time *time);
                void OnViewportChanged(const iRect &value, const iRect &oldValue);
                void OnMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue);

            public:
                float forwardSpeed;
                float strafeSpeed;
                float angleSpeed;

                static const ComponentType Type;
                ComponentCameraMove();
                ~ComponentCameraMove();

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone);
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap);

            };

        }
    }

}