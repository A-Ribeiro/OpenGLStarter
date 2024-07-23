#pragma once

#include <memory>

namespace AppKit
{
    namespace GLEngine
    {
        template <typename _ComponentType
        // ,typename std::enable_if<
        //             std::is_base_of< Component, _ComponentType >::value ||
        //             std::is_base_of< Transform, _ComponentType >::value,
        //             bool>::type = true
                    >
        static inline std::shared_ptr<_ComponentType> ToShared( std::weak_ptr<_ComponentType> ref ) {
            if (ref.use_count() == 0)
                return nullptr;
            return std::shared_ptr<_ComponentType>(ref);
        }
    }
}