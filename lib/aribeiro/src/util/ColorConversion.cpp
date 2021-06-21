
#include "ColorConversion.h"

#define maxInt(a,b) ((a>b)?(a):(b))

#define clampInt(a,low,high) if (a < low) \
a = low; \
else if (a > high) \
a = high

#include "math.h"
#include <aribeiro/all_math.h>

namespace aRibeiro {


    vec3 FloatColorConversion::RGBtoHSV(const vec3 &rgb) {
        float r = rgb.r, g = rgb.g, b = rgb.b;
        vec3 result;
        float min = minimum(minimum(r, g), b);
        float max = maximum(maximum(r, g), b);
        
        float delta = max - min;
        if (delta == 0) {
            result.x = 0;
        } else if (max == r && g >= b) {
            result.x = 60.0f * ((g - b) / delta) + 0.0f;
        } else if (max == r && g < b) {
            result.x = 60.0f * ((g - b) / delta) + 360.0f;
        }
        else if (max == g) {
            result.x = 60.0f * ((b - r) / delta) + 120.0f;
        }
        else if (max == b) {
            result.x = 60.0f * ((r - g) / delta) + 240.0f;
        }
        
        if (max == 0)
            result.y = 0;
        else
            result.y = (max - min) / max;
        result.z = max;
        return result;
    }
    
    vec3 FloatColorConversion::HSVtoRGB(const vec3 &hsv) {
        
        float h = hsv.x;
        float s = hsv.y;
        float v = hsv.z;
        
        int Hi = (int)(h / 60.0f) % 6;
        //float f = h / 60.0f - Hi;
        float f = fmod(h/60.0f,1.0f);
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f)*s);
        
        if (Hi == 0)
            return vec3(v, t, p);
        else if (Hi == 1)
            return vec3(q, v, p);
        else if (Hi == 2)
            return vec3(p, v, t);
        else if (Hi == 3)
            return vec3(p, q, v);
        else if (Hi == 4)
            return vec3(t, p, v);
        else if (Hi == 5)
            return vec3(v, p, q);
        
        return vec3();
    }
    
    vec3 FloatColorConversion::RGBtoCIE(const vec3 &rgb) {
        vec3 result;
        result.x = rgb.r*0.49f + rgb.g*0.31f + rgb.b*0.2f;
        result.y = rgb.r*0.17697f + rgb.g*0.8124f + rgb.b*0.01063f;
        result.z = rgb.g*0.01f + rgb.b*0.99f;
        result /= 0.17697f;
        return result;
    }
    
    vec3 FloatColorConversion::CIEtoRGB(const vec3 &cie) {
        vec3 result;
        result.r = cie.x*0.41847f - cie.y*0.15866f - cie.z*0.082835f;
        result.g = -cie.x*0.091169f + cie.y*0.25243f + cie.z*0.015708f;
        result.b = cie.x*0.00092090f - cie.y*0.0025498f + cie.z*0.1786f;
        return result;
    }

    void UByteColorConversion::RGBtoCMY(uint8_t r, uint8_t g, uint8_t b, uint8_t *c, uint8_t *m, uint8_t *y){
        *c = 255 - r;
        *m = 255 - g;
        *y = 255 - b;
    }
    
    void UByteColorConversion::CMYtoRGB(uint8_t c, uint8_t m, uint8_t y, uint8_t *r, uint8_t *g, uint8_t *b){
        *r = 255 - c;
        *g = 255 - m;
        *b = 255 - y;
    }
    
    void UByteColorConversion::RGBtoCMYK(uint8_t r, uint8_t g, uint8_t b, uint8_t *c, uint8_t *m, uint8_t *y, uint8_t *k){
        int gray = maxInt(maxInt(r,g),b);
        int kaux = (255 - gray);
        
        *k = kaux;
        
        if (gray == 0){
            *c = 0;
            *m = 0;
            *y = 0;
        }else{
            int cAux = ((255 -(int)r - kaux) * 255) / gray;
            int mAux = ((255 -(int)g - kaux) * 255) / gray;
            int yAux = ((255 -(int)b - kaux) * 255) / gray;
            *c = (uint8_t)cAux;
            *m = (uint8_t)mAux;
            *y = (uint8_t)yAux;
        }
    }
    
    void UByteColorConversion::CMYKtoRGB(uint8_t c, uint8_t m, uint8_t y, uint8_t k, uint8_t *r, uint8_t *g, uint8_t *b){
        int gray = 255 - k;
        int raux = ((255 - (int)c) * gray) / 255;
        int gaux = ((255 - (int)m) * gray) / 255;
        int baux = ((255 - (int)y) * gray) / 255;
        *r = (uint8_t)raux;
        *g = (uint8_t)gaux;
        *b = (uint8_t)baux;
    }
    
    void UByteColorConversion::YUVtoRGB(uint8_t y, uint8_t u, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
        int c = y-16;
        int d = u-128;
        int e = v-128;
        
        int rAux = (298 * c + 409 * e + 128) >> 8;
        int gAux = (298 * c - 100 * d - 208 * e + 128) >> 8;
        int bAux = (298 * c + 516 * d + 128) >> 8;
        
        clampInt(rAux, 0, 255);
        clampInt(gAux, 0, 255);
        clampInt(bAux, 0, 255);
        
        *r = (uint8_t)rAux;
        *g = (uint8_t)gAux;
        *b = (uint8_t)bAux;
    }
    
    void UByteColorConversion::RGBtoYUV(uint8_t r, uint8_t g, uint8_t b, uint8_t *y, uint8_t *u, uint8_t *v) {
        
        int ir = (int)r;
        int ig = (int)g;
        int ib = (int)b;
        
        *y = (uint8_t)(((66 * ir + 129 * ig +  25 * ib + 128) >> 8) +  16);
        *u = (uint8_t)(((-38 * ir - 74 * ig + 112 * ib + 128) >> 8) + 128);
        *v = (uint8_t)(((112 * ir - 94 * ig -  18 * ib + 128) >> 8) + 128);
        
    }
}
