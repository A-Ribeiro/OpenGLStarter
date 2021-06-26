#ifndef vec2_h
#define vec2_h

#include <aribeiro/common.h>
#include <aribeiro/SSE2.h>
#include <aribeiro/floatOPs.h>

namespace aRibeiro{

#if defined(ARIBEIRO_SSE2)
    #pragma pack(push, 16)
#endif

/// \brief Vector 2D (vec2)
///
/// Stores two components(x,y) to represent a bidimensional vector. <br/>
/// It can be used as points or vectors in 2D.
///
/// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
///
/// It is possible to use any arithmetic with vec2 and float combinations.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// vec2 a, b, result;
///
/// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
/// \endcode
///
/// \author Alessandro Ribeiro
///
class _SSE2_ALIGN_PRE vec2{
    public:
    union _SSE2_ALIGN_PRE {
      float _SSE2_ALIGN_PRE array[2] _SSE2_ALIGN_POS;
      struct _SSE2_ALIGN_PRE { float x,y; } _SSE2_ALIGN_POS;
#if defined(ARIBEIRO_SSE2)
      __m128 _SSE2_ALIGN_PRE array_sse _SSE2_ALIGN_POS;
#endif

#if defined(ARIBEIRO_NEON)
        float32x4_t _SSE2_ALIGN_PRE array_neon _SSE2_ALIGN_POS;
#endif

    }_SSE2_ALIGN_POS;

#if defined(ARIBEIRO_SSE2)
    //special SSE2 constructor
    ARIBEIRO_INLINE vec2( const __m128 &v ){
        array_sse = v;
    }
#endif

#if defined(ARIBEIRO_NEON)
    ARIBEIRO_INLINE vec2( const float32x4_t &v ){
        array_neon = v;
    }
#endif

