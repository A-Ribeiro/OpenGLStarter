#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/common.h>

#include <appkit-gl-engine/Transform.h>

namespace AppKit
{
    namespace GLEngine
    {
        /// \brief Animation utilities including keyframe storage and interpolation helpers.
        ///
        /// Provides the foundational types for keyframe-based animation systems,
        /// including the Key template for storing time-value pairs and interpolation
        /// specializations for different value types (e.g., lerp for scalars, slerp for quaternions).
        ///
        /// \author Alessandro Ribeiro
        ///
        namespace Animation
        {

            /// \brief A keyframe storing a value at a specific time.
            ///
            /// Used as the fundamental unit in animation keyframe sequences.
            /// Each key holds a timestamp and the corresponding animated value.
            ///
            /// \author Alessandro Ribeiro
            ///
            /// \tparam C Type of the animated value.
            ///
            template <class C>
            class Key
            {
            public:
                /// \brief The time position of this keyframe in seconds.
                ///
                float time;

                /// \brief The animated value at this keyframe.
                ///
                C value;

                /// \brief Construct a default Key with time 0 and default-constructed value.
                ///
                /// \author Alessandro Ribeiro
                ///
                inline Key() {}

                /// \brief Construct a Key with the given time and value.
                ///
                /// \author Alessandro Ribeiro
                /// \param _time The time position of this keyframe.
                /// \param _value The animated value at this keyframe.
                ///
                inline Key(float _time, const C &_value)
                {
                    time = _time;
                    value = _value;
                }
            };

            /// \brief Default interpolation specialization using linear interpolation (lerp).
            ///
            /// For scalar types and types without special interpolation requirements,
            /// linear interpolation is used between the two input values.
            ///
            /// \author Alessandro Ribeiro
            ///
            /// \tparam T Type to interpolate.
            ///
            template <typename T>
            struct InterpolateSpecialization
            {
                /// \brief Perform linear interpolation between a and b using blend factor lrp.
                ///
                /// \author Alessandro Ribeiro
                /// \param a First value.
                /// \param b Second value.
                /// \param lrp Blend factor in [0, 1].
                /// \return Interpolated value.
                ///
                static inline T doInterpolate(const T &a, const T &b, float lrp) noexcept { return MathCore::OP<T>::lerp(a, b, lrp); }
            };

            /// \brief Quaternion interpolation specialization using spherical linear interpolation (slerp).
            ///
            /// Quaternions require slerp instead of lerp to maintain constant angular velocity
            /// and avoid gimbal lock artifacts during rotation animation.
            ///
            /// \author Alessandro Ribeiro
            ///
            /// \tparam _BaseType Numeric type for quaternion components.
            /// \tparam _SimdType SIMD type for optimized operations.
            ///
            template <typename _BaseType, typename _SimdType>
            struct InterpolateSpecialization<MathCore::quat<_BaseType, _SimdType>>
            {
                /// \brief The quaternion type handled by this specialization.
                ///
                using type = MathCore::quat<_BaseType, _SimdType>;

                /// \brief Perform spherical linear interpolation between two quaternions.
                ///
                /// \author Alessandro Ribeiro
                /// \param a First quaternion.
                /// \param b Second quaternion.
                /// \param lrp Blend factor in [0, 1].
                /// \return Interpolated quaternion.
                ///
                static inline type doInterpolate(const type &a, const type &b, float lrp) noexcept { return MathCore::OP<type>::slerp(a, b, lrp); }
            };

        }
    }
}
