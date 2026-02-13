#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
// #include <glew/glew.h> // extensions here

namespace AppKit
{

    namespace OpenGL
    {
        typedef const char *ShaderType;

        /// \brief Parent class to implement shader code
        ///
        /// This class has some methods to aid OpenGL shader loading and parameter set.
        ///
        /// Example:
        ///
        /// \code
        /// #include <appkit-gl-base/opengl-wrapper.h>
        /// using namespace AppKit::OpenGL;
        ///
        /// class ShaderTwoTextures : public GLShader {
        /// protected:
        ///     int uSampler2DTextureA;
        ///     int uSampler2DTextureB;
        ///     int uMat4ModelViewProjection;
        ///     int uFloatBlend;
        /// public:
        ///     static const int aVec3Position = 0;
        ///     static const int aVec2UV = 1;
        ///     ShaderTwoTextures() :GLShader() {
        ///         const char vertexShaderCode[] = {
        ///             "attribute vec3 aVec3Position;"
        ///             "attribute vec2 aVec2UV;"
        ///             "uniform mat4 uMat4ModelViewProjection;"
        ///             "varying vec2 vVec2UV;"
        ///             "void main() {"
        ///                 "vVec2UV = aVec2UV;"
        ///                 "gl_Position = uMat4ModelViewProjection * vec4(aVec3Position, 1.0);"
        ///             "}"
        ///         };
        ///         const char fragmentShaderCode[] = {
        ///             "uniform sampler2D uSampler2DTextureA;"
        ///             "uniform sampler2D uSampler2DTextureB;"
        ///             "uniform float uFloatBlend;"
        ///             "varying vec2 vVec2UV;"
        ///             "void main() {"
        ///                 "vec4 textelA = texture2D(uSampler2DTextureA, vVec2UV);"
        ///                 "vec4 textelB = texture2D(uSampler2DTextureB, vVec2UV);"
        ///                 "gl_FragColor = mix(textelA, textelB, uFloatBlend);"
        ///             "}"
        ///         };
        ///         compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
        ///         bindAttribLocation(ShaderTwoTextures::aVec3Position, "aVec3Position");
        ///         bindAttribLocation(ShaderTwoTextures::aVec2UV, "aVec2UV");
        ///         link(__FILE__, __LINE__);
        ///         uSampler2DTextureA = getUniformLocation("uSampler2DTextureA");
        ///         uSampler2DTextureB = getUniformLocation("uSampler2DTextureB");
        ///         uMat4ModelViewProjection = getUniformLocation("uMat4ModelViewProjection");
        ///         uFloatBlend = getUniformLocation("uFloatBlend");
        ///     }
        ///     void setTextureA(int textureUnit) {
        ///         setUniform(uSampler2DTextureA, textureUnit);
        ///     }
        ///     void setTextureB(int textureUnit) {
        ///         setUniform(uSampler2DTextureB, textureUnit);
        ///     }
        ///     void setBlend(float blend) {
        ///         setUniform(uFloatBlend, blend);
        ///     }
        ///     void setModelViewProjection(const mat4 &matrix) {
        ///         setUniform(uMat4ModelViewProjection, matrix);
        ///     }
        /// };
        ///
        /// ...
        ///
        /// ShaderTwoTextures shader_instance;
        ///
        /// shader_instance.enable();
        /// shader_instance.setTextureA(0); // setup to use texture unit 0
        /// shader_instance.setTextureB(1); // setup to use texture unit 1
        /// shader_instance.setBlend(0.5f);
        /// shader_instance.setModelViewProjection(mvp);
        ///
        /// // OpenGL drawing code
        /// ...
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLShader
        {
        public:
            GLint mProgram;

            GLint mVertShader;
            GLint mFragShader;

        protected:
            /// \brief Load one shader type
            ///
            /// Types available: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER.
            ///
            /// Example:
            ///
            /// \code
            /// char* vertexShaderCode;
            /// char* fragmentShaderCode;
            ///
            /// GLint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderCode, __FILE__, __LINE__);
            /// GLint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderCode, __FILE__, __LINE__);
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            /// \param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
            /// \param shaderCode the shader code
            /// \param file debug file: __FILE__
            /// \param line debug line: __LINE__
            /// \return the OpenGL shader ID
            ///
            // static GLint loadShader(int type, const char* shaderCode, const char* file, unsigned int line);

            /// \brief Load vertex and fragment shader and returns the linked shader program
            ///
            /// This method will call the #setupAttribLocation from the shaderObj parameter.
            ///
            /// Example:
            ///
            /// \code
            /// char* vertexShaderCode;
            /// char* fragmentShaderCode;
            ///
            /// GLint mProgram = loadShaderStrings(this, vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            /// \param shaderObj the current shader class
            /// \param vertexShaderCode the vertex shader code
            /// \param fragmentShaderCode the fragment shader code
            /// \param file debug file: __FILE__
            /// \param line debug line: __LINE__
            /// \return the OpenGL shader program ID
            ///
            // static GLint loadShaderStrings(GLShader *shaderObj, const char* vertexShaderCode, const  char* fragmentShaderCode, const char* file, unsigned int line);

            /// \brief Get the current shader active in the OpenGL pipeline.
            ///
            /// \author Alessandro Ribeiro
            /// \return the active OpenGL shader
            ///
            static GLint getCurrentShader();

            /// \brief Load vertex and fragment shader into the current shader class
            ///
            /// This method will call the #setupAttribLocation from the shaderObj parameter.
            ///
            /// You can call this method inside the class constructor.
            ///
            /// Example:
            ///
            /// \code
            /// char* vertexShaderCode;
            /// char* fragmentShaderCode;
            ///
            /// LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            /// \param vertexShaderCode the vertex shader code
            /// \param fragmentShaderCode the fragment shader code
            /// \param file debug file: __FILE__
            /// \param line debug line: __LINE__
            ///
            // void LoadShaderProgram(const char* vertexShaderCode, const char* fragmentShaderCode, const char* file, unsigned int line);

            void compile(const char *vertex, const char *fragment, const char *file, unsigned int line);

            /// \brief Bind an integer value to a vertex attribute location inside the shader.
            ///
            /// Should be called inside the #setupAttribLocation method.
            ///
            /// \author Alessandro Ribeiro
            /// \param location the integer to be set
            /// \param attrib the attribute you want to set
            ///
            void bindAttribLocation(int location, const char *attrib);

            void link(const char *file, unsigned int line);

            /// \brief Query the vertex attribute location inside the vertex shader.
            ///
            /// Position, UV, Normal, Binormal, Tangent, etc...
            ///
            /// \author Alessandro Ribeiro
            /// \param name the name of the attribute
            ///
            GLint getAttribLocation(const char *name);

            /// \brief Query uniform location in the vertex or fragment shader.
            ///
            /// Color, ModelViewProjectionMatrix, Blend, etc...
            ///
            /// \author Alessandro Ribeiro
            /// \param name the name of the uniform
            ///
            GLint getUniformLocation(const char *name);

            void setUniform(int location, int v);                    ///< Setup an uniform from int value
            void setUniform(int location, float v);                  ///< Setup an uniform from float value
            void setUniform(int location, const MathCore::vec2f &v); ///< Setup an uniform from vec2 value
            void setUniform(int location, const MathCore::vec3f &v); ///< Setup an uniform from vec3 value
            void setUniform(int location, const MathCore::vec4f &v); ///< Setup an uniform from vec4 value
            void setUniform(int location, const MathCore::mat4f &v); ///< Setup an uniform from mat4 value

            int getUniformInt(int location);              ///< Query an uniform from int value
            float getUniformFloat(int location);          ///< Query an uniform from float value
            MathCore::vec2f getUniformVec2(int location); ///< Query an uniform from vec2 value
            MathCore::vec3f getUniformVec3(int location); ///< Query an uniform from vec3 value
            MathCore::vec4f getUniformVec4(int location); ///< Query an uniform from vec4 value
            MathCore::mat4f getUniformMat4(int location); ///< Query an uniform from mat4 value

            /// \brief Reference to setup attribute locations.
            ///
            /// The subclass need to implement this to setup the vertex layout used by the VBO and VAO.
            ///
            /// \author Alessandro Ribeiro
            ///
            // virtual void setupAttribLocation()=0;

            ShaderType type_const_ref;
            GLShader(ShaderType type);

        public:
            /// \brief Set this shader to be activated in the OpenGL pipeline.
            ///
            /// \author Alessandro Ribeiro
            ///
            void enable();

            /// \brief Disable OpenGL shader pipeline
            ///
            /// \author Alessandro Ribeiro
            ///
            static void disable();

            // GLShader();
            virtual ~GLShader();

            // char class_name[128];

            //deleted copy constructor and assign operator, to avoid copy...
            GLShader(const GLShader &v) = delete;
            GLShader& operator=(const GLShader &v) = delete;

            ShaderType getType() const;
            bool compareType(ShaderType t) const;

        };

    }

}