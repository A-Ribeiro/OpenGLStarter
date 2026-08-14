#pragma once

#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>

template <typename T>
struct IEEE_754_Info
{
};

template <>
struct IEEE_754_Info<double>
{
    typedef uint64_t utype;

    static constexpr uint64_t minus_one_u = UINT64_C(0xFFFFFFFFFFFFFFFF);
    static constexpr uint64_t one_u = UINT64_C(1);
    static constexpr uint64_t sign_bit_u = UINT64_C(0x8000000000000000);
    static constexpr uint64_t mantissa_bit_u = UINT64_C(0x000FFFFFFFFFFFFF);
    static constexpr uint64_t expoent_bit_u = UINT64_C(0x7FF0000000000000);
    static constexpr uint64_t number_except_sign_bit_u = mantissa_bit_u | expoent_bit_u;

    static constexpr uint64_t mantissa_min_u = UINT64_C(1);

    static constexpr uint64_t inf_u = UINT64_C(0x7FF0000000000000);
    static constexpr uint64_t max_float_u = UINT64_C(0x7FEFFFFFFFFFFFFF);
    static constexpr uint64_t neg_max_float_u = UINT64_C(0xFFEFFFFFFFFFFFFF);
    static constexpr uint64_t q_nan_u = UINT64_C(0x7FF8000000000000);
    static constexpr double inf = std::numeric_limits<double>::infinity();
    static constexpr double q_nan = std::numeric_limits<double>::quiet_NaN();

    static constexpr int shift_to_sign = 63;

    static inline uint64_t &as_uint(double &v) noexcept { return *(uint64_t *)&v; }
    static inline constexpr uint64_t invert_signal_2complement(const uint64_t &v) noexcept { return uint64_t(-int64_t(v)); }
    static inline constexpr bool is_nan(const double &v) noexcept { return !(v == v); }
    static inline constexpr bool is_inf(const double &v) noexcept { return (v == inf) || (v == -inf); }
    static inline constexpr bool is_nan(const uint64_t &v) noexcept { return ((v & expoent_bit_u) == inf_u) && ((v & mantissa_bit_u) != 0); }
    static inline constexpr bool is_inf(const uint64_t &v) noexcept { return ((v & expoent_bit_u) == inf_u) && ((v & mantissa_bit_u) == 0); }
};

template <>
struct IEEE_754_Info<float>
{
    typedef uint32_t utype;

    static constexpr uint32_t minus_one_u = UINT32_C(0xFFFFFFFF);
    static constexpr uint32_t one_u = UINT32_C(1);
    static constexpr uint32_t sign_bit_u = UINT32_C(0x80000000);
    static constexpr uint32_t mantissa_bit_u = UINT32_C(0x007FFFFF);
    static constexpr uint32_t expoent_bit_u = UINT32_C(0x7F800000);
    static constexpr uint32_t number_except_sign_bit_u = mantissa_bit_u | expoent_bit_u;

    static constexpr uint32_t mantissa_min_u = UINT32_C(1);

    static constexpr uint32_t inf_u = UINT32_C(0x7F800000);
    static constexpr uint32_t max_float_u = UINT32_C(0x7F7FFFFF);
    static constexpr uint32_t neg_max_float_u = UINT32_C(0xFF7FFFFF);
    static constexpr uint32_t q_nan_u = UINT32_C(0x7FC00000);
    static constexpr float inf = std::numeric_limits<float>::infinity();
    static constexpr float q_nan = std::numeric_limits<float>::quiet_NaN();

    static constexpr int shift_to_sign = 31;

    static inline uint32_t &as_uint(float &v) noexcept { return *(uint32_t *)&v; }
    static inline constexpr uint32_t invert_signal_2complement(const uint32_t &v) noexcept { return uint32_t(-int32_t(v)); }
    static inline constexpr bool is_nan(const float &v) noexcept { return !(v == v); }
    static inline constexpr bool is_inf(const float &v) noexcept { return (v == inf) || (v == -inf); }
    static inline constexpr bool is_nan(const uint32_t &v) noexcept { return ((v & expoent_bit_u) == inf_u) && ((v & mantissa_bit_u) != 0); }
    static inline constexpr bool is_inf(const uint32_t &v) noexcept { return ((v & expoent_bit_u) == inf_u) && ((v & mantissa_bit_u) == 0); }
};

template <typename type_>
static inline type_ nextafter_optim(type_ x, type_ y)
{
    if (IEEE_754_Info<type_>::is_nan(x) || IEEE_754_Info<type_>::is_nan(y))
        return IEEE_754_Info<type_>::q_nan;
    if (x == y)
        return y;

    typedef typename IEEE_754_Info<type_>::utype utype;

    utype &bits_x = IEEE_754_Info<type_>::as_uint(x);
    utype bits_number_only = bits_x & IEEE_754_Info<type_>::number_except_sign_bit_u;

    utype is_descending = utype(y < x);
    // Only treat as zero when both mantissa AND exponent are zero
    utype is_descending_sign = is_descending << IEEE_754_Info<type_>::shift_to_sign;
    if (!bits_number_only)
    {
        // is zero
        bits_x = is_descending_sign | IEEE_754_Info<type_>::mantissa_min_u;
        return x;
    }

    utype is_sign_negative_x = (bits_x & IEEE_754_Info<type_>::sign_bit_u);

    // increment will be -1 if is_descenting is true, and 1 if not
    utype increment = (is_sign_negative_x ^ is_descending_sign) ? IEEE_754_Info<type_>::minus_one_u : IEEE_754_Info<type_>::one_u;

    utype max_float = is_sign_negative_x | IEEE_754_Info<type_>::max_float_u;
    
    bits_number_only = (bits_number_only == IEEE_754_Info<type_>::inf_u) ? max_float : (bits_number_only + increment);

    bits_x = is_sign_negative_x | bits_number_only;

    return x;
}

