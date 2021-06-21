#include "GLDynamicFBO.h"

namespace openglWrapper {

    GLuint FBO_ATTACHMENT_ENUM[16] = {
        GL_COLOR_ATTACHMENT0_EXT,
        GL_COLOR_ATTACHMENT1_EXT,
        GL_COLOR_ATTACHMENT2_EXT,
        GL_COLOR_ATTACHMENT3_EXT,
        GL_COLOR_ATTACHMENT4_EXT,
        GL_COLOR_ATTACHMENT5_EXT,
        GL_COLOR_ATTACHMENT6_EXT,
        GL_COLOR_ATTACHMENT7_EXT,
        GL_COLOR_ATTACHMENT8_EXT,
        GL_COLOR_ATTACHMENT9_EXT,
        GL_COLOR_ATTACHMENT10_EXT,
        GL_COLOR_ATTACHMENT11_EXT,
        GL_COLOR_ATTACHMENT12_EXT,
        GL_COLOR_ATTACHMENT13_EXT,
        GL_COLOR_ATTACHMENT14_EXT,
        GL_COLOR_ATTACHMENT15_EXT
    };

    //private copy constructores, to avoid copy...
    GLDynamicFBO::GLDynamicFBO(const GLDynamicFBO& v){}
    void GLDynamicFBO::operator=(const GLDynamicFBO& v){}

    void GLDynamicFBO::setSize(int w,int h,
                GLuint colorFormat,
                GLenum depthFormat
                ){
        width = w;
        height = h;
        for (int i = 0; i < 16; i++) {
            if (colorAttachment[i] != NULL)
                colorAttachment[i]->setSize(w, h, colorFormat);

            if (cubeAttachment[i] != NULL)
                cubeAttachment[i]->setSizeAndFormat(w, h, colorFormat);
        }
        if (depthRenderBuffer != NULL)
            depthRenderBuffer->setSize(w,h,depthFormat);
        if (depthTexture != NULL)
            depthTexture->setSize(w,h,depthFormat);
    }

    GLDynamicFBO::GLDynamicFBO(){
        ARIBEIRO_ABORT(!GLEW_ARB_framebuffer_object,"ARB_framebuffer_object not supported.");
        OPENGL_CMD(glGenFramebuffersEXT(1, &mFBO));

        for(int i=0;i<16;i++)
            colorAttachment[i] = NULL;

        for (int i = 0; i < 16; i++)
            cubeAttachment[i] = NULL;

        depthRenderBuffer = NULL;
        depthTexture = NULL;

        width = 0;
        height = 0;
    }
    GLDynamicFBO::~GLDynamicFBO(){
        if (glIsFramebufferEXT(mFBO))
            glDeleteFramebuffersEXT(1, &mFBO);
        mFBO = 0;
    }

    void GLDynamicFBO::setDrawBufferCount() {
        //if (GLEW_ARB_draw_buffers)
        {
            int colorCount = 0;
            for(int i=0;i<16;i++){
                if (colorAttachment[i] == NULL && cubeAttachment[i] == NULL)
                    break;
                colorCount++;
            }
            /*
            if (colorCount == 0){
                OPENGL_CMD(glDrawBuffer(GL_NONE));
            } else {
                OPENGL_CMD(glDrawBuffer(FBO_ATTACHMENT_ENUM[colorCount]));
                OPENGL_CMD(glDrawBuffers(colorCount, FBO_ATTACHMENT_ENUM));
            }
            */
            if (GLEW_ARB_draw_buffers)
                OPENGL_CMD(glDrawBuffers(colorCount, FBO_ATTACHMENT_ENUM));
        }
    }

