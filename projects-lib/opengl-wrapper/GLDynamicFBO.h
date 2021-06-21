#ifndef GLDynamicFBO_h_
#define GLDynamicFBO_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLCubeMap.h>
#include <opengl-wrapper/PlatformGL.h>
#include <vector>

#include "GLRenderBuffer.h"

namespace openglWrapper {

    /// \brief Wrapper to Framebuffer Object
    ///
    /// This component handle the framebuffer object.
    ///
    /// The dynamic, is for the capacity to attach and detach render targets on the fly.
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE GLDynamicFBO {

        
        //private copy constructores, to avoid copy...
        GLDynamicFBO(const GLDynamicFBO& v);
        void operator=(const GLDynamicFBO& v);

    public:
        int width;///< current width of this FBO
        int height;///< current height of this FBO
        GLuint mFBO;

        GLCubeMap *cubeAttachment[16];
        GLTexture *colorAttachment[16];
        GLRenderBuffer *depthRenderBuffer;
        GLTexture *depthTexture;


        void setDrawBufferCount();
        void checkAttachment();

        /// \brief Set the size of the all internal textures attached to this FBO.
        ///
        /// \author Alessandro Ribeiro
        /// \param w width
        /// \param h height
        /// \param colorFormat the color attachment color format
        /// \param depthFormat the depth attachment color format (either render buffer or float texture)
        ///
        void setSize(int w,int h, 
                    GLuint colorFormat = GL_RGBA ,
                    GLenum depthFormat = GL_DEPTH_COMPONENT24
                    );

        GLDynamicFBO();
        virtual ~GLDynamicFBO();
        
        
        /// \brief Set a color attachment to point to a texture
        ///
        /// \author Alessandro Ribeiro
        /// \param texture the texture to setup
        /// \param color_index the target color attachment index
        ///
        void setColorAttachment(GLTexture *texture, int color_index);
        
        /// \brief Set a color attachment to point to a cube texture
        ///
        /// does not affect colorAttachment
        ///
        /// \author Alessandro Ribeiro
        /// \param texture the texture to setup
        /// \param side GL_TEXTURE_CUBE_MAP_POSITIVE_X, etc...
        /// \param color_index the target color attachment index
        ///
        void setColorAttachmentCube(GLCubeMap *texture,GLenum side, int color_index);

        /// \brief Set a depth attachment to point to a render buffer
        ///
        /// \author Alessandro Ribeiro
        /// \param renderBuffer the render buffer to setup
        /// \param depth_stencil if true, setup GL_DEPTH24_STENCIL8 internal format.
        ///
        void setDepthRenderBufferAttachment(GLRenderBuffer *renderBuffer, bool depth_stencil = false);

        /// \brief Set a depth attachment to point to a float texture
        ///
        /// \author Alessandro Ribeiro
        /// \param texture the texture to setup
        /// \param depth_stencil if true, setup GL_DEPTH24_STENCIL8 internal format.
        ///
        void setDepthTextureAttachment(GLTexture *texture, bool depth_stencil = false);

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

        SSE2_CLASS_NEW_OPERATOR

    }_SSE2_ALIGN_POS;

}

#endif
