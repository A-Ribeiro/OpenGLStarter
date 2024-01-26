#pragma once

//#include <aRibeiroCore/aRibeiroCore.h>
#include <InteractiveToolkit/Platform/Time.h>

#include <appkit-gl-engine/Component.h>
//#include <appkit-gl-engine/Transform.h>
//#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            class RotatingCross : public Component
            {
            public:
                static const ComponentType Type;

                float speed_degrees;

                RotatingCross();
                ~RotatingCross();

                void start();
                void OnUpdate(Platform::Time *time);
            };

        }

    }
}