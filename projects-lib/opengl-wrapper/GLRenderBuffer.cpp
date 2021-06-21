#include "GLRenderBuffer.h"

namespace openglWrapper {

    GLRenderBuffer::GLRenderBuffer() {
        width = 0;
        height = 0;
        OPENGL_CMD(glGenRenderbuffers(1, &mRenderbuffer));
    }

    //GL_DEPTH_COMPONENT24 or GL_DEPTH24_STENCIL8
    void GLRenderBuffer::setSize(int w, int h,GLenum internalformat) {
        if ( width==w && height==h)
            return;
        width = w;
        height = h;
        enable();
        OPENGL_CMD(glRenderbufferStorage(GL_RENDERBUFFER, internalformat, w, h));
        disable();
    }

    void GLRenderBuffer::enable(){
        OPENGL_CMD(glBindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer));
    }

    void GLRenderBuffer::disable() {
        OPENGL_CMD(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }

    GLRenderBuffer::~GLRenderBuffer(){
        OPENGL_CMD(glDeleteRenderbuffers(1, &mRenderbuffer));
    }
}
