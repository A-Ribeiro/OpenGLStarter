/// \file
#ifndef uintgcd__H
#define uintgcd__H

#include <stdint.h>
namespace rtaudioWrapper {
    /// \brief Unsigned integer GCD (greatest common divisor)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <rtaudio-wrapper/rtaudio-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace rtaudioWrapper;
    ///
    /// uint32_t width = 1920;
    /// uint32_t height = 1080;
    ///
    /// uint32_t gcd = uint32_gcd(width,height);
    ///
    /// uint32_t aspect_x = width / gcd; // 16
    /// uint32_t aspect_y = height / gcd; // 9
    ///
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a input number a
    /// \param b input number b
    /// \return the greatest common divisor considering 'a' and 'b'
    ///
    uint32_t uint32_gcd(uint32_t a, uint32_t b)
    {
        const uint32_t zero = 0;
        while (true)
        {
            if (a == zero)
                return b;
            b %= a;

            if (b == zero)
                return a;
            a %= b;
        }
    }
}


#endif