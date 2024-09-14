#pragma once

#include "SceneSerializer.h"

namespace AppKit
{
    namespace GLEngine
    {

        class JSONSceneSerializer: public SceneSerializer
        {
        public:
            Platform::ObjectBuffer serialize(std::shared_ptr<Transform> transform, bool include_root);
            std::shared_ptr<Transform> deserialize(const Platform::ObjectBuffer &src);
        };

    }
}
