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

            ShaderRender1x1CubeMap();

            void setCubeTexture(int texunit);

            ~ShaderRender1x1CubeMap();
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