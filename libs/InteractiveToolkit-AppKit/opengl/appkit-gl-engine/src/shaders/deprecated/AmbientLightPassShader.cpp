#include <appkit-gl-engine/shaders/deprecated/AmbientLightPassShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        void printV(const MathCore::vec3f &v, const char *txt)
        {
            printf("%s -> %.11f %.11f %.11f\n", txt, v.x, v.y, v.z);
        }

        MathCore::vec3f fakevec3topolar(const MathCore::vec3f &i)
        {
            MathCore::vec3f result;
            result.x = acos(i.z);
            result.y = acos(i.x);
            return result;
        }

        MathCore::vec3f vec3topolar(const MathCore::vec3f &i)
        {
            MathCore::vec3f result;
            result.x = acos(i.z);
            result.y = atan2(i.y, i.x);
            // result.x = MathCore::OP<float>::fmod(result.x + 2.0f*PI, 2.0f*PI);
            // result.y = MathCore::OP<float>::fmod(result.y + 2.0f*PI, 2.0f*PI);
            return result;
        }

        MathCore::vec3f polaToVec3(const MathCore::vec3f & i)
        {
            MathCore::vec3f result;
            result.x = sin(i.x) * cos(i.y);
            result.y = sin(i.x) * sin(i.y);
            result.z = cos(i.x);
            return result;
        }

        MathCore::vec3f baricentricCoord(const MathCore::vec3f & a, const MathCore::vec3f & b, const MathCore::vec3f & c, const MathCore::vec3f & p)
        {
            MathCore::vec3f bc = c - b;
            MathCore::vec3f ba = a - b;
            MathCore::vec3f N = MathCore::OP<MathCore::vec3f>::cross(bc, ba);
            MathCore::vec3f Nnorm = MathCore::OP<MathCore::vec3f>::normalize(N);
            float areaTriangle = MathCore::OP<MathCore::vec3f>::length(N);
            MathCore::vec3f bp = p - b;
            MathCore::vec3f uvw;
            uvw.x = (MathCore::OP<MathCore::vec3f>::dot( MathCore::OP<MathCore::vec3f>::cross(bc, bp), Nnorm)) / areaTriangle;
            uvw.z = (MathCore::OP<MathCore::vec3f>::dot(MathCore::OP<MathCore::vec3f>::cross(bp, ba), Nnorm)) / areaTriangle;
            uvw.y = 1.0f - uvw.x - uvw.z;
            return uvw;
        }

        AmbientLight_tex_cube_PassShader::AmbientLight_tex_cube_PassShader()
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
                "varying vec2 uv;"
                "varying mat3 worldTBN;"
                "void main() {"
                "  uv = aUV0.xy;"
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
                "varying mat3 worldTBN;"
                "uniform vec4 uColor;"
                "uniform sampler2D uTexture;"
                "uniform sampler2D uTextureNormal;"
                "uniform samplerCube uTextureCubeEnvironment;"
                /*
                "vec3 gammaToLinear(vec3 gamma){"
                "  return pow(gamma, vec3(2.2));"
                "}"
                "vec3 linearToGamma(vec3 linear){"
                "  return pow( linear / (linear + vec3(1)) , vec3( 1.0 / 2.2 ) );"
                "}"
                */
                /*
                "vec3 baricentricCoordvec3(vec3 a, vec3 b, vec3 c, vec3 p){"
                "  vec3 bc = c-b;"
                "  vec3 ba = a-b;"
                "  vec3 N = cross(bc,ba);"
                "  float Nlength = length(N);"
                "  vec3 Nnorm = N * vec3( 1.0 / Nlength );"
                "  float areaTriangle = Nlength;"
                "  vec3 bp = p-b;"
                "  vec3 uvw;"
                "  uvw.x = (dot(cross(bc,bp),Nnorm)) / areaTriangle;"
                "  uvw.z = (dot(cross(bp,ba),Nnorm)) / areaTriangle;"
                "  uvw.y = 1.0 - uvw.x - uvw.z;"
                "  return uvw;"
                "}"
                */
                "vec3 baricentricCoordvec2(vec2 a, vec2 b, vec2 c, vec2 p){"
                "  vec3 bc = vec3(c-b,0);"
                "  vec3 ba = vec3(a-b,0);"
                "  float areaTriangle_inv = 1.0/cross(bc,ba).z;"
                "  vec3 bp = vec3(p-b,0);"
                "  vec3 uvw;"
                "  uvw.x = cross(bc,bp).z;"
                "  uvw.z = cross(bp,ba).z;"
                "  uvw.xz = uvw.xz * areaTriangle_inv;"
                "  uvw.y = 1.0 - uvw.x - uvw.z;"
                "  return uvw;"
                "}"
                /*
                "vec2 vec3topolar(vec3 i){"
                "  vec2 r = vec2( acos(i.z), atan(i.y, i.x) );"
                "  return r;"
                "}"
                "vec3 polarToVec3(vec2 i){"
                "  float sinx = sin(i.x);"
                "  vec3 result;"
                "  result.x = sinx*cos(i.y);"
                "  result.y = sinx*sin(i.y);"
                "  result.z = cos(i.x);"
                "  return result;"
                "}"
                */
                // convert i to cossine space
                "vec2 fake_vec3topolar(vec3 i){"
                "  return acos(i.zx);"
                "}"
                "vec3 sampleEnvironmentCubeBaryLerp(vec3 normal){"
                "  vec3 sign = sign(normal);"
                "  vec3 signX = vec3(sign.x,0,0);"
                "  vec3 signY = vec3(0,sign.y,0);"
                "  vec3 signZ = vec3(0,0,sign.z);"
                "  mat3 tex = mat3("
                "    textureCube(uTextureCubeEnvironment, signX).rgb,"
                "    textureCube(uTextureCubeEnvironment, signY).rgb,"
                "    textureCube(uTextureCubeEnvironment, signZ).rgb"
                "  );"
                //"  vec2 polarSignX = fake_vec3topolar(signX);"
                //"  vec2 polarSignY = fake_vec3topolar(signY);"
                //"  vec2 polarSignZ = fake_vec3topolar(signZ);"

                // vec2(-0.5) >= sign.xz : step( sign.xz, vec2(-0.5) )
                "  vec2 signXxZz = step( sign.xz, vec2(-0.5) );"
                //                 vec2( (signX.x <= -0.5), (signZ.z <= -0.5) );"

                "  signXxZz = signXxZz * 3.14159265359;"
                "  vec2 polarSignX = vec2(1.57079637051,signXxZz.x );"
                "  vec2 polarSignY = vec2(1.57079637051);"
                "  vec2 polarSignZ = vec2(signXxZz.y,1.57079637051);"

                "  vec2 polarNormal = fake_vec3topolar(normal);"
                "  vec3 bariCentricCoord = baricentricCoordvec2("
                "    polarSignX,polarSignY,polarSignZ,polarNormal"
                "  );"
                "  return tex * bariCentricCoord;"
                "}"
                /*
                "vec3 sampleEnvironmentCube(vec3 normal){"
                "  vec3 sign = sign(normal);"
                "  vec3 signX = vec3(sign.x,0,0);"
                "  vec3 signY = vec3(0,sign.y,0);"
                "  vec3 signZ = vec3(0,0,sign.z);"
                "  mat3 tex = mat3("
                "    gammaToLinear(textureCube(uTextureCubeEnvironment, signX).rgb),"
                "    gammaToLinear(textureCube(uTextureCubeEnvironment, signY).rgb),"
                "    gammaToLinear(textureCube(uTextureCubeEnvironment, signZ).rgb)"
                "  );"
                "  vec3 dot_normal = vec3( "
                "    dot(normal,signX),"
                "    dot(normal,signY),"
                "    dot(normal,signZ) "
                "  );"
                "  dot_normal = max( vec3(0), dot_normal );"
                //linearize the angle mix asin(dot_normal)/(PI/2)
                "  dot_normal = asin(dot_normal) * vec3(6.36619806290e-01);"
                "  return tex * dot_normal;"
                "}"
                */
                "vec3 readNormalMap(){"
                "  vec3 normal = texture2D(uTextureNormal, uv).xyz;"
                "  normal = normal * 2.0 - 1.0;"
                "  normal = worldTBN * normal;"
                "  return normalize(normal);"
                "}"
                "void main() {"
                "  vec4 texel = texture2D(uTexture, uv);"
                "  vec3 texelEnvironment = sampleEnvironmentCubeBaryLerp(readNormalMap());"
                "  texel.a *= uColor.a;"
                "  texel.rgb = texel.rgb * uColor.rgb * texelEnvironment;"
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
            u_textureCubeEnvironment = getUniformLocation("uTextureCubeEnvironment");
            u_color = getUniformLocation("uColor");

            /*
            {
                printf("%.11f %.11f \n.",acos(1),acos(-1));
                exit(-1);
            }
            */

            /*

            {
                vec3 _sign = MathCore::vec3f(1,1,1);
                vec3 signX = MathCore::vec3f(_sign.x,0,0);
                vec3 signY = MathCore::vec3f(0,_sign.y,0);
                vec3 signZ = MathCore::vec3f(0,0,_sign.z);
                vec3 polarSignX = fakevec3topolar(signX);
                vec3 polarSignY = fakevec3topolar(signY);
                vec3 polarSignZ = fakevec3topolar(signZ);

                printV(polarSignX, "polarSignX");
                printV(polarSignY, "polarSignY");
                printV(polarSignZ, "polarSignZ");

                exit(-1);
            }

            */

            /*
            for(int i=0;i<100;i++){

                vec3 sample = Random::getVec3Direction();
                vec3 _sign = MathCore::vec3f( sign(sample.x), sign(sample.y), sign(sample.z) );
                vec3 signX = MathCore::vec3f(_sign.x,0,0);
                vec3 signY = MathCore::vec3f(0,_sign.y,0);
                vec3 signZ = MathCore::vec3f(0,0,_sign.z);



                vec3 polarSignX = vec3topolar(signX);
                vec3 polarSignY = vec3topolar(signY);
                vec3 polarSignZ = vec3topolar(signZ);
                vec3 polarSample = vec3topolar(sample);

                vec3 polarBary = baricentricCoord(polarSignX,polarSignY,polarSignZ,polarSample);

                vec3 polarReconstruction = polarSignX * polarBary.x + polarSignY * polarBary.y + polarSignZ * polarBary.z;

                printV(sample,"sample");
                printV(polarSample,"polarSample");
                printV(polarReconstruction,"polarReconstruction");
                vec3 sampleReconstruction = polarToVec3(polarReconstruction);
                printV(sampleReconstruction,"sampleReconstruction");

                if (distance(sample,sampleReconstruction) > EPSILON){
                    printf("Error...\n");
                    exit(-1);
                }
            }
            */

            /*
            for (int i=0;i<=10;i++){
                float _cos = (float)i/10.0f;
                float _acos = asin(_cos);
                float _acosd = _acos / (3.141592653 / 2.0);
                printf("cos=%f acos=%.11e _acosd=%f\n",_cos,1.0f/_acos,_acosd);
            }
             */

            // printf("%.11e\n",1.0/PI);
        }

    }
}