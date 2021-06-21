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
        x = b.x - a.x;
        y = b.y - a.y;
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
        x+=v.x;
        y+=v.y;
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
        x-=v.x;
        y-=v.y;
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
        return vec2(-x,-y);
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
        x*=v.x;
        y*=v.y;
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
        x/=v.x;
        y/=v.y;
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
        x+=v;
        y+=v;
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
        x-=v;
        y-=v;
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
        x*=v;
        y*=v;
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
        x/=v;
        y/=v;
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
    #pragma pack(pop)
#endif

}

#endif
