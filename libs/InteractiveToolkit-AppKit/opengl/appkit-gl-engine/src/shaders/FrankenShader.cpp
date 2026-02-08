#include <appkit-gl-engine/shaders/FrankenShader.h>

#define PBR_MODE_OPTIMIZED

// custom inverse trigonometric inspired from:
// https://seblagarde.wordpress.com/2014/12/01/inverse-trigonometric-functions-gpu-optimization-for-amd-gcn-architecture/

#define TRIGONOMETRIC_CONSTANTS       \
    "#define fast_pi 3.14159265358\n" \
    "#define fast_half_pi 1.57079632679\n"

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
    CROSS_VEC2                                           \
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

namespace AppKit
{
    namespace GLEngine
    {

        static void findAndReplaceAll(std::string *data, const std::string &toSearch, const std::string &replaceStr)
        {
            // Get the first occurrence
            size_t pos = data->find(toSearch);

            // Repeat till end is reached
            while (pos != std::string::npos)
            {
                // Replace this occurrence of Sub String
                data->replace(pos, toSearch.size(), replaceStr);
                // Get the next occurrence from the current position
                pos = data->find(toSearch, pos + replaceStr.size());
            }
        }

        static std::string findAndReplaceAllExt(const std::string &in, const std::string &toSearch, const std::string &replaceStr)
        {
            std::string data = in;

            // Get the first occurrence
            size_t pos = data.find(toSearch);

            // Repeat till end is reached
            while (pos != std::string::npos)
            {
                // Replace this occurrence of Sub String
                data.replace(pos, toSearch.size(), replaceStr);
                // Get the next occurrence from the current position
                pos = data.find(toSearch, pos + replaceStr.size());
            }

            return data;
        }

        const AppKit::OpenGL::ShaderType FrankenShader::Type = "FrankenShader";

        FrankenShader::FrankenShader(
            ShaderAlgorithmsBitMask shaderAlgorithms,
            ShaderPBRAlgorithmEnum _shaderPBRAlgorithm,
            ShaderShadowAlgorithmEnum _shaderShadowAlgorithm) : DefaultEngineShader(FrankenShader::Type),
                                                                frankenUniformManager(this)
        {

            frankenFormat = shaderAlgorithms;
            shaderPBRAlgorithm = _shaderPBRAlgorithm;
            shaderShadowAlgorithm = _shaderShadowAlgorithm;

            compileShader();

            // frankenUniformManager.readUniformsFromShaderAndInitStatic();

            // sprintf(class_name, "FrankenShader");
        }

