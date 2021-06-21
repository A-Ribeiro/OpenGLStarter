#ifndef _component_camera_move_h_
#define _component_camera_move_h_

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include "ComponentCameraPerspective.h"

namespace GLEngine {

    namespace Components {
        
        class ComponentCameraMove:public Component {
            
            SharedPointer<ComponentCameraPerspective> camera;
            aRibeiro::PressReleaseDetector left, right, up, down;
            aRibeiro::vec3 euler;
            
        protected:
            void start();
            void OnLateUpdate(aRibeiro::PlatformTime *time);
            void OnWindowSizeChanged(aRibeiro::Property<sf::Vector2i> *prop);
            void OnMousePosChanged(aRibeiro::Property<aRibeiro::vec2> *prop);
        public:
            
            float forwardSpeed;
            float strafeSpeed;
            float angleSpeed;
            
            static const ComponentType Type;
            ComponentCameraMove();
            ~ComponentCameraMove();

            
        };

    }
}

#endif
