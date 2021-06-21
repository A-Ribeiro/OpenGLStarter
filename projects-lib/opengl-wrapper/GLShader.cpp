
#include "GLShader.h"

#include <opengl-wrapper/PlatformGL.h>

using namespace aRibeiro;

namespace openglWrapper {

    //
    // Static
    //
    GLint GLShader::loadShader(int type, const char* shaderCode, const char* file, unsigned int line) {
        GLint shader;// = glCreateShader(type);
        OPENGL_CMD_FL(shader = glCreateShader(type), file, line);
        const char* shaderArray[] = { shaderCode };
        OPENGL_CMD_FL(glShaderSource(shader, 1, shaderArray, NULL), file, line);
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

    void GLShader::disable() {
        OPENGL_CMD(glUseProgram(0));
    }

    GLint GLShader::getCurrentShader() {
        GLint result;
        OPENGL_CMD(glGetIntegerv(GL_CURRENT_PROGRAM, &result));
        return result;
    }

    //
    // Non-Static
    //
    void GLShader::LoadShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode, const char* file, unsigned int line) {
        mProgram = loadShaderStrings(this, vertexShaderCode, fragmentShaderCode, file, line);
    }
    GLint GLShader::getAttribLocation(const char* name) {
        GLint result;
        OPENGL_CMD(result = glGetAttribLocation(mProgram, name));
        return result;
    }
    GLint GLShader::getUniformLocation(const char* name) {
        GLint result;
        OPENGL_CMD(result = glGetUniformLocation(mProgram, name));
        return result;
    }

    void GLShader::setUniform(int location, int v) {
        OPENGL_CMD(glUniform1i(location, v));
    }

    void GLShader::setUniform(int location, float v) {
        OPENGL_CMD(glUniform1f(location, v));
    }
    void GLShader::setUniform(int location, const vec2 &v) {
        OPENGL_CMD(glUniform2fv(location, 1, v.array));
    }
    void GLShader::setUniform(int location, const vec3 &v) {
        OPENGL_CMD(glUniform3fv(location, 1, v.array));
    }
    void GLShader::setUniform(int location, const vec4 &v) {
        OPENGL_CMD(glUniform4fv(location, 1, v.array));
    }
    void GLShader::setUniform(int location, const mat4 &m) {
        OPENGL_CMD(glUniformMatrix4fv(location, 1, GL_FALSE , m.array));
    }

    int GLShader::getUniformInt(int location) {
        int v;
        OPENGL_CMD(glGetUniformiv(mProgram, location, &v));
        return v;
    }

    float GLShader::getUniformFloat(int location) {
        float v;
        OPENGL_CMD(glGetUniformfv(mProgram, location, &v));
        return v;
    }

    vec2 GLShader::getUniformVec2(int location) {
        vec2 v;
        OPENGL_CMD(glGetUniformfv(mProgram, location, v.array));
        return v;
    }

    vec3 GLShader::getUniformVec3(int location) {
        vec3 v;
        OPENGL_CMD(glGetUniformfv(mProgram, location, v.array));
        return v;
    }

    vec4 GLShader::getUniformVec4(int location) {
        vec4 v;
        OPENGL_CMD(glGetUniformfv(mProgram, location, v.array));
        return v;
    }

    mat4 GLShader::getUniformMat4(int location) {
        mat4 v;
        OPENGL_CMD(glGetUniformfv(mProgram, location, v.array));
        return v;
    }

    void GLShader::bindAttribLocation(int location, const char* attribName) {
        OPENGL_CMD(glBindAttribLocation(mProgram, location, attribName));
    }

    void GLShader::enable() {
        if (GLShader::getCurrentShader() != mProgram)
            OPENGL_CMD(glUseProgram(mProgram));
    }

    GLShader::GLShader() {

        ARIBEIRO_ABORT(!GLEW_ARB_shading_language_100,"ARB_shading_language_100 not supported.");
        ARIBEIRO_ABORT(!GLEW_ARB_shader_objects,"ARB_shader_objects not supported.");
        ARIBEIRO_ABORT(!GLEW_ARB_vertex_shader,"ARB_vertex_shader not supported.");
        ARIBEIRO_ABORT(!GLEW_ARB_fragment_shader,"ARB_fragment_shader not supported.");
        
        mProgram = -1;
    }

    GLShader::~GLShader() {
        if (mProgram != -1) {
            if (GLShader::getCurrentShader() == mProgram)
                GLShader::disable();
            glDeleteProgram(mProgram);
            mProgram = -1;
        }
    }
}
