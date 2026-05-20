#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>

#include <appkit-gl-base/GLTexture.h>
// #include <glew/glew.h>
#include <vector>

namespace AppKit
{

    namespace OpenGL
    {

        // uses the following extensions:
        //    GLAD_GL_ARB_framebuffer_object,
        //    GLAD_GL_ARB_draw_buffers,
        //    GLAD_GL_ARB_depth_texture
        //    GLAD_GL_ARB_texture_non_power_of_two

        /// \brief Wrapper to Framebuffer Object
        ///
        /// To use this component, you need to setup all textures (color or depth).
        ///
        /// After that call #attachTextures and you can use this framebuffer and the textures.
        ///
        /// Example:
        ///
        /// \code
        /// #include <appkit-gl-base/opengl-wrapper.h>
        /// using namespace AppKit::OpenGL;
        ///
        /// ...
        ///
        /// GLFramebufferObject fbo = new GLFramebufferObject();
        ///
        /// // Setup the color attachments and depth mode
        /// fbo->useRenderbufferDepth = true;
        /// fbo->color.push_back(new GLTexture());
        ///
        /// // resize all color and depth attachments
        /// fbo->setSize(512, 512);
        ///
        /// // construct fbo and setup attachments
        /// fbo->attachTextures();
        ///
        /// // setup for draw
        /// fbo->enable();
        /// glViewport(0,0,fbo->width,fbo->height);
        ///
        /// // draw code
        /// ...
        ///
        /// // close fbo
        /// GLFramebufferObject::disable();
        /// glViewport(0,0,screen_width,screen_height);
        ///
        /// // use the textures from this fbo in the main draw routine
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLFramebufferObject
        {

            GLuint mFbo;
            bool initialized;

            GLuint mDepthRenderbuffer;

        public:

            //deleted copy constructor and assign operator, to avoid copy...
            GLFramebufferObject(const GLFramebufferObject &v) = delete;
            GLFramebufferObject& operator=(const GLFramebufferObject &v) = delete;

            std::vector<GLTexture *> color;   ///< Color textures to setup color attachments
            GLTexture *depth;                 ///< if set, tries to setup depth texture as attachment
            bool useRenderbufferDepth;        ///< generate a depth render buffer and attach to this framebuffer object
            bool useSharedRenderbufferDepth;  ///< allow use framebuffer object without depth textures by sharing a render buffer
            GLuint sharedDepthRenderbufferID; ///< use it if useSharedRenderbufferDepth is true
            GLTexture *stencil;               ///< if set, tries to setup stencil texture as attachment

            int width;  ///< current width of this FBO
            int height; ///< current height of this FBO

            GLFramebufferObject();
            ~GLFramebufferObject();

            /// \brief Get generated depth render buffer
            ///
            /// \author Alessandro Ribeiro
            ///
            GLuint getDepthRenderBufferID();

            /// \brief Setup and attach all parameters to create this FBO
            ///
            /// It must call this method after creating the color and depth attachments.
            ///
            /// \author Alessandro Ribeiro
            ///
            void attachTextures();

            // GL_RGBA, etc...
            // GL_RGBA32F_ARB, GL_RGB32F_ARB, GL_RGBA16F_ARB, GL_RGB16F_ARB
            //
            /// \brief Set internal texture sizes
            ///
            /// It is possible to setup the input textures and the depth format.
            ///
            /// \author Alessandro Ribeiro
            /// \param w width
            /// \param h height
            /// \param colorFormat color attachment array format
            /// \param depthFormat depth attachment format
            /// \param stencilFormat stencil attachment format
            ///
            void setSize(int w, int h, GLuint colorFormat = GL_RGB,
                         GLuint depthFormat = GL_DEPTH_COMPONENT24,
                         GLuint stencilFormat = GL_STENCIL_INDEX8);

            /// \brief Set the OpenGL rendering to this FBO
            ///
            /// \author Alessandro Ribeiro
            ///
            void enable();

            /// \brief Set the OpenGL rendering to the default output device
            ///
            /// \author Alessandro Ribeiro
            ///
            static void disable();

            /// \brief Get the current target depth bits
            ///
            /// \author Alessandro Ribeiro
            /// \return current target depth bits
            ///
            static int currentDepthBits();

            /// \brief Get the maximum color attachments
            ///
            /// \author Alessandro Ribeiro
            /// \return maximum color attachments
            ///
            static int maxColorAttachments();

            /// \brief Get the maximum draw buffers
            ///
            /// \author Alessandro Ribeiro
            /// \return maximum draw buffers
            ///
            static int maxDrawBuffers();
        };

    }

    /*bool isPowerOf2(int num){
      if (num == 0) return false;
      while((num&0x01)!=1)
        num = num >> 1;
      return ((num&(~0x01))==0) ;
    }*/

}