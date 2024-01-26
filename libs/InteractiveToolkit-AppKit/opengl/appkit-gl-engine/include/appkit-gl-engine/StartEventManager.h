#pragma once

#include <vector>

namespace AppKit
{
    namespace GLEngine
    {

        class Transform;
        class Component;

        class StartEventManager
        {
            std::vector<Component *> componentList;

        public:
            static StartEventManager *Instance();
            void registerNewComponent(Component *c);
            void unregisterComponent(Component *c);
            void processAllComponentsWithTransform();
        };

    }

}