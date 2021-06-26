/// \file
#ifndef floatOPs__H
#define floatOPs__H

#include "constants.h"
#include <aribeiro/SSE2.h>

namespace aRibeiro {


#if defined(ARIBEIRO_NEON)

    ARIBEIRO_INLINE float32x4_t vdivq_f32(const float32x4_t &a, const float32x4_t &b)
    {
        float32x4_t recip0 = vrecpeq_f32(b);
        float32x4_t recip1 = vmulq_f32(recip0, vrecpsq_f32(recip0, b));
        return vmulq_f32(a, recip1);
    }

    ARIBEIRO_INLINE float32x4_t vset1(const float32_t &a)
    {
        return vdupq_n_f32(a);
    }




    ARIBEIRO_INLINE float32x4_t vshuffle_2301(const float32x4_t &a)
    {
        return vrev64q_f32(a);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1032(const float32x4_t &a)
    {
        return vcombine_f32(vget_high_f32(a), vget_low_f32(a));
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0123(const float32x4_t &a)
    {
        return vshuffle_2301(vshuffle_1032(a));
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0000(const float32x4_t &a)
    {
        return vdupq_lane_f32(vget_low_f32(a), 0); //a[0]
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1111(const float32x4_t &a)
    {
        return vdupq_lane_f32(vget_low_f32(a), 1); //a[1]
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_2222(const float32x4_t &a)
    {
        return vdupq_lane_f32(vget_high_f32(a), 0); //a[2]
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_3333(const float32x4_t &a)
    {
        return vdupq_lane_f32(vget_high_f32(a), 1); //a[3]
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0210(const float32x4_t &a)
    {
        float32x2_t l0 = vget_low_f32(a);
        float32x2x2_t r = vtrn_f32(vget_high_f32(a), l0);
        return vcombine_f32(l0, r.val[0]);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0333(const float32x4_t &a)
    {
        float32x4_t r = vshuffle_3333(a);
        //const float32x4_t _zero = vset1(0);
        float32x4_t _zero = vshuffle_0000(a);
        return vextq_f32(r, _zero, 1);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1021(const float32x4_t &a)
    {
        return vcombine_f32(vget_low_f32(vextq_f32(a, a, 1)), vget_low_f32(a));
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1102(const float32x4_t &a)
    {
        float32x2_t l0 = vget_low_f32(a);
        float32x2x2_t r = vtrn_f32(vget_high_f32(a), l0);
        return vcombine_f32(r.val[0], vdup_lane_f32(l0, 1));
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_2102(const float32x4_t &a)
    {
        float32x4_t r = vshuffle_2222(a);
        return vextq_f32(r, a, 3);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_2021(const float32x4_t &a)
    {
        float32x4_t _21 = vextq_f32(a, a, 1);
        float32x2x2_t _trn = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        return vcombine_f32(vget_low_f32(_21), _trn.val[0]);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_3021(const float32x4_t &a)
    {
        float32x2x2_t r = vtrn_f32(vrev64_f32(vget_low_f32(a)), vget_high_f32(a));
        return vcombine_f32(r.val[0], r.val[1]);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_3102(const float32x4_t &a)
    {
        float32x2x2_t r = vtrn_f32(vget_high_f32(a), vget_low_f32(a));
        return vcombine_f32(r.val[0], vrev64_f32(r.val[1]));
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_2000(const float32x4_t &a)
    {
        float32x4_t _zero = vdupq_lane_f32(vget_low_f32(a), 0);
        float32x4_t _two = vdupq_lane_f32(vget_high_f32(a), 0);
        return vextq_f32(_zero, _two, 1);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_2220(const float32x4_t &a)
    {
        float32x4_t _zero = vdupq_lane_f32(vget_low_f32(a), 0);
        float32x4_t _two = vdupq_lane_f32(vget_high_f32(a), 0);
        return vextq_f32(_zero, _two, 3);
    }


    ARIBEIRO_INLINE float32x4_t vshuffle_1110_test(const float32x4_t &a)
    {
        float32x4_t _a = vdupq_lane_f32(vget_low_f32(a), 0);
        float32x4_t _b = vdupq_lane_f32(vget_high_f32(a), 1);
        return vextq_f32(_a, _b, 3);
    }


    ARIBEIRO_INLINE float32x4_t vshuffle_1110(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 1);
        return vcombine_f32(_a_low, _b);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0001(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 0);
        return vcombine_f32(vrev64_f32(_a_low), _b);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1101(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 1);
        return vcombine_f32(vrev64_f32(_a_low), _b);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0010(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 0);
        return vcombine_f32(_a_low, _b);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1011(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 1);
        return vcombine_f32(_b, _a_low);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0100(const float32x4_t &a)
    {
        float32x2_t _a_low = vget_low_f32(a);
        float32x2_t _b = vdup_lane_f32(_a_low, 0);
        return vcombine_f32(_b, vrev64_f32(_a_low));
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0221(const float32x4_t &a)
    {
        float32x2_t l0 = vget_low_f32(a);
        float32x2x2_t r = vtrn_f32(vget_high_f32(a), l0);
        return vcombine_f32(vget_low_f32(vextq_f32(a, a, 1)), r.val[0]);
    }


    ARIBEIRO_INLINE float32x4_t vshuffle_0000(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t a_ = vdup_lane_f32(vget_low_f32(a), 0);
        float32x2_t b_ = vdup_lane_f32(vget_low_f32(b), 0);
        return vcombine_f32(a_, b_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1111(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t a_ = vdup_lane_f32(vget_low_f32(a), 1);
        float32x2_t b_ = vdup_lane_f32(vget_low_f32(b), 1);
        return vcombine_f32(a_, b_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_2222(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t a_ = vdup_lane_f32(vget_high_f32(a), 0);
        float32x2_t b_ = vdup_lane_f32(vget_high_f32(b), 0);
        return vcombine_f32(a_, b_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_3333(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t a_ = vdup_lane_f32(vget_high_f32(a), 1);
        float32x2_t b_ = vdup_lane_f32(vget_high_f32(b), 1);
        return vcombine_f32(a_, b_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_2020(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2x2_t a_ = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        float32x2x2_t b_ = vtrn_f32(vget_low_f32(b), vget_high_f32(b));
        return vcombine_f32(a_.val[0], b_.val[0]);
    }

    /*
    __m128 a = _mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 b = _mm_set_ps(4.0f, 5.0f, 6.0f, 7.0f);
    __m128 result = _mm_movehl_ps(a, b);
    printf("%f %f %f %f\n", _mm_f32_(result,0), _mm_f32_(result, 1), _mm_f32_(result, 2), _mm_f32_(result, 3));
    //5 4 2 1

    */
    ARIBEIRO_INLINE float32x4_t vmovehl(const float32x4_t &__A, const float32x4_t &__B)
    {
        return vcombine_f32(vget_high_f32(__B), vget_high_f32(__A));
    }


    ARIBEIRO_INLINE float32x4_t vshuffle_0112(const float32x4_t &a)
    {
        float32x2_t _01_ = vrev64_f32(vget_low_f32(a));
        float32x2_t _12_ = vrev64_f32( vget_low_f32(vextq_f32(a, a, 1)) );
        return vcombine_f32(_12_, _01_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_3233(const float32x4_t &a)
    {
        float32x2_t _32_ = vget_high_f32(a);
        float32x2_t _33_ = vdup_lane_f32(vget_high_f32(a), 1);
        return vcombine_f32(_33_, _32_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0012(const float32x4_t &a)
    {
        float32x2_t _12_ = vrev64_f32( vget_low_f32(vextq_f32(a, a, 1)) );
        float32x2_t _00_ = vdup_lane_f32(vget_low_f32(a), 0);
        return vcombine_f32(_12_,_00_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1200(const float32x4_t &a)
    {
        float32x2_t _12_ = vrev64_f32( vget_low_f32(vextq_f32(a, a, 1)) );
        float32x2_t _00_ = vdup_lane_f32(vget_low_f32(a), 0);
        return vcombine_f32(_00_, _12_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_2100(const float32x4_t &a)
    {
        float32x2_t _21_ = vget_low_f32(vextq_f32(a, a, 1));
        float32x2_t _00_ = vdup_lane_f32(vget_low_f32(a), 0);
        return vcombine_f32(_00_, _21_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0031(const float32x4_t &a)
    {
        float32x2x2_t _31_a = vtrn_f32(vget_high_f32(a), vget_low_f32(a));
        float32x2_t _31_ = vrev64_f32(_31_a.val[1]);
        float32x2_t _00_ = vdup_lane_f32(vget_low_f32(a), 0);
        return vcombine_f32(_31_, _00_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_3110(const float32x4_t &a)
    {
        float32x2x2_t _31_a = vtrn_f32(vget_high_f32(a), vget_low_f32(a));
        float32x2_t _31_ = vrev64_f32(_31_a.val[1]);
        float32x2_t _10_ = vget_low_f32(a);
        return vcombine_f32(_10_, _31_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1122(const float32x4_t &a)
    {
        float32x4_t _two = vshuffle_2222(a);
        float32x4_t _one = vshuffle_1111(a);
        return vextq_f32(_two, _one, 2);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1022(const float32x4_t &a)
    {
        float32x2_t _10_ = vget_low_f32(a);
        float32x2_t _22_ = vdup_lane_f32(vget_high_f32(a), 0);
        return vcombine_f32(_22_, _10_);
    }


    ARIBEIRO_INLINE float32x4_t vshuffle_3320(const float32x4_t &a)
    {
        float32x2x2_t _20_ = vtrn_f32(vget_low_f32(a), vget_high_f32(a));
        float32x2_t _33_ = vdup_lane_f32(vget_high_f32(a), 1);
        return vcombine_f32(_20_.val[0], _33_);
    }


    ARIBEIRO_INLINE float32x4_t vshuffle_2333(const float32x4_t &a)
    {
        float32x4_t r = vshuffle_3333(a);
        float32x4_t _two = vshuffle_2222(a);
        return vextq_f32(r, _two, 1);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_0031(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2x2_t _31_a = vtrn_f32(vget_high_f32(a), vget_low_f32(a));
        float32x2_t _31_ = vrev64_f32(_31_a.val[1]);
        //float32x2_t _31_ = vdup_lane_f32(vget_low_f32(a), 1);
        float32x2_t _00_ = vdup_lane_f32(vget_low_f32(b), 0);
        return vcombine_f32(_31_, _00_);
    }

    ARIBEIRO_INLINE float32x4_t vshuffle_1022(const float32x4_t &a, const float32x4_t &b)
    {
        float32x2_t _22_ = vdup_lane_f32(vget_high_f32(a), 0);
        float32x2_t _10_ = vget_low_f32(b);
        return vcombine_f32(_22_, _10_);
    }


#endif


    const float _float_bitsign = -.0f; // -0.f = 1 << 31
    const uint32_t _float_bitsign_uint32_t = (*(uint32_t*)(&_float_bitsign));
    const uint32_t _float_bitsign_uint32_t_neg = ~_float_bitsign_uint32_t;
    const float _float_one = 1.0f;
    const uint32_t _float_one_uint32_t = (*(uint32_t*)(&_float_one));

    /// \brief Compute the absolute value of a number
    ///
    /// The implementation uses bitwise operation on CPU, resulting in faster run time.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// // result = 1.0f
    /// float result = absv(-1.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a A vector
    /// \return |a|
    ///
    ARIBEIRO_INLINE float absv(const float &a) {
        uint32_t result = (_float_bitsign_uint32_t_neg & (*(uint32_t*)&a));
        return *((float*)&result);

        //uint32_t result = ((~_float_bitsign_uint32_t) & (*(uint32_t*)&a));
        //return *((float*)&result);

        //return (a < 0) ? (-a) : (a);
    }

    /// \brief Component wise clamp values
    ///
    /// For each component of the vector, evaluate:
    /// ```
    ///     if min < value then return min
    ///     if max > value then return max
    ///     else return value
    /// ```
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// float result;
    /// // result = 5.0f
    /// result = clamp(30.0f, 0.0f, 5.0f);
    /// // result = 0.0f
    /// result = clamp(-100.0f, 0.0f, 5.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param value The value to evaluate
    /// \param min The min threshold
    /// \param max The max threshold
    /// \return The evaluated value
    ///
    ARIBEIRO_INLINE float clamp(const float &value, const float &min, const float &max) {
#if defined(ARIBEIRO_SSE2)
        __m128 maxStep = _mm_max_ss(_mm_set_ss(value), _mm_set_ss(min));
        __m128 minStep = _mm_min_ss(maxStep, _mm_set_ss(max));
        return _mm_f32_(minStep, 0);
#else
        return (value < min) ? min : ((value > max) ? max : value);
#endif
    }

    /// \brief Computes the squared distance between two 1D vectors
    ///
    /// The squared distance is the euclidian distance, without the square root:
    ///
    /// |b-a|^2
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// float result;
    /// // result = 25.0f
    /// result = sqrDistance(25.0f, 20.0f);
    /// // result = 25.0f
    /// result = sqrDistance(20.0f, 25.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a The First vector
    /// \param b The Second vector
    /// \return The squared distance between a and b
    ///
    ARIBEIRO_INLINE float sqrDistance(const float &a, const float &b) {
        float ab = b - a;
        return ab * ab;
    }

    /// \brief Computes the distance between two 1D vectors
    ///
    /// The distance is the euclidian distance from a point a to point b:
    ///
    /// |b-a|
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// float result;
    /// // result = 5.0f
    /// result = distance(25.0f, 20.0f);
    /// // result = 5.0f
    /// result = distance(20.0f, 25.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a The First vector
    /// \param b The Second vector
    /// \return The distance between a and b
    ///
    ARIBEIRO_INLINE float distance(const float &a, const float &b) {
        float ab = b - a;
        //return sqrtf(ab*ab);
        return absv(ab);
    }


    /// \brief Return the maximum between the two parameters
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// float result;
    /// // result = 25.0f
    /// result = maximum(25.0f, 20.0f);
    /// // result = 25.0f
    /// result = maximum(20.0f, 25.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a A value to test
    /// \param b A value to test
    /// \return The maximum value of the parameters
    ///
    ARIBEIRO_INLINE float maximum(const float &a, const float &b) {
#if defined(ARIBEIRO_SSE2)
        return _mm_f32_(_mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)), 0);
#else
        return (a > b) ? a : b;
#endif
    }


    /// \brief Return the minimum between the two parameters
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// float result;
    /// // result = 20.0f
    /// result = minimum(25.0f, 20.0f);
    /// // result = 20.0f
    /// result = minimum(20.0f, 25.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a A value to test
    /// \param b A value to test
    /// \return The minimum value of the parameters
    ///
    ARIBEIRO_INLINE float minimum(const float &a, const float &b) {
#if defined(ARIBEIRO_SSE2)
        return _mm_f32_(_mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)), 0);
#else
        return (a < b) ? a : b;
#endif
    }


    /// \brief Compute the sign of a float
    ///
    /// ```
    /// if a >=0 then sign = 1
    /// else sign = -1
    /// ```
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// float result;
    /// // result = 1.0f
    /// result = sign(25.0f);
    /// // result = -1.0f
    /// result = sign(-25.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a The value to test
    /// \return The sign of a
    ///
    ARIBEIRO_INLINE float sign(const float &a) {
        //uint32_t result = _float_bitsign_uint32_t & (*(uint32_t*)&a);
        //result |= _float_one_uint32_t;
        //return *((float*)&result);
        //return a & _float_bitsign;
        //return (a >= 0) ? 1.0f : -1.0f;

        uint32_t &value_int = *(uint32_t*)(&a);
        uint32_t sign_int = (value_int & _float_bitsign_uint32_t) | _float_one_uint32_t;
        return *(float*)(&sign_int);
    }


    /// \brief Computes the linear interpolation
    ///
    /// When the fator is between 0 and 1 it returns the convex relation (linear interpolation) between a and b.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// float result;
    /// // result = 25.0f
    /// result = lerp(0.0f, 100.0f, 0.25f);
    /// // result = 75.0f
    /// result = lerp(0.0f, 100.0f, 0.75f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a Origin Vector
    /// \param b Target Vector
    /// \param factor The amount (%) to leave the Origin to the Target.
    /// \return The interpolation result
    ///
    ARIBEIRO_INLINE float lerp(const float &a, const  float &b, const float &factor) {
        //  return a+(b-a)*factor;
        return a * (1.0f - factor) + (b*factor);
    }

    /// \brief Move from current to target, considering the max variation
    ///
    /// This function could be used as a constant motion interpolation<br />
    /// between two values considering the delta time and max speed variation.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// PlatformTime timer;
    /// float moveSpeed;
    /// float current;
    /// float target;
    ///
    /// {
    ///     timer.update();
    ///     ...
    ///     // current will be modified to be the target,
    ///     // but the delta time and move speed will make
    ///     // this transition smoother.
    ///     current = move( current, target, time.deltaTime * moveSpeed );
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \sa move(float current, float target, float maxDistanceVariation)
    /// \param current The current state
    /// \param target The target state
    /// \param maxDistanceVariation The max amount the current can be modified to reach target
    /// \return the lerp from current to target according max variation
    ///
    ARIBEIRO_INLINE float move(const float &current, const float &target, const float &maxDistanceVariation) {
        const float EPSILON = 1e-6f;
        float deltaDistance = distance(current, target);
        if (deltaDistance < maxDistanceVariation + EPSILON)
            return target;
        return lerp(current, target, maxDistanceVariation / deltaDistance);
    }


/// \brief Implements the aritmetic operator overload for a type
///
/// You can use it with any type defined that has some operators already implemented.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// class vec2{
/// public:
///     vec2(const float &v) {
///         ...
///     }
///     vec2(const vec2 &v) {
///         ...
///     }
///     vec2& operator+=(const float &v) {
///         ...
///     }
///     vec2& operator+=(const vec2& v) {
///         ...
///     }
///     vec2 operator-()const {
///         ...
///     }
///     vec2& operator-=(const float &v) {
///         ...
///     }
///     vec2& operator-=(const vec2& v) {
///         ...
///     }
///     vec2& operator*=(const float &v) {
///         ...
///     }
///     vec2& operator*=(const vec2& v) {
///         ...
///     }
///     vec2& operator/=(const float &v) {
///         ...
///     }
///     vec2& operator/=(const vec2& v) {
///         ...
///     }
/// };
///
/// //The operator overload definition for float and the type
/// INLINE_OPERATION_IMPLEMENTATION(vec2)
///
/// //After the definition, it is possible to use the aritmetic overload as follow:
/// vec2 a,b,c;
/// a = b - 0.5f;
/// c = ( a + b ) * 0.5f;
/// ...
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define INLINE_OPERATION_IMPLEMENTATION(TTYPE)\
static ARIBEIRO_INLINE TTYPE operator/( const TTYPE& vecA, const TTYPE& vecB ){ return (TTYPE(vecA)/=vecB); } \
static ARIBEIRO_INLINE TTYPE operator/( const TTYPE& vec , const float value ){ return (TTYPE(vec)/=value); } \
static ARIBEIRO_INLINE TTYPE operator/( const float value, const TTYPE& vec  ){ return (TTYPE(value)/=vec); } \
static ARIBEIRO_INLINE TTYPE operator*( const TTYPE& vecA, const TTYPE& vecB ){ return (TTYPE(vecA)*=vecB); } \
static ARIBEIRO_INLINE TTYPE operator*( const TTYPE& vec , const float value ){ return (TTYPE(vec)*=value); } \
static ARIBEIRO_INLINE TTYPE operator*( const float value, const TTYPE& vec  ){ return (TTYPE(value)*=vec); } \
static ARIBEIRO_INLINE TTYPE operator+( const TTYPE& vecA, const TTYPE& vecB ){ return (TTYPE(vecA)+=vecB); } \
static ARIBEIRO_INLINE TTYPE operator+( const TTYPE& vec , const float value ){ return (TTYPE(vec)+=value); } \
static ARIBEIRO_INLINE TTYPE operator+( const float value, const TTYPE& vec  ){ return (TTYPE(value)+=vec); } \
static ARIBEIRO_INLINE TTYPE operator-( const TTYPE& vecA, const TTYPE& vecB ){ return (TTYPE(vecA)-=vecB); } \
static ARIBEIRO_INLINE TTYPE operator-( const TTYPE& vec , const float value ){ return (TTYPE(vec)-=value); } \
static ARIBEIRO_INLINE TTYPE operator-( const float value, const TTYPE& vec  ){ return (TTYPE(value)-=vec); }


}

#endif

