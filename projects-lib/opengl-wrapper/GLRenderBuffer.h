#ifndef GLRenderBuffer_H
#define GLRenderBuffer_H

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/PlatformGL.h>
#include <vector>

namespace openglWrapper {

    //used for depth rendering on FBO
    /// \brief Handle OpenGL a render buffer
    ///
    /// This class was created to be possible com create a
    /// depth buffer attached to a framebuffer object in devices
    /// that doesn't have float texture support.
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE GLRenderBuffer {
    public:
        int width, height;
        GLuint mRenderbuffer;
        GLRenderBuffer();
        //GL_DEPTH_COMPONENT24 or GL_DEPTH24_STENCIL8
        void setSize(int w, int h,GLenum internalformat =  GL_DEPTH_COMPONENT24);
        void enable();
        void disable();
        virtual ~GLRenderBuffer();
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
