#include <appkit-gl-base/platform/PlatformGL.h>
// #include <aRibeiroCore/common.h>

// #include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>

namespace AppKit
{

    namespace OpenGL
    {

        void PlatformGL::glCheckError(const char *file, unsigned int line, const char *expression)
        {
            // Get the last error
            GLenum errorCode = glGetError();

            if (errorCode != GL_NO_ERROR)
            {
                const char *fileString = file;
                const char *error = "Unknown error";
                const char *description = "No description";

                // Decode the error code
                switch (errorCode)
                {
                case GL_INVALID_ENUM:
                {
                    error = "GL_INVALID_ENUM";
                    description = "An unacceptable value has been specified for an enumerated argument.";
                    break;
                }

                case GL_INVALID_VALUE:
                {
                    error = "GL_INVALID_VALUE";
                    description = "A numeric argument is out of range.";
                    break;
                }

                case GL_INVALID_OPERATION:
                {
                    error = "GL_INVALID_OPERATION";
                    description = "The specified operation is not allowed in the current state.";
                    break;
                }
#ifndef ITK_RPI
                case GL_STACK_OVERFLOW:
                {
                    error = "GL_STACK_OVERFLOW";
                    description = "This command would cause a stack overflow.";
                    break;
                }

                case GL_STACK_UNDERFLOW:
                {
                    error = "GL_STACK_UNDERFLOW";
                    description = "This command would cause a stack underflow.";
                    break;
                }
#endif
                case GL_OUT_OF_MEMORY:
                {
                    error = "GL_OUT_OF_MEMORY";
                    description = "There is not enough memory left to execute the command.";
                    break;
                }

                case GL_INVALID_FRAMEBUFFER_OPERATION:
                {
                    error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
                    break;
                }
                }

                // Log the error
                printf("An internal OpenGL call failed in %s(%i). \n"
                       "Expression:%s\n"
                       "Error description:%s\n"
                       "%s\n\n",
                       fileString, line, expression, error, description);

                exit(-1);
            }
        }

        void PlatformGL::checkShaderStatus(GLuint shader, const char *file, unsigned int line, const char *aux_string)
        {
            GLint success;
#if defined(ITK_RPI) || true
            OPENGL_CMD_FL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success), file, line);
#else
            OPENGL_CMD_FL(glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &success), file, line);
#endif
            if (success == GL_FALSE)
            {
                char log[1024];
#if defined(ITK_RPI) || true
                OPENGL_CMD_FL(glGetShaderInfoLog(shader, sizeof(log), 0, log), file, line);
#else
                OPENGL_CMD_FL(glGetInfoLogARB(shader, sizeof(log), 0, log), file, line);
#endif

                ITK_ABORT_FL(file, line,
                             true,
                             "Failed to compile shader: %s %s\n", aux_string, log);

                // printf("Failed to compile shader: %s\n", log);
                // exit(-1);
            }
        }

        void PlatformGL::checkProgramStatus(GLuint program, const char *file, unsigned int line, const char *aux_string)
        {
            GLint success;
#if defined(ITK_RPI) || true
            OPENGL_CMD_FL(glGetProgramiv(program, GL_LINK_STATUS, &success), file, line);
#else
            OPENGL_CMD_FL(glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &success), file, line);
#endif
            if (success == GL_FALSE)
            {
                char log[1024];
#if defined(ITK_RPI) || true
                OPENGL_CMD_FL(glGetProgramInfoLog(program, sizeof(log), 0, log), file, line);
#else
                OPENGL_CMD_FL(glGetInfoLogARB(program, sizeof(log), 0, log), file, line);
#endif

                ITK_ABORT_FL(file, line,
                             true,
                             "Failed to link shader: %s %s\n", aux_string, log);

                // printf("Failed to link shader: %s\n", log);
                // exit(-1);
            }
        }

    }
}
