#pragma once

//#include <aRibeiroCore/aRibeiroCore.h>
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/SharedPointer/SharedPointer.h>

#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentCameraLookToNode : public Component
            {

                RenderWindowRegion *renderWindowRegion;

                SharedPointer<ComponentCameraPerspective> camera;
                EventCore::PressReleaseDetector up, down;
                MathCore::vec3f euler;
                float distance_to_target;

                SharedPointer<Transform> target;

                Transform *getTarget();
                void setTarget(Transform *transform);

            protected:
                void start();
                void OnLateUpdate(Platform::Time *time);
                void OnViewportChanged(const iRect &value, const iRect &oldValue);
                void OnMousePosChanged(const MathCore::vec2f &value, const MathCore::vec2f &oldValue);

            public:
                EventCore::VirtualProperty<Transform *> Target;

                float zoomSpeed;
                float angleSpeed;

                static const ComponentType Type;
                ComponentCameraLookToNode();
                ~ComponentCameraLookToNode();
            };

        }
    }

}