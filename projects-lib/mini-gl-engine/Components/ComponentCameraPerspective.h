#ifndef _component_camera_perspective_h_
#define _component_camera_perspective_h_

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include "ComponentCamera.h"

namespace GLEngine {

    namespace Components {

        class ComponentCameraPerspective:public ComponentCamera {

            void OnUpdateCameraFloatParameter(aRibeiro::Property<float> *prop);
            void OnWindowSizeChanged(aRibeiro::Property<sf::Vector2i> *prop);
            
            void OnLeftHandedChanged(aRibeiro::Property<bool> *prop);
            void OnRightHandedChanged(aRibeiro::Property<bool> *prop);
            
            void configureProjection();
        public:
            
            static ComponentType Type;
            
            aRibeiro::Property<float> fovDegrees;
            aRibeiro::Property<float> nearPlane;
            aRibeiro::Property<float> farPlane;
            
            aRibeiro::Property<bool> leftHanded;
            aRibeiro::Property<bool> rightHanded;
            
            //aRibeiro::mat4 projection;
            
            ComponentCameraPerspective();
            ~ComponentCameraPerspective();
            
            //void precomputeViewProjection(bool useVisitedFlag);
            
            /*
            aRibeiro::mat4 viewProjection;
            aRibeiro::mat4 view;
            aRibeiro::mat4 viewIT;
            aRibeiro::mat4 viewInv;
            */
            
            void addLinesComponent();
        };

    }
}

#endif
