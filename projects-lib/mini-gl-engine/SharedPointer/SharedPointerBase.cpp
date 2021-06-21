#include "SharedPointer.h"

namespace GLEngine {

    SharedPointerBase::SharedPointerBase() {
        reference = NULL;
    }

    SharedPointerBase::~SharedPointerBase() {
        setNULL();
    }

    bool SharedPointerBase::isNULL() {
        return reference == NULL;
    }


    void SharedPointerBase::setNULL(bool unregister) {
        if (unregister)
            SharedPointerDatabase::Instance()->unregisterPointer(this, reference);
        reference = NULL;
    }

}