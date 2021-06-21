/// \file

#ifndef EasingEquations__h
#define EasingEquations__h
/*
General information:

I've implemented the Robert Penner's equations based on the 
Tweener implementation (MIT license).

The original code was based on time based interpolation. 
I've changed it to be based on raw 0 to 1 interpolation value.

The value returned was fixed to Number (equivalent to float in C). 
I've changed it to be used from templates.

Any kind of object that implements the aritmetic operators can be used in the interpolation.

I've kept the original disclaimer from Robert Penner's, 
although I haven't use directly his equations/reference.

There is only an equation that I've come up with:

*The Elastic equations.

The code in Tweener uses the period, duration and time directly in the equations. 
To make it usable in the lerp (a,b,factor) fashion, I've 
written a new function that generates similar values.


*/
/*
Disclaimer for Robert Penner's Easing Equations license:

TERMS OF USE - EASING EQUATIONS

Open source under the BSD License.

Copyright � 2001 Robert Penner
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, 
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of contributors may be used to 
      endorse or promote products derived from this software without specific 
      prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/
#include <aribeiro/common.h>
namespace aRibeiro {
    

        /// \brief Compose two easing equations in sequence
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 interpolationFunctionA(const vec3& a, const vec3& b, float lerp){
        ///     ...
        /// }
        ///
        /// vec3 interpolationFunctionB(const vec3& a, const vec3& b, float lerp){
        ///     ...
        /// }
        ///
        /// vec3 result = easeCompose<vec3>( vec3(0.0), vec3(1.0), 0.5f, &interpolationFunctionA, &interpolationFunctionB );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @param interplFuncIn First easing function pointer
        /// @param interplFuncOut Second easing function pointer
        /// @return        The value.
        ///
        template <class T>
        inline T easeCompose (const T& _startValue, const T& _endValue, float lerp, 
            T (*interplFuncIn) (const T& _startValue, const T& _endValue, float lerp),
            T (*interplFuncOut) (const T& _startValue, const T& _endValue, float lerp) ) {
            T mid = (_endValue + _startValue)*0.5f;
            lerp *= 2.0f;
            if (lerp <= 1.0f)
                return interplFuncIn(_startValue,mid,lerp);
            else
                return interplFuncOut(mid,_endValue,lerp-1.0f);
        }

/// \brief Declare a composed ease template function inline.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// template <class T>
/// inline T easeInQuad(const T& _startValue, const T& _endValue, float lerp) {
///     ...
/// }
/// 
/// template <class T>
/// inline T easeOutQuad(const T& _startValue, const T& _endValue, float lerp) {
///     ...
/// }
///
/// DECLARE_COMPOSED_EASE_FUNCTION(easeInOutQuad,easeInQuad,easeOutQuad)
/// \endcode
///
/// \author Alessandro Ribeiro
/// @param name    The function template name.
/// @param fa   First function (from 0 to 0.5).
/// @param fb   Second function (from 0.5 to 1.0).
/// @return        The value.
///
#define DECLARE_COMPOSED_EASE_FUNCTION(__name__,__fa__,__fb__) \
template <class T> \
inline T __name__ (const T& _startValue, const T& _endValue, float lerp ) { \
    T mid = (_endValue + _startValue)*0.5f; \
    lerp *= 2.0f; \
    if (lerp <= 1.0f) \
        return __fa__<T>(_startValue,mid,lerp); \
    else \
        return __fb__<T>(mid,_endValue,lerp-1.0f); \
}

        /// \brief Easing equation function for a linear (t) easing.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeNone<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeNone (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return (delta)*(lerp) + _startValue;
        }
    
        /// \brief Easing equation function for a quadratic (t^2) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInQuad<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInQuad (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return (delta)*(lerp*lerp) + _startValue;
        }
    
        /// \brief Easing equation function for a quadratic (t^2) easing out: decelerating to zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutQuad<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutQuad (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return (delta)*(lerp*(2.0f-lerp)) + _startValue;
        }
        //public static function easeOutQuad (t:Number, b:Number, c:Number, d:Number, p_params:Object = null):Number {
        //    return -c *(t/=d)*(t-2) + b;
        //}
    
        /// \brief Easing equation function for a quadratic (t^2) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutQuad<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutQuad,easeInQuad,easeOutQuad)

        /// \brief Easing equation function for a quadratic (t^2) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInQuad<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInQuad,easeOutQuad,easeInQuad)
    
        /// \brief Easing equation function for a cubic (t^3) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInCubic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInCubic (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return (delta)*(lerp*lerp*lerp) + _startValue;
        }
    
        /// \brief Easing equation function for a cubic (t^3) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutCubic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutCubic (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            lerp -= 1.0f;
            return (delta)*(lerp*lerp*lerp+1.0f) + _startValue;
        }
    
        /// \brief Easing equation function for a cubic (t^3) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutCubic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutCubic,easeInCubic,easeOutCubic)

        /// \brief Easing equation function for a cubic (t^3) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInCubic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInCubic,easeOutCubic,easeInCubic)
    
        /// \brief Easing equation function for a quartic (t^4) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInQuart<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInQuart (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return (delta)*(lerp*lerp*lerp*lerp) + _startValue;
        }
    
        /// \brief Easing equation function for a quartic (t^4) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutQuart<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutQuart (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            lerp -= 1.0f;
            return (delta)*(1.0f-lerp*lerp*lerp*lerp) + _startValue;
        }
    
        /// \brief Easing equation function for a quartic (t^4) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutQuart<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutQuart,easeInQuart,easeOutQuart)
    
        /// \brief Easing equation function for a quartic (t^4) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInQuart<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInQuart,easeOutQuart,easeInQuart)
    
        /// \brief Easing equation function for a quintic (t^5) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInQuint<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInQuint (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return (delta)*(lerp*lerp*lerp*lerp*lerp) + _startValue;
        }
    
        /// \brief Easing equation function for a quintic (t^5) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutQuint<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutQuint (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            lerp -= 1.0f;
            return (delta)*(1.0f+lerp*lerp*lerp*lerp*lerp) + _startValue;
        }
    
        /// \brief Easing equation function for a quintic (t^5) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutQuint<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutQuint,easeInQuint,easeOutQuint)
    
        /// \brief Easing equation function for a quintic (t^5) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInQuint<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInQuint,easeOutQuint,easeInQuint)
        
        /// \brief Easing equation function for a sinusoidal (sin(t)) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInSine<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInSine (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return delta*(1.0f - cosf(lerp*PI*0.5f)) + _startValue;
        }
    
        /// \brief Easing equation function for a sinusoidal (sin(t)) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutSine<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutSine (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return delta*(sinf(lerp*PI*0.5f)) + _startValue;
        }
    
        /// \brief Easing equation function for a sinusoidal (sin(t)) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutSine<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutSine,easeInSine,easeOutSine)
    
        /// \brief Easing equation function for a sinusoidal (sin(t)) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInSine<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInSine,easeOutSine,easeInSine)
    
        /// \brief Easing equation function for an exponential (2^t) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInExpo<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInExpo (const T& _startValue, const T& _endValue, float lerp) {
            if (lerp == 0)
                return _startValue;
            T delta = _endValue - _startValue;
            return delta*( (float)pow(2,10.0f*lerp-10.0f) - 0.001f ) + _startValue;
        }
    
        /// \brief Easing equation function for an exponential (2^t) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutExpo<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutExpo (const T& _startValue, const T& _endValue, float lerp) {
            if (lerp == 1)
                return _endValue;
            T delta = _endValue - _startValue;
            return delta*(  1.001f - (float)(1.001*pow(2,-10.0f*lerp) ) ) + _startValue;
        }
    
        /// \brief Easing equation function for an exponential (2^t) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutExpo<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutExpo,easeInExpo,easeOutExpo)

        /// \brief Easing equation function for an exponential (2^t) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInExpo<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInExpo,easeOutExpo,easeInExpo)

        /// \brief Easing equation function for a circular (sqrt(1-t^2)) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInCirc<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInCirc (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            return delta*( 1.0f - sqrtf(1.0f - lerp*lerp)) + _startValue;
        }
    
        /// \brief Easing equation function for a circular (sqrt(1-t^2)) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutCirc<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutCirc (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            lerp -= 1.0f;
            return delta*( sqrtf(1.0f - lerp*lerp)) + _startValue;
        }
    
        /// \brief Easing equation function for a circular (sqrt(1-t^2)) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutCirc<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutCirc,easeInCirc,easeOutCirc)
        
        /// \brief Easing equation function for a circular (sqrt(1-t^2)) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInCirc<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInCirc,easeOutCirc,easeInCirc)

        /// \brief Easing equation function for an elastic (exponentially decaying sine wave) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInElastic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInElastic (const T& _startValue, const T& _endValue, float lerp) {
            if (lerp == 0)
                return _startValue;
            T delta = _endValue - _startValue;
            return delta*(cosf(lerp*PI*7.0f))*
                         (0.001f- (float)pow(2,10.0f*lerp-10.0f)) + _startValue;
        }
    
        /// \brief Easing equation function for an elastic (exponentially decaying sine wave) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutElastic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutElastic (const T& _startValue, const T& _endValue, float lerp) {
            if (lerp == 1)
                return _endValue;
            T delta = _endValue - _startValue;
            return delta*(  (cosf(lerp*PI*7.0f))*
                            (-1.001f*(float)pow(2,-10.0f*lerp))
                            + 1.0f ) + _startValue;
        }
    
        /// \brief Easing equation function for an elastic (exponentially decaying sine wave) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutElastic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutElastic,easeInElastic,easeOutElastic)
    
        /// \brief Easing equation function for an elastic (exponentially decaying sine wave) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInElastic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInElastic,easeOutElastic,easeInElastic)

        /// \brief Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInBack<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInBack (const T& _startValue, const T& _endValue, float lerp) {
            const float overshoot = 1.70158f;
            T delta = _endValue - _startValue;
            return delta*lerp*lerp*((overshoot+1.0f)*lerp - overshoot) + _startValue;
        }
    
        /// \brief Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutBack<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutBack (const T& _startValue, const T& _endValue, float lerp) {
            const float overshoot = 1.70158f;
            T delta = _endValue - _startValue;
            lerp -= 1.0f;
            return delta*( lerp*lerp*((overshoot+1.0f)*lerp + overshoot) + 1.0f ) + _startValue;
        }
    
        /// \brief Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutBack<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutBack,easeInBack,easeOutBack)
    
        /// \brief Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInBack<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInBack,easeOutBack,easeInBack)

        /// \brief Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInBounce<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeInBounce (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            lerp = 1.0f - lerp;
            if (lerp < 1.0f/2.75f){
                lerp = 7.5625f*lerp*lerp;
            }else if (lerp < 2.0f/2.75f){
                lerp -= 1.5f/2.75f;
                lerp = 7.5625f*lerp*lerp + 0.75f;
            }else if (lerp < 2.5f/2.75f){
                lerp -= 2.25f/2.75f;
                lerp = 7.5625f*lerp*lerp + 0.9375f;
            }else{
                lerp -= 2.625f/2.75f;
                lerp = 7.5625f*lerp*lerp + 0.984375f;
            }
            return delta*(1.0f-lerp) + _startValue;
        }
    
        /// \brief Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutBounce<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        inline T easeOutBounce (const T& _startValue, const T& _endValue, float lerp) {
            T delta = _endValue - _startValue;
            if (lerp < 1.0f/2.75f){
                lerp = 7.5625f*lerp*lerp;
            }else if (lerp < 2.0f/2.75f){
                lerp -= 1.5f/2.75f;
                lerp = 7.5625f*lerp*lerp + 0.75f;
            }else if (lerp < 2.5f/2.75f){
                lerp -= 2.25f/2.75f;
                lerp = 7.5625f*lerp*lerp + 0.9375f;
            }else{
                lerp -= 2.625f/2.75f;
                lerp = 7.5625f*lerp*lerp + 0.984375f;
            }
            return delta*lerp + _startValue;
        }
    
        /// \brief Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeInOutBounce<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeInOutBounce,easeInBounce,easeOutBounce)
        
        /// \brief Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeOutInBounce<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(easeOutInBounce,easeOutBounce,easeInBounce)
}

#endif
