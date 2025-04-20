#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>

//#include <aRibeiroCore/aRibeiroCore.h>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLCubeMap.h>
#include <vector>

#include "GLRenderBuffer.h"

namespace AppKit
{

    namespace OpenGL
    {

    /// \brief Wrapper to Framebuffer Object
    ///
    /// This component handle the framebuffer object.
    ///
    /// The dynamic, is for the capacity to attach and detach render targets on the fly.
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLDynamicFBO {

        
    public:

        //deleted copy constructor and assign operator, to avoid copy...
        GLDynamicFBO(const GLDynamicFBO& v) = delete;
        GLDynamicFBO& operator=(const GLDynamicFBO& v) = delete;

        int width;///< current width of this FBO
        int height;///< current height of this FBO
        GLuint mFBO;

        GLCubeMap *cubeAttachment[16];
        GLenum cubeAttachment_side[16];
        GLint cubeAttachment_mip[16];

        GLTexture *colorAttachment[16];
        GLint colorAttachment_mip[16];
        
        GLRenderBuffer *depthRenderBuffer;

        GLTexture *depthTexture;
        GLint depthTexture_mip;


        void setDrawBufferCount(GLsizei count);
        void checkAttachment();

        /// \brief Set the size of the all internal textures attached to this FBO.
        ///
        /// \author Alessandro Ribeiro
        /// \param w width
        /// \param h height
        ///
        void setSize(int w,int h);

        GLDynamicFBO();
        ~GLDynamicFBO();
        
        
        /// \brief Set a color attachment to point to a texture
        ///
        /// \author Alessandro Ribeiro
        /// \param texture the texture to setup
        /// \param color_index the target color attachment index
        ///
        void setColorAttachment(GLTexture *texture, int color_index, GLint mip_level = 0);
        
        /// \brief Set a color attachment to point to a cube texture
        ///
        /// does not affect colorAttachment
        ///
        /// \author Alessandro Ribeiro
        /// \param texture the texture to setup
        /// \param side GL_TEXTURE_CUBE_MAP_POSITIVE_X, etc...
        /// \param color_index the target color attachment index
        ///
        void setColorAttachmentCube(GLCubeMap *texture,GLenum side, int color_index, GLint mip_level = 0);

        /// \brief Set a depth attachment to point to a render buffer
        ///
        /// \author Alessandro Ribeiro
        /// \param renderBuffer the render buffer to setup
        /// \param depth_stencil if true, setup GL_DEPTH24_STENCIL8 internal format.
        ///
        void setDepthRenderBufferAttachment(GLRenderBuffer *renderBuffer);

        /// \brief Set a depth attachment to point to a float texture
        ///
        /// \author Alessandro Ribeiro
        /// \param texture the texture to setup
        /// \param depth_stencil if true, setup GL_DEPTH24_STENCIL8 internal format.
        ///
        void setDepthTextureAttachment(GLTexture *texture, GLint mip_level = 0);

        bool isEnabled();

        bool isEnabledReading();
        bool isEnabledDrawing();

        void enableReading();
        void enableDrawing();

        static void disableReading();
        static void disableDrawing();

        /// \brief Set the current FBO to draw to this color attachment set
        ///
        /// \author Alessandro Ribeiro
        ///
        void enable();

        /// \brief disable FBO
        ///
        /// \author Alessandro Ribeiro
        ///
        static void disable();

        TextureBuffer readPixels(int attachmentIndex = 0, TextureBuffer *output = nullptr);

        void blitFrom(GLDynamicFBO *sourceFBO, GLint sourceColorIndex, GLint targetColorIndex, 
            GLbitfield bufferToBlitBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, 
            GLenum filter = GL_NEAREST) ;
        
        void blitFromBackBuffer(GLint targetColorIndex, 
            GLbitfield bufferToBlitBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, 
            GLenum filter = GL_NEAREST);

        void blitToBackBuffer(GLint sourceColorIndex, 
            GLbitfield bufferToBlitBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, 
            GLenum filter = GL_NEAREST);

        
    };

}

}