#ifndef vec3_h
#define vec3_h

#include <aribeiro/common.h>
#include <aribeiro/vec2.h>
#include <aribeiro/SSE2.h>
#include <aribeiro/floatOPs.h>

namespace aRibeiro{

class vec2;

#if defined(ARIBEIRO_SSE2)
    #pragma pack(push, 16)
#endif

/// \brief Vector 3D (vec3)
///
/// Stores three components(x,y,z) to represent a tridimensional vector. <br/>
/// It can be used as points or vectors in 3D.
/// \warning The class is not designed to represent 2D homogeneous space.
///
/// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
///
/// It is possible to use any arithmetic with vec3 and float combinations.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// vec3 a, b, result;
///
/// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
/// \endcode
///
/// \author Alessandro Ribeiro
///
class _SSE2_ALIGN_PRE vec3{
    public:
    union _SSE2_ALIGN_PRE {
        float _SSE2_ALIGN_PRE array[3] _SSE2_ALIGN_POS;
        struct _SSE2_ALIGN_PRE { float x,y,z; } _SSE2_ALIGN_POS;
        struct _SSE2_ALIGN_PRE { float r,g,b; } _SSE2_ALIGN_POS;
#if defined(ARIBEIRO_SSE2)
        __m128 _SSE2_ALIGN_PRE array_sse _SSE2_ALIGN_POS;
#endif

#if defined(ARIBEIRO_NEON)
        float32x4_t _SSE2_ALIGN_PRE array_neon _SSE2_ALIGN_POS;
#endif

    }_SSE2_ALIGN_POS;


#if defined(ARIBEIRO_SSE2)
    //special SSE2 constructor
    ARIBEIRO_INLINE vec3( const __m128 &v ){
        array_sse = v;
    }
#endif

#if defined(ARIBEIRO_NEON)
    ARIBEIRO_INLINE vec3( const float32x4_t &v ){
        array_neon = v;
    }
#endif


    /// \brief Construct a ZERO vec3 class
    ///
    /// The ZERO vec3 class have the point information in the origin (x=0,y=0,z=0)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 vec = vec3();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    ARIBEIRO_INLINE vec3() {
#if defined(ARIBEIRO_SSE2)
        static const __m128 _vec3_zero_sse = _mm_set1_ps(0.0f);
        array_sse = _vec3_zero_sse;
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){0,0,0,0};
#else
        x = y = z = 0.0f;
#endif
    }
    /// \brief Constructs a tridimensional Vector
    ///
    /// Initialize the vec3 components with the same float value (by scalar)
    ///
    /// X = v, Y = v and Z = v
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 vec = vec3( 0.5f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to initialize the components
    ///
    ARIBEIRO_INLINE vec3( const float &v ){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_load_(v,v,v,0);
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){v,v,v,0};
#else
        x = y = z = v;
#endif
    }
    /// \brief Constructs a tridimensional Vector
    ///
    /// Initialize the vec3 components from the parameters
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 vec = vec3( 0.1f, 0.2f, 0.3f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param x Value to assign to the X component of the vector
    /// \param y Value to assign to the Y component of the vector
    /// \param z Value to assign to the Z component of the vector
    ///
    ARIBEIRO_INLINE vec3( const float &x, const float &y, const float &z ){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_load_(x,y,z,0);
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){x,y,z,0};
#else
        this->x = x;
        this->y = y;
        this->z = z;
#endif
    }
    /// \brief Constructs a tridimensional Vector
    ///
    /// Initialize the vec3 components from a vec2 xy and an isolated z value
    ///
    /// this->xy = xy <br />
    /// this->z = z
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 vec = vec3( vec2( 0.1f, 0.2f ), 0.3f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param xy Vector 2D to assign to the components x and y of the instance respectively
    /// \param z Value to assign to the component z of the instance
    ///
    ARIBEIRO_INLINE vec3( const vec2 &xy , const float &z){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_load_(xy.x,xy.y,z,0);
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){xy.x,xy.y,z,0};
#else
        x = xy.x;
        y = xy.y;
        this->z = z;
#endif
    }
    /// \brief Constructs a tridimensional Vector
    ///
    /// Initialize the vec3 components from an isolated x value and a vec2 yz
    ///
    /// this->x = x <br />
    /// this->yz = yz
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 vec = vec3( 0.1f, vec2( 0.2f, 0.3f ) );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param x Value to assign to the component x of the instance
    /// \param yz Vector 2D to assign to the components y and z of the instance respectively
    ///
    ARIBEIRO_INLINE vec3( const float &x, const vec2 &yz){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_load_(x,yz.x,yz.y,0);
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){x,yz.x,yz.y,0};
#else
        this->x = x;
        y = yz.x;
        z = yz.y;
