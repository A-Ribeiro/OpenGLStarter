#include "Component.h"
#include "Transform.h"

namespace GLEngine {

    //std::vector<Component *> componentList;
    StartEventManager* StartEventManager::Instance(){
        static StartEventManager startEventManager;
        return &startEventManager;
    }

    void StartEventManager::registerNewComponent(Component *c){
        componentList.push_back(c);
    }
    void StartEventManager::unregisterComponent(Component *c){
        for(int i=componentList.size()-1;i>=0;i--){
            if (componentList[i] == c){
                componentList.erase(componentList.begin()+i);
                return;
            }
        }
    }
    void StartEventManager::processAllComponentsWithTransform(){
        for(int i=componentList.size()-1;i>=0;i--){
            if (componentList[i]->transform.size()>0){
                componentList[i]->start();
                componentList.erase(componentList.begin()+i);
            }
        }
    }

}
