
#include <appkit-gl-base/GLShader.h>
#include <appkit-gl-base/platform/PlatformGL.h>

namespace AppKit
{

    namespace OpenGL
    {

        //
        // Static
        //
        /*
        GLint GLShader::loadShader(int type, const char* shaderCode, const char* file, unsigned int line) {
            GLint shader;// = glCreateShader(type);
            OPENGL_CMD_FL(shader = glCreateShader(type), file, line);
            const char* shaderArray[] = { shaderCode };
            OPENGL_CMD_FL(glShaderSource(shader, 1, shaderArray, nullptr), file, line);
            OPENGL_CMD_FL(glCompileShader(shader), file, line);
            return shader;
        }

        GLint GLShader::loadShaderStrings(GLShader *shaderObj, const char* vertexShaderCode, const  char* fragmentShaderCode, const char* file, unsigned int line) {
            GLint vertexShader;
            OPENGL_CMD_FL(vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderCode, file, line), file, line);
            PlatformGL::checkShaderStatus(vertexShader, file, line, "[VERTEX]");

            GLint fragmentShader;
            OPENGL_CMD_FL(fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderCode, file, line), file, line);
            PlatformGL::checkShaderStatus(fragmentShader, file, line, "[FRAGMENT]");

            int mProgram;
            OPENGL_CMD_FL(mProgram = glCreateProgram(), file, line);

            //
            // Attach shaders
            //
            OPENGL_CMD_FL(glAttachShader(mProgram, vertexShader), file, line);
            OPENGL_CMD_FL(glAttachShader(mProgram, fragmentShader), file, line);

            shaderObj->mProgram = mProgram;
            shaderObj->setupAttribLocation();

            //
            // Perform shader link
            //
            OPENGL_CMD_FL(glLinkProgram(mProgram), file, line);

            PlatformGL::checkProgramStatus(mProgram, file, line, "[PROGRAM]");

            //
            // Shader information is not needed any more... can be detached and deleted
            //
            OPENGL_CMD_FL(glDetachShader(mProgram, vertexShader), file, line);
            OPENGL_CMD_FL(glDetachShader(mProgram, fragmentShader), file, line);

            OPENGL_CMD_FL(glDeleteShader(vertexShader), file, line);
            OPENGL_CMD_FL(glDeleteShader(fragmentShader), file, line);

            return mProgram;
        }
        */

        void GLShader::compile(const char *vertex, const char *fragment, const char *file, unsigned int line)
        {

            {
                OPENGL_CMD_FL(mVertShader = glCreateShader(GL_VERTEX_SHADER), file, line);
                const char *shaderArray[] = {vertex};
                OPENGL_CMD_FL(glShaderSource(mVertShader, 1, shaderArray, nullptr), file, line);
                OPENGL_CMD_FL(glCompileShader(mVertShader), file, line);
                PlatformGL::checkShaderStatus(mVertShader, file, line, "[VERTEX]");
            }

            {
                OPENGL_CMD_FL(mFragShader = glCreateShader(GL_FRAGMENT_SHADER), file, line);
                const char *shaderArray[] = {fragment};
                OPENGL_CMD_FL(glShaderSource(mFragShader, 1, shaderArray, nullptr), file, line);
                OPENGL_CMD_FL(glCompileShader(mFragShader), file, line);
                PlatformGL::checkShaderStatus(mFragShader, file, line, "[FRAGMENT]");
            }

            {
                OPENGL_CMD_FL(mProgram = glCreateProgram(), file, line);

                OPENGL_CMD_FL(glAttachShader(mProgram, mVertShader), file, line);
                OPENGL_CMD_FL(glAttachShader(mProgram, mFragShader), file, line);
            }
        }

        void GLShader::link(const char *file, unsigned int line)
        {
            OPENGL_CMD_FL(glLinkProgram(mProgram), file, line);

            PlatformGL::checkProgramStatus(mProgram, file, line, "[PROGRAM]");

            OPENGL_CMD_FL(glDetachShader(mProgram, mVertShader), file, line);
            OPENGL_CMD_FL(glDetachShader(mProgram, mFragShader), file, line);

            OPENGL_CMD_FL(glDeleteShader(mVertShader), file, line);
            OPENGL_CMD_FL(glDeleteShader(mFragShader), file, line);

            mVertShader = 0;
            mFragShader = 0;
        }

        void GLShader::disable()
        {
            OPENGL_CMD(glUseProgram(0));
        }

