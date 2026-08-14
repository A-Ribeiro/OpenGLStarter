#pragma once
// #include <iostream>
// #include <cmath>
// #include <cstdint>
// #include <limits>

// // --- Funções Auxiliares (Mantidas como no seu código) ---

// static inline void extract_double_bits(double value, uint64_t &sign_out, uint64_t &exponent_out, uint64_t &mantissa_out)
// {
//     uint64_t raw_bits = *(uint64_t *)&value;
//     sign_out = (raw_bits >> 63) & 0x1;
//     exponent_out = (raw_bits >> 52) & 0x7FF;         // 11 bits
//     mantissa_out = raw_bits & 0x000FFFFFFFFFFFFFULL; // 52 bits
// }

// static inline double reconstruct_double(uint64_t sign, uint64_t exponent, uint64_t mantissa)
// {
//     uint64_t raw_bits = (sign << 63) | (exponent << 52) | mantissa;
//     return *(double *)&raw_bits;
// }

// // --- Constantes de IEEE 754 ---
// // Máscara de 52 bits
// constexpr uint64_t MANTISSA_MASK = 0x000FFFFFFFFFFFFF;
// // Limite máximo da mantissa (52 bits)
// constexpr uint64_t MAX_MANTISSA_VALUE = 0x000FFFFFFFFFFFFF;

// static inline double nextafter_custom_double(double x, double y)
// {
//     int64_t sign, exponent, mantissa;
//     extract_double_bits(x, *(uint64_t *)&sign, *(uint64_t *)&exponent, *(uint64_t *)&mantissa);

//     bool positive = y > x;
//     bool sign_dir = (bool)sign ^ !positive;

//     sign_dir = sign_dir ^ !mantissa;

//     if (sign_dir)
//     {
//         mantissa = mantissa - 1;
//         if (mantissa < 0)
//         {
//             if (mantissa == 0)
//                 return 0.0;
//             mantissa = 0;
//             exponent--;
//         }
//     }
//     else
//     {
//         mantissa = mantissa + 1;
//         if (mantissa > MAX_MANTISSA_VALUE)
//         {
//             mantissa = 0;
//             exponent++;
//             if (exponent > 2047) // 11 bits - 1
//                 return std::numeric_limits<double>::infinity();
//         }
//     }

//     return reconstruct_double((uint64_t)sign, (uint64_t)exponent, (uint64_t)mantissa);
// }

#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>

static inline uint64_t double_to_bits(double x)
{
    uint64_t bits;
    std::memcpy(&bits, &x, sizeof(bits));
    return bits;
}

static inline double bits_to_double(uint64_t bits)
{
    double x;
    std::memcpy(&x, &bits, sizeof(x));
    return x;
}

static inline uint32_t float_to_bits(float x)
{
    uint32_t bits;
    std::memcpy(&bits, &x, sizeof(bits));
    return bits;
}

static inline float bits_to_float(uint32_t bits)
{
    float x;
    std::memcpy(&x, &bits, sizeof(x));
    return x;
}

template <typename T>
struct IEEE_754_Info
{
};

union double_bits
{
    uint64_t u;
    double d;
};

union float_bits
{
    uint32_t u;
    float d;
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
    // utype mantissa_x = bits_x & IEEE_754_Info<type_>::mantissa_bit_u;
    // utype exponent_x = bits_x & IEEE_754_Info<type_>::expoent_bit_u;
    utype bits_number_only = bits_x & IEEE_754_Info<type_>::number_except_sign_bit_u;

    utype is_descending = utype(y < x);
    // Only treat as zero when both mantissa AND exponent are zero
    if (!bits_number_only)
    {
        // is zero
        utype is_descending_sign = is_descending << IEEE_754_Info<type_>::shift_to_sign;
        bits_x = is_descending_sign | IEEE_754_Info<type_>::mantissa_min_u;
        return x;
    }

    utype is_sign_negative_x = (bits_x & IEEE_754_Info<type_>::sign_bit_u);

