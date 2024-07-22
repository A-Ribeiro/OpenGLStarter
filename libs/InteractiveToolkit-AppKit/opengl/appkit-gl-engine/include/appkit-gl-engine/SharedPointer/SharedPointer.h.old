#pragma once

#include "SharedPointerBase.h"
#include "SharedPointerDatabase.h"

namespace AppKit
{
    namespace GLEngine
    {

        template <typename T>
        class SharedPointer : public SharedPointerBase
        {
        protected:
        public:
            SharedPointer() : SharedPointerBase()
            {
            }

            /*
            bool operator==(T* v) {
                return v == reference;
            }*/

            bool operator==(void *ptr) const
            {
                return ptr == reference;
            }
            bool operator!=(void *ptr) const
            {
                return ptr != reference;
            }

            void operator=(T *v)
            {
                SharedPointerDatabase::Instance()->unregisterPointer(this, reference);
                SharedPointerDatabase::Instance()->registerPointer(this, (void *)v);
                reference = (void *)v;
            }

            operator T *()
            {
                return (T *)reference;
            }

            T *operator->()
            {
                return (T *)reference;
            }
        };

    }

}