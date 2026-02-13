#pragma once

#include <string>

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLVertexBufferObject.h>
#include <appkit-gl-base/shaders/GLShaderTextureColor.h>

namespace AppKit
{
    namespace GLEngine
    {

        /// \brief This definition uses 6 plain 2D textures to draw a skybox.
        ///
        /// Example:
        ///
        /// \code
        /// #include <appkit-gl-engine/mini-gl-engine.h>
        /// using namespace AppKit::OpenGL;
        /// using namespace AppKit::GLEngine;
        ///
        /// GLSkyBox *skybox = GLSkyBox(
        ///     true && Engine::Instance()->isRGBCapable, // load sRGB
        ///     "negz.jpg","posz.jpg",
        ///     "negx.jpg","posx.jpg",
        ///     "negy.jpg","posy.jpg",
        ///     100.0f, // distance
        ///     0.0f // rotation
        /// );
        ///
        /// ...
        ///
        /// skybox->draw(viewMatrix,projectionMatrix);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLSkyBox
        {

        public:
            AppKit::OpenGL::GLTexture *back;
            AppKit::OpenGL::GLTexture *front;
            AppKit::OpenGL::GLTexture *left;
            AppKit::OpenGL::GLTexture *right;
            AppKit::OpenGL::GLTexture *bottom;
            AppKit::OpenGL::GLTexture *top;

            std::vector<MathCore::vec3f> vertex;
            std::vector<MathCore::vec2f> uv;

            AppKit::OpenGL::GLShaderTextureColor shader;

            GLSkyBox(bool sRGB,
                     const std::string &_negz, const std::string &_posz,
                     const std::string &_negx, const std::string &_posx,
                     const std::string &_negy, const std::string &_posy,
                     float _distance, float rotation);

            ~GLSkyBox();

            void draw(const MathCore::mat4f &viewMatrix, const MathCore::mat4f &projectionMatrix, bool leftHanded = true);
        };

    }

}