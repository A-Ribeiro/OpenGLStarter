# OpenGLStarter

[Back to HOME](../../index)

## Color Conversion

There are two categories to convert color systems.

You need to check the input and output range to use the convertion methods/functions.

* Floating Point (__FloatColorConversion__)
    * __RGBtoHSV:__ Converts RGB [0..1] to HSV ( H = [0..360], S = [0..1], V = [0..1] )
    * __HSVtoRGB:__ Converts HSV ( H = [0..360], S = [0..1], V = [0..1] ) to RGB [0..1]
    * __RGBtoCIE:__ Converts RGB [0..1] to CIE ( float )
    * __CIEtoRGB:__ Converts CIE ( float ) to RGB [0..1]
* Unsigned Byte (__UByteColorConversion__)
    * __RGBtoCMY:__ Converts RGB [0..255] to CMY [0..255]
    * __CMYtoRGB:__ Converts CMY [0..255] to RGB [0..255]
    * __RGBtoCMYK:__ Converts RGB [0..255] to CMYK [0..255]
    * __CMYKtoRGB:__ Converts CMYK [0..255] to RGB [0..255]
    * __RGBtoYUV:__ Converts RGB [0..255] to YUV [0..255]
    * __YUVtoRGB:__ Converts YUV [0..255] to RGB [0..255]

## Examples

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

//
// float example
//
vec3 rgb;
vec3 hsv;

hsv = FloatColorConversion::RGBtoHSV( rgb );

//
// ubyte example
//
uint8_t r, g, b;
uint8_t c, m, y;

UByteColorConversion::RGBtoCMY( r, g, b, &c, &m, &y );

```
