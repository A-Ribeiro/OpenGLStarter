#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/types/iRect.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentCamera : public Component
            {
            public:
                ComponentCamera(ComponentType type);
                virtual ~ComponentCamera();

                MathCore::mat4f projection;
                AppKit::GLEngine::iRect viewport;

                void precomputeViewProjection(bool useVisitedFlag);

                MathCore::mat4f viewProjection;
                MathCore::mat4f view;
                MathCore::mat4f viewIT;
                MathCore::mat4f viewInv;
            };

        }
    }

}