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
        void StartEventManager::unregisterComponent(Component *c)
        {
            for (int i = (int)componentList.size() - 1; i >= 0; i--)
            {
                if (componentList[i] == c)
                {
                    componentList.erase(componentList.begin() + i);
                    return;
                }
            }
        }
        void StartEventManager::processAllComponentsWithTransform()
        {
            for (int i = (int)componentList.size() - 1; i >= 0; i--)
            {
                if (componentList[i]->getTransformCount() > 0)
                {
                    componentList[i]->start();
                    componentList.erase(componentList.begin() + i);
                }
            }
        }

    }

}