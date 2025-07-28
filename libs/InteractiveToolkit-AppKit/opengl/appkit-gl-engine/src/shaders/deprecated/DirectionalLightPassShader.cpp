#include <appkit-gl-engine/shaders/deprecated/DirectionalLightPassShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        /*
         Equations from: https://learnopengl.com/PBR/Lighting
         */

        DirectionalLightPassShader::DirectionalLightPassShader()
        {
            format = ITKExtension::Model::CONTAINS_POS |
                     ITKExtension::Model::CONTAINS_UV0 |
                     ITKExtension::Model::CONTAINS_NORMAL |
                     ITKExtension::Model::CONTAINS_TANGENT;

            const char vertexShaderCode[] = {
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "attribute vec4 aPosition;"
                "attribute vec3 aNormal;"
                "attribute vec3 aTangent;"
                "attribute vec3 aUV0;"
                "uniform mat4 uMVP;"
                "uniform mat4 uLocalToWorld;"
                "uniform mat4 uLocalToWorld_it;"
                "uniform vec3 uCameraPosWorld;"
                "varying vec2 uv;"
                "varying vec3 viewWorld;"
                "varying mat3 worldTBN;"
                "void main() {"
                "  uv = aUV0.xy;"
                "  viewWorld = (uCameraPosWorld - (uLocalToWorld * aPosition).xyz);"
                "  vec3 N = normalize( uLocalToWorld_it * vec4( aNormal, 0.0 ) ).xyz;"
                "  vec3 T = normalize( uLocalToWorld * vec4( aTangent, 0.0 ) ).xyz;"
                // re-orthogonalize T with respect to N
                "  T = normalize(T - dot(T, N) * N);"
                "  vec3 B = cross(T, N);"
                "  worldTBN = mat3(T,B,N);"
                "  gl_Position = uMVP * aPosition;"
                "}"
            };

            const char fragmentShaderCode[] = {
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "varying vec2 uv;"
                "varying vec3 viewWorld;"
                "varying mat3 worldTBN;"
                "uniform vec3 uMaterialAlbedoColor;"
                "uniform float uMaterialRoughness;"
                "uniform float uMaterialMetallic;"
                "uniform vec3 uLightRadiance;"
                "uniform vec3 uLightDir;"
                "uniform sampler2D uTexture;"
                "uniform sampler2D uTextureNormal;"
                /*
                "vec3 gammaToLinear(vec3 gamma){"
                "  return pow(gamma, vec3(2.2));"
                "}"

                "vec3 linearToGamma(vec3 linear){"
                "  return pow( linear / (linear + vec3(1)) , vec3( 1.0 / 2.2 ) );"
                "}"
                */

                "vec3 readNormalMap(){"
                "  vec3 normal = texture2D(uTextureNormal, uv).xyz;"
                "  normal = normal * 2.0 - 1.0;"
                "  normal = worldTBN * normal;"
                "  return normalize(normal);"
                "}"

                "vec3 fresnelSchlick(float cosTheta, vec3 F0){"
                "  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);"
                "}"

                "float DistributionGGX(vec3 N, vec3 H, float roughness){"
                "  float a      = roughness*roughness;"
                "  float a2     = a*a;"
                "  float NdotH  = max(dot(N, H), 0.0);"
                "  float NdotH2 = NdotH*NdotH;"
                "  float num   = a2;"
                "  float denom = (NdotH2 * (a2 - 1.0) + 1.0);"
                "  denom = 3.14159265358 * denom * denom;" // PI = 3.14159265358
                "  return num / denom;"
                "}"
                "float GeometrySchlickGGX(float NdotV, float roughness){"
                "  float r = (roughness + 1.0);"
                "  float k = (r*r) / 8.0;"
                "  float num   = NdotV;"
                "  float denom = NdotV * (1.0 - k) + k;"
                "  return num / denom;"
                "}"
                "float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){"
                "  float NdotV = max(dot(N, V), 0.0);"
                "  float NdotL = max(dot(N, L), 0.0);"
                "  float ggx2  = GeometrySchlickGGX(NdotV, roughness);"
                "  float ggx1  = GeometrySchlickGGX(NdotL, roughness);"
                "  return ggx1 * ggx2;"
                "}"
                "vec3 computePBR(vec3 albedo, vec3 radiance, vec3 N, vec3 V, vec3 L, vec3 H){"
                // fresnel factor calculation
                "  vec3 F0 = vec3(0.04);"
                "  F0      = mix(F0, albedo, uMaterialMetallic);"
                "  vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);"

                // geometry factor
                "  float G = GeometrySmith(N, V, L, uMaterialRoughness);"

                // normal distribution function
                "  float NDF = DistributionGGX(N, H, uMaterialRoughness);"

                // combining all factors
                "  vec3 numerator    = NDF * G * F;"
                "  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);"
                "  vec3 specular     = numerator / max(denominator, 0.001);"

                "  vec3 kS = F;"
                "  vec3 kD = vec3(1.0) - kS;"

                "  kD *= 1.0 - uMaterialMetallic;"

                "  float NdotL = max(dot(N, L), 0.0);"
                // 1/PI = 3.18309877326e-01
                "  return (kD * albedo * 3.18309877326e-01 + specular) * radiance * NdotL;"
                "}"

                "void main() {"
                "  vec4 texel = texture2D(uTexture, uv);"
                "  texel.rgb = texel.rgb * uMaterialAlbedoColor;"
                "  vec3 N = readNormalMap();"
                "  vec3 V = normalize(viewWorld);"
                "  vec3 L = -uLightDir;"
                "  vec3 H = normalize(V + L);"

                "  vec3 radiance = uLightRadiance;"
                //"  vec3 albedo = gammaToLinear(texel.rgb);"
                "  vec3 albedo = texel.rgb;"

                "  texel.rgb = computePBR(albedo,radiance,N,V,L,H);"

                //"  texel.rgb = linearToGamma(texel.rgb);"

                "  gl_FragColor = texel;"
                "}"
            };
            // PI = 3.14159265358 //1/PI = 3.18309877326e-01

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            u_mvp = getUniformLocation("uMVP");

            u_localToWorld = getUniformLocation("uLocalToWorld");
            u_localToWorld_it = getUniformLocation("uLocalToWorld_it");
            u_texture = getUniformLocation("uTexture");
            u_textureNormal = getUniformLocation("uTextureNormal");
            u_materialAlbedoColor = getUniformLocation("uMaterialAlbedoColor");
            u_materialRoughness = getUniformLocation("uMaterialRoughness");
            u_materialMetallic = getUniformLocation("uMaterialMetallic");
            u_lightRadiance = getUniformLocation("uLightRadiance");
            u_lightDir = getUniformLocation("uLightDir");
            u_cameraPosWorld = getUniformLocation("uCameraPosWorld");
        }

    }
}
