#pragma once

#include <InteractiveToolkit/Platform/Core/SmartVector.h>

namespace AppKit
{
    namespace GLEngine
    {

        class Transform;
        class Component;

        class StartEventManager
        {
            Platform::SmartVector<Component *> componentList;

        public:
            // static StartEventManager *Instance();
            void registerNewComponent(Component *c);
            bool unregisterComponent(Component *c);
            void processAllComponentsWithTransform();
        };

    }

}