#if defined(ITK_SSE2)

#include <InteractiveToolkit/MathCore/impl/simd_common.h>

const __m128i _all_bits_set = _mm_set1_epi32((int)0xffffffff);
const __m128i _mantissa_bit_u = _mm_set1_epi32((int)0x007FFFFF);
const __m128i _mantissa_min_u = _mm_set1_epi32((int)1);
const __m128i _sign_bit_u = _mm_set1_epi32((int)0x80000000);
const __m128i _minus_one_u = _mm_set1_epi32((int)0xFFFFFFFF);
const __m128i _one_u = _mm_set1_epi32((int)1);
const __m128i _number_except_sign_bit_u = _mm_set1_epi32((int)0x7FFFFFFF);
const __m128i _expoent_bit_u = _mm_set1_epi32((int)0x7F800000);
const __m128i _inf_u = _mm_set1_epi32((int)0x7F800000);

const __m128i _max_float_u = _mm_set1_epi32((int)0x7F7FFFFF);
const __m128i _neg_max_float_u = _mm_set1_epi32((int)0xFF7FFFFF);

const __m128i _zero = _mm_set1_epi32((int)0);
const __m128i _q_nan = _mm_set1_epi32((int)0x7FC00000);

static inline __m128i _sse2_is_nan_ps(const __m128 &v)
{
    // v == v returns false on NaN
    return _mm_xor_si128(_mm_castps_si128(_mm_cmpeq_ps(v, v)), _all_bits_set);
}

static inline __m128 _sse2_nextafter_ps(__m128 x, __m128 y)
{
    __m128i bits_x = _mm_castps_si128(x);
    __m128i bits_y = _mm_castps_si128(y);

    // NaN check
    __m128i is_nan_mask = _mm_or_si128(_sse2_is_nan_ps(x), _sse2_is_nan_ps(y));

    // x == y
    __m128i is_eq_mask = _mm_castps_si128(_mm_cmpeq_ps(x, y));

    // y < x (descending)
    __m128i is_descending = _mm_castps_si128(_mm_cmplt_ps(y, x));

    __m128i bits_number_only = _mm_and_si128(bits_x, _number_except_sign_bit_u);

    // Zero check
    __m128i is_zero = _mm_cmpeq_epi32(bits_number_only, _zero);
    __m128i zero_result = _mm_or_si128(_mm_and_si128(is_descending, _sign_bit_u), _mantissa_min_u);

    // Sign bit and its mask
    __m128i sign_x = _mm_and_si128(bits_x, _sign_bit_u);
    __m128i sign_mask = _mm_srai_epi32(sign_x, 31);

    // Increment direction: -1 if descending XOR negative, else +1
    __m128i increment = _mm_xor_si128(is_descending, sign_mask);
    increment = _mm_or_si128(_mm_and_si128(increment, _minus_one_u),
                             _mm_andnot_si128(increment, _one_u));

    // Inf check
    __m128i is_inf = _mm_cmpeq_epi32(bits_number_only, _inf_u);

    // For inf: use max_float with correct sign
    __m128i max_float = _mm_or_si128(sign_x, _max_float_u);

    // Increment or use max for inf
    __m128i result_number = _mm_add_epi32(bits_number_only, increment);
    result_number = _mm_or_si128(_mm_and_si128(is_inf, max_float),
                                 _mm_andnot_si128(is_inf, result_number));

    // // Overflow check: if result is inf, clamp to max_float
    // __m128i is_overflow = _mm_cmpeq_epi32(_mm_and_si128(result_number, _number_except_sign_bit_u), _inf_u);
    // result_number = _mm_or_si128(_mm_and_si128(is_overflow, max_float),
    //                               _mm_andnot_si128(is_overflow, result_number));

    // Reconstruct with sign
    __m128i result = _mm_or_si128(sign_x, result_number);

    // Zero case
    result = _mm_or_si128(_mm_and_si128(is_zero, zero_result),
                          _mm_andnot_si128(is_zero, result));

    // x == y case
    result = _mm_or_si128(_mm_and_si128(is_eq_mask, bits_y),
                          _mm_andnot_si128(is_eq_mask, result));

    // NaN case
    result = _mm_or_si128(_mm_and_si128(is_nan_mask, _q_nan),
                          _mm_andnot_si128(is_nan_mask, result));

    return _mm_castsi128_ps(result);
}
#endif
