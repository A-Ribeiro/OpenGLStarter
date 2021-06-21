#ifndef color_conversion__H
#define color_conversion__H

#include <aribeiro/vec3.h>
#include <aribeiro/common.h>


#if defined(OS_TARGET_win)
#include <inttypes.h>
#include <sys/types.h>
#include <stdint.h>
#else
#include <sys/types.h>
#endif

namespace aRibeiro {
    
    /// \brief Common float color conversion methods
    ///
    /// Color systems: RGB, HSV, CIE
    ///
    /// \author Alessandro Ribeiro
    ///
    class FloatColorConversion {
    public:

        /// \brief Converts RGB to HSV
        ///
        /// RGB: Red Green Blue
        ///
        /// HSV: Hue Saturation Value
        ///
        /// The RGB are in the range 0..1
        ///
        /// The HSV uses the ranges: H = 0..360, S = 0..1, V = 0..1
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 rgb;
        ///
        /// vec3 hsv = FloatColorConversion::RGBtoHSV( rgb );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rgb Red Green Blue
        /// \return Hue Saturation Value
        ///
        static vec3 RGBtoHSV(const vec3 &rgb);

        /// \brief Converts HSV to RGB
        ///
        /// HSV: Hue Saturation Value
        ///
        /// RGB: Red Green Blue
        ///
        /// The HSV uses the ranges: H = 0..360, S = 0..1, V = 0..1
        ///
        /// The RGB are in the range 0..1
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 hsv;
        ///
        /// vec3 rgb = FloatColorConversion::HSVtoRGB( hsv );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param hsv Hue Saturation Value
        /// \return Red Green Blue
        ///
        static vec3 HSVtoRGB(const vec3 &hsv);

        /// \brief Converts RGB to CIE 1931 color space
        ///
        /// RGB: Red Green Blue
        ///
        /// CIE: Commission internationale de l´Eclairage
        ///
        /// The RGB are in the range 0..1
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 rgb;
        ///
        /// vec3 cie = FloatColorConversion::RGBtoCIE( rgb );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rgb Red Green Blue
        /// \return cie 1931 color space
        ///
        static vec3 RGBtoCIE(const vec3 &rgb);

        /// \brief Converts CIE 1931 color space to RGB
        ///
        /// CIE: Commission internationale de l´Eclairage
        ///
        /// RGB: Red Green Blue
        ///
        /// The RGB are in the range 0..1
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 cie;
        ///
        /// vec3 rgb = FloatColorConversion::RGBtoCIE( cie );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param cie cie 1931 color space
        /// \return Red Green Blue
        ///
        static vec3 CIEtoRGB(const vec3 &cie);
    };

    /// \brief Common unsigned byte color conversion methods
    ///
    /// Color systems: RGB, CMY, CMYK, YUV
    ///
    /// \author Alessandro Ribeiro
    ///
    class UByteColorConversion {
    public:

        /// \brief Converts RGB to CMY
        ///
        /// RGB: Red Green Blue
        ///
        /// CMY: Cian Magenta Yellow
        ///
        /// The RGB are in the range 0..255
        ///
        /// The CMY are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// uint8_t r, g, b;
        /// uint8_t c, m, y;
        ///
        /// UByteColorConversion::RGBtoCMY( r, g, b, &c, &m, &y );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r Red
        /// \param g Green
        /// \param b Blue
        /// \param c **return** Cian
        /// \param m **return** Magenta
        /// \param y **return** Yellow
        ///
        static void RGBtoCMY(uint8_t r, uint8_t g, uint8_t b, uint8_t *c, uint8_t *m, uint8_t *y);

        /// \brief Converts CMY to RGB
        ///
        /// CMY: Cian Magenta Yellow
        ///
        /// RGB: Red Green Blue
        ///
        /// The CMY are in the range 0..255
        ///
        /// The RGB are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// uint8_t c, m, y;
        /// uint8_t r, g, b;
        ///
        /// UByteColorConversion::CMYtoRGB( c, m, y, &r, &g, &b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param c Cian
        /// \param m Magenta
        /// \param y Yellow
        /// \param r **return** Red
        /// \param g **return** Green
        /// \param b **return** Blue
        ///
        static void CMYtoRGB(uint8_t c, uint8_t m, uint8_t y, uint8_t *r, uint8_t *g, uint8_t *b);

        /// \brief Converts RGB to CMYK
        ///
        /// RGB: Red Green Blue
        ///
        /// CMYK: Cian Magenta Yellow (Amount of Black)
        ///
        /// The RGB are in the range 0..255
        ///
        /// The CMYK are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// uint8_t r, g, b;
        /// uint8_t c, m, y, k;
        ///
        /// UByteColorConversion::RGBtoCMYK( r, g, b, &c, &m, &y, &k );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r Red
        /// \param g Green
        /// \param b Blue
        /// \param c **return** Cian
        /// \param m **return** Magenta
        /// \param y **return** Yellow
        /// \param k **return** Amount of black
        ///
        static void RGBtoCMYK(uint8_t r, uint8_t g, uint8_t b, uint8_t *c, uint8_t *m, uint8_t *y, uint8_t *k);

        /// \brief Converts CMYK to RGB
        ///
        /// CMYK: Cian Magenta Yellow (Amount of Black)
        ///
        /// RGB: Red Green Blue
        ///
        /// The CMYK are in the range 0..255
        ///
        /// The RGB are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// uint8_t c, m, y, k;
        /// uint8_t r, g, b;
        ///
        /// UByteColorConversion::CMYKtoRGB( c, m, y, k, &r, &g, &b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param c Cian
        /// \param m Magenta
        /// \param y Yellow
        /// \param k Amount of black
        /// \param r **return** Red
        /// \param g **return** Green
        /// \param b **return** Blue
        ///
        static void CMYKtoRGB(uint8_t c, uint8_t m, uint8_t y, uint8_t k, uint8_t *r, uint8_t *g, uint8_t *b);

        /// \brief Converts RGB to YUV
        ///
        /// RGB: Red Green Blue
        ///
        /// YUV: luma component (Y) chrominance U (blue projection) chrominance V (red projection)
        ///
        /// The RGB are in the range 0..255
        ///
        /// The YUV are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// uint8_t r, g, b;
        /// uint8_t y, u, v;
        ///
        /// UByteColorConversion::RGBtoYUV( r, g, b, &y, &u, &v );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r Red
        /// \param g Green
        /// \param b Blue
        /// \param y **return** Luma
        /// \param u **return** Blue Projection
        /// \param v **return** Red Projection
        ///
        static void RGBtoYUV(uint8_t r, uint8_t g, uint8_t b, uint8_t *y, uint8_t *u, uint8_t *v);

        /// \brief Converts YUV to RGB
        ///
        /// YUV: luma component (Y) chrominance U (blue projection) chrominance V (red projection)
        ///
        /// RGB: Red Green Blue
        ///
        /// The YUV are in the range 0..255
        ///
        /// The RGB are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// uint8_t y, u, v;
        /// uint8_t r, g, b;
        ///
        /// UByteColorConversion::YUVtoRGB( y, u, v, &r, &g, &b, );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param y Luma
        /// \param u Blue Projection
        /// \param v Red Projection
        /// \param r **return** Red
        /// \param g **return** Green
        /// \param b **return** Blue
        ///
        static void YUVtoRGB(uint8_t y, uint8_t u, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b);
        
    };

}


#endif
