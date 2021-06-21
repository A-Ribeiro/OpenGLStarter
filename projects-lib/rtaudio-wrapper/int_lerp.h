/// \file
#ifndef int_lerp__H
#define int_lerp__H

#define _UINT8_MAX 0x7f
#define _UINT16_MAX 0x7fff
#define _UINT24_MAX 0x7fffff
#define _UINT32_MAX 0x7fffffff

#include <stdint.h>

namespace rtaudioWrapper {
    /// \brief Compute the int32_t linear interpolation (lerp) using convertion to float as basis.
    ///
    /// This template can by applied to the int24_t, int16_t or int8_t.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <rtaudio-wrapper/rtaudio-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace rtaudioWrapper;
    ///
    /// int32_t a, b;
    /// float amount = 0.75f;
    /// int32_t result = int_lerp32_single( a, b, amount );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a the input value a
    /// \param b the input value b
    /// \param lerp the amount of linear interpolation
    /// \return the linear interpolation from 'a' to 'b' considering the amount 'lerp'
    ///
    static inline
        int32_t int_lerp32_single(const int32_t&a, const int32_t&b, float lerp) {
        float result = (float)a * (1.0f - lerp) + (float)b * lerp;
        if (result < 0)
            result -= 0.5f;
        else
            result += 0.5f;
        return (int32_t)result;
    }

    /// \brief Compute the int32_t linear interpolation (lerp) using convertion to double as basis.
    ///
    /// This template can by applied to the int32_t.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <rtaudio-wrapper/rtaudio-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace rtaudioWrapper;
    ///
    /// int32_t a, b;
    /// float amount = 0.75f;
    /// int32_t result = int_lerp32_double( a, b, amount );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a the input value a
    /// \param b the input value b
    /// \param lerp the amount of linear interpolation
    /// \return the linear interpolation from 'a' to 'b' considering the amount 'lerp'
    ///
    static inline
        int32_t int_lerp32_double(const int32_t&a, const int32_t&b, double lerp) {
        double result = (double)a * (1.0 - lerp) + (double)b * lerp;
        if (result < 0)
            result -= 0.5f;
        else
            result += 0.5f;
        if (result <= -_UINT32_MAX)
            return -_UINT32_MAX;
        else if (result >= _UINT32_MAX)
            return _UINT32_MAX;
        return (int32_t)result;
    }

    /// \brief Compute any int type linear interpolation (lerp)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <rtaudio-wrapper/rtaudio-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace rtaudioWrapper;
    ///
    /// float amount = 0.75f;
    ///
    /// ...
    ///
    /// int32_t a, b;
    /// int32_t result = int_lerp<int32_t>( a, b, amount );
    ///
    /// ...
    ///
    /// int24_t a, b;
    /// int24_t result = int_lerp<int24_t>( a, b, amount );
    ///
    /// ...
    ///
    /// int16_t a, b;
    /// int16_t result = int_lerp<int16_t>( a, b, amount );
    ///
    /// ...
    ///
    /// int8_t a, b;
    /// int8_t result = int_lerp<int8_t>( a, b, amount );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a the input value a
    /// \param b the input value b
    /// \param lerp the amount of linear interpolation
    /// \return the linear interpolation from 'a' to 'b' considering the amount 'lerp'
    ///
    template<typename C>
    static inline
        C int_lerp(const C&a, const C&b, float lerp) {
        if (sizeof(C) >= 4)
            return int_lerp32_double(a, b, lerp);
        else
            return int_lerp32_single(a, b, lerp);
    }
}

#endif