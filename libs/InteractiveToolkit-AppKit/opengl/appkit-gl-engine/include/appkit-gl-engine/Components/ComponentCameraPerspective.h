#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/ComponentCamera.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentCameraPerspective : public ComponentCamera
            {

                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

                void OnUpdateCameraFloatParameter(const float &value, const float &oldValue);
                void OnViewportChanged(const iRect &value, const iRect &oldValue);

                void OnLeftHandedChanged(const bool &value, const bool &oldValue);
                void OnRightHandedChanged(const bool &value, const bool &oldValue);

                void configureProjection();

            protected:
                void start();

            public:
                static ComponentType Type;

                EventCore::Property<float> fovDegrees;
                EventCore::Property<float> nearPlane;
                EventCore::Property<float> farPlane;

                EventCore::Property<bool> leftHanded;
                EventCore::Property<bool> rightHanded;

                // MathCore::mat4f projection;

                ComponentCameraPerspective();
                ~ComponentCameraPerspective();

                // void precomputeViewProjection(bool useVisitedFlag);

                /*
                MathCore::mat4f viewProjection;
                MathCore::mat4f view;
                MathCore::mat4f viewIT;
                MathCore::mat4f viewInv;
                */

                void addLinesComponent();

                void attachToTransform(std::shared_ptr<Transform> t){
                    renderWindowRegionRef = t->renderWindowRegion;
                }
                
            };

        }
    }

}