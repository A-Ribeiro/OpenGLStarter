#pragma once

#include "SharedPointerBase.h"

#include <map>
#include <vector>

namespace AppKit
{
    namespace GLEngine
    {

        class SharedPointerDatabase
        {
            std::map<void *, std::vector<SharedPointerBase *>> refs;

        public:
            static SharedPointerDatabase *Instance();

            void notifyDeletion(void *ref);
            void registerPointer(SharedPointerBase *pointer, void *ref);
            void unregisterPointer(SharedPointerBase *pointer, void *ref);
        };

    }
}