        void FrankenShader::compileShader()
        {
            format = ITKExtension::Model::CONTAINS_POS; // | ITKExtension::Model::CONTAINS_UV0;

            // check need of normalMap
            if (!(frankenFormat & (ShaderAlgorithms_AmbientLightSkybox |
                                   ShaderAlgorithms_AmbientLightSpheremap |
                                   ShaderAlgorithms_SunLight0 |
                                   ShaderAlgorithms_SunLight1 |
                                   ShaderAlgorithms_SunLight2 |
                                   ShaderAlgorithms_SunLight3 |
                                   ShaderAlgorithms_ShadowSunLight0 |
                                   ShaderAlgorithms_ShadowSunLight1 |
                                   ShaderAlgorithms_ShadowSunLight2 |
                                   ShaderAlgorithms_ShadowSunLight3)))
            {
                frankenFormat &= ~ShaderAlgorithms_NormalMap;
            }

            //
            // Vertex shader mounter
            //

            std::string vertexShader =
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "attribute vec4 aPosition;\n"
                "uniform mat4 uMVP;\n"
                "VARIABLES"
                "FUNCTIONS"
                "void main(){\n"
                "SHADER_CODE"
                "}";

            // skinned mesh code
            if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_128)
            {
                format |= ITKExtension::Model::CONTAINS_VERTEX_WEIGHT128;

                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "uniform mat4 uSkinGradientMatrix[128];\n"
                                  "attribute vec4 aSkinIndex;\n"
                                  "attribute vec4 aSkinWeight;\n"
                                  "VARIABLES");
            }
            else if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_96)
            {
                format |= ITKExtension::Model::CONTAINS_VERTEX_WEIGHT96;

                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "uniform mat4 uSkinGradientMatrix[96];\n"
                                  "attribute vec4 aSkinIndex;\n"
                                  "attribute vec4 aSkinWeight;\n"
                                  "VARIABLES");
            }
            else if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_64)
            {
                format |= ITKExtension::Model::CONTAINS_VERTEX_WEIGHT64;

                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "uniform mat4 uSkinGradientMatrix[64];\n"
                                  "attribute vec4 aSkinIndex;\n"
                                  "attribute vec4 aSkinWeight;\n"
                                  "VARIABLES");
            }
            else if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_32)
            {
                format |= ITKExtension::Model::CONTAINS_VERTEX_WEIGHT32;

                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "uniform mat4 uSkinGradientMatrix[32];\n"
                                  "attribute vec4 aSkinIndex;\n"
                                  "attribute vec4 aSkinWeight;\n"
                                  "VARIABLES");
            }
            else if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_16)
            {
                format |= ITKExtension::Model::CONTAINS_VERTEX_WEIGHT16;

                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "uniform mat4 uSkinGradientMatrix[16];\n"
                                  "attribute vec4 aSkinIndex;\n"
                                  "attribute vec4 aSkinWeight;\n"
                                  "VARIABLES");
            }

            if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
            {

                findAndReplaceAll(&vertexShader,
                                  "FUNCTIONS",
                                  "mat3 inverse_transpose_rotation_3(mat3 m) {\n"
                                  "  float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];\n"
                                  "  float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];\n"
                                  "  float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];\n"
                                  "  float b01 = a22 * a11 - a12 * a21;\n"
                                  "  float b11 = a12 * a20 - a22 * a10;\n"
                                  "  float b21 = a21 * a10 - a11 * a20;\n"
                                  "  return mat3(b01, b11, b21,\n"
                                  "    (a02 * a21 - a22 * a01), (a22 * a00 - a02 * a20), (a01 * a20 - a21 * a00),\n"
                                  "    (a12 * a01 - a02 * a11), (a02 * a10 - a12 * a00), (a11 * a00 - a01 * a10))\n"
                                  "    / (a00 * b01 + a01 * b11 + a02 * b21);\n" // result / determinant
                                  "}\n"
                                  "FUNCTIONS");

                findAndReplaceAll(&vertexShader,
                                  "SHADER_CODE",

                                  /*
                                  "  mat4 v_gradient_ = mat4(0);\n"
                                  "  for (int i=0;i<4;i++) {\n"
                                  "    v_gradient_ += uSkinGradientMatrix[int(aSkinIndex[i])] * aSkinWeight[i];\n"
                                  "  }\n"
                                  //*/

                                  // MACOS shader 120 compatible
                                  "  mat4 v_gradient_ = uSkinGradientMatrix[int(aSkinIndex.x)] * aSkinWeight.x;\n"
                                  "  v_gradient_ += uSkinGradientMatrix[int(aSkinIndex.y)] * aSkinWeight.y;\n"
                                  "  v_gradient_ += uSkinGradientMatrix[int(aSkinIndex.z)] * aSkinWeight.z;\n"
                                  "  v_gradient_ += uSkinGradientMatrix[int(aSkinIndex.w)] * aSkinWeight.w;\n"
                                  // */

                                  "  vec4 inputPosition = v_gradient_ * aPosition;\n"
                                  "  mat3 v_gradient_mat3 = mat3(v_gradient_);\n"
                                  "  mat3 v_gradient_IT = inverse_transpose_rotation_3( v_gradient_mat3 );\n"
                                  "SHADER_CODE");
            }
            else
            {
                findAndReplaceAll(&vertexShader,
                                  "SHADER_CODE",
                                  "  vec4 inputPosition = aPosition;\n"
                                  "SHADER_CODE");
            }

            bool uses_uv = false;

            if (frankenFormat & (ShaderAlgorithms_TextureAlbedo | ShaderAlgorithms_TextureSpecular | ShaderAlgorithms_TextureEmission))
            {
                format |= ITKExtension::Model::CONTAINS_UV0;
                uses_uv = true;
            }

            bool usesLocalToWorld = false;
            bool usesLocalToWorld_it = false;
            bool usesNormal = false;

            if (frankenFormat & ShaderAlgorithms_NormalMap)
            {
                format |= ITKExtension::Model::CONTAINS_NORMAL;
                format |= ITKExtension::Model::CONTAINS_TANGENT;
                format |= ITKExtension::Model::CONTAINS_UV0;

                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "attribute vec3 aNormal;\n"
                                  "attribute vec3 aTangent;\n"
                                  "varying mat3 worldTBN;\n"
                                  "VARIABLES");

                if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
                {
                    findAndReplaceAll(&vertexShader,
                                      "SHADER_CODE",
                                      "  vec3 N = normalize( mat3(uLocalToWorld_it) * ( v_gradient_IT * aNormal ) );\n"
                                      "  vec3 T = normalize( mat3(uLocalToWorld) * ( v_gradient_mat3 * aTangent ) );\n"
                                      // re-orthogonalize T with respect to N
                                      "  T = normalize(T - dot(T, N) * N);\n"
                                      "  vec3 B = cross(T, N);\n"
                                      "  worldTBN = mat3(T,B,N);\n"
                                      //"  gl_Position = uMVP * inputPosition;"
                                      "SHADER_CODE");
                }
                else
                {
                    findAndReplaceAll(&vertexShader,
                                      "SHADER_CODE",
                                      "  vec3 N = normalize( mat3(uLocalToWorld_it) * aNormal );\n"
                                      "  vec3 T = normalize( mat3(uLocalToWorld) * aTangent );\n"
                                      // re-orthogonalize T with respect to N
                                      "  T = normalize(T - dot(T, N) * N);\n"
                                      "  vec3 B = cross(T, N);\n"
                                      "  worldTBN = mat3(T,B,N);\n"
                                      //"  gl_Position = uMVP * inputPosition;"
                                      "SHADER_CODE");
                }

                usesLocalToWorld = true;
                usesLocalToWorld_it = true;
                uses_uv = true;
            }
            else
            {

                usesNormal = ((frankenFormat & (ShaderAlgorithms_AmbientLightSkybox |
                                                ShaderAlgorithms_AmbientLightSpheremap |
                                                ShaderAlgorithms_SunLight0 |
                                                ShaderAlgorithms_SunLight1 |
                                                ShaderAlgorithms_SunLight2 |
                                                ShaderAlgorithms_SunLight3 |
                                                ShaderAlgorithms_ShadowSunLight0 |
                                                ShaderAlgorithms_ShadowSunLight1 |
                                                ShaderAlgorithms_ShadowSunLight2 |
                                                ShaderAlgorithms_ShadowSunLight3)));
                if (usesNormal)
                {
                    format |= ITKExtension::Model::CONTAINS_NORMAL;

                    findAndReplaceAll(&vertexShader,
                                      "VARIABLES",
                                      "attribute vec3 aNormal;\n"
                                      "varying vec3 varyingNormal;\n"
                                      "VARIABLES");

                    if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
                    {
                        findAndReplaceAll(&vertexShader,
                                          "SHADER_CODE",
                                          "  varyingNormal = mat3(uLocalToWorld_it) * ( v_gradient_IT * aNormal );\n"
                                          "SHADER_CODE");
                    }
                    else
                    {
                        findAndReplaceAll(&vertexShader,
                                          "SHADER_CODE",
                                          "  varyingNormal = mat3(uLocalToWorld_it) * aNormal;\n"
                                          "SHADER_CODE");
                    }

                    usesLocalToWorld_it = true;
                }
            }

            bool has_shadow = (frankenFormat & (ShaderAlgorithms_ShadowSunLight0 |
                                                ShaderAlgorithms_ShadowSunLight1 |
                                                ShaderAlgorithms_ShadowSunLight2 |
                                                ShaderAlgorithms_ShadowSunLight3)) != 0;

            if (frankenFormat & (ShaderAlgorithms_SunLight0 |
                                 ShaderAlgorithms_SunLight1 |
                                 ShaderAlgorithms_SunLight2 |
                                 ShaderAlgorithms_SunLight3 |
                                 ShaderAlgorithms_ShadowSunLight0 |
                                 ShaderAlgorithms_ShadowSunLight1 |
                                 ShaderAlgorithms_ShadowSunLight2 |
                                 ShaderAlgorithms_ShadowSunLight3))
            {

                if (has_shadow)
                {
                    findAndReplaceAll(&vertexShader,
                                      "VARIABLES",
                                      "uniform vec3 uCameraPosWorld;\n"
                                      "varying vec3 viewWorld;\n"
                                      "VARIABLES");

                    findAndReplaceAll(&vertexShader,
                                      "SHADER_CODE",
                                      "  vec4 inputPositionWorld = uLocalToWorld * inputPosition;\n"
                                      "  viewWorld = (uCameraPosWorld - inputPositionWorld.xyz);\n"
                                      "SHADER_CODE");

                    // compute all shadow projections...
                    // shadow sun light
                    for (int k = 0; k < 4; k++)
                    {
                        char out_num[8];
                        sprintf(out_num, "%i", k);
                        if (frankenFormat & (ShaderAlgorithms_ShadowSunLight0 << k))
                        {
                            findAndReplaceAll(&vertexShader,
                                              "VARIABLES",
                                              findAndReplaceAllExt(
                                                  "uniform mat4 uShadowProjMatrix_ID;\n"
                                                  "varying vec4 vShadowLight_proj_ID;\n"
                                                  "VARIABLES",
                                                  "_ID", out_num));

                            findAndReplaceAll(&vertexShader,
                                              "SHADER_CODE",
                                              findAndReplaceAllExt(
                                                  "  vShadowLight_proj_ID = uShadowProjMatrix_ID * inputPositionWorld;\n"
                                                  "  vShadowLight_proj_ID = vShadowLight_proj_ID / vShadowLight_proj_ID.w;\n"
                                                  "  vShadowLight_proj_ID = vShadowLight_proj_ID * 0.5 + 0.5;\n"
                                                  "SHADER_CODE",
                                                  "_ID", out_num));
                        }
                    }
                }
                else
                {
                    findAndReplaceAll(&vertexShader,
                                      "VARIABLES",
                                      "uniform vec3 uCameraPosWorld;\n"
                                      "varying vec3 viewWorld;\n"
                                      "VARIABLES");

                    findAndReplaceAll(&vertexShader,
                                      "SHADER_CODE",
                                      "  viewWorld = (uCameraPosWorld - (uLocalToWorld * inputPosition).xyz);\n"
                                      "SHADER_CODE");
                }

                usesLocalToWorld = true;
            }

            if (usesLocalToWorld)
                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "uniform mat4 uLocalToWorld;\n"
                                  "VARIABLES");
            if (usesLocalToWorld_it)
                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "uniform mat4 uLocalToWorld_it;\n"
                                  "VARIABLES");

            if (uses_uv)
            {
                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "attribute vec3 aUV0;\n"
                                  "varying vec2 uv;\n"
                                  "VARIABLES");

                findAndReplaceAll(&vertexShader,
                                  "SHADER_CODE",
                                  "  uv = aUV0.xy;\n"
                                  "SHADER_CODE");
            }

            // close vertex shader
            findAndReplaceAll(&vertexShader,
                              "VARIABLES",
                              "");
            findAndReplaceAll(&vertexShader,
                              "FUNCTIONS",
                              "");
            findAndReplaceAll(&vertexShader,
                              "SHADER_CODE",
                              "  gl_Position = uMVP * inputPosition;\n");

            //
            // Fragment shader mounter
            //

            std::string fragmentShader =
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "uniform vec3 uMaterialAlbedoColor;\n"
                "uniform vec3 uMaterialEmissionColor;\n"
                "uniform float uMaterialRoughness;\n"
                "uniform float uMaterialMetallic;\n"
                "VARIABLES"
                "FUNCTIONS"
                "void main(){\n"
                "  vec4 texel = vec4(0,0,0,1);\n"
                "SHADER_CODE"
                "}";

            if (uses_uv)
            {
                findAndReplaceAll(&fragmentShader,
                                  "VARIABLES",
                                  "varying vec2 uv;\n"
                                  "VARIABLES");

                if (frankenFormat & ShaderAlgorithms_TextureAlbedo)
                {

                    findAndReplaceAll(&fragmentShader,
                                      "VARIABLES",
                                      "uniform sampler2D uTextureAlbedo;\n"
                                      "VARIABLES");

                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  texel = texture2D(uTextureAlbedo, uv);\n"
                                      "SHADER_CODE");
                }

                if (frankenFormat & ShaderAlgorithms_TextureSpecular)
                {
                    findAndReplaceAll(&fragmentShader,
                                      "VARIABLES",
                                      "uniform sampler2D uTextureSpecular;\n"
                                      "VARIABLES");
                }
            }

            if (frankenFormat & ShaderAlgorithms_sRGB)
            {

                findAndReplaceAll(&fragmentShader,
                                  "FUNCTIONS",
                                  "vec3 gammaToLinear(vec3 gamma){\n"
                                  "  return pow(gamma, vec3(2.2));\n"
                                  "}\n"
                                  "vec3 linearToGamma(vec3 linear){\n"
                                  //"  return pow( linear / (linear + vec3(1)) , vec3( 1.0 / 2.2 ) );\n"
                                  "  return pow( linear , vec3( 1.0 / 2.2 ) );\n"
                                  "}\n"
                                  "FUNCTIONS");

                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  texel.rgb = gammaToLinear(texel.rgb);\n"
                                  "SHADER_CODE");
            }

            // forwarded because of the gammaToLinear access...
            if (frankenFormat & ShaderAlgorithms_TextureAlbedo)
            {
                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  texel.rgb *= uMaterialAlbedoColor;\n"
                                  "SHADER_CODE");
            }
            else
            {
                // if dont set the albedo texture, so it is the first assignment
                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  texel.rgb = uMaterialAlbedoColor;\n"
                                  "SHADER_CODE");
            }

            if (frankenFormat & ShaderAlgorithms_TextureSpecular)
            {
                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  float metallic_final = uMaterialMetallic * texture2D(uTextureSpecular, uv).x;\n"
                                  "SHADER_CODE");
            }
            else
            {
                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  float metallic_final = uMaterialMetallic;\n"
                                  "SHADER_CODE");
            }

            if (frankenFormat & ShaderAlgorithms_NormalMap)
            {

                findAndReplaceAll(&fragmentShader,
                                  "VARIABLES",
                                  "varying mat3 worldTBN;\n"
                                  "uniform sampler2D uTextureNormal;\n"
                                  "VARIABLES");

                findAndReplaceAll(&fragmentShader,
                                  "FUNCTIONS",
                                  "vec3 readNormalMap(){\n"
                                  "  vec3 normal = texture2D(uTextureNormal, uv).xyz;\n"
                                  "  normal = normal * 2.0 - 1.0;\n"
                                  "  normal = worldTBN * normal;\n"
                                  "  return clamp( normalize(normal), vec3(-1.0), vec3(1.0));\n"
                                  "}\n"
                                  "FUNCTIONS");

                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  vec3 N = readNormalMap();\n"
                                  "SHADER_CODE");
            }
            else
            {

                if (usesNormal)
                {
                    findAndReplaceAll(&fragmentShader,
                                      "VARIABLES",
                                      "varying vec3 varyingNormal;\n"
                                      "VARIABLES");

                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  vec3 N = clamp( normalize(varyingNormal), vec3(-1.0), vec3(1.0));\n"
                                      "SHADER_CODE");
                }
            }

            if (frankenFormat & (ShaderAlgorithms_SunLight0 |
                                 ShaderAlgorithms_SunLight1 |
                                 ShaderAlgorithms_SunLight2 |
                                 ShaderAlgorithms_SunLight3 |
                                 ShaderAlgorithms_ShadowSunLight0 |
                                 ShaderAlgorithms_ShadowSunLight1 |
                                 ShaderAlgorithms_ShadowSunLight2 |
                                 ShaderAlgorithms_ShadowSunLight3))
            {

                if (shaderPBRAlgorithm == ShaderPBRAlgorithm_Normal)
                {
#if defined(PBR_MODE_LEARN_OPENGL)
                    findAndReplaceAll(&fragmentShader,
                                      "VARIABLES",
                                      "varying vec3 viewWorld;\n"
                                      "VARIABLES");

                    findAndReplaceAll(&fragmentShader,
                                      "FUNCTIONS",
                                      "vec3 fresnelSchlick(float cosTheta, vec3 F0){\n"
                                      "  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);\n"
                                      "}\n"
                                      "float DistributionGGX(vec3 N, vec3 H, float roughness){\n"
                                      "  float a      = roughness*roughness;\n"
                                      "  float a2     = a*a;\n"
                                      "  float NdotH  = max(dot(N, H), 0.0);\n"
                                      "  float NdotH2 = NdotH*NdotH;\n"
                                      "  float num   = a2;\n"
                                      "  float denom = (NdotH2 * (a2 - 1.0) + 1.0);\n"
                                      "  denom = 3.14159265358 * denom * denom;\n" // PI = 3.14159265358
                                      // avoid number overflow (greater than 1.0)
                                      "  return min(num / denom,1.0);\n"
                                      "}\n"
                                      "float GeometrySchlickGGX(float NdotV, float roughness){\n"
                                      "  float r = (roughness + 1.0);\n"
                                      "  float k = (r*r) / 8.0;\n"
                                      "  float num   = NdotV;\n"
                                      "  float denom = NdotV * (1.0 - k) + k;\n"
                                      "  return num / denom;\n"
                                      "}\n"
                                      "float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){\n"
                                      "  float NdotV = max(dot(N, V), 0.0);\n"
                                      "  float NdotL = max(dot(N, L), 0.0);\n"
                                      "  float ggx2  = GeometrySchlickGGX(NdotV, roughness);\n"
                                      "  float ggx1  = GeometrySchlickGGX(NdotL, roughness);\n"
                                      "  return ggx1 * ggx2;\n"
                                      "}\n"
                                      "vec3 computePBR(vec3 albedo, vec3 radiance, vec3 N, vec3 V, vec3 L, vec3 H, float metallic, float clamped_roughness){\n"
                                      // fresnel factor calculation
                                      "  vec3 F0 = vec3(0.04);\n"
                                      "  F0      = mix(F0, albedo, metallic);\n"
                                      "  vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);\n"

                                      // geometry factor
                                      "  float G = GeometrySmith(N, V, L, clamped_roughness);\n"

                                      // normal distribution function
                                      "  float NDF = DistributionGGX(N, H, clamped_roughness);\n"

                                      // combining all factors
                                      "  vec3 numerator    = NDF * G * F;\n"
                                      "  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);\n"
                                      "  vec3 specular     = numerator / max(denominator, 0.001);\n"

                                      "  vec3 kS = F;\n"
                                      "  vec3 kD = vec3(1.0) - kS;\n"

                                      "  kD *= 1.0 - metallic;\n"

                                      "  float NdotL = max(dot(N, L), 0.0);\n"
                                      // 1/PI = 3.18309877326e-01
                                      "  return (kD * albedo * 3.18309877326e-01 + specular) * radiance * NdotL;\n"
                                      "}\n"
                                      "FUNCTIONS");

                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  vec3 V = normalize(viewWorld);\n"
                                      "  vec3 L,H,radiance;\n"
                                      // fix roughness issue on GGX
                                      "  float clamped_roughness = max(uMaterialRoughness, 0.085);\n"
                                      "SHADER_CODE");
#elif defined(PBR_MODE_OPTIMIZED)
                    findAndReplaceAll(&fragmentShader,
                                      "VARIABLES",
                                      "varying vec3 viewWorld;\n"
                                      "VARIABLES");

                    findAndReplaceAll(&fragmentShader,
                                      "FUNCTIONS",
                                      "vec3 fresnelSchlick(float cosTheta, vec3 F0){\n"
                                      "  float pw_5_a = 1.0 - cosTheta;\n"
                                      "  float pw_5 = pw_5_a*pw_5_a;\n"
                                      "  pw_5 = pw_5*pw_5*pw_5_a;\n"
                                      "  return F0 + (1.0 - F0) * pw_5; //pow(1.0 - cosTheta, 5.0);\n"
                                      "}\n"
                                      "float DistributionGGX(float NdotH, float roughness){\n"
                                      "  float a      = roughness*roughness;\n"
                                      "  float a2     = a*a;\n"
                                      //"  float NdotH  = max(dot(N, H), 0.0);\n"
                                      "  float NdotH2 = NdotH*NdotH;\n"
                                      "  float num   = a2;\n"
                                      "  float denom = (NdotH2 * (a2 - 1.0) + 1.0);\n"
                                      "  denom = 3.14159265358 * denom * denom;\n" // PI = 3.14159265358
                                      // avoid number overflow (greater than 1.0)
                                      "  return min(num / denom,1.0);\n"
                                      "}\n"
                                      "float GeometrySchlickGGX(float NdotV, float roughness){\n"
                                      "  float r = (roughness + 1.0);\n"
                                      "  float k = (r*r) / 8.0;\n"
                                      "  float num   = NdotV;\n"
                                      "  float denom = NdotV * (1.0 - k) + k;\n"
                                      "  return num / denom;\n"
                                      "}\n"
                                      "float GeometrySmith(float NdotV, float NdotL, float roughness){\n"
                                      //"  float NdotV = max(dot(N, V), 0.0);\n"
                                      //"  float NdotL = max(dot(N, L), 0.0);\n"
                                      "  float ggx2  = GeometrySchlickGGX(NdotV, roughness);\n"
                                      "  float ggx1  = GeometrySchlickGGX(NdotL, roughness);\n"
                                      "  return ggx1 * ggx2;\n"
                                      "}\n"
                                      "vec3 computePBR(vec3 albedo, vec3 radiance, float NdotV, float NdotH, float NdotL, float HdotV, float metallic, float clamped_roughness){\n"
                                      // fresnel factor calculation
                                      "  vec3 F0 = vec3(0.04);\n"
                                      "  F0      = mix(F0, albedo, metallic);\n"
                                      "  vec3 F  = fresnelSchlick(HdotV, F0);\n"

                                      // geometry factor
                                      "  float G = GeometrySmith(NdotV, NdotL, clamped_roughness);\n"

                                      // normal distribution function
                                      "  float NDF = DistributionGGX(NdotH, clamped_roughness);\n"

                                      // combining all factors
                                      "  vec3 numerator    = NDF * G * F;\n"
                                      "  float denominator = 4.0 * NdotV * NdotL;\n"
                                      "  vec3 specular     = numerator / max(denominator, 0.001);\n"

                                      "  vec3 kS = F;\n"
                                      "  vec3 kD = vec3(1.0) - kS;\n"

                                      "  kD *= 1.0 - metallic;\n"

                                      //"  float NdotL = max(dot(N, L), 0.0);\n"
                                      // 1/PI = 3.18309877326e-01
                                      "  return (kD * albedo * 3.18309877326e-01 + specular) * radiance * NdotL;\n"
                                      "}\n"
                                      "FUNCTIONS");

                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  vec3 V = normalize(viewWorld);\n"
                                      "  float NdotV = max( dot(N,V), 0.0 );\n"
                                      "  float NdotH, NdotL, HdotV;\n"
                                      "  vec3 L,H,radiance;\n"
                                      // fix roughness issue on GGX
                                      "  float clamped_roughness = max(uMaterialRoughness, 0.085);\n"
                                      "SHADER_CODE");
