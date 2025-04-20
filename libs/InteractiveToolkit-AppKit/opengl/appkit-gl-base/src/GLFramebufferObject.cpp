#include <appkit-gl-base/GLFramebufferObject.h>
#include <appkit-gl-base/platform/PlatformGL.h>

#include <stdio.h>
#include <stdlib.h>

namespace AppKit
{

    namespace OpenGL
    {

        // uses the following extensions:
        //    GLAD_GL_ARB_framebuffer_object,
        //    GLAD_GL_ARB_draw_buffers,
        //    GLAD_GL_ARB_framebuffer_no_attachments,
        //    GLAD_GL_ARB_depth_texture
        //    GLAD_GL_ARB_texture_non_power_of_two

        GLuint DrawBuffersUnit[16] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4,
            GL_COLOR_ATTACHMENT5,
            GL_COLOR_ATTACHMENT6,
            GL_COLOR_ATTACHMENT7,
            GL_COLOR_ATTACHMENT8,
            GL_COLOR_ATTACHMENT9,
            GL_COLOR_ATTACHMENT10,
            GL_COLOR_ATTACHMENT11,
            GL_COLOR_ATTACHMENT12,
            GL_COLOR_ATTACHMENT13,
            GL_COLOR_ATTACHMENT14,
            GL_COLOR_ATTACHMENT15};

        GLFramebufferObject::GLFramebufferObject()
        {

            ITK_ABORT(!GLAD_GL_ARB_framebuffer_object, "ARB_framebuffer_object not supported.");

            initialized = false;
            depth = nullptr;
            stencil = nullptr;
            mFbo = 0;
            width = 0;
            height = 0;

            useRenderbufferDepth = false;
            mDepthRenderbuffer = 0;

            useSharedRenderbufferDepth = false;
            sharedDepthRenderbufferID = 0;

            OPENGL_CMD(glGenFramebuffers(1, &mFbo));
        }

        GLFramebufferObject::~GLFramebufferObject()
        {

            if (useRenderbufferDepth && mDepthRenderbuffer != 0)
            {
                OPENGL_CMD(glDeleteRenderbuffers(1, &mDepthRenderbuffer));
                mDepthRenderbuffer = 0;
            }

            if (glIsFramebuffer(mFbo))
                glDeleteFramebuffers(1, &mFbo);
            mFbo = 0;
        }

        GLuint GLFramebufferObject::getDepthRenderBufferID()
        {
            return mDepthRenderbuffer;
        }

