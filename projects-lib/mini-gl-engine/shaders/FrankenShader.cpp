#include "FrankenShader.h"

namespace GLEngine {
    
    void FrankenShader::compileShader(){
        format = model::CONTAINS_POS;// | model::CONTAINS_UV0;
        
        //check need of normalMap
        if (!(frankenFormat & (ShaderAlgorithms_AmbientLightSkybox |
                             ShaderAlgorithms_SunLight0 |
                             ShaderAlgorithms_SunLight1 |
                             ShaderAlgorithms_SunLight2 |
                             ShaderAlgorithms_SunLight3
                               ))){
            frankenFormat &= ~ShaderAlgorithms_NormalMap;
        }
        
        //
        // Vertex shader mounter
        //
        
        std::string vertexShader =
        "#version 120\n"
        "attribute vec4 aPosition;\n"
        "uniform mat4 uMVP;\n"
        "VARIABLES"
        "FUNCTIONS"
        "void main(){\n"
        "SHADER_CODE"
        "}";

        //skinned mesh code
        if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_128) {
            format |= model::CONTAINS_VERTEX_WEIGHT128;

            findAndReplaceAll(&vertexShader,
                "VARIABLES",
                "uniform mat4 uSkinGradientMatrix[128];\n"
                "attribute vec4 aSkinIndex;\n"
                "attribute vec4 aSkinWeight;\n"
                "VARIABLES");

        }
        else if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_96) {
            format |= model::CONTAINS_VERTEX_WEIGHT96;

            findAndReplaceAll(&vertexShader,
                "VARIABLES",
                "uniform mat4 uSkinGradientMatrix[96];\n"
                "attribute vec4 aSkinIndex;\n"
                "attribute vec4 aSkinWeight;\n"
                "VARIABLES");

        }
        else if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_64) {
            format |= model::CONTAINS_VERTEX_WEIGHT64;

            findAndReplaceAll(&vertexShader,
                "VARIABLES",
                "uniform mat4 uSkinGradientMatrix[64];\n"
                "attribute vec4 aSkinIndex;\n"
                "attribute vec4 aSkinWeight;\n"
                "VARIABLES");

        }
        else if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_32) {
            format |= model::CONTAINS_VERTEX_WEIGHT32;

            findAndReplaceAll(&vertexShader,
                "VARIABLES",
                "uniform mat4 uSkinGradientMatrix[32];\n"
                "attribute vec4 aSkinIndex;\n"
                "attribute vec4 aSkinWeight;\n"
                "VARIABLES");

        }
        else if (frankenFormat & ShaderAlgorithms_skinGradientMatrix_16) {
            format |= model::CONTAINS_VERTEX_WEIGHT16;

            findAndReplaceAll(&vertexShader,
                "VARIABLES",
                "uniform mat4 uSkinGradientMatrix[16];\n"
                "attribute vec4 aSkinIndex;\n"
                "attribute vec4 aSkinWeight;\n"
                "VARIABLES");

        }

        if (format & model::CONTAINS_VERTEX_WEIGHT_ANY) {

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
                "    / (a00 * b01 + a01 * b11 + a02 * b21);\n"  // result / determinant
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
        else {
            findAndReplaceAll(&vertexShader,
                "SHADER_CODE",
                "  vec4 inputPosition = aPosition;\n"
                "SHADER_CODE");
        }

        bool uses_uv = false;

        if ( frankenFormat & (ShaderAlgorithms_TextureAlbedo | ShaderAlgorithms_TextureSpecular ) ) {
            format |= model::CONTAINS_UV0;
            uses_uv = true;
        }

        bool usesLocalToWorld = false;
        bool usesLocalToWorld_it = false;
        bool usesNormal = false;
        
        if ( frankenFormat & ShaderAlgorithms_NormalMap) {
            format |= model::CONTAINS_NORMAL;
            format |= model::CONTAINS_TANGENT;
            format |= model::CONTAINS_UV0;
            
            findAndReplaceAll(&vertexShader,
                              "VARIABLES",
                              "attribute vec3 aNormal;\n"
                              "attribute vec3 aTangent;\n"
                              "varying mat3 worldTBN;\n"
                              "VARIABLES");

            if (format & model::CONTAINS_VERTEX_WEIGHT_ANY) {
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
            else {
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
            
        } else {
            
            
            usesNormal = ((frankenFormat & (ShaderAlgorithms_AmbientLightSkybox |
                              ShaderAlgorithms_SunLight0 |
                              ShaderAlgorithms_SunLight1 |
                              ShaderAlgorithms_SunLight2 |
                              ShaderAlgorithms_SunLight3
                                            )));
            if (usesNormal) {
                format |= model::CONTAINS_NORMAL;
                
                findAndReplaceAll(&vertexShader,
                                  "VARIABLES",
                                  "attribute vec3 aNormal;\n"
                                  "varying vec3 varyingNormal;\n"
                                  "VARIABLES");
                
                if (format & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                    findAndReplaceAll(&vertexShader,
                        "SHADER_CODE",
                        "  varyingNormal = mat3(uLocalToWorld_it) * ( v_gradient_IT * aNormal );\n"
                        "SHADER_CODE");
                }
                else {
                    findAndReplaceAll(&vertexShader,
                        "SHADER_CODE",
                        "  varyingNormal = mat3(uLocalToWorld_it) * aNormal;\n"
                        "SHADER_CODE");
                }
                
                usesLocalToWorld_it = true;
            }
            
        }
        
        if (frankenFormat & (ShaderAlgorithms_SunLight0 |
                             ShaderAlgorithms_SunLight1 |
                             ShaderAlgorithms_SunLight2 |
                             ShaderAlgorithms_SunLight3 ) ){
            findAndReplaceAll(&vertexShader,
                              "VARIABLES",
                              "uniform vec3 uCameraPosWorld;\n"
                              "varying vec3 viewWorld;\n"
                              "VARIABLES");
            
            findAndReplaceAll(&vertexShader,
                              "SHADER_CODE",
                              "  viewWorld = (uCameraPosWorld - (uLocalToWorld * inputPosition).xyz);\n"
                              "SHADER_CODE");
            
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
        
        if (uses_uv) {
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
        
        
        //close vertex shader
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
            "#version 120\n"
            "uniform vec4 uMaterialAlbedoColor;\n"
            "uniform float uMaterialRoughness;\n"
            "uniform float uMaterialMetallic;\n"
            "VARIABLES"
            "FUNCTIONS"
            "void main(){\n"
            "  vec4 texel;\n"
            "SHADER_CODE"
            "}";

        if (uses_uv) {
            findAndReplaceAll(&fragmentShader,
                "VARIABLES",
                "varying vec2 uv;\n"
                "VARIABLES");

            if (frankenFormat & ShaderAlgorithms_TextureAlbedo) {

                findAndReplaceAll(&fragmentShader,
                    "VARIABLES",
                    "uniform sampler2D uTextureAlbedo;\n"
                    "VARIABLES");

                findAndReplaceAll(&fragmentShader,
                    "SHADER_CODE",
                    "  texel = texture2D(uTextureAlbedo, uv);\n"
                    "SHADER_CODE");
            }

            if (frankenFormat & ShaderAlgorithms_TextureSpecular) {
                findAndReplaceAll(&fragmentShader,
                    "VARIABLES",
                    "uniform sampler2D uTextureSpecular;\n"
                    "VARIABLES");
            }
        }
        
        if (frankenFormat & ShaderAlgorithms_sRGB) {

            findAndReplaceAll(&fragmentShader,
                "FUNCTIONS",
                "vec3 gammaToLinear(vec3 gamma){"
                "  return pow(gamma, vec3(2.2));"
                "}"
                "vec3 linearToGamma(vec3 linear){"
                //"  return pow( linear / (linear + vec3(1)) , vec3( 1.0 / 2.2 ) );"
                "  return pow( linear , vec3( 1.0 / 2.2 ) );"
                "}"
                "FUNCTIONS");

            findAndReplaceAll(&fragmentShader,
                "SHADER_CODE",
                "  texel.rgb = gammaToLinear(texel.rgb);\n"
                "SHADER_CODE");
        }

        //forwarded because of the gammaToLinear access...
        if (frankenFormat & ShaderAlgorithms_TextureAlbedo) {
            findAndReplaceAll(&fragmentShader,
                "SHADER_CODE",
                "  texel *= uMaterialAlbedoColor;\n"
                "SHADER_CODE");
        }
        else {
            //if dont set the albedo texture, so it is the first assignment
            findAndReplaceAll(&fragmentShader,
                "SHADER_CODE",
                "  texel = uMaterialAlbedoColor;\n"
                "SHADER_CODE");
        }

        if (frankenFormat & ShaderAlgorithms_TextureSpecular) {
            findAndReplaceAll(&fragmentShader,
                "SHADER_CODE",
                "  float metallic_final = uMaterialMetallic * texture2D(uTextureSpecular, uv).x;\n"
                "SHADER_CODE");
        }
        else {
            findAndReplaceAll(&fragmentShader,
                "SHADER_CODE",
                "  float metallic_final = uMaterialMetallic;\n"
                "SHADER_CODE");
        }
        
        if (frankenFormat & (ShaderAlgorithms_SunLight0 |
                             ShaderAlgorithms_SunLight1 |
                             ShaderAlgorithms_SunLight2 |
                             ShaderAlgorithms_SunLight3 ) ){
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
                              "  denom = 3.14159265358 * denom * denom;\n"//PI = 3.14159265358
                              "  return num / denom;\n"
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
                              "vec3 computePBR(vec3 albedo, vec3 radiance, vec3 N, vec3 V, vec3 L, vec3 H, float metallic){\n"
                              //fresnel factor calculation
                              "  vec3 F0 = vec3(0.04);\n"
                              "  F0      = mix(F0, albedo, metallic);\n"
                              "  vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);\n"
                              
                              //geometry factor
                              "  float G = GeometrySmith(N, V, L, uMaterialRoughness);\n"
                              
                              //normal distribution function
                              "  float NDF = DistributionGGX(N, H, uMaterialRoughness);\n"
                              
                              //combining all factors
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
                              "SHADER_CODE");
        }
        
        
        if ( frankenFormat & ShaderAlgorithms_NormalMap) {
            
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
                              "  return normalize(normal);\n"
                              "}\n"
                              "FUNCTIONS");
            
            findAndReplaceAll(&fragmentShader,
                              "SHADER_CODE",
                              "  vec3 N = readNormalMap();\n"
                              "SHADER_CODE");
            
        } else {
            
            if (usesNormal) {
                findAndReplaceAll(&fragmentShader,
                                  "VARIABLES",
                                  "varying vec3 varyingNormal;\n"
                                  "VARIABLES");
                
                findAndReplaceAll(&fragmentShader,
                                  "SHADER_CODE",
                                  "  vec3 N = normalize(varyingNormal);\n"
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
        
        
        //ambient light
        
        if (frankenFormat & ShaderAlgorithms_AmbientLightSkybox){
            
            frankenFormat &= ~ShaderAlgorithms_AmbientLightColor;
            
            findAndReplaceAll(&fragmentShader,
                              "VARIABLES",
                              "uniform samplerCube uTextureCubeAmbient;\n"
                              "VARIABLES");
            
            findAndReplaceAll(&fragmentShader,
                              "FUNCTIONS",
                              "vec3 baricentricCoordVec2(vec2 a, vec2 b, vec2 c, vec2 p){\n"
                              "  vec3 bc = vec3(c-b,0);\n"
                              "  vec3 ba = vec3(a-b,0);\n"
                              "  float areaTriangle_inv = 1.0/cross(bc,ba).z;\n"
                              "  vec3 bp = vec3(p-b,0);\n"
                              "  vec3 uvw;\n"
                              "  uvw.x = cross(bc,bp).z;\n"
                              "  uvw.z = cross(bp,ba).z;\n"
                              "  uvw.xz = uvw.xz * areaTriangle_inv;\n"
                              "  uvw.y = 1.0 - uvw.x - uvw.z;\n"
                              "  return uvw;\n"
                              "}\n"
                              "vec3 sampleEnvironmentCubeBaryLerp(vec3 normal){\n"
                              "  vec3 sign = sign(normal);\n"
                              "  vec3 signX = vec3(sign.x,0,0);\n"
                              "  vec3 signY = vec3(0,sign.y,0);\n"
                              "  vec3 signZ = vec3(0,0,sign.z);\n"
                              "  mat3 tex = mat3(\n"
                              "    textureCube(uTextureCubeAmbient, signX).rgb,\n"
                              "    textureCube(uTextureCubeAmbient, signY).rgb,\n"
                              "    textureCube(uTextureCubeAmbient, signZ).rgb\n"
                              "  );\n"
                              "  vec2 signXxZz = step( sign.xz, vec2(-0.5) );\n"
                              "  signXxZz = signXxZz * 3.14159265359;\n"
                              "  vec2 polarSignX = vec2(1.57079637051,signXxZz.x );\n"
                              "  vec2 polarSignY = vec2(1.57079637051);\n"
                              "  vec2 polarSignZ = vec2(signXxZz.y,1.57079637051);\n"
                              "  vec2 polarNormal = acos(normal.zx);\n"
                              "  vec3 bariCentricCoord = baricentricCoordVec2(\n"
                              "    polarSignX,polarSignY,polarSignZ,polarNormal\n"
                              "  );\n"
                              "  return tex * bariCentricCoord;\n"
                              "}\n"
                              "FUNCTIONS");

            if (frankenFormat & ShaderAlgorithms_sRGB) {
                findAndReplaceAll(&fragmentShader,
                    "SHADER_CODE",
                    "  vec3 texelEnvironment = gammaToLinear(sampleEnvironmentCubeBaryLerp(N));\n"
                    "  texel.rgb += albedo * texelEnvironment;\n"
                    "SHADER_CODE");
            }
            else {
                findAndReplaceAll(&fragmentShader,
                    "SHADER_CODE",
                    "  vec3 texelEnvironment = sampleEnvironmentCubeBaryLerp(N);\n"
                    "  texel.rgb += albedo * texelEnvironment;\n"
                    "SHADER_CODE");
            }
            
        }else if (frankenFormat & ShaderAlgorithms_AmbientLightColor){
            findAndReplaceAll(&fragmentShader,
                              "VARIABLES",
                              "uniform vec3 uAmbientColor;\n"
                              "VARIABLES");
            
            findAndReplaceAll(&fragmentShader,
                              "SHADER_CODE",
                              "  texel.rgb += albedo * pow(uAmbientColor, vec3(2.2));\n"
                              "SHADER_CODE");
        }
        
        //sun light
        if (frankenFormat & ShaderAlgorithms_SunLight0){
            findAndReplaceAll(&fragmentShader,
                              "VARIABLES",
                              "uniform vec3 uLightRadiance0;\n"
                              "uniform vec3 uLightDir0;\n"
                              "VARIABLES");
            
            findAndReplaceAll(&fragmentShader,
                              "SHADER_CODE",
                              "  L = -uLightDir0;\n"
                              "  H = normalize(V + L);\n"
                              "  radiance = uLightRadiance0;\n"
                              "  texel.rgb += computePBR(albedo,radiance,N,V,L,H,metallic_final);\n"
                              "SHADER_CODE");
        }
        if (frankenFormat & ShaderAlgorithms_SunLight1){
            findAndReplaceAll(&fragmentShader,
                              "VARIABLES",
                              "uniform vec3 uLightRadiance1;\n"
                              "uniform vec3 uLightDir1;\n"
                              "VARIABLES");
            
            findAndReplaceAll(&fragmentShader,
                              "SHADER_CODE",
                              "  L = -uLightDir1;\n"
                              "  H = normalize(V + L);\n"
                              "  radiance = uLightRadiance1;\n"
                              "  texel.rgb += computePBR(albedo,radiance,N,V,L,H,metallic_final);\n"
                              "SHADER_CODE");
        }
        if (frankenFormat & ShaderAlgorithms_SunLight2){
            findAndReplaceAll(&fragmentShader,
                              "VARIABLES",
                              "uniform vec3 uLightRadiance2;\n"
                              "uniform vec3 uLightDir2;\n"
                              "VARIABLES");
            
            findAndReplaceAll(&fragmentShader,
                              "SHADER_CODE",
                              "  L = -uLightDir2;\n"
                              "  H = normalize(V + L);\n"
                              "  radiance = uLightRadiance2;\n"
                              "  texel.rgb += computePBR(albedo,radiance,N,V,L,H,metallic_final);\n"
                              "SHADER_CODE");
        }
        if (frankenFormat & ShaderAlgorithms_SunLight3){
            findAndReplaceAll(&fragmentShader,
                              "VARIABLES",
                              "uniform vec3 uLightRadiance3;\n"
                              "uniform vec3 uLightDir3;\n"
                              "VARIABLES");
            
            findAndReplaceAll(&fragmentShader,
                              "SHADER_CODE",
                              "  L = -uLightDir3;\n"
                              "  H = normalize(V + L);\n"
                              "  radiance = uLightRadiance3;\n"
                              "  texel.rgb += computePBR(albedo,radiance,N,V,L,H,metallic_final);\n"
                              "SHADER_CODE");
        }

        if (frankenFormat & ShaderAlgorithms_sRGB) {
            findAndReplaceAll(&fragmentShader,
                "SHADER_CODE",
                "  texel.rgb = linearToGamma(texel.rgb);\n"
                "SHADER_CODE");
        }
        
        
        //close fragment shader
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
        printf("  skinGradient16: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_16) != 0);
        printf("  skinGradient32: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_32) != 0);
        printf("  skinGradient64: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_64) != 0);
        printf("  skinGradient96: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_96) != 0);
        printf("  skinGradient128: %i\n", (frankenFormat & ShaderAlgorithms_skinGradientMatrix_128) != 0);
        printf("  sRGB: %i\n", (frankenFormat & ShaderAlgorithms_sRGB) != 0);
        printf("VERTEX\n");
        printf("%s\n\n",vertexShader.c_str());
        printf("FRAGMENT\n");
        printf("%s\n\n",fragmentShader.c_str());
        
        LoadShaderProgram(vertexShader.c_str(), fragmentShader.c_str(), __FILE__, __LINE__);
        
        queryShaderUniforms();
    }
    
    void FrankenShader::queryShaderUniforms(){
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
        PBR.u_materialAlbedoColor = getUniformLocation("uMaterialAlbedoColor");
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


        u_gradient_matrix_array = getUniformLocation("uSkinGradientMatrix");
        
    }

    void FrankenShader::sendSkinMatrix(aRibeiro::aligned_vector<aRibeiro::mat4> *gradient_matrix) {

        if (u_gradient_matrix_array >= 0)
            OPENGL_CMD(glUniformMatrix4fv(u_gradient_matrix_array, gradient_matrix->size(), GL_FALSE, gradient_matrix->at(0).array));

    }
    
    void FrankenShader::sendParamaters(const aRibeiro::mat4 *mvp, Transform *node, Components::ComponentCamera *camera,
        const Components::ComponentMaterial* material,const aRibeiro::vec3 &ambientLightColor,
                        int albedoTexUnit,int specularTexUnit, int normalTexUnit, int ambientCubeTexUnit){
        
        
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
        setUniform(PBR.u_materialAlbedoColor, material->pbr.albedoColor);
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
        if ( ambientLight.u_textureCubeAmbient >=0 )
            setUniform(ambientLight.u_textureCubeAmbient, ambientCubeTexUnit );
        if ( ambientLight.u_ambientColor >=0 )
            setUniform(ambientLight.u_ambientColor, ambientLightColor );
        
    }
    void FrankenShader::sendSunLightConfig(int index, const aRibeiro::vec3 &radiance, const aRibeiro::vec3 &worldDirection){
        
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
        
    }
    
}