#endif
    }
    /// \brief Constructs a tridimensional Vector
    ///
    /// Initialize the vec3 components from other vec3 instance by copy
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 vec_source;
    /// 
    /// vec3 vec = vec3( vec_source );
    ///
    /// vec3 veca = vec_source;
    ///
    /// vec3 vecb;
    /// vecb = vec_source;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to assign to the instance
    ///
    ARIBEIRO_INLINE vec3( const vec3 &v ){
#if defined(ARIBEIRO_SSE2)
        array_sse = v.array_sse;
#elif defined(ARIBEIRO_NEON)
        array_neon = v.array_neon;
#else
        *this = v;
#endif
    }
    /// \brief Constructs a tridimensional Vector from the subtraction b-a
    ///
    /// Initialize the vec3 components from two other vectors using the equation: <br />
    /// this = b - a
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 vec_a, vec_b;
    ///
    /// vec3 vec_a_to_b = vec3( vec_a, vec_b );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a Orign vector
    /// \param b Destiny vector
    ///
    ARIBEIRO_INLINE vec3( const vec3 &a, const vec3 &b ){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(b.array_sse, a.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(b.array_neon, a.array_neon);
#else
        x = b.x - a.x;
        y = b.y - a.y;
        z = b.z - a.z;
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
    /// vec3 vec_a, vec_b;
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
    ARIBEIRO_INLINE bool operator==(const vec3&v) const {
#if defined(ARIBEIRO_SSE2)
        __m128 diff_abs = _mm_sub_ps(array_sse, v.array_sse);
        //abs
        static const __m128 _vec3_sign_mask = _mm_load_(-0.f,-0.f,-0.f,0.0f);
        diff_abs = _mm_andnot_ps(_vec3_sign_mask, diff_abs);

        _mm_f32_(diff_abs, 3) = 0.0f;

#if defined(_MSC_VER) || true

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

        diff_abs[3] = 0.0f;

        float32x2_t acc_2_elements = vadd_f32(vget_high_f32(diff_abs), vget_low_f32(diff_abs));
        acc_2_elements = vpadd_f32(acc_2_elements, acc_2_elements);

        if (acc_2_elements[0] > EPSILON2)
            return false;

        //static const __m128 epsilon = _mm_set1_ps(1e-4f); // -0.f = 1 << 31
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
        for (int i = 0; i < 3; i++) {
            accumulator += absv(array[i] - v.array[i]);
        }
        if (accumulator >= EPSILON2)//EPSILON
            return false;

        /*
        for(int i=0;i<3;i++){
            if (absv(array[i]-v.array[i]) > EPSILON)
                return false;
        }
        */

        return true;
        //return memcmp(array, v.array, sizeof(float) * 3) == 0;
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
    /// vec3 vec_a, vec_b;
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
    ARIBEIRO_INLINE bool operator!=(const vec3&v) const{
        return !((*this) == v);
        //return memcmp(array, v.array, sizeof(float) * 3) != 0;
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
    /// vec3 vec, vec_b;
    ///
    /// vec += vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to increment the current vector instance
    /// \return A reference to the current instance after the increment
    ///
    ARIBEIRO_INLINE vec3& operator+=(const vec3& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_add_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vaddq_f32(array_neon, v.array_neon);
#else
        x+=v.x;
        y+=v.y;
        z+=v.z;
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
    /// vec3 vec, vec_b;
    ///
    /// vec -= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to decrement the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec3& operator-=(const vec3& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(array_neon, v.array_neon);
#else
        x-=v.x;
        y-=v.y;
        z-=v.z;
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
    /// vec3 vec;
    ///
    /// vec = -vec;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return A copy of the current instance after the negation operation
    ///
    ARIBEIRO_INLINE vec3 operator-() const{
#if defined(ARIBEIRO_SSE2)
        static const __m128 _vec3_sign_mask = _mm_load_(-0.f,-0.f,-0.f,0.0f);
        return _mm_xor_ps(_vec3_sign_mask, array_sse);
#elif defined(ARIBEIRO_NEON)
        static const float32x4_t minus_one = (float32x4_t){-1.0f,-1.0f,-1.0f,0.0f};
        return vmulq_f32(minus_one, array_neon);
#else
        return vec3(-x,-y,-z);
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
    /// vec3 vec, vec_b;
    ///
    /// vec *= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to multiply the current vector instance
    /// \return A reference to the current instance after the multiplication
    ///
    ARIBEIRO_INLINE vec3& operator*=(const vec3& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_mul_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, v.array_neon);
#else
        x*=v.x;
        y*=v.y;
        z*=v.z;
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
    /// vec3 vec, vec_b;
    ///
    /// vec /= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to divide the current vector instance
    /// \return A reference to the current instance after the division
    ///
    ARIBEIRO_INLINE vec3& operator/=(const vec3& v){
#if defined(ARIBEIRO_SSE2)
        __m128 param = _mm_load_(v.x,v.y,v.z,1.0f);
        array_sse = _mm_div_ps(array_sse, param);
#elif defined(ARIBEIRO_NEON)
        array_neon = vdivq_f32(array_neon, v.array_neon);
#else
        x/=v.x;
        y/=v.y;
        z/=v.z;
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
    /// vec3 vec;
    ///
    /// vec += 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to increment all components of the current vector instance
    /// \return A reference to the current instance after the increment
    ///
    ARIBEIRO_INLINE vec3& operator+=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_add_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vaddq_f32(array_neon, vset1(v));
#else
        x+=v;
        y+=v;
        z+=v;
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
    /// vec3 vec;
    ///
    /// vec -= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to decrement all components of the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec3& operator-=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(array_neon, vset1(v));
#else
        x-=v;
        y-=v;
        z-=v;
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
    /// vec3 vec;
    ///
    /// vec *= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to decrement all components of the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec3& operator*=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_mul_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, vset1(v));
#else
        x*=v;
        y*=v;
        z*=v;
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
    /// vec3 vec;
    ///
    /// vec /= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to divide all components of the current vector instance
    /// \return A reference to the current instance after the division
    ///
    ARIBEIRO_INLINE vec3& operator/=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_div_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, vset1( 1.0f / v ));
#else
        x/=v;
        y/=v;
        z/=v;
#endif
        return (*this);
    }
    /// \brief Index the components of the vec3 as a C array
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 vec;
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

    /// \brief Index the components of the vec3 as a C array
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// void process_vec( const vec3 &vec ) {
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

INLINE_OPERATION_IMPLEMENTATION(vec3)

#if defined(ARIBEIRO_SSE2)
    #pragma pack(pop)
#endif

}

#endif
