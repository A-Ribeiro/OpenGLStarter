/// \file
#ifndef constants_h
#define constants_h

#include <aribeiro/common.h>

//#include "math.h"
#include <limits.h>
#include <float.h>
#include <cmath> //para sqrtf

#ifdef __cplusplus
namespace aRibeiro {
#endif

const float PI = 3.1415926535897932384626433832795f;///< Number PI
const float _PI_180 = 0.0174532925199432957692222222222222f;///< PI/180. Used to convert degrees to radians
const float _180_PI = 57.2957795130823208767981548141052f;///< 180/PI. Used to convert radians to degrees

/// \brief Converts radians to degrees
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// // 90.0f
/// float degrees = RAD2DEG(PI/2.0f);
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define RAD2DEG(rad) ((rad)*(aRibeiro::_180_PI))

/// \brief Converts degrees to radians
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// // PI/2.0f
/// float radians = DEG2RAD(90.0f);
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define DEG2RAD(deg) ((deg)*(aRibeiro::_PI_180))

/// \brief Union to access binary representation of a float value
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// const FloatFromBinaryContent SPD_FLOAT_INFINITY_UNION = {0x7f800000};
/// const float SPD_FLT_INF = SPD_FLOAT_INFINITY_UNION.value;
/// \endcode
///
/// \author Alessandro Ribeiro
///
union FloatFromBinaryContent{
  unsigned int binaryContent;
  float value;
};
const FloatFromBinaryContent SPD_FLOAT_INFINITY_UNION = {0x7f800000};
const float SPD_FLT_INF = SPD_FLOAT_INFINITY_UNION.value;


#define _sqrt sqrtf
#define _rsqrt(x) (1.0f/sqrtf(x))

#ifndef sqrtf
#define sqrtf(V) (float)(sqrt(V))
#endif

#ifndef cosf
#define cosf(V) (float)(cos(V))
#endif

#ifndef sinf
#define sinf(V) (float)(sin(V))
#endif

#ifndef tanf
#define tanf(V) (float)(tan(V))
#endif

#undef Myabs
#define Myabs(V) ((V<0)?-V:V)


/// \brief Low precision float delta: 0.0001f
///
/// Lower precision related to #EPSILON2.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// float a,b;
///
/// if ( absv(a-b) < EPSILON ) {
///     // four decimal places precision near zero comparison OK
///     ....
/// }
/// \endcode
///
/// \author Alessandro Ribeiro
///
const float EPSILON = 1e-4f;

/// \brief High precision float delta: 0.000001f
///
/// HIgher precision related to #EPSILON.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// float a,b;
///
/// if ( absv(a-b) < EPSILON2 ) {
///     // six decimal places precision near zero comparison OK
///     ....
/// }
/// \endcode
///
/// \author Alessandro Ribeiro
///
const float EPSILON2 = 1e-6f;

#ifdef __cplusplus
}
#endif

//#include "CossineSineArray.h"

#endif

