
#ifndef start_event_manager__H__
#define start_event_manager__H__

#include <vector>

namespace GLEngine {

    class Transform;
    class Component;

    class StartEventManager{
        std::vector<Component *> componentList;
    public:
        static StartEventManager* Instance();
        void registerNewComponent(Component *c);
        void unregisterComponent(Component *c);
        void processAllComponentsWithTransform();
    };

}

#endif
