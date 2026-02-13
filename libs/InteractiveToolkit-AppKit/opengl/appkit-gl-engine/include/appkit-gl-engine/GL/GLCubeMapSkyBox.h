#pragma once

#include <string>

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLVertexBufferObject.h>
#include <appkit-gl-base/shaders/GLShaderTextureColor.h>

#include <appkit-gl-base/GLCubeMap.h>
#include <appkit-gl-base/shaders/GLShaderCubeMapSkyBox.h>

namespace AppKit
{
    namespace GLEngine
    {

        /// \brief Loads a CubeMap to draw a skybox.
        ///
        /// Example:
        ///
        /// \code
        /// #include <appkit-gl-engine/mini-gl-engine.h>
        /// using namespace AppKit::OpenGL;
        /// using namespace AppKit::GLEngine;
        ///
        /// GLCubeMapSkyBox *cube_skybox = GLCubeMapSkyBox(
        ///     true && Engine::Instance()->isRGBCapable, // load sRGB
        ///     "negz.jpg","posz.jpg",
        ///     "negx.jpg","posx.jpg",
        ///     "negy.jpg","posy.jpg",
        ///     100.0f, // distance
        ///     1024 // max cubeResolution
        /// );
        ///
        /// ...
        ///
        /// cube_skybox->draw(viewMatrix,projectionMatrix);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLCubeMapSkyBox
        {

            void createVertex(float distance);

        public:
            AppKit::OpenGL::GLCubeMap cubeMap;
            AppKit::OpenGL::GLShaderCubeMapSkyBox cubeSkyShader;

            std::vector<MathCore::vec3f> vertex;
            AppKit::OpenGL::GLVertexBufferObject *vbo;
            // std::vector<MathCore::vec2f> uv;

            GLCubeMapSkyBox(
                bool sRGB,
                const std::string &_negz, const std::string &_posz,
                const std::string &_negx, const std::string &_posx,
                const std::string &_negy, const std::string &_posy,
                float _distance, int cubeResolution, bool leftHanded = true);

            GLCubeMapSkyBox(bool sRGB,
                            std::string single_file,
                            float _distance, int cubeResolution, bool leftHanded = true);

            ~GLCubeMapSkyBox();

            void createVBO();

            void draw(const MathCore::mat4f &viewMatrix, const MathCore::mat4f &projectionMatrix);

            void drawAnotherCube(const MathCore::mat4f &viewMatrix, const MathCore::mat4f &projectionMatrix, AppKit::OpenGL::GLCubeMap *cubeMap);
        };

    }

}