        GLint GLShader::getCurrentShader()
        {
            GLint result;
            OPENGL_CMD(glGetIntegerv(GL_CURRENT_PROGRAM, &result));
            return result;
        }

        //
        // Non-Static
        //
        /*
        void GLShader::LoadShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode, const char* file, unsigned int line) {
            mProgram = loadShaderStrings(this, vertexShaderCode, fragmentShaderCode, file, line);
        }
        */

        GLint GLShader::getAttribLocation(const char *name)
        {
            GLint result;
            OPENGL_CMD(result = glGetAttribLocation(mProgram, name));
            return result;
        }
        GLint GLShader::getUniformLocation(const char *name)
        {
            GLint result;
            OPENGL_CMD(result = glGetUniformLocation(mProgram, name));
            return result;
        }

        void GLShader::setUniform(int location, int v)
        {
            OPENGL_CMD(glUniform1i(location, v));
        }

        void GLShader::setUniform(int location, float v)
        {
            OPENGL_CMD(glUniform1f(location, v));
        }
        void GLShader::setUniform(int location, const MathCore::vec2f &v)
        {
            OPENGL_CMD(glUniform2fv(location, 1, v.array));
        }
        void GLShader::setUniform(int location, const MathCore::vec3f &v)
        {
            OPENGL_CMD(glUniform3fv(location, 1, v.array));
        }
        void GLShader::setUniform(int location, const MathCore::vec4f &v)
        {
            OPENGL_CMD(glUniform4fv(location, 1, v.array));
        }
        void GLShader::setUniform(int location, const MathCore::mat4f &m)
        {
            OPENGL_CMD(glUniformMatrix4fv(location, 1, GL_FALSE, m.array));
        }

        int GLShader::getUniformInt(int location)
        {
            int v;
            OPENGL_CMD(glGetUniformiv(mProgram, location, &v));
            return v;
        }

        float GLShader::getUniformFloat(int location)
        {
            float v;
            OPENGL_CMD(glGetUniformfv(mProgram, location, &v));
            return v;
        }

        MathCore::vec2f GLShader::getUniformVec2(int location)
        {
            MathCore::vec2f v;
            OPENGL_CMD(glGetUniformfv(mProgram, location, v.array));
            return v;
        }

        MathCore::vec3f GLShader::getUniformVec3(int location)
        {
            MathCore::vec3f v;
            OPENGL_CMD(glGetUniformfv(mProgram, location, v.array));
            return v;
        }

        MathCore::vec4f GLShader::getUniformVec4(int location)
        {
            MathCore::vec4f v;
            OPENGL_CMD(glGetUniformfv(mProgram, location, v.array));
            return v;
        }

        MathCore::mat4f GLShader::getUniformMat4(int location)
        {
            MathCore::mat4f v;
            OPENGL_CMD(glGetUniformfv(mProgram, location, v.array));
            return v;
        }

        void GLShader::bindAttribLocation(int location, const char *attribName)
        {
            OPENGL_CMD(glBindAttribLocation(mProgram, location, attribName));
        }

        void GLShader::enable()
        {
            if (GLShader::getCurrentShader() != mProgram)
                OPENGL_CMD(glUseProgram(mProgram));
        }

        GLShader::GLShader()
        {

#if !defined(GLAD_GLES2)
            ITK_ABORT(!GLAD_GL_ARB_shading_language_100, "ARB_shading_language_100 not supported.");
            // ITK_ABORT(!GLAD_GL_ARB_shader_objects, "ARB_shader_objects not supported.");
            // ITK_ABORT(!GLAD_GL_ARB_vertex_shader, "ARB_vertex_shader not supported.");
            // ITK_ABORT(!GLAD_GL_ARB_fragment_shader, "ARB_fragment_shader not supported.");
#endif
            mProgram = 0;
            mVertShader = 0;
            mFragShader = 0;

            snprintf(class_name, 128, "GLShader");
        }

        GLShader::~GLShader()
        {
            if (mProgram != 0)
            {
                if (GLShader::getCurrentShader() == mProgram)
                    GLShader::disable();

                if (mVertShader != 0)
                {
                    OPENGL_CMD(glDetachShader(mProgram, mVertShader));
                    OPENGL_CMD(glDeleteShader(mVertShader));
                    mVertShader = 0;
                }

                if (mFragShader != 0)
                {
                    OPENGL_CMD(glDetachShader(mProgram, mFragShader));
                    OPENGL_CMD(glDeleteShader(mFragShader));
                    mFragShader = 0;
                }

                glDeleteProgram(mProgram);
                mProgram = 0;
            }
        }
    }

}