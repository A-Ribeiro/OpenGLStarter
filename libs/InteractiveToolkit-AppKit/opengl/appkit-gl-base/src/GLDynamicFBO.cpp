#include <appkit-gl-base/GLDynamicFBO.h>

namespace AppKit
{

    namespace OpenGL
    {

        const GLuint FBO_ATTACHMENT_ENUM[16] = {
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

        void GLDynamicFBO::setSize(int w, int h)
        {
            width = w;
            height = h;
            for (int i = 0; i < 16; i++)
            {
                if (colorAttachment[i] != nullptr)
                    colorAttachment[i]->setSize(w, h);

                if (cubeAttachment[i] != nullptr)
                    cubeAttachment[i]->setSize(w, h);
            }
            if (depthRenderBuffer != nullptr)
                depthRenderBuffer->setSize(w, h);
            if (depthTexture != nullptr)
                depthTexture->setSize(w, h);
        }

        GLDynamicFBO::GLDynamicFBO()
        {
#if !defined(GLAD_GLES2)
            ITK_ABORT(!GLAD_GL_ARB_framebuffer_object, "ARB_framebuffer_object not supported.");
#endif
            OPENGL_CMD(glGenFramebuffers(1, &mFBO));

            for (int i = 0; i < 16; i++)
                colorAttachment[i] = nullptr;

            for (int i = 0; i < 16; i++)
                cubeAttachment[i] = nullptr;

            depthRenderBuffer = nullptr;
            depthTexture = nullptr;

            width = 0;
            height = 0;
        }
        GLDynamicFBO::~GLDynamicFBO()
        {
            if (glIsFramebuffer(mFBO))
            {
                if (!GLAD_GL_EXT_framebuffer_blit)
                {
                    if (isEnabled())
                        GLDynamicFBO::disable();
                }
                else
                {
                    if (isEnabledReading())
                        GLDynamicFBO::disableReading();
                    if (isEnabledDrawing())
                        GLDynamicFBO::disableDrawing();
                }
                glDeleteFramebuffers(1, &mFBO);
            }
            mFBO = 0;
        }

        void GLDynamicFBO::setDrawBufferCount(GLsizei colorCount)
        {
            if (GLAD_GL_ARB_draw_buffers)
                OPENGL_CMD(glDrawBuffers(colorCount, FBO_ATTACHMENT_ENUM));
        }

        void GLDynamicFBO::setColorAttachment(GLTexture *texture, int color_index, GLint mip_level)
        {
#ifndef ITK_RPI
            int max_color_attachment;
            OPENGL_CMD(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachment));
            ITK_ABORT((color_index >= max_color_attachment), "Trying to write to color buffer %i. Max Color attachment is: %i.\n", color_index, max_color_attachment);
#endif

            if (texture == nullptr)
                OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, FBO_ATTACHMENT_ENUM[color_index], GL_TEXTURE_2D, 0, 0));
            else
            {
                texture->getMipResolution(&mip_level, &width, &height);
                OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, FBO_ATTACHMENT_ENUM[color_index], GL_TEXTURE_2D, texture->mTexture, mip_level));
                // width = texture->width;
                // height = texture->height;
                colorAttachment_mip[color_index] = mip_level;
            }
            colorAttachment[color_index] = texture;
        }

        void GLDynamicFBO::setColorAttachmentCube(GLCubeMap *texture, GLenum side, int color_index, GLint mip_level)
        {
#ifndef ITK_RPI
            int max_color_attachment;
            OPENGL_CMD(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachment));
            ITK_ABORT((color_index >= max_color_attachment), "Trying to write to color buffer %i. Max Color attachment is: %i.\n", color_index, max_color_attachment);
#endif

            if (texture == nullptr)
                OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, FBO_ATTACHMENT_ENUM[color_index], side, 0, 0));
            else
            {
                texture->getMipResolution(&mip_level, &width, &height);
                OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, FBO_ATTACHMENT_ENUM[color_index], side, texture->mCube, mip_level));

                // width = texture->width;
                // height = texture->height;

                cubeAttachment_side[color_index] = side;
                cubeAttachment_mip[color_index] = mip_level;
            }
            cubeAttachment[color_index] = texture;
        }

        void GLDynamicFBO::setDepthRenderBufferAttachment(GLRenderBuffer *renderBuffer)
        {

            if (renderBuffer == nullptr)
            {
#ifndef ITK_RPI
                if (depthRenderBuffer != nullptr && depthRenderBuffer->is_depth_stencil)
                    OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0));
                else
#endif
                    OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
            }
            else
            {
#ifndef ITK_RPI
                if (renderBuffer->is_depth_stencil)
                    OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->mRenderbuffer));
                else
