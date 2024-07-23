#pragma once

// #include <aRibeiroCore/aRibeiroCore.h>
// #include <appkit-gl-engine/SharedPointer.h>
// #include "StartEventManager.h"
#include <vector>
#include <InteractiveToolkit/EventCore/HandleCallback.h>
#include "ToShared.h"

namespace AppKit
{
    namespace GLEngine
    {

        class Component;
        class Transform;

        // BEGIN_DECLARE_DELEGATE(VoidEvent)
        // CALL_PATTERN() END_DECLARE_DELEGATE;
        // BEGIN_DECLARE_DELEGATE(TwoComponentsEvent, Component *a, Component *b)
        // CALL_PATTERN(a, b) END_DECLARE_DELEGATE;

        typedef const char *ComponentType;

        class Component : public EventCore::HandleCallback
        {
        private:
            Component(const Component &v);
            void operator=(const Component &v);
            // bool mStartCalled;
        protected:
            // all subclasses need to provide a const char* ref in the constructor
            ComponentType type_const_ref;
            Component(ComponentType type);

            std::weak_ptr<Component> mSelf;
            std::vector<std::weak_ptr<Transform>> mTransform;
        public:
            std::shared_ptr<Transform> getTransform(int i = 0){
                return ToShared(mTransform[i]);
            }
            int getTransformCount() const {
                return (int)mTransform.size();
            }
            
            ComponentType getType() const;
            bool compareType(ComponentType t) const;

            virtual ~Component();

            virtual void start();
            // void callStartOnce();

            virtual void attachToTransform(std::shared_ptr<Transform> t);
            virtual void detachFromTransform(std::shared_ptr<Transform> t);

            inline std::shared_ptr<Component> self() {
                return std::shared_ptr<Component>(mSelf);
            }

            template <typename _ComponentType,
                  typename std::enable_if<
                      std::is_base_of< Component, _ComponentType >::value,
                      bool>::type = true>
            inline std::shared_ptr<_ComponentType> self() {
                return std::dynamic_pointer_cast<_ComponentType>(self());
            }

            template <typename _ComponentType,
                  typename std::enable_if<
                      std::is_base_of< Component, _ComponentType >::value,
                      bool>::type = true>
            static inline std::shared_ptr<_ComponentType> CreateShared()
            {
                auto result = std::make_shared<_ComponentType>();
                result->mSelf = std::weak_ptr<Component>(result);
                return result;
            }
            
            friend class Transform;
        };

    }

}