#ifndef vec4_h
#define vec4_h

#include <aribeiro/common.h>
#include <aribeiro/vec3.h>
#include <aribeiro/SSE2.h>
#include <aribeiro/floatOPs.h>


namespace aRibeiro{

class vec3;

#if defined(ARIBEIRO_SSE2)
    #pragma pack(push, 16)
#endif


/// \brief Homogeneous 3D (vec4)
///
/// Stores three components(x,y,z,w) to represent a tridimensional vector with the homogeneous component w. <br/>
/// It can be used to represent points or vectors in 3D.
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
class _SSE2_ALIGN_PRE vec4{
    public:
    union _SSE2_ALIGN_PRE {
        float _SSE2_ALIGN_PRE array[4] _SSE2_ALIGN_POS;
        struct _SSE2_ALIGN_PRE { float x,y,z,w; } _SSE2_ALIGN_POS;
        struct _SSE2_ALIGN_PRE { float r,g,b,a; } _SSE2_ALIGN_POS;
#if defined(ARIBEIRO_SSE2)
        __m128 _SSE2_ALIGN_PRE array_sse _SSE2_ALIGN_POS;
#endif

#if defined(ARIBEIRO_NEON)
        float32x4_t _SSE2_ALIGN_PRE array_neon _SSE2_ALIGN_POS;
#endif

    }_SSE2_ALIGN_POS;

#if defined(ARIBEIRO_SSE2)
    //special SSE2 constructor
    ARIBEIRO_INLINE vec4( const __m128 &v ){
        array_sse = v;
    }
#endif

#if defined(ARIBEIRO_NEON)
    ARIBEIRO_INLINE vec4( const float32x4_t &v ){
        array_neon = v;
    }
#endif

