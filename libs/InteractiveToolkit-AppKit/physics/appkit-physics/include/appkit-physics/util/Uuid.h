#pragma once

#include <InteractiveToolkit/Platform/Core/SmartVector.h>
#include <InteractiveToolkit/Platform/Mutex.h>
#include <InteractiveToolkit/Platform/AutoLock.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Util
        {
            template <typename T>
            class Uuid
            {
                Platform::SmartVector<T> available_ids;
                T next_id;
                Platform::Mutex mutex;

            public:
                void setNextId(T id)
                {
                    Platform::AutoLock lock(&mutex);
                    next_id = id;
                }

                T next()
                {
                    Platform::AutoLock lock(&mutex);
                    if (!available_ids.empty())
                    {
                        T id = available_ids.front();
                        available_ids.pop_front();
                        return id;
                    }
                    return next_id++;
                }

                void release(T id)
                {
                    Platform::AutoLock lock(&mutex);
                    available_ids.push_back(id);
                }
            };

            using UuidUint32 = Uuid<uint32_t>;
            using UuidUint64 = Uuid<uint64_t>;
        }
    }
}