        //
        // attach the textures set and depth
        //
        void GLFramebufferObject::attachTextures()
        {
            if (initialized)
                return;
            initialized = true;

            // check render buffer creation
            if (useRenderbufferDepth)
            {
                OPENGL_CMD(glGenRenderbuffers(1, &mDepthRenderbuffer));
                OPENGL_CMD(glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer));
                OPENGL_CMD(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height));
                // OPENGL_CMD(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
                OPENGL_CMD(glBindRenderbuffer(GL_RENDERBUFFER, 0));
            }

            int max_value = maxDrawBuffers();
            if (maxColorAttachments() > max_value)
                max_value = maxColorAttachments();

            ITK_ABORT(
                ((int)color.size() > max_value),
                "Trying to use more draw buffers or color attachments than the current hardware support. \n"
                "(color buffer size: %i, max draw buffers: %i, max color attachment: %i)\n",
                (int)color.size(), maxDrawBuffers(), maxColorAttachments());

            // if ((int)color.size() > maxDrawBuffers()) {
            //     fprintf(stderr,
            //         "[GLFramebufferObject] Trying to use more draw buffers than the current hardware support. \n"
            //         "                      (color buffer size: %lu, max draw buffers: %i)\n", color.size(), maxDrawBuffers());
            //     exit(-1);
            // }

            ITK_ABORT(
                (width == 0 || height == 0),
                "You need to call setSize before attachTextures. \n");

            // if (width == 0 || height == 0) {
            //     fprintf(stderr, "[GLFramebufferObject] You need to call setSize before attachTextures. \n" );
            //     exit(-1);
            // }

            ITK_ABORT(
                (depth == nullptr && color.size() == 0),
                "Cannot create a framebuffer object without any texture. \n");

            // if (depth == nullptr && color.size() == 0) {
            //     fprintf(stderr, "[GLFramebufferObject] Cannot create a framebuffer object without any texture. \n");
            //     exit(-1);
            // }

            // force create 32x32 texture, if texture is not initialized
            // setSize(32,32);

            OPENGL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, mFbo));

            for (int i = 0; i < (int)color.size(); i++)
                OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, DrawBuffersUnit[i], GL_TEXTURE_2D, color[i]->mTexture, 0));

            if (depth != nullptr)
            {
                OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->mTexture, 0));
            }

            if (stencil != nullptr)
            {
                OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil->mTexture, 0));
            }

            if (useRenderbufferDepth)
                OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer));
            // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
            else if (useSharedRenderbufferDepth)
                OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sharedDepthRenderbufferID));

            // check the fbo attachment status...
            GLenum status = (GLenum)glCheckFramebufferStatus(GL_FRAMEBUFFER);
            switch (status)
            {
            case GL_FRAMEBUFFER_COMPLETE:
                // printf("Framebuffer OK!!\n");
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                ITK_ABORT(
                    true,
                    "Unsupported framebuffer format\n");
                // fprintf(stderr, "Unsupported framebuffer format\n");
                // exit(-1);
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                ITK_ABORT(
                    true,
                    "Framebuffer incomplete, missing attachment\n");
                // fprintf(stderr, "Framebuffer incomplete, missing attachment\n");
                // exit(-1);
                break;
            // case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
            case 0x8CD8:
                ITK_ABORT(
                    true,
                    "Framebuffer incomplete, duplicate attachment\n");
                // fprintf(stderr, "Framebuffer incomplete, duplicate attachment\n");
                // exit(-1);
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                ITK_ABORT(
                    true,
                    "Framebuffer incomplete, attached images must have same dimensions\n");
                // fprintf(stderr, "Framebuffer incomplete, attached images must have same dimensions\n");
                // exit(-1);
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                ITK_ABORT(
                    true,
                    "Framebuffer incomplete, attached images must have same format\n");
                // fprintf(stderr, "Framebuffer incomplete, attached images must have same format\n");
                // exit(-1);
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                ITK_ABORT(
                    true,
                    "Framebuffer incomplete, missing draw buffer\n");
                // fprintf(stderr, "Framebuffer incomplete, missing draw buffer\n");
                // exit(-1);
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                ITK_ABORT(
                    true,
                    "Framebuffer incomplete, missing read buffer\n");
                // fprintf(stderr, "Framebuffer incomplete, missing read buffer\n");
                // exit(-1);
                break;
            default:
                ITK_ABORT(
                    true,
                    "Opengl Call ERROR!!!");
                // fprintf(stderr, "Opengl Call ERROR!!!");
                // exit(-1);
                break;
            }

            if (GLAD_GL_ARB_draw_buffers)
                // set the draw buffers to this FBO
                OPENGL_CMD(glDrawBuffers((GLsizei)color.size(), DrawBuffersUnit));

            OPENGL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }

        // GL_RGBA, etc...
        // GL_RGBA32F_ARB, GL_RGB32F_ARB, GL_RGBA16F_ARB, GL_RGB16F_ARB
        //
        void GLFramebufferObject::setSize(int w, int h, GLuint colorFormat, GLuint depthFormat, GLuint stencilFormat)
        {
            if (this->width == w && this->height == h)
                return;
            this->width = w;
            this->height = h;
            if (depth != nullptr)
            {
                ITK_ABORT(
                    !GLAD_GL_ARB_depth_texture,
                    "Error, this hardware does not support depth texture...\n");
                // if (!GLAD_GL_ARB_depth_texture){
                //     printf("Error, this hardware does not support depth texture...\n");
                //     exit(-1);
                // }
                depth->setSize(w, h, depthFormat);
            }
            if (stencil != nullptr)
                stencil->setSize(w, h, stencilFormat);
            for (int i = 0; i < (int)color.size(); i++)
                color[i]->setSize(w, h, colorFormat);

            if (useRenderbufferDepth && mDepthRenderbuffer != 0)
            {
                OPENGL_CMD(glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer));
                OPENGL_CMD(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height));
                // OPENGL_CMD(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
                OPENGL_CMD(glBindRenderbuffer(GL_RENDERBUFFER, 0));
            }
        }

        void GLFramebufferObject::enable()
        {
            if (!initialized)
                return;
            OPENGL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, mFbo));
            // OPENGL_CMD(glDrawBuffers(color.size(), DrawBuffersUnit));
        }

        void GLFramebufferObject::disable()
        {
            // error to setup glDrawBuffers without any fbo in the current state set
            // OPENGL_CMD(glDrawBuffers(1, DrawBuffersUnit));
            //
            // ITK_ABORT(!GLAD_GL_ARB_framebuffer_object,"ARB_framebuffer_object not supported.");

            OPENGL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }

        int GLFramebufferObject::currentDepthBits()
        {
            int depthBits;
            OPENGL_CMD(glGetIntegerv(GL_DEPTH_BITS, &depthBits));
            return depthBits;
        }
        int GLFramebufferObject::maxColorAttachments()
        {
            int v;
            OPENGL_CMD(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &v));
            return v;
        }
        int GLFramebufferObject::maxDrawBuffers()
        {
            int v = 1;
            if (GLAD_GL_ARB_draw_buffers)
                OPENGL_CMD(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &v));
            return v;
        }

    }

}
