#ifndef GL_SHADER_TEXTURE_COLOR_h_
#define GL_SHADER_TEXTURE_COLOR_h_

#include <opengl-wrapper/GLShader.h>

namespace openglWrapper {

    /// \brief Shader that draws textured triangles with a color multiplier
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLShaderTextureColor : public GLShader {

    protected:
        
        void setupAttribLocation();
        
    public:
        //
        // attrib locations
        //
        static const int vPosition = 0;///< vertex atribute layout position
        static const int vUV = 1;///< vertex atribute layout uv

        //
        // uniform locations
        //
        int color;
        int matrix;
        int texture;

        GLShaderTextureColor();

        void setColor(const aRibeiro::vec4 &c);///< set the color to fill the triangles
        void setMatrix(const aRibeiro::mat4 & m);///< set the transform matrix
        void setTexture(int activeTextureUnit);///< set the OpenGL texture unit to be used by the texture sampler.

    };

}

#endif