#endif
                }
                else if (shaderPBRAlgorithm == ShaderPBRAlgorithm_TexLookUp)
                {
                    findAndReplaceAll(&fragmentShader,
                                      "VARIABLES",
                                      "varying vec3 viewWorld;\n"
                                      "uniform sampler2D uTexturePBR_F_NDF_G_GHigh;\n"
                                      "VARIABLES");

                    findAndReplaceAll(&fragmentShader,
                                      "FUNCTIONS",
                                      /*
                                      "vec3 fresnelSchlick(float cosTheta, vec3 F0){\n"
                                      "  float pw_5_a = 1.0 - cosTheta;\n"
                                      "  float pw_5 = pw_5_a*pw_5_a;\n"
                                      "  pw_5 = pw_5*pw_5*pw_5_a;\n"
                                      "  return F0 + (1.0 - F0) * pw_5; //pow(1.0 - cosTheta, 5.0);\n"
                                      "}\n"
                                      */
                                      /*"vec3 fresnelSchlick(float cosTheta, vec3 F0){\n"
                                      "  float gray_color = max (max (F0.x, F0.y), F0.z);\n"
                                      "  return F0 * texture2D(uTexturePBR_F_NDF_G_GHigh, vec2(cosTheta, gray_color)).r;\n"
                                      "}\n"*/
                                      "float fresnelSchlick(float cosTheta, float F0){\n"
                                      "  return texture2D(uTexturePBR_F_NDF_G_GHigh, vec2(cosTheta, F0)).r;\n"
                                      "}\n"
                                      "float DistributionGGX(float NdotH, float roughness){\n"
                                      "  return texture2D(uTexturePBR_F_NDF_G_GHigh, vec2(NdotH, roughness)).g;\n"
                                      "}\n"
                                      "float GeometrySchlickGGX(float NdotV, float roughness){\n"
                                      "  return texture2D(uTexturePBR_F_NDF_G_GHigh, vec2(NdotV, roughness)).b;\n"
                                      "}\n"
                                      "float GeometrySmith(float NdotV, float NdotL, float roughness){\n"
                                      "  float ggx2  = GeometrySchlickGGX(NdotV, roughness);\n"
                                      "  float ggx1  = GeometrySchlickGGX(NdotL, roughness);\n"
                                      "  return ggx1 * ggx2;\n"
                                      "}\n"
                                      "vec3 computePBR(vec3 albedo, vec3 radiance, float NdotV, float NdotH, float NdotL, float HdotV, float metallic, float clamped_roughness){\n"
                                      // fresnel factor calculation
                                      //"  vec3 F0 = mix( vec3(0.04), albedo, metallic);\n"
                                      "  float F0 = mix( 0.04, 1.0, metallic);\n"
                                      "  float F  = fresnelSchlick(HdotV, F0);\n"
                                      //"  vec3 F  = fresnelSchlick(HdotV, F0);\n"

                                      // geometry factor
                                      "  float G = GeometrySmith(NdotV, NdotL, clamped_roughness);\n"

                                      // normal distribution function
                                      "  float NDF = DistributionGGX(NdotH, clamped_roughness);\n"

                                      // combining all factors
                                      "  float numerator    = NDF * G * F;\n"
                                      "  float denominator = 4.0 * NdotV * NdotL;\n"
                                      "  float specular     = numerator / max(denominator, 0.001);\n"

                                      "  float kS = F;\n"
                                      "  float kD = 1.0 - kS;\n"

                                      "  kD *= 1.0 - metallic;\n"

                                      //"  float NdotL = max(dot(N, L), 0.0);\n"
                                      // 1/PI = 3.18309877326e-01
                                      "  return (albedo * (kD * 3.18309877326e-01) + vec3(specular)) * radiance * NdotL;\n"
                                      "}\n"
                                      "FUNCTIONS");

                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  vec3 V = normalize(viewWorld);\n"
                                      "  float NdotV = max( dot(N,V), 0.0 );\n"
                                      "  float NdotH, NdotL, HdotV;\n"
                                      "  vec3 L,H,radiance;\n"
                                      // fix roughness issue on GGX
                                      "  float clamped_roughness = max(uMaterialRoughness, 0.085);\n"
                                      "SHADER_CODE");
                }
            }

            findAndReplaceAll(&fragmentShader,
                              "SHADER_CODE",
                              "  vec3 albedo = texel.rgb;\n"
                              "  texel.rgb = vec3(0);\n"
                              "SHADER_CODE");

            /*
            //lighting ambient and sun calculation
            if (frankenFormat & (
                                 ShaderAlgorithms_AmbientLightSkybox |
                                 ShaderAlgorithms_AmbientLightColor |
                                 ShaderAlgorithms_SunLight0 |
                                 ShaderAlgorithms_SunLight1 |
                                 ShaderAlgorithms_SunLight2 |
                                 ShaderAlgorithms_SunLight3 ) ){

                //has light calculation... so start with 0
                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  vec3 albedo = texel.rgb;\n"
                                  "  texel.rgb = vec3(0);\n"
                                  "SHADER_CODE");
            }
            */
            if (frankenFormat & (ShaderAlgorithms_AmbientLightSpheremap | ShaderAlgorithms_AmbientLightSkybox))
            {
                findAndReplaceAll(&fragmentShader,
                                  "FUNCTIONS",
                                  TRIGONOMETRIC_CONSTANTS
                                      TRIGONOMETRIC_LOW_RES_ACOS_ASIN
                                          TRIGONOMETRIC_LOW_RES_ACOS_ASIN_GENTYPE(vec2) "FUNCTIONS");
                if (frankenFormat & ShaderAlgorithms_AmbientLightSpheremap)
                {
                    findAndReplaceAll(&fragmentShader,
                                      "FUNCTIONS",
                                      TRIGONOMETRIC_ATAN
                                      "FUNCTIONS");
                }
            }

            // ambient light
            if (frankenFormat & ShaderAlgorithms_AmbientLightSpheremap)
            {
                frankenFormat &= ~(ShaderAlgorithms_AmbientLightColor | ShaderAlgorithms_AmbientLightSkybox);

                findAndReplaceAll(&fragmentShader,
                                  "VARIABLES",
                                  "uniform sampler2D uTextureSphereAmbient;\n"
                                  "VARIABLES");

                findAndReplaceAll(&fragmentShader,
                                  "FUNCTIONS",
                                  /*"vec3 st2normal(vec2 st) {\n"
                                  "    float phi = st.y * 3.14159265358;// aRibeiro::PI\n"
                                  "    float theta = st.x * 6.28318530716;// 2.0 * aRibeiro::PI;\n"
                                  "    vec3 result;\n"
                                  "    result.x = sin(phi) * cos(theta);\n"
                                  "    result.y = cos(phi);\n"
                                  "    result.z = sin(phi) * sin(theta);\n"
                                  "    return result;\n"
                                  "}\n"*/
                                  "vec2 normal2st(vec3 normal) {\n"
                                  "    float t = fast_acos(normal.y) * 0.318309886; // / 3.14159265358;// PI\n"
                                  "    float s = fast_atan2(normal.z, normal.x) * 0.159154943; // / 6.28318530716;// 2.0 * PI;\n"
                                  "    s = fract(s + 1.0);\n"
                                  "    return vec2(s, t);\n"
                                  "}\n"
                                  "vec3 sampleEnvironmentSpheremap(vec3 normal){\n"
                                  "  vec2 uv = normal2st(normal);\n"
                                  "  uv.y = uv.y * 0.5;\n"
                                  "  return texture2D(uTextureSphereAmbient, uv).rgb;\n"
                                  "}\n"
                                  "FUNCTIONS");

                if (frankenFormat & ShaderAlgorithms_sRGB)
                {
                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  vec3 texelEnvironment = gammaToLinear(sampleEnvironmentSpheremap(N));\n"
                                      "  texel.rgb += albedo * texelEnvironment;\n"
                                      "SHADER_CODE");
                }
                else
                {
                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  vec3 texelEnvironment = sampleEnvironmentSpheremap(N);\n"
                                      "  texel.rgb += albedo * texelEnvironment;\n"
                                      "SHADER_CODE");
                }
            }
            else if (frankenFormat & ShaderAlgorithms_AmbientLightSkybox)
            {

                frankenFormat &= ~ShaderAlgorithms_AmbientLightColor;

                findAndReplaceAll(&fragmentShader,
                                  "VARIABLES",
                                  "uniform samplerCube uTextureCubeAmbient;\n"
                                  "VARIABLES");

                findAndReplaceAll(&fragmentShader,
                                  "FUNCTIONS",
                                  BARYCENTRIC_VEC2
                                  //   "vec3 baricentricCoordvec2(vec2 a, vec2 b, vec2 c, vec2 p){\n"
                                  //   "  vec3 bc = vec3(c-b,0);\n"
                                  //   "  vec3 ba = vec3(a-b,0);\n"
                                  //   "  float areaTriangle_inv = 1.0/cross(bc,ba).z;\n"
                                  //   "  vec3 bp = vec3(p-b,0);\n"
                                  //   "  vec3 uvw;\n"
                                  //   "  uvw.x = cross(bc,bp).z;\n"
                                  //   "  uvw.z = cross(bp,ba).z;\n"
                                  //   "  uvw.xz = uvw.xz * areaTriangle_inv;\n"
                                  //   "  uvw.y = 1.0 - uvw.x - uvw.z;\n"
                                  //   "  return uvw;\n"
                                  //   "}\n"
                                  //   "vec3 sampleEnvironmentCubeBaryLerp(vec3 normal){\n"
                                  //   "  vec3 sign = sign(normal);\n"
                                  //   "  vec3 signX = vec3(sign.x,0,0);\n"
                                  //   "  vec3 signY = vec3(0,sign.y,0);\n"
                                  //   "  vec3 signZ = vec3(0,0,sign.z);\n"
                                  //   "  mat3 tex = mat3(\n"
                                  //   "    textureCube(uTextureCubeAmbient, signX).rgb,\n"
                                  //   "    textureCube(uTextureCubeAmbient, signY).rgb,\n"
                                  //   "    textureCube(uTextureCubeAmbient, signZ).rgb\n"
                                  //   "  );\n"
                                  //   "  vec2 signXxZz = step( sign.xz, vec2(-0.5) );\n"
                                  //   "  signXxZz = signXxZz * 3.14159265359;\n"
                                  //   "  vec2 polarSignX = vec2(1.57079637051,signXxZz.x );\n"
                                  //   "  vec2 polarSignY = vec2(1.57079637051);\n"
                                  //   "  vec2 polarSignZ = vec2(signXxZz.y,1.57079637051);\n"
                                  //   "  vec2 polarNormal = fast_acos(normal.zx);\n"
                                  //   "  vec3 bariCentricCoord = barycentric(\n"
                                  //   "    polarSignX,polarSignY,polarSignZ,polarNormal\n"
                                  //   "  );\n"
                                  //   "  return tex * bariCentricCoord;\n"
                                  //   "}\n"
                                  "vec3 sampleEnvironmentCubeBaryLerp(vec3 normal){\n"
                                  "    vec3 s = sign(normal);\n"
                                  "    vec3 texX = textureCube(uTextureCubeAmbient, vec3(s.x, 0, 0)).rgb;\n"
                                  "    vec3 texY = textureCube(uTextureCubeAmbient, vec3(0, s.y, 0)).rgb;\n"
                                  "    vec3 texZ = textureCube(uTextureCubeAmbient, vec3(0, 0, s.z)).rgb;\n"
                                  "    vec2 flip = step(s.xz, vec2(-0.5)) * 3.14159265;\n"
                                  "    vec2 pX = vec2(1.57079637, flip.x);\n"
                                  "    vec2 pY = vec2(1.57079637);\n"
                                  "    vec2 pZ = vec2(flip.y, 1.57079637);\n"
                                  "    vec2 polarN = fast_acos(normal.zx);\n"
                                  "    vec3 w = barycentric(pX, pY, pZ, polarN);\n"
                                  "    return texX * w.x + texY * w.y + texZ * w.z;\n"
                                  "}\n"
                                  "FUNCTIONS");

                if (frankenFormat & ShaderAlgorithms_sRGB)
                {
                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  vec3 texelEnvironment = gammaToLinear(sampleEnvironmentCubeBaryLerp(N));\n"
                                      "  texel.rgb += albedo * texelEnvironment;\n"
                                      "SHADER_CODE");
                }
                else
                {
                    findAndReplaceAll(&fragmentShader,
                                      "SHADER_CODE",
                                      "  vec3 texelEnvironment = sampleEnvironmentCubeBaryLerp(N);\n"
                                      "  texel.rgb += albedo * texelEnvironment;\n"
                                      "SHADER_CODE");
                }
            }
            else if (frankenFormat & ShaderAlgorithms_AmbientLightColor)
            {
                findAndReplaceAll(&fragmentShader,
                                  "VARIABLES",
                                  "uniform vec3 uAmbientColor;\n"
                                  "VARIABLES");

                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  texel.rgb += albedo * uAmbientColor;\n"
                                  //"  texel.rgb += albedo * pow(uAmbientColor, vec3(2.2));\n"
                                  "SHADER_CODE");
            }

            bool use_optimized_pbr;