    // increment will be -1 if is_descenting is true, and 1 if not
    utype is_sign_negative_x_b = is_sign_negative_x >> IEEE_754_Info<type_>::shift_to_sign;
    utype select_minus_one = is_descending ^ is_sign_negative_x_b;
    utype increment = (select_minus_one) ? IEEE_754_Info<type_>::minus_one_u : IEEE_754_Info<type_>::one_u;

    // utype increment = (is_descending) ? IEEE_754_Info<type_>::minus_one_u : IEEE_754_Info<type_>::one_u;
    // increment = (is_sign_negative_x) ? IEEE_754_Info<type_>::invert_signal_2complement(increment) : increment;

    // utype bits_number_only = bits_x & IEEE_754_Info<type_>::number_except_sign_bit_u;

    bool is_inf = IEEE_754_Info<type_>::is_inf(bits_number_only);
    if (is_inf)
    {
        utype selected_max_f = (is_descending) ? IEEE_754_Info<type_>::max_float_u : IEEE_754_Info<type_>::neg_max_float_u;
        bits_number_only = (is_sign_negative_x_b) ? (selected_max_f) : bits_number_only;
    }
    else
        bits_number_only = bits_number_only + increment;

    // overflow check: if incrementing max_float overflowed to infinity, clamp back to max_float
    // bool overflowed = ((bits_number_only & IEEE_754_Info<type_>::expoent_bit_u) == IEEE_754_Info<type_>::inf_u) &&
    //                   ((bits_number_only & IEEE_754_Info<type_>::mantissa_bit_u) == 0);
    bool overflowed = ((bits_number_only & IEEE_754_Info<type_>::number_except_sign_bit_u) == IEEE_754_Info<type_>::inf_u);
    bits_number_only = overflowed ? IEEE_754_Info<type_>::max_float_u : bits_number_only;

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
    __m128i is_nan_mask = _mm_or_si128(_sse2_is_nan_ps(x), _sse2_is_nan_ps(y));

    __m128i is_eq_x_y_mask = _mm_castps_si128(_mm_cmpeq_ps(x, y));

    // _mm_test_all_ones
    // y < x
    __m128i is_descending = _mm_castps_si128(_mm_cmplt_ps(y, x));

    __m128i bits_x = _mm_castps_si128(x);
    __m128i bits_number_only = _mm_and_si128(bits_x, _number_except_sign_bit_u);

    // __m128i mantissa_x = _mm_and_si128(bits_x, _mantissa_bit_u);
    // __m128i expoent_x = _mm_and_si128(bits_x, _expoent_bit_u);

    // Only treat as zero when both mantissa AND exponent are zero
    //__m128i is_value_zero = _mm_and_si128(_mm_cmpeq_epi32(mantissa_x, _zero), _mm_cmpeq_epi32(expoent_x, _zero));
    __m128i is_value_zero = _mm_cmpeq_epi32(bits_number_only, _zero);
    __m128i is_descending_sign = _mm_and_si128(is_descending, _sign_bit_u);
    __m128i result_bits_x_on_mantissa_zero = _mm_or_si128(is_descending_sign, _mantissa_min_u);

    __m128i is_sign_negative_x = _mm_and_si128(bits_x, _sign_bit_u);
    __m128i is_sign_negative_x_mask = _mm_srai_epi32(is_sign_negative_x, 31);

    __m128i select_minus_one = _mm_xor_si128(is_descending, is_sign_negative_x_mask);

    __m128i increment = _mm_or_si128(
        _mm_and_si128(select_minus_one, _minus_one_u),
        _mm_andnot_si128(select_minus_one, _one_u));

    // __m128i bits_number_only = _mm_and_si128(bits_x, _number_except_sign_bit_u);
    // __m128i expoent_x = _mm_and_si128(bits_x, _expoent_bit_u);
    __m128i is_inf_mask = _mm_cmpeq_epi32(bits_number_only, _inf_u);

    __m128i selected_max_f =
        _mm_or_si128(_mm_and_si128(is_descending, _max_float_u),
                     _mm_andnot_si128(is_descending, _neg_max_float_u));

    __m128i bits_number_only_aux =
        _mm_or_si128(_mm_and_si128(is_sign_negative_x_mask, selected_max_f),
                     _mm_andnot_si128(is_sign_negative_x_mask, bits_number_only));

