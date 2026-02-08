#pragma once

// custom inverse trigonometric inspired from:
// https://seblagarde.wordpress.com/2014/12/01/inverse-trigonometric-functions-gpu-optimization-for-amd-gcn-architecture/

#define TRIGONOMETRIC_CONSTANTS  \
    "#define fast_pi 3.141593\n" \
    "#define fast_half_pi 1.570796\n"

// max absolute error 9.0x10^-3
// Eberly's polynomial degree 1 - respect bounds

#define TRIGONOMETRIC_LOW_RES_ACOS_ASIN             \
    "// input [-1, 1] -> output [0, PI]\n"          \
    "float fast_acos(float _x)\n"                   \
    "{\n"                                           \
    "    float x = abs(_x);\n"                      \
    "    float t = -0.156583 * x + fast_half_pi;\n" \
    "    t *= sqrt(1.0 - x);\n"                     \
    "    return (_x < 0) ? fast_pi - t : t;\n"      \
    "}\n"                                           \
    "// input [-1, 1] -> output [-PI/2, PI/2]\n"    \
    "float fast_asin(float x)\n"                    \
    "{\n"                                           \
    "    return fast_half_pi - fast_acos(x);\n"     \
    "}\n"

#define TRIGONOMETRIC_HIGH_RES_ACOS_ASIN         \
    "// input [-1, 1] -> output [0, PI]\n"       \
    "float fast_acos(float _x)\n"                \
    "{\n"                                        \
    "    float x = abs(_x);\n"                   \
    "    float t = -0.0188236;  // a3\n"         \
    "    t = t * x + 0.0747737; // a2\n"         \
    "    t = t * x - 0.2125329; // a1\n"         \
    "    t = t * x + 1.570796;  // a0\n"         \
    "    t = t * sqrt(1.0 - x);\n"               \
    "    return (_x < 0) ? fast_pi - t : t;\n"   \
    "}\n"                                        \
    "// input [-1, 1] -> output [-PI/2, PI/2]\n" \
    "float fast_asin(float x)\n"                 \
    "{\n"                                        \
    "    return fast_half_pi - fast_acos(x);\n"  \
    "}\n"

// Eberly's odd polynomial degree 5 - respect bounds
#define TRIGONOMETRIC_ATAN                                                    \
    "// input [0, infinity] -> output [0, PI/2]\n"                            \
    "float fast_atan_positive(float _x)\n"                                    \
    "{\n"                                                                     \
    "    float x = (_x < 1.0) ? _x : 1.0 / _x;\n"                             \
    "    float x2 = x * x;\n"                                                 \
    "    float t = 0.0872929;\n"                                              \
    "    t = t * x2 - 0.301895;\n"                                            \
    "    t = t * x2 + 1.0;\n"                                                 \
    "    t = t * x;\n"                                                        \
    "    return (_x < 1.0) ? t : fast_half_pi - t;\n"                         \
    "}\n"                                                                     \
    "// input [-infinity, infinity] -> output [-PI/2, PI/2]\n"                \
    "float fast_atan(float x)\n"                                              \
    "{\n"                                                                     \
    "    float tn = fast_atan_positive(abs(x));\n"                            \
    "    return (x < 0.0) ? -tn : tn;\n"                                      \
    "}\n"                                                                     \
    "// input [any,any] -> output [0, 2PI]\n"                                 \
    "float fast_atan2(float y, float x)\n"                                    \
    "{\n"                                                                     \
    "    float ax = abs(x);\n"                                                \
    "    float ay = abs(y);\n"                                                \
    "    float inv = 1.0 / max(ax, 1e-8);\n"                                  \
    "    float t = fast_atan_positive(ay * inv);\n"                           \
    "    float sy = (y < 0.0) ? -1.0 : 1.0;\n"                                \
    "    float angle = (x < 0.0) ? fast_pi - t : t;\n"                        \
    "    angle *= sy;\n"                                                      \
    "    angle = (ax < 1e-8) ? sy * fast_half_pi : angle; // x == 0.0 case\n" \
    "    return angle;\n"                                                     \
    "}\n"