#if defined(PBR_MODE_LEARN_OPENGL)
            use_optimized_pbr = false;
#elif defined(PBR_MODE_OPTIMIZED)
            use_optimized_pbr = true;
#else
#error "PBR Macro Definition Error..."
#endif

            if (shaderPBRAlgorithm == ShaderPBRAlgorithm_TexLookUp)
                use_optimized_pbr = true;

            // sun light
            for (int k = 0; k < 4; k++)
            {
                char out_num[8];
                sprintf(out_num, "%i", k);
                if (frankenFormat & (ShaderAlgorithms_SunLight0 << k))
                {
                    findAndReplaceAll(&fragmentShader,
                                      "VARIABLES",
                                      findAndReplaceAllExt(
                                          "uniform vec3 uLightRadiance_ID;\n"
                                          "uniform vec3 uLightDir_ID;\n"
                                          "VARIABLES",
                                          "_ID", out_num));

                    if (!use_optimized_pbr)
                    {

                        findAndReplaceAll(&fragmentShader,
                                          "SHADER_CODE",
                                          findAndReplaceAllExt(
                                              "  L = -uLightDir_ID;\n"
                                              "  H = normalize(V + L);\n"
                                              "  radiance = uLightRadiance_ID;\n"
                                              // #if defined(PBR_MODE_LEARN_OPENGL)
                                              "  texel.rgb += computePBR(albedo,radiance,N,V,L,H,metallic_final,clamped_roughness);\n"
                                              "SHADER_CODE",
                                              "_ID", out_num));
                    }
                    else
                    {
                        findAndReplaceAll(&fragmentShader,
                                          "SHADER_CODE",
                                          findAndReplaceAllExt(
                                              "  L = -uLightDir_ID;\n"
                                              "  H = normalize(V + L);\n"
                                              "  radiance = uLightRadiance_ID;\n"
                                              "  NdotH = max(dot(N,H),0.0);\n"
                                              "  NdotL = max(dot(N,L),0.0);\n"
                                              "  HdotV = max(dot(H,V),0.0);\n"
                                              "  texel.rgb += computePBR(albedo, radiance, NdotV, NdotH, NdotL, HdotV, metallic_final, clamped_roughness);\n"
                                              "SHADER_CODE",
                                              "_ID", out_num));
                    }
                }
            }

            if (has_shadow)
            {
                if (shaderShadowAlgorithm == ShaderShadowAlgorithm_PCSS_PCF ||
                    shaderShadowAlgorithm == ShaderShadowAlgorithm_PCSS_DST_CENTER)
                {
                    findAndReplaceAll(&fragmentShader,
                                      "VARIABLES",
                                      "uniform vec2 shadow_spread_coefs[32];\n"
                                      "VARIABLES");

                    findAndReplaceAll(&fragmentShader,
                                      "FUNCTIONS",
                                      /*
                                      "vec3 find_blocker(float fragment_depth, sampler2D shadowMap, vec2 uv, vec2 worldDimension_inv, float worldRadius) {\n"
                                      "  vec2 scaler = worldDimension_inv * worldRadius;\n"
                                      "  float blockerSum = 0;\n"
                                      "  float blockerCount = 0;\n"
                                      "  for (int i = 0; i < 32; i++) { \n"
                                      "    vec2 access = uv + shadow_spread_coefs[i] * scaler;\n"
                                      "    float sampled_depth = texture2D(shadowMap, access).r;\n"
                                      "    if ( sampled_depth < fragment_depth ) {\n"
                                      "      blockerSum += sampled_depth;\n"
                                      "      blockerCount += 1.0;\n"
                                      "    }\n"
                                      "  }\n"
                                      "  float avgBlockerDepth = blockerSum/blockerCount;\n"
                                      "  return vec3(avgBlockerDepth, blockerCount, blockerCount / 32.0 );\n"
                                      "}\n"*/
                                      "float disc_area(float r) {\n"
                                      "  return 3.14159265358 * r * r;"
                                      "}\n"
                                      "float smart_blocker_sampler_pcf(float fragmentDepth_01, sampler2D shadowMap, vec2 shadowUV, vec3 textureToWorldScaler, vec3 worldToTextureScaler, float coneCos, float coneTangent) {\n"
                                      //"  return float(texture2D(shadowMap, shadowUV).r < fragmentDepth_01);\n"
                                      //"  vec3 fragmentWorld = vec3( 0, 0, max(fragmentDepth_01,0.00001) * textureToWorldScaler.z );\n"
                                      // estimative in a sphere of radius 5
                                      "  float radiusAtTopWorld = coneTangent * 5.0;\n"
                                      "  vec2 radiusTexture = worldToTextureScaler.xy * radiusAtTopWorld;\n"
                                      "  float valid_samples = 1.0;\n"
                                      "  for (int i = 0; i < 32; i+=2) { \n"
                                      "    vec2 textureOffset = shadow_spread_coefs[i] * radiusTexture;\n"
                                      "    float shadowDepth_01 = texture2D(shadowMap, shadowUV + textureOffset).r;\n"
                                      //"    vec3 shadowPtnWorld = vec3( textureOffset, shadowDepth_01 ) * textureToWorldScaler;\n"
                                      //"    vec3 frag2shadow = shadowPtnWorld - fragmentWorld;\n"
                                      //"    vec3 frag2shadow_unit = normalize(frag2shadow);\n"
                                      "    valid_samples -= float( shadowDepth_01 < fragmentDepth_01 ) * (1.0/16.0);\n"
                                      "  }\n"

                                      //"  valid_samples = clamp(valid_samples, 0.0, 1.0);\n"
                                      //"  return 1.0 - valid_samples;\n"
                                      "  return valid_samples;\n"

                                      "}\n"
                                      "float hermite(float edge0, float edge1, float x) {\n"
                                      "  float t;\n" // Or genDType t;
                                      "  t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);\n"
                                      "  return t * t * (3.0 - 2.0 * t);\n"
                                      "}\n"

                                      "float smart_blocker_sampler_inigo_quilez(float fragmentDepth_01, sampler2D shadowMap, vec2 shadowUV, vec3 textureToWorldScaler, vec3 worldToTextureScaler, float coneCos, float coneTangent) {\n"
                                      // estimative in a sphere of radius 5
                                      "  float radiusAtTopWorld = coneTangent * 5.0;\n"
                                      "  vec2 radiusTexture = worldToTextureScaler.xy * radiusAtTopWorld;\n"
                                      "  float shadowEdgeDistanceToCenter = radiusAtTopWorld;\n"
                                      "  float sampledDepth = fragmentDepth_01;\n"
                                      "  for (int i = 0; i < 32; i++) { \n"
                                      "    vec2 textureOffset = shadow_spread_coefs[i] * radiusTexture;\n"
                                      "    vec2 worldOffset = textureOffset * textureToWorldScaler.xy;\n"
                                      "    float worldOffsetLength = length(worldOffset);\n"
                                      "    float shadowDepth_01 = texture2D(shadowMap, shadowUV + textureOffset).r;\n"
                                      // valid blocker...
                                      "    if (shadowDepth_01 < fragmentDepth_01 && worldOffsetLength < shadowEdgeDistanceToCenter ) {\n"
                                      //"      shadowEdgeDistanceToCenter = min(shadowEdgeDistanceToCenter, worldOffsetLength);\n"
                                      "      shadowEdgeDistanceToCenter = worldOffsetLength;\n"
                                      "      sampledDepth = shadowDepth_01;"
                                      "    }\n"
                                      //"    shadowEdgeDistanceToCenter = mix(shadowEdgeDistanceToCenter, min(shadowEdgeDistanceToCenter, worldOffsetLength), float(shadowDepth_01 < fragmentDepth_01));\n"
                                      "  }\n"

                                      "  float shadow_weight = shadowEdgeDistanceToCenter / radiusAtTopWorld;\n"

                                      //"  float depth_scaler = (fragmentDepth_01 - sampledDepth) * textureToWorldScaler.z;\n"
                                      //"  depth_scaler = depth_scaler * (1.0/10.0);\n"//10 meters to change the shadow blur
                                      //"  depth_scaler = min( 1.0, depth_scaler );\n"
                                      //"  shadow_weight = 1.0 - (1.0 -shadow_weight) * (1.0 -depth_scaler);\n"

                                      "  shadow_weight = hermite(0, 1, shadow_weight);\n"

                                      "  return shadow_weight;\n"
                                      "}\n"

                                      "vec2 pcss_find_blocker(bool returnOnFirstDistance, float fragmentDepth_01, sampler2D shadowMap, vec2 shadowUV, float radiusAtTopWorld, vec3 textureToWorldScaler, vec3 worldToTextureScaler) {\n"
                                      // estimative in a sphere of radius 5
                                      "  vec2 radiusTexture = worldToTextureScaler.xy * radiusAtTopWorld;\n"
                                      "  float shadowEdgeDistanceToCenter = radiusAtTopWorld;\n"
                                      //"  float sampledDepth = fragmentDepth_01;\n"
                                      "  float sampledDepth = 0.0;\n"
                                      "  float sampledDepthCount = 0.0;\n"
                                      "  for (int i = 0; i < 32; i++) { \n"
                                      "    vec2 textureOffset = shadow_spread_coefs[i] * radiusTexture;\n"
                                      "    vec2 worldOffset = textureOffset * textureToWorldScaler.xy;\n"
                                      "    float worldOffsetLength = length(worldOffset);\n"
                                      "    float shadowDepth_01 = texture2D(shadowMap, shadowUV + textureOffset).r;\n"
                                      // valid blocker...

                                      //"    if (shadowDepth_01 < fragmentDepth_01 && worldOffsetLength < shadowEdgeDistanceToCenter ) {\n"
                                      "    if ( shadowDepth_01 < fragmentDepth_01 ) {\n"
                                      "      if (returnOnFirstDistance)\n"
                                      "        return vec2(worldOffsetLength, 0.0);\n"
                                      "      shadowEdgeDistanceToCenter = min(shadowEdgeDistanceToCenter, worldOffsetLength);\n"
                                      "      sampledDepth = sampledDepth + shadowDepth_01;\n"
                                      "      sampledDepthCount = sampledDepthCount+1.0;\n"
                                      //"      if (shadowEdgeDistanceToCenter <= 0.00001)\n"
                                      //"        break;\n"
                                      "    }\n"

                                      //"    float selector = float(shadowDepth_01 < fragmentDepth_01 && worldOffsetLength < shadowEdgeDistanceToCenter);"
                                      //"    shadowEdgeDistanceToCenter = mix(shadowEdgeDistanceToCenter, worldOffsetLength, selector);\n"
                                      //"    sampledDepth = mix(sampledDepth, shadowDepth_01, selector);\n"
                                      "  }\n"
                                      "  if( sampledDepthCount == 0.0)\n"
                                      "    sampledDepth = fragmentDepth_01;\n"
                                      "  else\n"
                                      "    sampledDepth = sampledDepth / sampledDepthCount;\n"
                                      "  return vec2(shadowEdgeDistanceToCenter, sampledDepth);\n"
                                      //"  return sampledDepth;\n"
                                      "}\n"

                                      "float smart_blocker_sampler_pcss_pcf(float fragmentDepth_01, sampler2D shadowMap, vec2 shadowUV, vec3 textureToWorldScaler, vec3 worldToTextureScaler, float coneCos, float coneTangent) {\n"
                                      // estimative in a sphere of radius 5
                                      "  float radiusAtTopWorld = coneTangent * 5.0;\n"
                                      "  vec2 blocker_search_result = pcss_find_blocker(false, fragmentDepth_01, shadowMap, shadowUV, radiusAtTopWorld, textureToWorldScaler, worldToTextureScaler);\n"
                                      "  float distance_to_center = blocker_search_result.x;\n"

                                      "  if (distance_to_center >= radiusAtTopWorld - 0.00001)\n"
                                      "    return 1.0;\n"
                                      "  else {\n"

                                      "  float blocker_depth = blocker_search_result.y;\n"

                                      "  float depth_scaler = (fragmentDepth_01 - blocker_depth) * textureToWorldScaler.z;\n"
                                      "  depth_scaler = depth_scaler * (0.9/10.0);\n"      // 10 meters to change the shadow blur
                                      "  depth_scaler = min( 0.9, depth_scaler ) + 0.1;\n" // avoid division by zero

                                      // sample Distance to center
                                      //"  if (distance_to_center <= 0.00001)\n"
                                      //"    return 0.0;\n"
                                      //"  radiusAtTopWorld = radiusAtTopWorld * depth_scaler * 1.25;\n"
                                      //"  float blocker_distance_to_center = pcss_find_blocker(true, fragmentDepth_01, shadowMap, shadowUV, radiusAtTopWorld, textureToWorldScaler, worldToTextureScaler).x;\n"
                                      //"  float shadow_weight = blocker_distance_to_center / radiusAtTopWorld;\n"
                                      //"  shadow_weight = hermite(0, 1, shadow_weight);\n"

                                      "  float shadow_weight = smart_blocker_sampler_pcf(fragmentDepth_01, shadowMap, shadowUV, textureToWorldScaler, worldToTextureScaler, coneCos, coneTangent * depth_scaler);\n"

                                      "  return shadow_weight;\n"
                                      "  }\n"
                                      "}\n"

                                      "float smart_blocker_sampler_pcss_dst_center(float fragmentDepth_01, sampler2D shadowMap, vec2 shadowUV, vec3 textureToWorldScaler, vec3 worldToTextureScaler, float coneCos, float coneTangent) {\n"
                                      // estimative in a sphere of radius 5
                                      "  float radiusAtTopWorld = coneTangent * 5.0;\n"
                                      "  vec2 blocker_search_result = pcss_find_blocker(false, fragmentDepth_01, shadowMap, shadowUV, radiusAtTopWorld, textureToWorldScaler, worldToTextureScaler);\n"
                                      "  float distance_to_center = blocker_search_result.x;\n"

                                      "  if (distance_to_center >= radiusAtTopWorld - 0.00001)\n"
                                      "    return 1.0;\n"
                                      "  else {\n"

                                      "  float blocker_depth = blocker_search_result.y;\n"

                                      "  float depth_scaler = (fragmentDepth_01 - blocker_depth) * textureToWorldScaler.z;\n"
                                      "  depth_scaler = depth_scaler * (0.9/10.0);\n"      // 10 meters to change the shadow blur
                                      "  depth_scaler = min( 0.9, depth_scaler ) + 0.1;\n" // avoid division by zero

                                      // sample Distance to center
                                      "  if (distance_to_center <= 0.00001)\n"
                                      "    return 0.0;\n"
                                      "  radiusAtTopWorld = radiusAtTopWorld * depth_scaler * 1.25;\n"
                                      "  float blocker_distance_to_center = pcss_find_blocker(true, fragmentDepth_01, shadowMap, shadowUV, radiusAtTopWorld, textureToWorldScaler, worldToTextureScaler).x;\n"
                                      "  float shadow_weight = blocker_distance_to_center / radiusAtTopWorld;\n"
                                      "  shadow_weight = hermite(0, 1, shadow_weight);\n"

                                      //"  float shadow_weight = smart_blocker_sampler_pcf(fragmentDepth_01, shadowMap, shadowUV, textureToWorldScaler, worldToTextureScaler, coneCos, coneTangent * depth_scaler);\n"

                                      "  return shadow_weight;\n"
                                      "  }\n"
                                      "}\n"

                                      "FUNCTIONS");
                    // blocker  wlight
                    // sub      wpenumbra
                    // https://developer.download.nvidia.com/shaderlibrary/docs/shadow_PCSS.pdf
                }
                findAndReplaceAll(&fragmentShader,
                                  "FUNCTIONS",
                                  "mat2 inv_mat2(mat2 m) {\n"
                                  "  float m00 = m[0][0], m01 = m[0][1];\n"
                                  "  float m10 = m[1][0], m11 = m[1][1];\n"
                                  "  float det_inv = 1.0 / (m00*m11 - m01*m10);\n"
                                  "  return mat2(m11,-m01,\n"
                                  "             -m10, m00) * det_inv;\n"
                                  "}\n"
                                  "mat2 shadow_to_screen_mat2(vec3 shadow_ddx, vec3 shadow_ddy) {\n"
                                  "  mat2 matScreenToShadow = mat2( shadow_ddx.xy, shadow_ddy.xy );\n"
                                  "  mat2 matShadowToScreen = inv_mat2(matScreenToShadow);\n"
                                  "  return matShadowToScreen;\n"
                                  "}\n"
                                  "FUNCTIONS");
            }

            // shadow sun light
            for (int k = 0; k < 4; k++)
            {
                char out_num[8];
                sprintf(out_num, "%i", k);
                if (frankenFormat & (ShaderAlgorithms_ShadowSunLight0 << k))
                {

                    if (shaderShadowAlgorithm == ShaderShadowAlgorithm_PCSS_PCF ||
                        shaderShadowAlgorithm == ShaderShadowAlgorithm_PCSS_DST_CENTER)
                    {
                        findAndReplaceAll(&fragmentShader,
                                          "VARIABLES",
                                          findAndReplaceAllExt(
                                              "uniform vec3 uShadowLightRadiance_ID;\n"
                                              "uniform vec3 uShadowLightDir_ID;\n"
                                              "uniform vec3 uShadowDimension_ID;\n"
                                              "uniform vec3 uShadowDimension_inv_ID;\n"
                                              "uniform vec3 uShadowCone_ID;\n" // cos, sin, tan
                                              "uniform sampler2D uShadowTextureDepth_ID;\n"
                                              "varying vec4 vShadowLight_proj_ID;\n"
                                              "VARIABLES",
                                              "_ID", out_num));
                    }
                    else
                    {
                        findAndReplaceAll(&fragmentShader,
                                          "VARIABLES",
                                          findAndReplaceAllExt(
                                              "uniform vec3 uShadowLightRadiance_ID;\n"
                                              "uniform vec3 uShadowLightDir_ID;\n"
                                              "uniform sampler2DShadow uShadowTextureDepth_ID;\n"
                                              "varying vec4 vShadowLight_proj_ID;\n"
                                              "VARIABLES",
                                              "_ID", out_num));
                    }

                    if (shaderShadowAlgorithm == ShaderShadowAlgorithm_Basic || shaderShadowAlgorithm == ShaderShadowAlgorithm_2x2PCF)
                    {

                        findAndReplaceAll(&fragmentShader,
                                          "SHADER_CODE",
                                          findAndReplaceAllExt(

                                              //"  float shadow_depth_ID = texture2D(uShadowTextureDepth_ID, vShadowLight_proj_ID.xy).r;\n"
                                              //"  shadow_depth_ID = step(vShadowLight_proj_ID.z, shadow_depth_ID);\n"

                                              // basic / 2x2 PCF
                                              "  float shadow_depth_ID = shadow2D(uShadowTextureDepth_ID, vShadowLight_proj_ID.xyz).r;\n"
                                              "SHADER_CODE",
                                              "_ID", out_num));
                    }
                    else if (shaderShadowAlgorithm == ShaderShadowAlgorithm_4x4PCF)
                    {

                        findAndReplaceAll(&fragmentShader,
                                          "SHADER_CODE",
                                          findAndReplaceAllExt(

                                              // 4x4 PCF naive
                                              "  vec3 shadow_coord_ID_3d = vShadowLight_proj_ID.xyz;\n"
                                              "  float shadow_texel_size_ID = 1.0/2048.0;\n"
                                              "  vec4 shadow_samples_ID;\n"
                                              "  shadow_samples_ID.x = shadow2D(uShadowTextureDepth_ID, shadow_coord_ID_3d + vec3( -shadow_texel_size_ID, -shadow_texel_size_ID, 0 ) ).r;\n"
                                              "  shadow_samples_ID.y = shadow2D(uShadowTextureDepth_ID, shadow_coord_ID_3d + vec3(  shadow_texel_size_ID, -shadow_texel_size_ID, 0 ) ).r;\n"
                                              "  shadow_samples_ID.z = shadow2D(uShadowTextureDepth_ID, shadow_coord_ID_3d + vec3(  shadow_texel_size_ID,  shadow_texel_size_ID, 0 ) ).r;\n"
                                              "  shadow_samples_ID.w = shadow2D(uShadowTextureDepth_ID, shadow_coord_ID_3d + vec3( -shadow_texel_size_ID,  shadow_texel_size_ID, 0 ) ).r;\n"
                                              "  float shadow_depth_ID = dot( shadow_samples_ID, vec4(0.25) );\n"
                                              //*/

                                              /*
                                              // 4x4 PCF With depth offset derivatives
                                              "  vec3 shadow_coord_ID_3d = vShadowLight_proj_ID.xyz;\n"
                                              "  float shadow_texel_size_ID = 1.0/2048.0;\n"

                                              "  vec3 shadow_ddx_ID = dFdx(shadow_coord_ID_3d);\n"
                                              "  vec3 shadow_ddy_ID = dFdy(shadow_coord_ID_3d);\n"
                                              "  mat2 shadowToScreen_ID = shadow_to_screen_mat2(shadow_ddx_ID, shadow_ddy_ID);\n"
                                              "  vec2 shadow_texel_right_ID = shadowToScreen_ID * vec2( shadow_texel_size_ID, 0.0 );\n"
                                              "  vec2 shadow_texel_up_ID = shadowToScreen_ID * vec2( 0.0, shadow_texel_size_ID );\n"
                                              "  vec2 shadow_ddx_ddy_z_ID = vec2(shadow_ddx_ID.z,shadow_ddy_ID.z);\n"
                                              "  float rightTextelDepthDelta_ID = dot(shadow_texel_right_ID, shadow_ddx_ddy_z_ID);\n"
                                              "  float upTextelDepthDelta_ID = dot(shadow_texel_up_ID, shadow_ddx_ddy_z_ID);\n"

                                              "  vec4 shadow_samples_ID;\n"
                                              "  shadow_samples_ID.x = shadow2D(uShadowTextureDepth_ID, shadow_coord_ID_3d + vec3( -shadow_texel_size_ID, -shadow_texel_size_ID, -rightTextelDepthDelta_ID - upTextelDepthDelta_ID ) ).r;\n"
                                              "  shadow_samples_ID.y = shadow2D(uShadowTextureDepth_ID, shadow_coord_ID_3d + vec3(  shadow_texel_size_ID, -shadow_texel_size_ID,  rightTextelDepthDelta_ID - upTextelDepthDelta_ID ) ).r;\n"
                                              "  shadow_samples_ID.z = shadow2D(uShadowTextureDepth_ID, shadow_coord_ID_3d + vec3(  shadow_texel_size_ID,  shadow_texel_size_ID,  rightTextelDepthDelta_ID + upTextelDepthDelta_ID ) ).r;\n"
                                              "  shadow_samples_ID.w = shadow2D(uShadowTextureDepth_ID, shadow_coord_ID_3d + vec3( -shadow_texel_size_ID,  shadow_texel_size_ID, -rightTextelDepthDelta_ID + upTextelDepthDelta_ID ) ).r;\n"
                                              "  float shadow_depth_ID = dot( shadow_samples_ID, vec4(0.25) );\n"
                                              //*/

                                              "SHADER_CODE",
                                              "_ID", out_num));
                    }
                    else if (shaderShadowAlgorithm == ShaderShadowAlgorithm_PCSS_PCF)
                    {

                        findAndReplaceAll(&fragmentShader,
                                          "SHADER_CODE",
                                          findAndReplaceAllExt(
                                              "  float shadow_depth_ID = smart_blocker_sampler_pcss_pcf(vShadowLight_proj_ID.z, uShadowTextureDepth_ID, vShadowLight_proj_ID.xy, uShadowDimension_ID, uShadowDimension_inv_ID, uShadowCone_ID.x, uShadowCone_ID.z);\n"
                                              "SHADER_CODE",
                                              "_ID", out_num));
                    }
                    else if (shaderShadowAlgorithm == ShaderShadowAlgorithm_PCSS_DST_CENTER)
                    {

                        findAndReplaceAll(&fragmentShader,
                                          "SHADER_CODE",
                                          findAndReplaceAllExt(
                                              "  float shadow_depth_ID = smart_blocker_sampler_pcss_dst_center(vShadowLight_proj_ID.z, uShadowTextureDepth_ID, vShadowLight_proj_ID.xy, uShadowDimension_ID, uShadowDimension_inv_ID, uShadowCone_ID.x, uShadowCone_ID.z);\n"
                                              "SHADER_CODE",
                                              "_ID", out_num));
                    }

                    if (!use_optimized_pbr)
                    {
                        findAndReplaceAll(&fragmentShader,
                                          "SHADER_CODE",
                                          findAndReplaceAllExt(
                                              "  L = -uShadowLightDir_ID;\n"
                                              "  H = normalize(V + L);\n"
                                              "  radiance = uShadowLightRadiance_ID;\n"
                                              // #if defined(PBR_MODE_LEARN_OPENGL)
                                              "  texel.rgb += computePBR(albedo,radiance,N,V,L,H,metallic_final,clamped_roughness) * shadow_depth_ID;\n"
                                              "SHADER_CODE",
                                              "_ID", out_num));
                    }
                    else
                    {
                        findAndReplaceAll(&fragmentShader,
                                          "SHADER_CODE",
                                          findAndReplaceAllExt(
                                              "  L = -uShadowLightDir_ID;\n"
                                              "  H = normalize(V + L);\n"
                                              "  radiance = uShadowLightRadiance_ID;\n"
                                              "  NdotH = max(dot(N,H),0.0);\n"
                                              "  NdotL = max(dot(N,L),0.0);\n"
                                              "  HdotV = max(dot(H,V),0.0);\n"
                                              "  texel.rgb += computePBR(albedo, radiance, NdotV, NdotH, NdotL, HdotV, metallic_final, clamped_roughness) * shadow_depth_ID;\n"
                                              "SHADER_CODE",
                                              "_ID", out_num));
                    }
                }
            }

            if (frankenFormat & ShaderAlgorithms_TextureEmission)
            {

                findAndReplaceAll(&fragmentShader,
                                  "VARIABLES",
                                  "uniform sampler2D uTextureEmission;\n"
                                  "VARIABLES");

                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  texel.rgb += uMaterialEmissionColor * texture2D(uTextureEmission, uv).rgb;\n"
                                  "SHADER_CODE");
            }
            else
            {
                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  texel.rgb += uMaterialEmissionColor;\n"
                                  "SHADER_CODE");
            }

            //
            // Tone Mapping Test
            //

            /*
            findAndReplaceAll(&fragmentShader,
                "SHADER_CODE",
                "  // reinhard tone mapping\n"
                "  texel.rgb = texel.rgb / (texel.rgb + vec3(1.0));\n"
                "SHADER_CODE");
            // */

            /*
            findAndReplaceAll(&fragmentShader,
                "SHADER_CODE",
                "  // exposure tone mapping\n"
                "  float exposure = 0.1;" // 0.1, 1.0, 5.0
                "  texel.rgb = vec3(1.0) - exp(-texel.rgb * exposure);\n"
                "SHADER_CODE");
            // */

            // primeiro caso o parametro �: cor inicial + exposure
            // calculo cor final
            // CorFinal = 1 - e^( -corInicial * exposure );

            // segundo caso o parametro �: corfinal + cor inicial
            // calculo exposure

            // -CorFinal + 1 = e^( -corInicial * exposure );
            // log(-CorFinal + 1) = log(e^( -corInicial * exposure ));
            // log(-CorFinal + 1) = -corInicial * exposure;
            // log(-CorFinal + 1)/-corInicial =  exposure;

            if (frankenFormat & ShaderAlgorithms_sRGB)
            {
                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  texel.rgb = linearToGamma(texel.rgb);\n"
                                  "SHADER_CODE");
            }

            // close fragment shader
            findAndReplaceAll(&fragmentShader,
                              "VARIABLES",
                              "");
            findAndReplaceAll(&fragmentShader,
                              "FUNCTIONS",
                              "");
            findAndReplaceAll(&fragmentShader,
                              "SHADER_CODE",
                              "  gl_FragColor = texel;\n");

            printf("Compiling FrankenShader\n");
            printf("--------------------------\n");
            printf("  textureAlbedo: %i\n", (frankenFormat & ShaderAlgorithms_TextureAlbedo) != 0);
            printf("  normalMap: %i\n", (frankenFormat & ShaderAlgorithms_NormalMap) != 0);
            printf("  ambientLightColor: %i\n", (frankenFormat & ShaderAlgorithms_AmbientLightColor) != 0);
            printf("  ambientLightSkybox: %i\n", (frankenFormat & ShaderAlgorithms_AmbientLightSkybox) != 0);
            printf("  sunLight0: %i\n", (frankenFormat & ShaderAlgorithms_SunLight0) != 0);
            printf("  sunLight1: %i\n", (frankenFormat & ShaderAlgorithms_SunLight1) != 0);
            printf("  sunLight2: %i\n", (frankenFormat & ShaderAlgorithms_SunLight2) != 0);
            printf("  sunLight3: %i\n", (frankenFormat & ShaderAlgorithms_SunLight3) != 0);
            printf("  ShadowSunLight0: %i\n", (frankenFormat & ShaderAlgorithms_ShadowSunLight0) != 0);
            printf("  ShadowSunLight1: %i\n", (frankenFormat & ShaderAlgorithms_ShadowSunLight1) != 0);
            printf("  ShadowSunLight2: %i\n", (frankenFormat & ShaderAlgorithms_ShadowSunLight2) != 0);
            printf("  ShadowSunLight3: %i\n", (frankenFormat & ShaderAlgorithms_ShadowSunLight3) != 0);
            printf("  skinGradient16: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_16) != 0);
            printf("  skinGradient32: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_32) != 0);
            printf("  skinGradient64: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_64) != 0);
            printf("  skinGradient96: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_96) != 0);
            printf("  skinGradient128: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_128) != 0);
            printf("  sRGB: %i\n", (frankenFormat & ShaderAlgorithms_sRGB) != 0);
            printf("VERTEX\n");
            printf("%s\n\n", vertexShader.c_str());
            printf("FRAGMENT\n");
            printf("%s\n\n", fragmentShader.c_str());

            compile(vertexShader.c_str(), fragmentShader.c_str(), __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            queryShaderUniforms();

            frankenUniformManager.readUniformsFromShaderAndInitStatic();
        }

        void FrankenShader::queryShaderUniforms()
        {
            //"uniform mat4 uMVP;\n"
            u_mvp = getUniformLocation("uMVP");

            //"uniform vec3 uCameraPosWorld;\n"
            u_cameraPosWorld = getUniformLocation("uCameraPosWorld");

            //"uniform sampler2D uTextureAlbedo;\n"
            //"uniform vec4 uMaterialAlbedoColor;\n"
            //"uniform float uMaterialRoughness;\n"
            //"uniform float uMaterialMetallic;\n"
            PBR.u_textureAlbedo = getUniformLocation("uTextureAlbedo");
            PBR.u_textureSpecular = getUniformLocation("uTextureSpecular");
            PBR.u_textureEmission = getUniformLocation("uTextureEmission");
            PBR.u_TexturePBR_F_NDF_G_GHigh = getUniformLocation("uTexturePBR_F_NDF_G_GHigh"); // new
            PBR.u_materialAlbedoColor = getUniformLocation("uMaterialAlbedoColor");
            PBR.u_materialEmissionColor = getUniformLocation("uMaterialEmissionColor");
            PBR.u_materialRoughness = getUniformLocation("uMaterialRoughness");
            PBR.u_materialMetallic = getUniformLocation("uMaterialMetallic");

            //"uniform mat4 uLocalToWorld;\n"
            //"uniform mat4 uLocalToWorld_it;\n"
            //"uniform sampler2D uTextureNormal;\n"
            normalMap.u_localToWorld = getUniformLocation("uLocalToWorld");
            normalMap.u_localToWorld_it = getUniformLocation("uLocalToWorld_it");
            normalMap.u_textureNormal = getUniformLocation("uTextureNormal");

            //"uniform samplerCube uTextureCubeAmbient;\n"
            //"uniform vec3 uAmbientColor;\n"
            ambientLight.u_textureCubeAmbient = getUniformLocation("uTextureCubeAmbient");
            ambientLight.u_textureSphereAmbient = getUniformLocation("uTextureSphereAmbient");
            ambientLight.u_ambientColor = getUniformLocation("uAmbientColor");

            //"uniform vec3 uLightRadiance0;\n"
            //"uniform vec3 uLightDir0;\n"
            //"uniform vec3 uLightRadiance1;\n"
            //"uniform vec3 uLightDir1;\n"
            //"uniform vec3 uLightRadiance2;\n"
            //"uniform vec3 uLightDir2;\n"
            //"uniform vec3 uLightRadiance3;\n"
            //"uniform vec3 uLightDir3;\n"
            sunLight[0].u_lightRadiance = getUniformLocation("uLightRadiance0");
            sunLight[0].u_lightDir = getUniformLocation("uLightDir0");
            sunLight[1].u_lightRadiance = getUniformLocation("uLightRadiance1");
            sunLight[1].u_lightDir = getUniformLocation("uLightDir1");
            sunLight[2].u_lightRadiance = getUniformLocation("uLightRadiance2");
            sunLight[2].u_lightDir = getUniformLocation("uLightDir2");
            sunLight[3].u_lightRadiance = getUniformLocation("uLightRadiance3");
            sunLight[3].u_lightDir = getUniformLocation("uLightDir3");

            for (int i = 0; i < 4; i++)
            {
                char out_num[8];
                sprintf(out_num, "%i", i);

                shadowSunLight[i].u_lightRadiance = getUniformLocation(findAndReplaceAllExt("uShadowLightRadiance_ID", "_ID", out_num).c_str());
                shadowSunLight[i].u_lightDir = getUniformLocation(findAndReplaceAllExt("uShadowLightDir_ID", "_ID", out_num).c_str());
                shadowSunLight[i].u_projMatrix = getUniformLocation(findAndReplaceAllExt("uShadowProjMatrix_ID", "_ID", out_num).c_str());
                shadowSunLight[i].u_textureDepth = getUniformLocation(findAndReplaceAllExt("uShadowTextureDepth_ID", "_ID", out_num).c_str());

                shadowSunLight[i].u_shadowDimension = getUniformLocation(findAndReplaceAllExt("uShadowDimension_ID", "_ID", out_num).c_str());
                shadowSunLight[i].u_shadowDimension_inv = getUniformLocation(findAndReplaceAllExt("uShadowDimension_inv_ID", "_ID", out_num).c_str());
                shadowSunLight[i].u_shadowCone = getUniformLocation(findAndReplaceAllExt("uShadowCone_ID", "_ID", out_num).c_str());
            }

            /*
                    shadowSunLight[1].u_lightRadiance = getUniformLocation("uShadowLightRadiance1");
                    shadowSunLight[1].u_lightDir = getUniformLocation("uShadowLightDir1");
                    shadowSunLight[1].u_projMatrix = getUniformLocation("uShadowProjMatrix1");
                    shadowSunLight[1].u_textureDepth = getUniformLocation("uShadowTextureDepth1");
                    shadowSunLight[2].u_lightRadiance = getUniformLocation("uShadowLightRadiance2");
                    shadowSunLight[2].u_lightDir = getUniformLocation("uShadowLightDir2");
                    shadowSunLight[2].u_projMatrix = getUniformLocation("uShadowProjMatrix2");
                    shadowSunLight[2].u_textureDepth = getUniformLocation("uShadowTextureDepth2");
                    shadowSunLight[3].u_lightRadiance = getUniformLocation("uShadowLightRadiance3");
                    shadowSunLight[3].u_lightDir = getUniformLocation("uShadowLightDir3");
                    shadowSunLight[3].u_projMatrix = getUniformLocation("uShadowProjMatrix3");
                    shadowSunLight[3].u_textureDepth = getUniformLocation("uShadowTextureDepth3");
            */

            // vec2 - 32
            u_shadow_spread_coefs = getUniformLocation("shadow_spread_coefs");

            u_gradient_matrix_array = getUniformLocation("uSkinGradientMatrix");
        }

        void FrankenShader::sendSkinMatrix(std::vector<MathCore::mat4f> *gradient_matrix)
        {

            if (u_gradient_matrix_array >= 0)
                OPENGL_CMD(glUniformMatrix4fv(u_gradient_matrix_array, (GLsizei)gradient_matrix->size(), GL_FALSE, gradient_matrix->at(0).array));
        }

        /*
        void FrankenShader::sendParamaters(const MathCore::mat4f *mvp, Transform *node, Components::ComponentCamera *camera,
            const Components::ComponentMaterial* material,const MathCore::vec3f &ambientLightColor,
                            int albedoTexUnit,int specularTexUnit, int normalTexUnit, int emissionTexUnit, int ambientCubeOrSphereTexUnit, int pbrOptimizationTexUnit){


            //"uniform mat4 uMVP;\n"
            setUniform(u_mvp, *mvp);

            //"uniform vec3 uCameraPosWorld;\n"
            if (u_cameraPosWorld >= 0)
                setUniform(u_cameraPosWorld, camera->transform[0]->getPosition(true));

            //"uniform sampler2D uTextureAlbedo;\n"
            //"uniform vec4 uMaterialAlbedoColor;\n"
            //"uniform float uMaterialRoughness;\n"
            //"uniform float uMaterialMetallic;\n"
            if (PBR.u_textureAlbedo >= 0)
                setUniform(PBR.u_textureAlbedo, albedoTexUnit);
            if (PBR.u_textureSpecular >= 0)
                setUniform(PBR.u_textureSpecular, specularTexUnit);
            if (PBR.u_textureEmission >= 0)
                setUniform(PBR.u_textureEmission, emissionTexUnit);
            if (PBR.u_TexturePBR_F_NDF_G_GHigh >= 0)
                setUniform(PBR.u_TexturePBR_F_NDF_G_GHigh, pbrOptimizationTexUnit);

            setUniform(PBR.u_materialAlbedoColor, material->pbr.albedoColor);
            setUniform(PBR.u_materialEmissionColor, material->pbr.emissionColor);
            setUniform(PBR.u_materialRoughness, material->pbr.roughness);
            setUniform(PBR.u_materialMetallic, material->pbr.metallic);

            //"uniform mat4 uLocalToWorld;\n"
            //"uniform mat4 uLocalToWorld_it;\n"
            //"uniform sampler2D uTextureNormal;\n"
            if ( normalMap.u_localToWorld >=0 )
                setUniform(normalMap.u_localToWorld, node->getMatrix(true) );
            if ( normalMap.u_localToWorld_it >=0 )
                setUniform(normalMap.u_localToWorld_it, node->getMatrixInverseTranspose(true) );
            if ( normalMap.u_textureNormal >=0 )
                setUniform(normalMap.u_textureNormal, normalTexUnit );


            //"uniform samplerCube uTextureCubeAmbient;\n"
            //"uniform vec3 uAmbientColor;\n"
            if (ambientLight.u_textureSphereAmbient >= 0)
                setUniform(ambientLight.u_textureSphereAmbient, ambientCubeOrSphereTexUnit);
            else if ( ambientLight.u_textureCubeAmbient >=0 )
                setUniform(ambientLight.u_textureCubeAmbient, ambientCubeOrSphereTexUnit);

            if ( ambientLight.u_ambientColor >=0 )
                setUniform(ambientLight.u_ambientColor, ambientLightColor );

        }
        */

        /*void FrankenShader::sendSunLightConfig(int index, const MathCore::vec3f &radiance, const MathCore::vec3f &worldDirection){

            //"uniform vec3 uLightRadiance0;\n"
            //"uniform vec3 uLightDir0;\n"
            //"uniform vec3 uLightRadiance1;\n"
            //"uniform vec3 uLightDir1;\n"
            //"uniform vec3 uLightRadiance2;\n"
            //"uniform vec3 uLightDir2;\n"
            //"uniform vec3 uLightRadiance3;\n"
            //"uniform vec3 uLightDir3;\n"
            if (sunLight[index].u_lightRadiance >= 0)
                setUniform(sunLight[index].u_lightRadiance, radiance );
            if (sunLight[index].u_lightDir >= 0)
                setUniform(sunLight[index].u_lightDir, worldDirection );

        }*/

        ShaderAlgorithmsBitMask FrankenShader::ValidateFormat(ShaderAlgorithmsBitMask flags)
        {

            /*
            ITK_ABORT(sunCount > 4,"FrankenShader support max of 4 sun light.\n");

            for(int i=0;i<sunCount;i++)
                flags |= ShaderAlgorithms_SunLight0 << i;
                */

            // check incompatible flags
            if (!(flags & (ShaderAlgorithms_AmbientLightSkybox |
                           ShaderAlgorithms_SunLight0 |
                           ShaderAlgorithms_SunLight1 |
                           ShaderAlgorithms_SunLight2 |
                           ShaderAlgorithms_SunLight3 |

                           ShaderAlgorithms_ShadowSunLight0 |
                           ShaderAlgorithms_ShadowSunLight1 |
                           ShaderAlgorithms_ShadowSunLight2 |
                           ShaderAlgorithms_ShadowSunLight3)))
                flags &= ~ShaderAlgorithms_NormalMap;
            if (flags & (ShaderAlgorithms_AmbientLightSkybox |
                         ShaderAlgorithms_AmbientLightSpheremap))
                flags &= ~ShaderAlgorithms_AmbientLightColor;

            return flags;
        }

    }

}