    /// \brief Construct a ZERO vec4 class
    ///
    /// The ZERO vec4 class have the point information in the origin as a vector (x=0,y=0,z=0,w=0)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec4 vec = vec4();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    ARIBEIRO_INLINE vec4(){
#if defined(ARIBEIRO_SSE2)
        static const __m128 _vec4_zero_sse = _mm_set1_ps(0.0f);
        array_sse = _vec4_zero_sse;
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){0,0,0,0};
#else
        x = y = z = w = 0.0f;
#endif
    }
    /// \brief Constructs a tridimensional Vector with homogeneous component
    ///
    /// Initialize the vec3 components with the same float value (by scalar)
    ///
    /// X = v, Y = v, Z = v and W = v
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec4 vec = vec4( 0.5f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to initialize the components
    ///
    ARIBEIRO_INLINE vec4( const float &v ){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_set1_ps(v);
#elif defined(ARIBEIRO_NEON)
        array_neon = vset1(v);
#else
        x = y = z = w = v;
#endif
    }
    /// \brief Constructs a tridimensional Vector with homogeneous component
    ///
    /// Initialize the vec4 components from the parameters
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec4 vec = vec4( 0.1f, 0.2f, 0.3f, 1.0f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param x Value to assign to the X component of the vector
    /// \param y Value to assign to the Y component of the vector
    /// \param z Value to assign to the Z component of the vector
    /// \param w Value to assign to the W component of the vector
    ///
    ARIBEIRO_INLINE vec4( const float &x, const float &y, const float &z, const float &w ){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_load_( x, y, z, w );
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){x,y,z,w};
#else
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
#endif
    }

    /// \brief Constructs a tridimensional Vector with homogeneous component
    ///
    /// Initialize the vec4 components from a vec3 xyz and an isolated w value
    ///
    /// this->xyz = xyz <br />
    /// this->w = w
    ///
    /// If the w is 0 the class represent a vector. <br />
    /// If the w is 1 the class represent a point. <br />
    /// Otherwise it might have a result of a projection
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec4 vec = vec4( vec3( 0.1f, 0.2f, 0.3f ), 1.0f );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param xyz Vector 3D to assign to the components x, y and Z of the instance respectively
    /// \param w Value to assign to the component w of the instance
    ///
    ARIBEIRO_INLINE vec4( const vec3 &xyz , const float &w){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_load_( xyz.x, xyz.y, xyz.z, w );
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){xyz.x, xyz.y, xyz.z, w};
#else
        x = xyz.x;
        y = xyz.y;
        z = xyz.z;
        this->w = w;
#endif
    }
    /// \brief Constructs a tridimensional Vector with homogeneous component
    ///
    /// Initialize the vec4 components from an isolated x value and a vec3 yzw
    ///
    /// this->x = x <br />
    /// this->yzw = yzw
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec4 vec = vec4( 0.1f, vec3( 0.2f, 0.3f, 1.0f ) );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param x Value to assign to the component x of the instance
    /// \param yzw Vector 3D to assign to the components y, z and w of the instance respectively
    ///
    ARIBEIRO_INLINE vec4( const float &x, const vec3 &yzw){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_load_(x, yzw.x, yzw.y, yzw.z );
#elif defined(ARIBEIRO_NEON)
        array_neon = (float32x4_t){x, yzw.x, yzw.y, yzw.z};
#else
        this->x = x;
        y = yzw.x;
        z = yzw.y;
        w = yzw.z;
#endif
    }
    /// \brief Constructs a tridimensional Vector with homogeneous component
    ///
    /// Initialize the vec4 components from other vec4 instance by copy
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec4 vec_source;
    /// 
    /// vec4 vec = vec4( vec_source );
    ///
    /// vec4 veca = vec_source;
    ///
    /// vec4 vecb;
    /// vecb = vec_source;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to assign to the instance
    ///
    ARIBEIRO_INLINE vec4( const vec4 &v ){
#if defined(ARIBEIRO_SSE2)
        array_sse = v.array_sse;
#elif defined(ARIBEIRO_NEON)
        array_neon = v.array_neon;
#else
        *this = v;
#endif
    }
    /// \brief Constructs a tridimensional Vector with homogeneous component from the subtraction b-a
    ///
    /// Initialize the vec4 components from two other vectors using the equation: <br />
    /// this = b - a
    ///
    /// If a and b were points then the result will be a vector pointing from a to b with the w component equals to zero.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec4 vec_a, vec_b;
    ///
    /// vec4 vec_a_to_b = vec4( vec_a, vec_b );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param a Orign point
    /// \param b Destiny point
    ///
    ARIBEIRO_INLINE vec4( const vec4 &a, const vec4 &b ){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(b.array_sse, a.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(b.array_neon, a.array_neon);
#else
        x = b.x - a.x;
        y = b.y - a.y;
        z = b.z - a.z;
        w = b.w - a.w;
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
    /// vec4 vec_a, vec_b;
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
    ARIBEIRO_INLINE bool operator==(const vec4&v) const {
#if defined(ARIBEIRO_SSE2)

        __m128 diff_abs = _mm_sub_ps(array_sse, v.array_sse);
        //abs
        static const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
        diff_abs = _mm_andnot_ps(_vec4_sign_mask, diff_abs);

#if true //defined(_MSC_VER) ||

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
            if ( _mm_f32_(diff_abs,i) > 1e-4f)
                return false;
        }*/

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
            if ( diff_abs[i] > 1e-4f)
                return false;
        }
        */

        return true;

#else

        float accumulator = 0.0f;
        for (int i = 0; i < 4; i++) {
            accumulator += absv(array[i] - v.array[i]);
        }
        if (accumulator >= EPSILON2)//EPSILON
            return false;

        return true;
        //return memcmp(array, v.array, sizeof(float) * 4) == 0;
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
    /// vec4 vec_a, vec_b;
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
    ARIBEIRO_INLINE bool operator!=(const vec4&v) const{
        return !((*this) == v);
        //return memcmp(array, v.array, sizeof(float) * 4) != 0;
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
    /// vec4 vec, vec_b;
    ///
    /// vec += vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to increment the current vector instance
    /// \return A reference to the current instance after the increment
    ///
    ARIBEIRO_INLINE vec4& operator+=(const vec4& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_add_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vaddq_f32(array_neon, v.array_neon);
#else
        x+=v.x;
        y+=v.y;
        z+=v.z;
        w+=v.w;
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
    /// vec4 vec, vec_b;
    ///
    /// vec -= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to decrement the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec4& operator-=(const vec4& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(array_neon, v.array_neon);
#else
        x-=v.x;
        y-=v.y;
        z-=v.z;
        w-=v.w;
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
    /// vec4 vec;
    ///
    /// vec = -vec;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return A copy of the current instance after the negation operation
    ///
    ARIBEIRO_INLINE vec4 operator-() const{
#if defined(ARIBEIRO_SSE2)
        static const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
        return _mm_xor_ps(_vec4_sign_mask, array_sse);
#elif defined(ARIBEIRO_NEON)
        static const float32x4_t minus_one = (float32x4_t){-1.0f,-1.0f,-1.0f,-1.0f};
        return vmulq_f32(minus_one, array_neon);
#else
        return vec4(-x,-y,-z,-w);
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
    /// vec4 vec, vec_b;
    ///
    /// vec *= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to multiply the current vector instance
    /// \return A reference to the current instance after the multiplication
    ///
    ARIBEIRO_INLINE vec4& operator*=(const vec4& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_mul_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, v.array_neon);
#else
        x*=v.x;
        y*=v.y;
        z*=v.z;
        w*=v.w;
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
    /// vec4 vec, vec_b;
    ///
    /// vec /= vec_b;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Vector to divide the current vector instance
    /// \return A reference to the current instance after the division
    ///
    ARIBEIRO_INLINE vec4& operator/=(const vec4& v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_div_ps(array_sse, v.array_sse);
#elif defined(ARIBEIRO_NEON)
        array_neon = vdivq_f32(array_neon, v.array_neon);
#else
        x/=v.x;
        y/=v.y;
        z/=v.z;
        w/=v.w;
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
    /// vec4 vec;
    ///
    /// vec += 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to increment all components of the current vector instance
    /// \return A reference to the current instance after the increment
    ///
    ARIBEIRO_INLINE vec4& operator+=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_add_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vaddq_f32(array_neon, vset1(v));
#else
        x+=v;
        y+=v;
        z+=v;
        w+=v;
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
    /// vec4 vec;
    ///
    /// vec -= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to decrement all components of the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec4& operator-=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_sub_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vsubq_f32(array_neon, vset1(v));
#else
        x-=v;
        y-=v;
        z-=v;
        w-=v;
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
    /// vec4 vec;
    ///
    /// vec *= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to decrement all components of the current vector instance
    /// \return A reference to the current instance after the decrement
    ///
    ARIBEIRO_INLINE vec4& operator*=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_mul_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, vset1(v));
#else
        x*=v;
        y*=v;
        z*=v;
        w*=v;
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
    /// vec4 vec;
    ///
    /// vec /= 0.5f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v Value to divide all components of the current vector instance
    /// \return A reference to the current instance after the division
    ///
    ARIBEIRO_INLINE vec4& operator/=(const float &v){
#if defined(ARIBEIRO_SSE2)
        array_sse = _mm_div_ps(array_sse, _mm_set1_ps(v));
#elif defined(ARIBEIRO_NEON)
        array_neon = vmulq_f32(array_neon, vset1( 1.0f / v ));
#else
        x/=v;
        y/=v;
        z/=v;
        w/=v;
#endif
        return (*this);
    }
    /// \brief Index the components of the vec4 as a C array
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec4 vec;
    ///
    /// float x = vec[0];
    ///
    /// vec[3] = 1.0f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v The index of the components starting by 0
    /// \return A reference to the element at the index v
    ///
    ARIBEIRO_INLINE float& operator[](const int v){
        return array[v];
    }

    /// \brief Index the components of the vec4 as a C array
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// void process_vec( const vec4 &vec ) {
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

INLINE_OPERATION_IMPLEMENTATION(vec4)

#if defined(ARIBEIRO_SSE2)
    #pragma pack(pop)
#endif

}

#endif
