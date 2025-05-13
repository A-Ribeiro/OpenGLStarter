#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit-Extension/image/PNG.h>

using namespace MathCore;

float fresnelSchlick(float cosTheta, float F0)
{
    return F0 + (1.0f - F0) * OP<float>::pow(1.0f - cosTheta, 5.0f);
}

float DistributionGGX(float NdotH, float roughness) {
    //NdotH = clamp(NdotH,0.0f, 0.999f);
    roughness = OP<float>::clamp(roughness, 0.085f, 1.0f);
    float a = roughness * roughness;
    float a2 = a * a;
    //float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = 3.14159265358f * denom * denom; //PI = 3.14159265358
    return OP<float>::clamp(num / denom, 0.0f, 1.0f);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;
    return num / denom;
}

/*
base 10 - 0..1.0

    1.0

    1.0  10.0

    x = 0.0
    y = 0.0

    x -= y/10
    x = 0.76 - 0.06
    x = 0.7


*/

//https://www.khronos.org/opengl/wiki/Normalized_Integer
// this is the direct byte to float conversion
float byte2float(uint8_t b) {
    return (float)b / 255.0f;
}
// the float to byte may vary
uint8_t float2byte(float f) {
    //can use the direct inverse
    return (uint8_t)(f * 255.0f);
    //or can round the value
    //return (uint8_t)(f * 255.0f + 0.5f);
}

vec2f encode_2el(float f) {

    float f_ge_1 = f >= 1.0;

    //if (f >= 1.0)
        //return MathCore::vec2f(1.0f,1.0f);

    vec2f enc = MathCore::vec2f(f, f * 255.0f);
    //enc = fract(enc);
    enc.x = MathCore::OP<float>::fmod(enc.x, 1.0f);
    enc.y = MathCore::OP<float>::fmod(enc.y, 1.0f);
    //enc -= enc.yzww * vec4(1. / 255., 1. / 255., 1. / 255., 0.);
    //enc -= enc.yzww * vec4(3.9215686275e-03, 3.9215686275e-03, 3.9215686275e-03, 0.0);

    uint8_t byte = float2byte(enc.x);
    //enc.x -= enc.y * (1.0f / 255.0f);
    enc.x = ((float)byte + 0.2f) / 255.0f;//force byte RGB value


    if (byte % 2) {
        //enc.y = 1.0f - enc.y;
        enc.y = 1.0f - enc.y;
    }

    return OP<vec2f>::lerp(enc, MathCore::vec2f(1.0f, 1.0f), f_ge_1);
}

float decode_2el(vec2f v) {
    //return dot(v, vec4(1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 16581375.0));
    //return dot(v, vec4(1.0, 3.9215686275e-03, 1.5378700500e-05, 6.0308629411e-08));
    //return dot(v, MathCore::vec2f(1.0, 1.0f / 255.0f));
    uint8_t byte = float2byte(v.x);
    if (byte % 2) {
        v.y = 1.0f - v.y;
    }

    return v.x + v.y * 3.9215686275e-03f;
}


void testPBR_Values() {

    int count = 10;

    printf("fresnelSchlick:\n");
    for (int y = 0; y < count; y++)
    {
        float lrp_param2 = (float)y / (float)(count - 1);
        for (int x = 0; x < count; x++)
        {
            float lrp_param1 = (float)x / (float)(count - 1);
            printf("%.3f ", fresnelSchlick(lrp_param1, lrp_param2));
        }
        printf("\n");
    }

    printf("DistributionGGX:\n");
    for (int y = 0; y < count; y++)
    {
        float lrp_param2 = (float)y / (float)(count - 1);
        for (int x = 0; x < count; x++)
        {
            float lrp_param1 = (float)x / (float)(count - 1);
            printf("%.6f ", DistributionGGX(lrp_param1, lrp_param2));
        }
        printf("\n");
    }

    printf("GeometrySchlickGGX:\n");
    for (int y = 0; y < count; y++)
    {
        float lrp_param2 = (float)y / (float)(count - 1);
        for (int x = 0; x < count; x++)
        {
            float lrp_param1 = (float)x / (float)(count - 1);
            printf("%.6f ", GeometrySchlickGGX(lrp_param1, lrp_param2));
        }
        printf("\n");
    }

    int size = 512;
    uint8_t* fresnel = (uint8_t*)ITKCommon::Memory::malloc(size * size * sizeof(uint8_t));
    uint8_t* geometry = (uint8_t*)ITKCommon::Memory::malloc(size * size * sizeof(uint8_t));
    uint8_t* distribution = (uint8_t*)ITKCommon::Memory::malloc(size * size * sizeof(uint8_t));
    uint8_t* texture_rgba = (uint8_t*)ITKCommon::Memory::malloc(size * size * sizeof(uint8_t) * 4);

    for (int y = 0; y < size; y++)
    {
        float lrp_param2 = (float)y / (float)(size - 1);
        for (int x = 0; x < size; x++)
        {
            float lrp_param1 = (float)x / (float)(size - 1);
            fresnel[x + y * size] = float2byte(fresnelSchlick(lrp_param1, lrp_param2));
            geometry[x + y * size] = float2byte(GeometrySchlickGGX(lrp_param1, lrp_param2));
            distribution[x + y * size] = float2byte(DistributionGGX(lrp_param1, lrp_param2));

            int index = (x + y * size) * 4;
            texture_rgba[index + 0] = float2byte(fresnelSchlick(lrp_param1, lrp_param2));
            texture_rgba[index + 1] = float2byte(GeometrySchlickGGX(lrp_param1, lrp_param2));
            vec2f encoded_distribution = encode_2el(DistributionGGX(lrp_param1, lrp_param2));
            texture_rgba[index + 2] = float2byte(encoded_distribution.x);
            texture_rgba[index + 3] = float2byte(encoded_distribution.y);
        }
    }
    
    ITKExtension::Image::PNG::writePNG("fresnel.png", size, size, 1, (char*)fresnel);
    ITKExtension::Image::PNG::writePNG("geometry.png", size, size, 1, (char*)geometry);
    ITKExtension::Image::PNG::writePNG("distribution.png", size, size, 1, (char*)distribution);
    ITKExtension::Image::PNG::writePNG("texture_rgba.png", size, size, 4, (char*)texture_rgba);

    ITKCommon::Memory::free(fresnel);
    ITKCommon::Memory::free(geometry);
    ITKCommon::Memory::free(distribution);
    ITKCommon::Memory::free(texture_rgba);



    /*
    for (int i=0;i<size;i++){
        float v = (float)i/(float)(size-1);

        uint8_t byte_a = float2byte(v);
        uint8_t byte = float2byte(byte2float(byte_a));


        vec2f encoded = encode_2el(v);
        //printf("before: %.8f\n", v);
        //printf("encoded: %.8f %.8f\n", encoded.x, encoded.y);
        uint8_t byte_a = float2byte(encoded.x);
        uint8_t byte_b = float2byte(encoded.y);
        //printf("encoded(byte): %u %u\n", byte_a, byte_b);


        float _v = decode_2el(MathCore::vec2f( byte2float(byte_a), byte2float(byte_b) ));
        //printf("after: %.8f\n", _v);

        uint8_t byte = float2byte(byte2float(byte_a));
        //printf("byte_a ->float->byte: %u\n", byte);
    // * /
        if (byte != byte_a) {
            printf("ERROR\n");
            printf("encoded(byte): %u \n", byte_a);
            printf("byte_a ->float->byte: %u\n", byte);
            break;
        }
    }
    */

}