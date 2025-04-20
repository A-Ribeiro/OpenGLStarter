#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/StartEventManager.h>


//#include <appkit-gl-engine/SharedPointer/SharedPointerDatabase.h>

namespace AppKit
{
    namespace GLEngine
    {

        Component::Component(const Component &v) {}
        Component& Component::operator=(const Component &v) {return *this;}

        Component::Component(ComponentType type)
        {
            this->type_const_ref = type;
            // transform = nullptr;
            // mStartCalled = false;
            StartEventManager::Instance()->registerNewComponent(this);
        }

        ComponentType Component::getType() const
        {
            return type_const_ref;
            // return type.c_str();
        }

        bool Component::compareType(ComponentType t) const
        {
            return (type_const_ref == t);
            // if (type_const_ref == t)
            // return true;
            // return strcmp(type_const_ref,t) == 0;
        }

        Component::~Component()
        {
            // SharedPointerDatabase::Instance()->notifyDeletion(this);
            StartEventManager::Instance()->unregisterComponent(this);
        }

        void Component::start()
        {
        }

        // void Component::callStartOnce() {
        //     if (mStartCalled)
        //         return;
        //     mStartCalled = true;
        //     start();
        // }

        void Component::attachToTransform(std::shared_ptr<Transform> t)
        {
        }

        void Component::detachFromTransform(std::shared_ptr<Transform> t)
        {
        }

    }

}