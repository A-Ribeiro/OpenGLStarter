#ifndef _component_camera_orthographic_h_
#define _component_camera_orthographic_h_

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>
#include <mini-gl-engine/iRect.h>

#include <mini-gl-engine/ComponentCamera.h>

namespace GLEngine {

    namespace Components {

        class ComponentCameraOrthographic:public ComponentCamera {

            void OnUpdateCameraFloatParameter(aRibeiro::Property<float> *prop);
            void OnUpdateCameraBoolParameter(aRibeiro::Property<bool> *prop);
            
            void OnWindowSizeChanged(aRibeiro::Property<sf::Vector2i> *prop);
            
            void OnLeftHandedChanged(aRibeiro::Property<bool> *prop);
            void OnRightHandedChanged(aRibeiro::Property<bool> *prop);
            
            void configureProjection();
        public:
            
            static ComponentType Type;
            
            aRibeiro::Property<float> nearPlane;
            aRibeiro::Property<float> farPlane;
            
            aRibeiro::Property<bool> leftHanded;
            aRibeiro::Property<bool> rightHanded;

            aRibeiro::Property<bool> useSizeY;
            aRibeiro::Property<float> sizeY;

            aRibeiro::Property<bool> useSizeX;
            aRibeiro::Property<float> sizeX;
            
            //aRibeiro::mat4 projection;
            //GLEngine::iRect viewport;
            
            ComponentCameraOrthographic();
            ~ComponentCameraOrthographic();
            
            //void precomputeViewProjection(bool useVisitedFlag);
            
            /*
            aRibeiro::mat4 viewProjection;
            aRibeiro::mat4 view;
            aRibeiro::mat4 viewIT;
            aRibeiro::mat4 viewInv;
            */
            
            void addLinesComponent();

            aRibeiro::vec3 App2MousePosition();
        };

    }
}

#endif