#endif
                    OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->mRenderbuffer));
            }
            depthRenderBuffer = renderBuffer;
        }

        void GLDynamicFBO::setDepthTextureAttachment(GLTexture *texture, GLint mip_level)
        {
            if (texture == nullptr)
            {
#ifndef ITK_RPI
                if (depthTexture != nullptr && depthTexture->is_depth_stencil)
                    OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0));
                else
#endif
                    OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0));
            }
            else
            {
#ifndef ITK_RPI
                if (texture->is_depth_stencil)
                    OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->mTexture, mip_level));
                else
#endif
                    OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->mTexture, mip_level));

                depthTexture_mip = mip_level;
            }
            depthTexture = texture;
        }

        void GLDynamicFBO::checkAttachment()
        {
            // check the fbo attachment status...
            GLenum status = (GLenum)glCheckFramebufferStatus(GL_FRAMEBUFFER);
            switch (status)
            {
            case GL_FRAMEBUFFER_COMPLETE:
                // printf("Framebuffer OK!!\n");
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                ITK_ABORT(true, "Unsupported framebuffer format\n");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                ITK_ABORT(true, "Framebuffer incomplete, missing attachment\n");
                break;
            // case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
            case 0x8CD8:
                ITK_ABORT(true, "Framebuffer incomplete, duplicate attachment\n");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                ITK_ABORT(true, "Framebuffer incomplete, attached images must have same dimensions\n");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                ITK_ABORT(true, "Framebuffer incomplete, attached images must have same format\n");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                ITK_ABORT(true, "Framebuffer incomplete, missing draw buffer\n");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                ITK_ABORT(true, "Framebuffer incomplete, missing read buffer\n");
                break;
            default:
                ITK_ABORT(true, "Opengl Call ERROR!!!");
                break;
            }
        }

        bool GLDynamicFBO::isEnabledReading()
        {
            GLint last_read_framebuffer;
            OPENGL_CMD(glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &last_read_framebuffer));
            return (last_read_framebuffer == mFBO);
        }

        bool GLDynamicFBO::isEnabledDrawing()
        {
            GLint last_draw_framebuffer;
            OPENGL_CMD(glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_draw_framebuffer));
            return (last_draw_framebuffer == mFBO);
        }

        void GLDynamicFBO::enableReading()
        {
            OPENGL_CMD(glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO));
        }
        void GLDynamicFBO::enableDrawing()
        {
            OPENGL_CMD(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO));
        }

        void GLDynamicFBO::disableReading()
        {
            OPENGL_CMD(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
        }
        void GLDynamicFBO::disableDrawing()
        {
            OPENGL_CMD(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        }

        bool GLDynamicFBO::isEnabled()
        {

            if (!GLAD_GL_EXT_framebuffer_blit)
            {
                // single FBO reading
                GLint currentFBO;
                OPENGL_CMD(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO));

                return currentFBO == mFBO;
            }
            else
            {
                GLint last_read_framebuffer;
                GLint last_draw_framebuffer;
                OPENGL_CMD(glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &last_read_framebuffer));
                OPENGL_CMD(glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_draw_framebuffer));

                return (last_read_framebuffer == mFBO || last_draw_framebuffer == mFBO);
            }
        }

        void GLDynamicFBO::enable()
        {
            OPENGL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, mFBO));
        }

        void GLDynamicFBO::disable()
        {
            OPENGL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }

        TextureBuffer GLDynamicFBO::readPixels(int attachmentIndex, TextureBuffer *output)
        {
            TextureBuffer result;

            if (colorAttachment[attachmentIndex] != nullptr)
            {
                GLTexture *tex = colorAttachment[attachmentIndex];
                GLint mip = colorAttachment_mip[attachmentIndex];
                GLint w = tex->width >> mip;
                GLint h = tex->height >> mip;
                if (w == 0)
                    w = 1;
                if (h == 0)
                    h = 1;
                uint32_t size_bytes = w * h * tex->input_raw_element_size;

                if (output != nullptr)
                {
                    if (output->data_size_bytes == size_bytes)
                    {
                        result = *output;
                    }
                    else
                    {
                        output->dispose();
                        result = TextureBuffer(tex->input_data_type, size_bytes);
                    }
                }
                else
                {
                    result = TextureBuffer(tex->input_data_type, size_bytes);
                }

                result.input_data_type = tex->input_data_type;
                result.width = w;
                result.height = h;
                result.input_component_count = tex->input_component_count;
                result.input_raw_element_size = tex->input_raw_element_size;

                OPENGL_CMD(glReadBuffer(FBO_ATTACHMENT_ENUM[attachmentIndex]));
                OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, tex->input_alignment));
                OPENGL_CMD(glReadPixels(0, 0, result.width, result.height, tex->input_format, tex->input_data_type, result.data));
                OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 4));
            }
            else if (cubeAttachment[attachmentIndex] != nullptr)
            {
                GLCubeMap *cube = cubeAttachment[attachmentIndex];
                GLint mip = cubeAttachment_mip[attachmentIndex];
                GLint w = cube->width >> mip;
                GLint h = cube->height >> mip;
                if (w == 0)
                    w = 1;
                if (h == 0)
                    h = 1;
                uint32_t size_bytes = w * h * cube->input_raw_element_size;

                if (output != nullptr)
                {
                    if (output->data_size_bytes == size_bytes)
                    {
                        result = *output;
                    }
                    else
                    {
                        output->dispose();
                        result = TextureBuffer(cube->input_data_type, size_bytes);
                    }
                }
                else
                {
                    result = TextureBuffer(cube->input_data_type, size_bytes);
                }

                result.input_data_type = cube->input_data_type;
                result.width = w;
                result.height = h;
                result.input_component_count = cube->input_component_count;
                result.input_raw_element_size = cube->input_raw_element_size;

                OPENGL_CMD(glReadBuffer(FBO_ATTACHMENT_ENUM[attachmentIndex]));
                OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, cube->input_alignment));
                OPENGL_CMD(glReadPixels(0, 0, result.width, result.height, cube->input_format, cube->input_data_type, result.data));
                OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 4));
            }
            else
            {
                ITK_ABORT(true, "Trying to read buffer nullptr from FBO...");
            }

            if (output != nullptr)
                *output = result;

            return result;
        }

        void GLDynamicFBO::blitFrom(GLDynamicFBO *sourceFBO,
                                    GLint sourceColorIndex, GLint targetColorIndex,
                                    GLbitfield bufferToBlitBits, GLenum filter)
        {
            ITK_ABORT(!GLAD_GL_EXT_framebuffer_blit, "GL_EXT_framebuffer_blit not found");

            GLint last_read_framebuffer;
            GLint last_draw_framebuffer;
            glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &last_read_framebuffer);
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_draw_framebuffer);

            OPENGL_CMD(glBindFramebuffer(GL_READ_FRAMEBUFFER, sourceFBO->mFBO));
            OPENGL_CMD(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO));

            OPENGL_CMD(glReadBuffer(FBO_ATTACHMENT_ENUM[sourceColorIndex]));
            OPENGL_CMD(glDrawBuffer(FBO_ATTACHMENT_ENUM[targetColorIndex]));

            OPENGL_CMD(glBlitFramebuffer(0, 0, sourceFBO->width, sourceFBO->height, 0, 0, width, height, bufferToBlitBits, filter));

            OPENGL_CMD(glBindFramebuffer(GL_READ_FRAMEBUFFER, last_read_framebuffer));
            OPENGL_CMD(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, last_draw_framebuffer));
        }

        void GLDynamicFBO::blitFromBackBuffer(GLint targetColorIndex, GLbitfield bufferToBlitBits, GLenum filter)
        {
            ITK_ABORT(!GLAD_GL_EXT_framebuffer_blit, "GL_EXT_framebuffer_blit not found");

            GLint last_read_framebuffer;
            GLint last_draw_framebuffer;
            glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &last_read_framebuffer);
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_draw_framebuffer);

            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            OPENGL_CMD(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
            OPENGL_CMD(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO));

            OPENGL_CMD(glReadBuffer(GL_BACK));
            OPENGL_CMD(glDrawBuffer(FBO_ATTACHMENT_ENUM[targetColorIndex]));

            OPENGL_CMD(glBlitFramebuffer(viewport[0], viewport[1], viewport[2], viewport[3], 0, 0, width, height, bufferToBlitBits, filter));

            OPENGL_CMD(glBindFramebuffer(GL_READ_FRAMEBUFFER, last_read_framebuffer));
            OPENGL_CMD(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, last_draw_framebuffer));
        }

        void GLDynamicFBO::blitToBackBuffer(GLint sourceColorIndex, GLbitfield bufferToBlitBits, GLenum filter)
        {
            ITK_ABORT(!GLAD_GL_EXT_framebuffer_blit, "GL_EXT_framebuffer_blit not found");

            GLint last_read_framebuffer;
            GLint last_draw_framebuffer;
            glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &last_read_framebuffer);
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_draw_framebuffer);

            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            OPENGL_CMD(glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO));
            OPENGL_CMD(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

            OPENGL_CMD(glReadBuffer(FBO_ATTACHMENT_ENUM[sourceColorIndex]));
            OPENGL_CMD(glDrawBuffer(GL_BACK));

            OPENGL_CMD(glBlitFramebuffer(0, 0, width, height, viewport[0], viewport[1], viewport[2], viewport[3], bufferToBlitBits, filter));

            OPENGL_CMD(glBindFramebuffer(GL_READ_FRAMEBUFFER, last_read_framebuffer));
            OPENGL_CMD(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, last_draw_framebuffer));
        }

    }

}
