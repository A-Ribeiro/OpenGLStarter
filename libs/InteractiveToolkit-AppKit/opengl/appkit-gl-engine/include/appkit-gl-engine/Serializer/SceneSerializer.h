#pragma once

#include "../Transform.h"
#include "../Component.h"

namespace AppKit
{
    namespace GLEngine
    {

        class SceneSerializer
        {
        public:
            virtual Platform::ObjectBuffer serialize(std::shared_ptr<Transform> transform, bool include_root) = 0;
            virtual std::shared_ptr<Transform> deserialize(const Platform::ObjectBuffer &src) = 0;
        };

    }
}
