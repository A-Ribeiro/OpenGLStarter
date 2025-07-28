#include <appkit-gl-engine/StartEventManager.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>

namespace AppKit
{
    namespace GLEngine
    {

        // std::vector<Component *> componentList;
        StartEventManager *StartEventManager::Instance()
        {
            static StartEventManager startEventManager;
            return &startEventManager;
        }

        void StartEventManager::registerNewComponent(Component *c)
        {
            componentList.push_back(c);
        }
        bool StartEventManager::unregisterComponent(Component *c)
        {
            for (auto it = componentList.begin(); it != componentList.end(); ++it)
            {
                if (*it == c)
                {
                    componentList.erase(it);
                    return true;
                }
            }
            return false;
        }
        void StartEventManager::processAllComponentsWithTransform()
        {
            while (componentList.size() > 0)
            {
                auto c = componentList.front();
                componentList.pop_front();
                c->start();
            }
        }

    }

}