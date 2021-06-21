#ifndef quat4_h
#define quat4_h

#include <aribeiro/common.h>
#include <aribeiro/SSE2.h>
#include <aribeiro/floatOPs.h>
#include <aribeiro/vec4.h>

namespace aRibeiro {

#if defined(ARIBEIRO_SSE2)
    #pragma pack(push, 16)
#endif

/// \brief Quaternion (quat)
///
/// Stores four components(x,y,z,w) to represent a quaternion. <br/>
/// The quaternion can be seen as a unit axis with an angle in radians in the imaginary space.
///
/// \author Alessandro Ribeiro
///
class _SSE2_ALIGN_PRE quat{
    public:
    union _SSE2_ALIGN_PRE {
        float _SSE2_ALIGN_PRE array[4]_SSE2_ALIGN_POS;
        struct _SSE2_ALIGN_PRE { float x,y,z,w; }_SSE2_ALIGN_POS;
#if defined(ARIBEIRO_SSE2)
        __m128 _SSE2_ALIGN_PRE array_sse _SSE2_ALIGN_POS;
#endif

#if defined(ARIBEIRO_NEON)
        float32x4_t _SSE2_ALIGN_PRE array_neon _SSE2_ALIGN_POS;
#endif

    }_SSE2_ALIGN_POS;

#if defined(ARIBEIRO_SSE2)
    //special SSE2 constructor
    ARIBEIRO_INLINE quat( const __m128 &v ){
        array_sse = v;
    }
#endif

#if defined(ARIBEIRO_NEON)
    ARIBEIRO_INLINE quat( const float32x4_t &v ){
        array_neon = v;
    }
#endif

    /// \brief Construct an identity quaternion quat class
    ///
    /// The identity quat class has the folow configuration (x=0,y=0,z=0,w=1)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// quat rotation = quat();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    ARIBEIRO_INLINE quat(){
#if defined(ARIBEIRO_SSE2)
        static const __m128 _load_0001_ = _mm_load_(0.0f,0.0f,0.0f,1.0f);
        array_sse = _load_0001_;
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){0.0f,0.0f,0.0f,1.0f};
#else
        x = y = z = 0.0f;
        w = 1;
#endif
    }
    /// \brief Constructs a quaterion
    ///
    /// Initialize the quat components from the parameters
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// quat rotation = quat( 0.0f, 0.0f, 0.0f, 1.0f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param x Value to assign to the X component
    /// \param y Value to assign to the Y component
    /// \param z Value to assign to the Z component
    /// \param w Value to assign to the W component
    ///
    ARIBEIRO_INLINE quat( const float x, const float y, const float z, const float w ){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_load_(x, y, z, w);
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){x, y, z, w};
#else
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
#endif
    }
    /// \brief Constructs a quaternion
    ///
    /// Initialize the quat components from other quat instance by copying the content of the other quat.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// quat rotation_source;
    ///
    /// quat rotation = quat( rotation_source );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to assign to the instance
    ///
    ARIBEIRO_INLINE quat( const quat &v ){
#if defined(ARIBEIRO_SSE2)
        array_sse = v.array_sse;
#elif defined(ARIBEIRO_NEON)
        array_neon = v.array_neon;
#else
        *this = v;
#endif
    }
    /// \brief Comparison of quaternions (equal)
    ///
    /// Compare two quaternions considering #EPSILON.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// quat rotation_a, rotation_b;
    ///
    /// if ( rotation_a == rotation_b ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Quaternion to compare against
    /// \return true if the quaternions are equal considering #EPSILON
    ///
    ARIBEIRO_INLINE bool operator==(const quat&v) const {

#if defined(ARIBEIRO_SSE2)

        __m128 diff_abs = _mm_sub_ps(array_sse, v.array_sse);
        //abs
        static const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
        diff_abs = _mm_andnot_ps(_vec4_sign_mask, diff_abs);

#if true

        diff_abs = _mm_dp_ps( diff_abs, diff_abs, 0xff );

#elif defined(_MSC_VER) || true

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

        //static const __m128 epsilon = _mm_set1_ps(1e-4f); // -0.f = 1 << 31
        //_mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));

        /*
        for(int i=0;i<4;i++){
            if (_mm_f32_(diff_abs,i) > EPSILON)
                return false;
        }
        */

        return true;
#elif defined(ARIBEIRO_NEON)

        float32x4_t diff_abs = vsubq_f32(array_neon, v.array_neon);
        //abs
        diff_abs = vabsq_f32(diff_abs);

        float32x2_t acc_2_elements = vadd_f32(vget_high_f32(diff_abs), vget_low_f32(diff_abs));
        acc_2_elements = vpadd_f32(acc_2_elements, acc_2_elements);

        if (acc_2_elements[0] > EPSILON2)
            return false;

        //static const __m128 epsilon = _mm_set1_ps(1e-4f); // -0.f = 1 << 31
        //_mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));

        /*
        for(int i=0;i<4;i++){
            if (diff_abs[i] > EPSILON)
                return false;
        }
        */

        return true;

#else

        float accumulator = 0.0f;
        for(int i=0;i<4;i++){
            accumulator += absv(array[i] - v.array[i]);
        }
        if (accumulator >= EPSILON2)//EPSILON
            return false;
        return true;
        //return memcmp(array, v.array, sizeof(float) * 4) == 0;
#endif
    }

    /// \brief Invert all signs of the quaternion
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// quat rotation;
    ///
    /// rotation = -rotation;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return the quaternion with sign of the elements inverted
    ///
    ARIBEIRO_INLINE quat operator-() const{
#if defined(ARIBEIRO_SSE2)
        static const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
        return _mm_xor_ps(_vec4_sign_mask, array_sse);
#elif defined(ARIBEIRO_NEON)
        static const float32x4_t minus_one = (float32x4_t){-1.0f,-1.0f,-1.0f,-1.0f};
        return vmulq_f32(minus_one, array_neon);
#else
        return quat(-x,-y,-z,-w);
#endif
    }

    /// \brief Comparison of quaternions (not equal)
    ///
    /// Compare two quaternions considering #EPSILON.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// quat rotation_a, rotation_b;
    ///
    /// if ( rotation_a != rotation_b ) {
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Quaternion to compare against
    /// \return true if the quaternions are not equal considering #EPSILON
    ///
    ARIBEIRO_INLINE bool operator!=(const quat&v) const{
        return !((*this) == v);
    }

    /// \brief Index the components of the quat as a C array
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// quat rotation;
    ///
    /// float x = rotation[0];
    ///
    /// rotation[3] = 1.0f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v The index of the components starting by 0
    /// \return A reference to the element at the index v
    ///
    ARIBEIRO_INLINE float& operator[](const int v){
        return array[v];
    }

    /// \brief Index the components of the quat as a C array
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// void processQuaternion ( const quat &rotation ) {
    ///     float x = rotation[0];
    ///     ...
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v The index of the components starting by 0
    /// \return A reference to the element at the index v
    ///
    ARIBEIRO_INLINE const float& operator[](const int v)const {
        return array[v];
    }

    SSE2_CLASS_NEW_OPERATOR

} _SSE2_ALIGN_POS;

#if defined(ARIBEIRO_SSE2)
    #pragma pack(pop)
#endif

}

#endif
