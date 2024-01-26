#include <appkit-gl-base/GLRenderBuffer.h>

namespace AppKit
{

    namespace OpenGL
    {

        GLRenderBuffer::GLRenderBuffer(int _w, int _h, GLenum _internalformat)
        {
            width = 0;
            height = 0;

            internalformat = _internalformat;
            is_depth_stencil = false;

            OPENGL_CMD(glGenRenderbuffers(1, &mRenderbuffer));

            if (_w != 0 && _h != 0 && _internalformat != 0xffffffff)
            {
                setSize(_w, _h, _internalformat);
            }
        }

        // GL_DEPTH_COMPONENT24 or GL_DEPTH24_STENCIL8
        void GLRenderBuffer::setSize(int w, int h, GLenum _internalformat)
        {
            if (width == w && height == h)
                return;
            width = w;
            height = h;
            if (_internalformat != 0xffffffff)
                internalformat = _internalformat;

            is_depth_stencil = internalformat != GL_DEPTH_COMPONENT16 && internalformat != GL_DEPTH_COMPONENT24 && internalformat != GL_DEPTH_COMPONENT32;

            enable();
            OPENGL_CMD(glRenderbufferStorage(GL_RENDERBUFFER, internalformat, w, h));
            disable();
        }

        void GLRenderBuffer::enable()
        {
            OPENGL_CMD(glBindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer));
        }

        void GLRenderBuffer::disable()
        {
            OPENGL_CMD(glBindRenderbuffer(GL_RENDERBUFFER, 0));
        }

        GLRenderBuffer::~GLRenderBuffer()
        {
            OPENGL_CMD(glDeleteRenderbuffers(1, &mRenderbuffer));
        }
    }
}