    void GLDynamicFBO::setColorAttachment(GLTexture *texture, int color_index){
        #ifndef ARIBEIRO_RPI
            int max_color_attachment;
            OPENGL_CMD(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_color_attachment));
            ARIBEIRO_ABORT((color_index >= max_color_attachment),"Trying to write to color buffer %i. Max Color attachment is: %i.\n",color_index, max_color_attachment);
        #endif
        if (texture == NULL){
            enable();
            OPENGL_CMD(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, FBO_ATTACHMENT_ENUM[color_index], GL_TEXTURE_2D, 0, 0));
            colorAttachment[color_index] = texture;
            setDrawBufferCount();
            //checkAttachment();
            //disable();
            return;
        }
        enable();
        OPENGL_CMD(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, FBO_ATTACHMENT_ENUM[color_index], GL_TEXTURE_2D, texture->mTexture, 0));
        colorAttachment[color_index] = texture;
        setDrawBufferCount();
        checkAttachment();
        disable();
    }

    void GLDynamicFBO::setColorAttachmentCube(GLCubeMap *texture,GLenum side, int color_index) {
        #ifndef ARIBEIRO_RPI
        int max_color_attachment;
        OPENGL_CMD(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_color_attachment));
        ARIBEIRO_ABORT((color_index >= max_color_attachment),"Trying to write to color buffer %i. Max Color attachment is: %i.\n",color_index, max_color_attachment);
        #endif
        if (texture == NULL){
            //enable();
            OPENGL_CMD(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, FBO_ATTACHMENT_ENUM[color_index], side, 0, 0));
            cubeAttachment[color_index] = NULL;
            setDrawBufferCount();
            //checkAttachment();
            //disable();
            return;
        }
        //enable();
        OPENGL_CMD(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, FBO_ATTACHMENT_ENUM[color_index], side, texture->mCube, 0));
        cubeAttachment[color_index] = texture;
        setDrawBufferCount();
        checkAttachment();
        //disable();
    }

    void GLDynamicFBO::setDepthRenderBufferAttachment(GLRenderBuffer *renderBuffer, bool depth_stencil){
        if (renderBuffer == NULL){
            //enable();
            #ifndef ARIBEIRO_RPI
            if (depth_stencil)
                OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0));
            else
            #endif
                OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
            depthRenderBuffer = renderBuffer;
            setDrawBufferCount();
            //checkAttachment();
            //disable();
            return;
        }
        //enable();
        #ifndef ARIBEIRO_RPI
        if (depth_stencil)
            OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->mRenderbuffer));
        else
        #endif
            OPENGL_CMD(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->mRenderbuffer));
        depthRenderBuffer = renderBuffer;
        //setDrawBufferCount();
        //checkAttachment();
        //disable();
    }

    void GLDynamicFBO::setDepthTextureAttachment(GLTexture *texture, bool depth_stencil){
        if (texture == NULL){
            //enable();
            #ifndef ARIBEIRO_RPI
            if (depth_stencil)
                OPENGL_CMD(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0));
            else
            #endif
                OPENGL_CMD(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0));
            depthTexture = texture;
            setDrawBufferCount();
            //checkAttachment();
            //disable();
            return;
        }
        //enable();
        #ifndef ARIBEIRO_RPI
        if (depth_stencil)
            OPENGL_CMD(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->mTexture, 0));
        else
        #endif
            OPENGL_CMD(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture->mTexture, 0));
        depthTexture = texture;
        setDrawBufferCount();
        //checkAttachment();
        //disable();
    }

    void GLDynamicFBO::checkAttachment(){
        //check the fbo attachment status...
        GLenum status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        switch (status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            //printf("Framebuffer OK!!\n");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            ARIBEIRO_ABORT(true,"Unsupported framebuffer format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            ARIBEIRO_ABORT(true,"Framebuffer incomplete, missing attachment\n");
            break;
        //case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        case 0x8CD8:
            ARIBEIRO_ABORT(true,"Framebuffer incomplete, duplicate attachment\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            ARIBEIRO_ABORT(true,"Framebuffer incomplete, attached images must have same dimensions\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            ARIBEIRO_ABORT(true,"Framebuffer incomplete, attached images must have same format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            ARIBEIRO_ABORT(true,"Framebuffer incomplete, missing draw buffer\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            ARIBEIRO_ABORT(true,"Framebuffer incomplete, missing read buffer\n");
            break;
        default:
            ARIBEIRO_ABORT(true,"Opengl Call ERROR!!!");
            break;
        }
    }

    void GLDynamicFBO::enable(){
        OPENGL_CMD(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFBO));
    }

    void GLDynamicFBO::disable(){
        OPENGL_CMD(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0));
    }
}