#define TRIGONOMETRIC_LOW_RES_ACOS_ASIN_GENTYPE(genType)                        \
    "// input [-1, 1] -> output [0, PI]\n"                                      \
    "" #genType " fast_acos(" #genType " x)\n"                                  \
    "{\n"                                                                       \
    "    " #genType " ax = abs(x);\n"                                           \
    "    " #genType " t  = (-0.156583 * ax + fast_half_pi) * sqrt(1.0 - ax);\n" \
    "    // step(0.0, x) = 0 if x < 0\n"                                        \
    "    return mix(fast_pi - t, t, step(0.0, x));\n"                           \
    "}\n"                                                                       \
    "// input [-1, 1] -> output [-PI/2, PI/2]\n"                                \
    "" #genType " fast_asin(" #genType " x)\n"                                  \
    "{\n"                                                                       \
    "    return fast_half_pi - fast_acos(x);\n"                                 \
    "}\n"

#define TRIGONOMETRIC_HIGH_RES_ACOS_ASIN_GENTYPE(genType)     \
    "// input [-1, 1] -> output [0, PI]\n"                    \
    "" #genType " fast_acos(" #genType " x)\n"                \
    "{\n"                                                     \
    "    " #genType " ax = abs(x);\n"                         \
    "    " #genType " t = " #genType "(-0.0188236);  // a3\n" \
    "    t = t * ax + 0.0747737; // a2\n"                     \
    "    t = t * ax - 0.2125329; // a1\n"                     \
    "    t = t * ax + 1.570796;  // a0\n"                     \
    "    t = t * sqrt(1.0 - ax);\n"                           \
    "    // step(0.0, x) = 0 if x < 0\n"                      \
    "    return mix(fast_pi - t, t, step(0.0, x));\n"         \
    "}\n"                                                     \
    "// input [-1, 1] -> output [-PI/2, PI/2]\n"              \
    "" #genType " fast_asin(" #genType " x)\n"                \
    "{\n"                                                     \
    "    return fast_half_pi - fast_acos(x);\n"               \
    "}\n"

#define TRIGONOMETRIC_ATAN_GENTYPE(genType)                                      \
    "// input [0, +inf] -> output [0, PI/2]\n"                                   \
    "" #genType " fast_atan_positive(" #genType " x)\n"                          \
    "{\n"                                                                        \
    "    " #genType " safe_x = max(x, " #genType "(1e-8));\n"                    \
    "    " #genType " use_inv = step(1.0, x);\n"                                 \
    "    " #genType " v = mix(x, 1.0 / safe_x, use_inv);\n"                      \
    "    " #genType " v2 = v * v;\n"                                             \
    "    " #genType " t = " #genType "(0.0872929);\n"                            \
    "    t = t * v2 - 0.301895;\n"                                               \
    "    t = t * v2 + 1.0;\n"                                                    \
    "    t = t * v;\n"                                                           \
    "    return mix(t, fast_half_pi - t, use_inv);\n"                            \
    "}\n"                                                                        \
    "// input [-inf, +inf] -> output [-PI/2, PI/2]\n"                            \
    "" #genType " fast_atan(" #genType " x)\n"                                   \
    "{\n"                                                                        \
    "    " #genType " t = fast_atan_positive(abs(x));\n"                         \
    "    return t * sign(x);\n"                                                  \
    "}\n"                                                                        \
    "// input [any,any] -> output [0, 2PI]\n"                                    \
    "" #genType " fast_atan2(" #genType " y, " #genType " x)\n"                  \
    "{\n"                                                                        \
    "    " #genType " ax = abs(x);\n"                                            \
    "    " #genType " ay = abs(y);\n"                                            \
    "    " #genType " safe_ax = max(ax, " #genType "(1e-8));\n"                  \
    "    " #genType " invx = 1.0 / safe_ax;\n"                                   \
    "    " #genType " t = fast_atan_positive(ay * invx);\n"                      \
    "    " #genType " sy = 2.0 * step(0.0, y) - 1.0;\n"                          \
    "    " #genType " angle = mix(fast_pi - t, t, step(0.0, x));\n"              \
    "    angle *= sy;\n"                                                         \
    "    // step(" #genType "(1e-8), ax) = 0 if ax < 1e-8\n"                     \
    "    angle = mix(sy * fast_half_pi, angle, step(" #genType "(1e-8), ax));\n" \
    "    return angle;\n"                                                        \
    "}\n"

