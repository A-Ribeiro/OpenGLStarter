#include <appkit-gl-base/GLVertexArrayObject.h>

#include <appkit-gl-base/platform/PlatformGL.h>

#if defined(_WIN32)
#include <inttypes.h>
#include <sys/types.h>
#include <stdint.h>
#else
#include <sys/types.h>
#endif

#include "stdio.h"

namespace AppKit
{

    namespace OpenGL
    {

        GLVertexArrayObject::GLVertexArrayObject()
        {

            ITK_ABORT(
                (!GLAD_GL_ARB_vertex_array_object && !GLAD_GL_APPLE_vertex_array_object),
                "Vertex Array Object not supported...\n");

            // if (! GLAD_GL_ARB_vertex_array_object || ! GLAD_GL_APPLE_vertex_array_object ){
            //     printf("Vertex Array Object not supported...\n");
            //     exit(-1);
            // }

#if __APPLE__
            OPENGL_CMD(glGenVertexArraysAPPLE(1, &mVAO));
#else
#ifndef ITK_RPI
            OPENGL_CMD(glGenVertexArrays(1, &mVAO));
#endif
#endif
        }

        GLVertexArrayObject::~GLVertexArrayObject()
        {
#if __APPLE__
            OPENGL_CMD(glDeleteVertexArraysAPPLE(1, &mVAO));
#else
#ifndef ITK_RPI
            OPENGL_CMD(glDeleteVertexArrays(1, &mVAO));
#endif
#endif
        }

        void GLVertexArrayObject::enable() const
        {
#if __APPLE__
            OPENGL_CMD(glBindVertexArrayAPPLE(mVAO));
#else
#ifndef ITK_RPI
            OPENGL_CMD(glBindVertexArray(mVAO));
#endif
#endif
        }

        void GLVertexArrayObject::disable()
        {
#if __APPLE__
            OPENGL_CMD(glBindVertexArrayAPPLE(0));
#else
#ifndef ITK_RPI
            OPENGL_CMD(glBindVertexArray(0));
#endif
#endif
        }

        void GLVertexArrayObject::drawIndex(GLint primitive, int count, GLint type, int offset)
        {
            OPENGL_CMD(glDrawElements(primitive,                // mode
                                      count,                    // count
                                      type,                     // type
                                      (void *)(uintptr_t)offset // element array buffer offset
                                      ));
        }

        void GLVertexArrayObject::drawArrays(GLint primitive, int count, int offset)
        {
            OPENGL_CMD(glDrawArrays(primitive, offset, count));
        }
    }

}
