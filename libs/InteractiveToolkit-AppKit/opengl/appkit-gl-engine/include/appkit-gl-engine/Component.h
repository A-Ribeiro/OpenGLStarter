#pragma once

// #include <aRibeiroCore/aRibeiroCore.h>
// #include <appkit-gl-engine/SharedPointer.h>
// #include "StartEventManager.h"
#include <vector>
#include <InteractiveToolkit/EventCore/HandleCallback.h>

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

        public:
            std::vector<Transform *> transform;
            ComponentType getType() const;
            bool compareType(ComponentType t) const;

            virtual ~Component();

            virtual void start();
            // void callStartOnce();

            virtual void attachToTransform(Transform *t);
            virtual void detachFromTransform(Transform *t);
        };

    }

}