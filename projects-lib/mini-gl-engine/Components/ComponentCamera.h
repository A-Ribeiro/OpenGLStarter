#ifndef _component_camera_h_
#define _component_camera_h_

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/iRect.h>

namespace GLEngine {

    namespace Components {

        class ComponentCamera:public Component {
        public:            

            ComponentCamera(ComponentType type);
            virtual ~ComponentCamera();

            aRibeiro::mat4 projection;
            GLEngine::iRect viewport;
            
            void precomputeViewProjection(bool useVisitedFlag);
            
            aRibeiro::mat4 viewProjection;
            aRibeiro::mat4 view;
            aRibeiro::mat4 viewIT;
            aRibeiro::mat4 viewInv;
        };

    }
}

#endif
