#ifndef shared_pointer_database__H_
#define shared_pointer_database__H_

#include "SharedPointerBase.h"

#include <map>
#include <vector>

namespace GLEngine {

    class SharedPointerDatabase {
        std::map <void *, std::vector<SharedPointerBase*> > refs;
    public:
        static SharedPointerDatabase* Instance();

        void notifyDeletion(void* ref);
        void registerPointer(SharedPointerBase *pointer, void *ref);
        void unregisterPointer(SharedPointerBase *pointer, void *ref);

    };

}

#endif