    /// \brief Construct a ZERO vec2 class
    ///
    /// The ZERO vec2 class have the point information in the origin (x=0,y=0)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec = vec2();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    ARIBEIRO_INLINE vec2(){
        x = y = 0.0f;
    }
    /// \brief Constructs a bidimensional Vector
    ///
    /// Initialize the vec2 components with the same float value (by scalar)
    ///
    /// X = v and Y = v
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec = vec2( 0.5f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to initialize the components
    ///
    ARIBEIRO_INLINE vec2( const float &v ){
        x = y = v;
    }
    /// \brief Constructs a bidimensional Vector
    ///
    /// Initialize the vec2 components from the parameters
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec = vec2( 0.1f, 0.2f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param x Value to assign to the X component of the vector
    /// \param y Value to assign to the Y component of the vector
    ///
    ARIBEIRO_INLINE vec2( const float &x, const float &y ){
        this->x = x;
        this->y = y;
    }
    /// \brief Constructs a bidimensional Vector
    ///
    /// Initialize the vec2 components from other vec2 instance by copy
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec_source;
    /// 
    /// vec2 vec = vec2( vec_source );
    ///
    /// vec2 veca = vec_source;
    ///
    /// vec2 vecb;
    /// vecb = vec_source;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to assign to the instance
    ///
    ARIBEIRO_INLINE vec2( const vec2 &v ){
        *this = v;
    }
    /// \brief Constructs a bidimensional Vector from the subtraction b-a
    ///
    /// Initialize the vec2 components from two other vectors using the equation: <br />
    /// this = b - a
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec_a, vec_b;
    ///
    /// vec2 vec_a_to_b = vec2( vec_a, vec_b );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a Orign vector
    /// \param b Destiny vector
    ///
    ARIBEIRO_INLINE vec2( const vec2 &a, const vec2 &b ){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(b.array_sse, a.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(b.array_neon, a.array_neon);
#else
        x = b.x - a.x;
        y = b.y - a.y;
#endif
    }

    /// \brief Compare vectors considering #EPSILON (equal)
    ///
    /// Compare two vectors using #EPSILON to see if they are the same.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec_a, vec_b;
    ///
    /// if ( vec_a == vec_b ){
    ///     //do something
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to compare against
    /// \return true if the values are the same considering #EPSILON
    ///
    ARIBEIRO_INLINE bool operator==(const vec2&v) const {
#if defined(ARIBEIRO_SSE2)
        __m128 diff_abs = _mm_sub_ps(array_sse, v.array_sse);
        //abs
        const __m128 _vec2_sign_mask = _mm_load_(-0.f, -0.f, 0.f, 0.0f);
        diff_abs = _mm_andnot_ps(_vec2_sign_mask, diff_abs);

#if true //defined(_MSC_VER) ||

        //_mm_f32_(diff_abs, 2) = 0.0f;
        //_mm_f32_(diff_abs, 3) = 0.0f;
        const __m128 _vec2_valid_bits = _mm_castsi128_ps(_mm_set_epi32(0, 0, (int)0xffffffff, (int)0xffffffff));

        diff_abs = _mm_and_ps(diff_abs, _vec2_valid_bits);

        diff_abs = _mm_hadd_ps(diff_abs, diff_abs);
        diff_abs = _mm_hadd_ps(diff_abs, diff_abs);

#else

        //swp0 = [1,0,3,2]
        __m128 swp0 = _mm_shuffle_ps(diff_abs, diff_abs, _MM_SHUFFLE(2, 3, 0, 1));
        //add0 = [0+1,1+0,2+3,3+2]
        __m128 add0 = _mm_add_ps(diff_abs, swp0);
        //swp1 = [3+2,2+3,1+0,0+1]
        __m128 swp1 = _mm_shuffle_ps(add0, add0, _MM_SHUFFLE(0, 1, 2, 3));
        //add1 = [0+1+3+2,1+0+2+3,2+3+1+0,3+2+0+1]
        diff_abs = _mm_add_ps(add0, swp1);
#endif

        if (_mm_f32_(diff_abs, 0) > EPSILON2)
            return false;

        //const __m128 epsilon = _mm_set1_ps(1e-4f); // -0.f = 1 << 31
        //_mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));

        /*
        for(int i=0;i<3;i++){
            if (_mm_f32_(diff_abs,i) > EPSILON)
                return false;
        }
        */

        return true;
#elif defined(ARIBEIRO_NEON)

        float32x4_t diff_abs = vsubq_f32(array_neon, v.array_neon);
        //abs
        diff_abs = vabsq_f32(diff_abs);

        diff_abs[2] = 0.0f;
        diff_abs[3] = 0.0f;

        float32x2_t acc_2_elements = vadd_f32(vget_high_f32(diff_abs), vget_low_f32(diff_abs));
        acc_2_elements = vpadd_f32(acc_2_elements, acc_2_elements);

        if (acc_2_elements[0] > EPSILON2)
            return false;

        //const __m128 epsilon = _mm_set1_ps(1e-4f); // -0.f = 1 << 31
        //_mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));

        /*
        for(int i=0;i<3;i++){
            if (diff_abs[i] > EPSILON)
                return false;
        }
        */

        return true;

#else
        float accumulator = 0.0f;
        for (int i = 0; i < 2; i++) {
            accumulator += absv(array[i] - v.array[i]);
        }
        if (accumulator >= EPSILON2)//EPSILON
            return false;

        /*
        for(int i=0;i<2;i++){
            if (absv(array[i]-v.array[i]) > EPSILON)
                return false;
        }
        */
        return true;
        //return memcmp(array, v.array, sizeof(float) * 2) == 0;
#endif
    }

    /// \brief Compare vectors considering #EPSILON (not equal)
    ///
    /// Compare two vectors using #EPSILON to see if they are the same.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec_a, vec_b;
    ///
    /// if ( vec_a != vec_b ){
    ///     //do something
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to compare against
    /// \return true if the values are not the same considering #EPSILON
    ///
    ARIBEIRO_INLINE bool operator!=(const vec2&v) const{
        return !((*this) == v);
        //return memcmp(array, v.array, sizeof(float) * 2) != 0;
    }

    /// \brief Component-wise sum (add) operator overload
    ///
    /// Increment the vector by the components of another vector
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec, vec_b;
    ///
    /// vec += vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to increment the current vector instance
    /// \return A reference to the current instance after the increment
    ///
    ARIBEIRO_INLINE vec2& operator+=(const vec2& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_add_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vaddq_f32(array_neon, v.array_neon);
#else
        x+=v.x;
        y+=v.y;
#endif
        return (*this);
    }

    /// \brief Component-wise subtract operator overload
    ///
    /// Decrement the vector by the components of another vector
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec, vec_b;
    ///
    /// vec -= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to decrement the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec2& operator-=(const vec2& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(array_neon, v.array_neon);
#else
        x-=v.x;
        y-=v.y;
#endif
        return (*this);
    }

    /// \brief Component-wise minus operator overload
    ///
    /// Negates the vector components with the operator minus
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec;
    ///
    /// vec = -vec;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return A copy of the current instance after the negation operation
    ///
    ARIBEIRO_INLINE vec2 operator-()const{
#if defined(ARIBEIRO_SSE2)
        const __m128 _vec2_sign_mask = _mm_setr_ps(-0.f, -0.f, 0.f, 0.0f);
        return _mm_xor_ps(_vec2_sign_mask, array_sse);
#elif defined(ARIBEIRO_NEON)
#if true
        return vnegq_f32(array_neon);
#else
        const float32x4_t minus_one = (float32x4_t) { -1.0f, -1.0f, 0.0f, 0.0f };
        return vmulq_f32(minus_one, array_neon);
#endif
#else
        return vec2(-x,-y);
#endif
    }

    /// \brief Component-wise multiply operator overload
    ///
    /// Multiply the vector by the components of another vector
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec, vec_b;
    ///
    /// vec *= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to multiply the current vector instance
    /// \return A reference to the current instance after the multiplication
    ///
    ARIBEIRO_INLINE vec2& operator*=(const vec2& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_mul_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, v.array_neon);
#else
        x*=v.x;
        y*=v.y;
#endif
        return (*this);
    }

    /// \brief Component-wise division operator overload
    ///
    /// Divides the vector by the components of another vector
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec, vec_b;
    ///
    /// vec /= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to divide the current vector instance
    /// \return A reference to the current instance after the division
    ///
    ARIBEIRO_INLINE vec2& operator/=(const vec2& v){
#if defined(ARIBEIRO_SSE2)
        //__m128 param = _mm_load_(v.x,v.y,v.z,1.0f);

        //const __m128 _vec3_valid_bits = _mm_castsi128_ps(_mm_set_epi32(0, (int)0xffffffff, (int)0xffffffff, (int)0xffffffff));
        //__m128 param = _mm_and_ps(v.array_sse, _vec3_valid_bits);

        __m128 param = v.array_sse;

        //_mm_f32_(param, 2) = 1.0f;
        //_mm_f32_(param, 3) = 1.0f;
        
        const __m128 _one_one = _mm_setr_ps(1.0f, 1.0f, 1.0f, 1.0f);
        param = _mm_blend_ps(param, _one_one, 0xC);

        array_sse = _mm_div_ps(array_sse, param);
#elif defined(ARIBEIRO_NEON)

        float32x4_t param = v.array_neon;
        param[2] = 1.0f;
        param[3] = 1.0f;

        array_neon = vdivq_f32(array_neon, param);
#else
        x/=v.x;
        y/=v.y;
#endif
        return (*this);
    }

    /// \brief Single value increment (add, sum) operator overload
    ///
    /// Increment the vector components by a single value (scalar)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec;
    ///
    /// vec += 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to increment all components of the current vector instance
    /// \return A reference to the current instance after the increment
    ///
    ARIBEIRO_INLINE vec2& operator+=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_add_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vaddq_f32(array_neon, vset1(v));
#else
        x+=v;
        y+=v;
#endif
        return (*this);
    }

