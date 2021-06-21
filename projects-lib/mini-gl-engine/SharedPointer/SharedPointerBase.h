#ifndef shared_pointer_base__H_
#define shared_pointer_base__H_

#include <stdlib.h>

namespace GLEngine {

    class SharedPointerBase {
    protected:
        void* reference;
        SharedPointerBase();
    public:
        virtual ~SharedPointerBase();
        virtual bool isNULL();
        virtual void setNULL(bool unregister = true);
    };

}

#endif