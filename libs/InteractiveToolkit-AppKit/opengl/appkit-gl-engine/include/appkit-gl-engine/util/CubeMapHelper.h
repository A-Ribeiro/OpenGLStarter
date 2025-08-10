#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLShader.h>

#include <appkit-gl-engine/GL/GLRenderState.h>

#include <appkit-gl-base/GLDynamicFBO.h>

namespace AppKit
{
    namespace GLEngine
    {

        class ShaderCopyCubeMap : public AppKit::OpenGL::GLShader
        {

        public:
            static const AppKit::OpenGL::ShaderType Type;

            //
            // attrib locations
            //
            static const int vPosition = 0;
            static const int vUV = 1;

            //
            // uniform locations
            //
            int cubeTexture;

            ShaderCopyCubeMap() : GLShader(ShaderCopyCubeMap::Type)
            {
                const char vertexShaderCode[] = {
                    "attribute vec4 vPosition;"
                    "attribute vec3 vUV;"
                    "varying vec3 cubeAccess;"
                    "void main() {"
                    "  cubeAccess = vUV;"
                    "  gl_Position = vPosition;"
                    "}"};

                const char fragmentShaderCode[] = {
                    //"precision mediump float;"
                    "varying vec3 cubeAccess;"
                    "uniform samplerCube cubeTexture;"
                    "void main() {"
                    "  vec4 result = textureCube(cubeTexture, cubeAccess);"
                    "  gl_FragColor = result;"
                    "}"};

                compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
                bindAttribLocation(ShaderCopyCubeMap::vPosition, "vPosition");
                bindAttribLocation(ShaderCopyCubeMap::vUV, "vUV");
                link(__FILE__, __LINE__);

                cubeTexture = getUniformLocation("cubeTexture");
            }

            void setCubeTexture(int texunit)
            {
                setUniform(cubeTexture, texunit);
            }

            ~ShaderCopyCubeMap()
            {
            }
        };

        class ShaderRender1x1CubeMap : public AppKit::OpenGL::GLShader
        {

        public:
            static const AppKit::OpenGL::ShaderType Type;

            //
            // attrib locations
            //
            static const int vPosition = 0;
            // static const int vUV = 1;

            //
            // uniform locations
            //
            int cubeTexture;

            ShaderRender1x1CubeMap() : GLShader(ShaderRender1x1CubeMap::Type)
            {
                const char vertexShaderCode[] = {
                    "attribute vec4 vPosition;\n"
                    //"attribute vec2 vUV;\n"
                    "varying vec2 uv;\n"
                    "void main() {\n"
                    "  uv = vPosition.xy*0.5 + vec2(0.5);\n"
                    "  gl_Position = vPosition;\n"
                    "}\n"};

                const char fragmentShaderCode[] = {
                    //"precision mediump float;"
                    "varying vec2 uv;\n"
                    "uniform samplerCube cubeTexture;\n"
                    "vec3 baricentricCoordvec2(vec2 a, vec2 b, vec2 c, vec2 p){\n"
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
                    "    textureCube(cubeTexture, signX).rgb,\n"
                    "    textureCube(cubeTexture, signY).rgb,\n"
                    "    textureCube(cubeTexture, signZ).rgb\n"
                    "  );\n"
                    "  vec2 signXxZz = step( sign.xz, vec2(-0.5) );\n"
                    "  signXxZz = signXxZz * 3.14159265359;\n"
                    "  vec2 polarSignX = vec2(1.57079637051,signXxZz.x );\n"
                    "  vec2 polarSignY = vec2(1.57079637051);\n"
                    "  vec2 polarSignZ = vec2(signXxZz.y,1.57079637051);\n"
                    "  vec2 polarNormal = acos(normal.zx);\n"
                    "  vec3 bariCentricCoord = baricentricCoordvec2(\n"
                    "    polarSignX,polarSignY,polarSignZ,polarNormal\n"
                    "  );\n"
                    "  return tex * bariCentricCoord;\n"
                    "}\n"
                    "vec3 st2normal(vec2 st) {\n"
                    "    float phi = st.y * 3.14159265358;// aRibeiro::PI\n"
                    "    float theta = st.x * 6.28318530716;// 2.0 * aRibeiro::PI;\n"
                    "    vec3 result;\n"
                    "    result.x = sin(phi) * cos(theta);\n"
                    "    result.y = cos(phi);\n"
                    "    result.z = sin(phi) * sin(theta);\n"
                    "    return result;\n"
                    "}\n"
                    /*
                    "vec2 normal2st(vec3 normal) {\n"
                    "    float t = acos(normal.y) / 3.14159265358;// PI\n"
                    "    float s = atan(normal.z, normal.x) / 6.28318530716;// 2.0 * PI;\n"
                    "    s = fract(s + 1.0);\n"
                    "    return vec2(s, t);\n"
                    "}\n"
                    */
                    "void main() {\n"
                    "  vec2 _uv = uv;\n"
                    "  _uv.y = 1.0 - abs( (_uv.y - 0.5 ) * 2.0 );\n"
                    "  vec3 result = sampleEnvironmentCubeBaryLerp( st2normal(_uv) );\n"
                    "  gl_FragColor = vec4(result, 1.0);\n"
                    "}\n"};

                compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
                bindAttribLocation(ShaderCopyCubeMap::vPosition, "vPosition");
                bindAttribLocation(ShaderCopyCubeMap::vUV, "vUV");
                link(__FILE__, __LINE__);

                cubeTexture = getUniformLocation("cubeTexture");
            }

            void setCubeTexture(int texunit)
            {
                setUniform(cubeTexture, texunit);
            }

            ~ShaderRender1x1CubeMap()
            {
            }
        };

        class CubeMapHelper
        {
        public:
            AppKit::OpenGL::GLDynamicFBO targetFBO;
            AppKit::OpenGL::GLDynamicFBO sourceFBO;
            // AppKit::OpenGL::GLRenderBuffer depthBuffer;
            ShaderCopyCubeMap shaderCopyCubeMap;
            ShaderRender1x1CubeMap shaderRender1x1CubeMap;

            CubeMapHelper();
            // void copyCubeMap(int resolution, AppKit::OpenGL::GLCubeMap *inputcubemap, AppKit::OpenGL::GLCubeMap *targetcubemap);

            void copyCubeMapEnhanced(AppKit::OpenGL::GLCubeMap *inputcubemap, int inputMip, AppKit::OpenGL::GLCubeMap *targetcubemap, int outputMip);

            AppKit::OpenGL::GLDynamicFBO sphereFBO;
            void render1x1CubeIntoSphereTexture(AppKit::OpenGL::GLCubeMap *inputcubemap, AppKit::OpenGL::GLTexture *targetTexture, int width, int height);
        };

    }

}