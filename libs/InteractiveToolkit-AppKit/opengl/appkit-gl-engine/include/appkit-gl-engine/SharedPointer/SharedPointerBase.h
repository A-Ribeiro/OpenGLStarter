#pragma once

#include <stdlib.h>

namespace AppKit
{
    namespace GLEngine
    {

        class SharedPointerBase
        {
        protected:
            void *reference;
            SharedPointerBase();

        public:
            virtual ~SharedPointerBase();
            virtual bool isNULL();
            virtual void setNULL(bool unregister = true);
        };

    }
}