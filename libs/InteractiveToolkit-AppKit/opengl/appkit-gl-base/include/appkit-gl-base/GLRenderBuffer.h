#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>


#include <appkit-gl-base/GLTexture.h>

#include <vector>

namespace AppKit
{

    namespace OpenGL
    {

        // used for depth rendering on FBO
        /// \brief Handle OpenGL a render buffer
        ///
        /// This class was created to be possible com create a
        /// depth buffer attached to a framebuffer object in devices
        /// that doesn't have float texture support.
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLRenderBuffer
        {
        public:
            int width, height;
            GLuint mRenderbuffer;
            GLenum internalformat;
            bool is_depth_stencil;

            GLRenderBuffer(int w = 0, int h = 0, GLenum internalformat = GL_DEPTH_COMPONENT24);
            // GL_DEPTH_COMPONENT24 or GL_DEPTH24_STENCIL8
            void setSize(int w, int h, GLenum internalformat = 0xffffffff);
            void enable();
            void disable();
            ~GLRenderBuffer();
        };

    }

}