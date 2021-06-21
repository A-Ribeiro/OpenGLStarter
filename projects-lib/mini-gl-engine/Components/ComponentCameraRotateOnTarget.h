#ifndef _component_camera_rotate_on_target_h_
#define _component_camera_rotate_on_target_h_

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/ComponentCameraPerspective.h>

namespace GLEngine {

    namespace Components {

        class ComponentCameraRotateOnTarget :public Component {

            float distanceRotation;
            aRibeiro::vec3 euler;
            aRibeiro::vec2 lastPosition;
            bool mouseMoving;

            SharedPointer<Transform> target;

            Transform* getTarget();
            void setTarget(Transform* _transform);

        protected:

            void start();
            void OnMouseUp(sf::Mouse::Button button, const aRibeiro::vec2 &pos);
            void OnMouseDown(sf::Mouse::Button button, const aRibeiro::vec2 &pos);
            void OnNormalizedMousePosChanged(aRibeiro::Property<aRibeiro::vec2> *prop);

        public:

            aRibeiro::VirtualProperty<Transform*> Target;

            float speedAngle;
            bool enabled;

            static const ComponentType Type;
            
            ComponentCameraRotateOnTarget();
            virtual ~ComponentCameraRotateOnTarget();

        };

    }
}

#endif