#define CROSS_VEC2                        \
    "float cross_vec2(vec2 a, vec2 b)\n"  \
    "{\n"                                 \
    "    return a.x * b.y - a.y * b.x;\n" \
    "}\n"

#define BARYCENTRIC_VEC2                                 \
    "vec3 barycentric(vec2 a, vec2 b, vec2 c, vec2 p)\n" \
    "{\n"                                                \
    "    vec2 v0 = c - b;\n"                             \
    "    vec2 v1 = a - b;\n"                             \
    "    vec2 v2 = p - b;\n"                             \
    "    float invArea = 1.0 / cross_vec2(v0, v1);\n"    \
    "    float u = cross_vec2(v0, v2) * invArea;\n"      \
    "    float w = cross_vec2(v2, v1) * invArea;\n"      \
    "    float v = 1.0 - u - w;\n"                       \
    "    return vec3(u, v, w);\n"                        \
    "}\n"

#define Shader_sampleEnvironmentCubeBaryLerp                                      \
    "vec3 sampleEnvironmentCubeBaryLerp(samplerCube cubeTexture, vec3 normal){\n" \
    "    vec3 s = sign(normal);\n"                                                \
    "    vec3 texX = textureCube(cubeTexture, vec3(s.x, 0, 0)).rgb;\n"            \
    "    vec3 texY = textureCube(cubeTexture, vec3(0, s.y, 0)).rgb;\n"            \
    "    vec3 texZ = textureCube(cubeTexture, vec3(0, 0, s.z)).rgb;\n"            \
    "    vec2 flip = step(s.xz, vec2(-0.5)) * 3.14159265;\n"                      \
    "    vec2 pX = vec2(1.57079637, flip.x);\n"                                   \
    "    vec2 pY = vec2(1.57079637);\n"                                           \
    "    vec2 pZ = vec2(flip.y, 1.57079637);\n"                                   \
    "    vec2 polarN = fast_acos(normal.zx);\n"                                   \
    "    vec3 w = barycentric(pX, pY, pZ, polarN);\n"                             \
    "    return texX * w.x + texY * w.y + texZ * w.z;\n"                          \
    "}\n"

#define Shader_Spherical_st2normal                     \
    "vec3 st2normal(vec2 st) {\n"                      \
    "    float phi = st.y * 3.14159265358;// pi\n"     \
    "    float theta = st.x * 6.28318530716;// 2pi;\n" \
    "    float sin_phi = sin(phi);\n"                  \
    "    vec3 result;\n"                               \
    "    result.x = sin_phi * cos(theta);\n"           \
    "    result.y = cos(phi);\n"                       \
    "    result.z = sin_phi * sin(theta);\n"           \
    "    return result;\n"                             \
    "}\n"

#define Shader_Spherical_normal2st                                             \
    "vec2 normal2st(vec3 normal) {\n"                                          \
    "    float t = fast_acos(normal.y) * 0.318309886; // 1/pi\n"               \
    "    float s = fast_atan2(normal.z, normal.x) * 0.159154943; // 1/(2pi)\n" \
    "    s = fract(s + 1.0);\n"                                                \
    "    return vec2(s, t);\n"                                                 \
    "}\n"

#define Shader_sampleEnvironmentSpheremap                                      \
    "vec3 sampleEnvironmentSpheremap(sampler2D sphereAmbient, vec3 normal){\n" \
    "  vec2 uv = normal2st(normal);\n"                                         \
    "  uv.y = uv.y * 0.5;\n"                                                   \
    "  return texture2D(sphereAmbient, uv).rgb;\n"                             \
    "}\n"

#define SphericalAndCubeMap_All_Functions                 \
    "" TRIGONOMETRIC_CONSTANTS                            \
    "" TRIGONOMETRIC_ATAN                                 \
    "" TRIGONOMETRIC_LOW_RES_ACOS_ASIN                    \
    "" TRIGONOMETRIC_LOW_RES_ACOS_ASIN_GENTYPE(vec2) /**/ \
        "" CROSS_VEC2                                     \
        "" BARYCENTRIC_VEC2                               \
        "" Shader_sampleEnvironmentCubeBaryLerp           \
        "" Shader_Spherical_st2normal                     \
        "" Shader_Spherical_normal2st                     \
        "" Shader_sampleEnvironmentSpheremap