    __m128i bits_number_only_inc = _mm_add_epi32(bits_number_only, increment);

    bits_number_only = _mm_or_si128(
        _mm_and_si128(is_inf_mask, bits_number_only_aux),
        _mm_andnot_si128(is_inf_mask, bits_number_only_inc));

    // overflow, transform bits_number_only into +inf
    //__m128i has_overflow_mask = _mm_srai_epi32(_mm_and_si128(bits_number_only, _sign_bit_u), 31);

    __m128i has_overflow_mask = _mm_cmpeq_epi32(
        _mm_and_si128(bits_number_only, _number_except_sign_bit_u),
        _inf_u);

    bits_number_only = _mm_or_si128(
        _mm_and_si128(has_overflow_mask, _max_float_u),
        _mm_andnot_si128(has_overflow_mask, bits_number_only));

    __m128i result_bits_x_on_valid_mantissa = _mm_or_si128(is_sign_negative_x, bits_number_only);

    __m128i result = _mm_or_si128(
        _mm_and_si128(is_value_zero, result_bits_x_on_mantissa_zero),
        _mm_andnot_si128(is_value_zero, result_bits_x_on_valid_mantissa));

    // remaining filters
    result = _mm_or_si128(
        _mm_and_si128(is_eq_x_y_mask, _mm_castps_si128(y)),
        _mm_andnot_si128(is_eq_x_y_mask, result));

    result = _mm_or_si128(
        _mm_and_si128(is_nan_mask, _q_nan),
        _mm_andnot_si128(is_nan_mask, result));

    return _mm_castsi128_ps(result);
}
#endif

static inline double nextafter_custom_double(double x, double y)
{
    // NaN
    if (std::isnan(x) || std::isnan(y))
        return std::numeric_limits<double>::quiet_NaN();

    // x == y, including equal infinities
    if (x == y)
        return y;

    // Starting from zero
    if (x == 0.0)
    {
        uint64_t bits = std::signbit(y)
                            ? 0x8000000000000001ULL  // -smallest subnormal
                            : 0x0000000000000001ULL; // +smallest subnormal

        return bits_to_double(bits);
    }

    uint64_t bits = double_to_bits(x);

    if (x > 0.0)
        bits += (y > x) ? 1ULL : UINT64_C(0xFFFFFFFFFFFFFFFF);
    else
        bits += (y > x) ? UINT64_C(0xFFFFFFFFFFFFFFFF) : 1ULL;

    return bits_to_double(bits);
}

static inline float nextafter_optim(float x, float y)
{
    if (IEEE_754_Info<float>::is_nan(x) || IEEE_754_Info<float>::is_nan(y))
        return IEEE_754_Info<float>::q_nan;
    if (x == y)
        return y;

    uint32_t &bits_x = IEEE_754_Info<float>::as_uint(x);
    uint32_t mantissa_x = bits_x & IEEE_754_Info<float>::mantissa_bit_u;
    uint32_t is_descending = uint32_t(y < x);
    if (!mantissa_x)
    {
        // is zero
        uint32_t is_descending_sign = is_descending << IEEE_754_Info<float>::shift_to_sign;
        bits_x = is_descending_sign | IEEE_754_Info<float>::mantissa_min_u;
        return x;
    }

    uint32_t is_sign_negative_x = (bits_x & IEEE_754_Info<float>::sign_bit_u);

    // increment will be -1 if is_descenting is true, and 1 if not
    uint32_t increment = (is_descending) ? IEEE_754_Info<float>::minus_one_u : IEEE_754_Info<float>::one_u;
    increment = (is_sign_negative_x) ? IEEE_754_Info<float>::invert_signal_2complement(increment) : increment;

    uint32_t bits_number_only = bits_x & IEEE_754_Info<float>::number_except_sign_bit_u;

    bits_number_only += increment;

    // overflow, transform bits_number_only into +inf
    if (bits_number_only & IEEE_754_Info<float>::sign_bit_u)
        bits_number_only = IEEE_754_Info<float>::inf_u;

    bits_x = is_sign_negative_x |
             (bits_number_only & IEEE_754_Info<float>::number_except_sign_bit_u);

    return x;
}