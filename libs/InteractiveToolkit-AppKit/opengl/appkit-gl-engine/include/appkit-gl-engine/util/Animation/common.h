#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/common.h>

#include <appkit-gl-engine/Transform.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Animation
        {

            template <class C>
            class Key
            {
            public:
                float time;
                C value;
                Key() {}
                Key(float _time, const C &_value)
                {
                    time = _time;
                    value = _value;
                }
            };

            template <typename T>
            struct InterpolateSpecialization
            {
                static inline T doInterpolate(const T &a, const T &b, float lrp) noexcept { return MathCore::OP<T>::lerp(a, b, lrp); }
            };
            template <typename _BaseType, typename _SimdType>
            struct InterpolateSpecialization<MathCore::quat<_BaseType, _SimdType>>
            {
                using type = MathCore::quat<_BaseType, _SimdType>;
                static inline type doInterpolate(const type &a, const type &b, float lrp) noexcept { return MathCore::OP<type>::slerp(a, b, lrp); }
            };

        }
    }
}
