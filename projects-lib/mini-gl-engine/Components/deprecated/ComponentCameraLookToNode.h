#ifndef _component_camera_look_to_node_h_
#define _component_camera_look_to_node_h_

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/ComponentCameraPerspective.h>

namespace GLEngine {

    namespace Components {

        class ComponentCameraLookToNode :public Component {

            SharedPointer<ComponentCameraPerspective> camera;
            aRibeiro::PressReleaseDetector up, down;
            aRibeiro::vec3 euler;
            float distance_to_target;

            SharedPointer<Transform> target;

            Transform* getTarget();
            void setTarget(Transform* transform);

        protected:
            void start();
            void OnLateUpdate(aRibeiro::PlatformTime *time);
            void OnWindowSizeChanged(aRibeiro::Property<sf::Vector2i> *prop);
            void OnMousePosChanged(aRibeiro::Property<aRibeiro::vec2> *prop);
        public:

            aRibeiro::VirtualProperty<Transform*> Target;

            float zoomSpeed;
            float angleSpeed;

            static const ComponentType Type;
            ComponentCameraLookToNode();
            ~ComponentCameraLookToNode();


        };

    }
}

#endif