    /// \brief Single value decrement (subtract) operator overload
    ///
    /// Decrement the vector components by a single value (scalar)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec;
    ///
    /// vec -= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to decrement all components of the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec2& operator-=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(array_neon, vset1(v));
#else
        x-=v;
        y-=v;
#endif
        return (*this);
    }

    /// \brief Single value multiply operator overload
    ///
    /// Decrement the vector components by a single value (scalar)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec;
    ///
    /// vec *= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to decrement all components of the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec2& operator*=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_mul_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, vset1(v));
#else
        x*=v;
        y*=v;
#endif
        return (*this);
    }

    /// \brief Single value division operator overload
    ///
    /// Divides the vector components by a single value (scalar)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec;
    ///
    /// vec /= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to divide all components of the current vector instance
    /// \return A reference to the current instance after the division
    ///
    ARIBEIRO_INLINE vec2& operator/=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_div_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, vset1(1.0f / v));
#else
        x/=v;
        y/=v;
#endif
        return (*this);
    }

    /// \brief Index the components of the vec2 as a C array
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 vec;
    ///
    /// float x = vec[0];
    ///
    /// vec[1] = 1.0f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v The index of the components starting by 0
    /// \return A reference to the element at the index v
    ///
    ARIBEIRO_INLINE float& operator[](const int v){
        return array[v];
    }

    /// \brief Index the components of the vec2 as a C array
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// void process_vec( const vec2 &vec ) {
    ///     float x = vec[0];
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v The index of the components starting by 0
    /// \return A reference to the element at the index v
    ///
    ARIBEIRO_INLINE const float &operator[](const int v)const{
        return array[v];
    }


    SSE2_CLASS_NEW_OPERATOR

}_SSE2_ALIGN_POS;

INLINE_OPERATION_IMPLEMENTATION(vec2)

#if defined(ARIBEIRO_SSE2)

    const __m128 _vec2_zero_sse = _mm_set1_ps(0.0f);
    const __m128 _vec2_sign_mask_sse = _mm_setr_ps(-0.f, -0.f, 0.f, 0.0f);
    const __m128 _vec2_one_sse = _mm_setr_ps(1.0f, 1.0f, 0.0f, 0.0f);
    const __m128 _vec2_minus_one_sse = _mm_setr_ps(-1.0f, -1.0f, 0.0f, 0.0f);
    const __m128 _vec2_valid_bits_sse = _mm_castsi128_ps(_mm_set_epi32(0, 0, (int)0xffffffff, (int)0xffffffff));

    #pragma pack(pop)
#endif

}

#endif
