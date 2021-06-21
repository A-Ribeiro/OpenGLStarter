#ifndef GL_SHADER_FONT_h_
#define GL_SHADER_FONT_h_

#include <opengl-wrapper/GLShader.h>

namespace openglWrapper {

    /// \brief Shader that draws 2D fonts
    ///
    /// This shader is intended to be used by #GLFont and #GLStripText classes.
    ///
    /// It uses the vertex color GPU algorithm (Gouraud)
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLShaderFont : public GLShader {

    protected:
        
        void setupAttribLocation();
        
    public:
        //
        // attrib locations
        //
        static const int vPosition = 0;///< vertex atribute layout position
        static const int vColor = 1;///< vertex atribute layout color
        static const int vUV = 2;///< vertex atribute layout uv

        //
        // uniform locations
        //
        int texture;
        int matrix;

        GLShaderFont();

        void setTexture(int activeTextureUnit);///< set the OpenGL texture unit to be used by the texture sampler.
        void setMatrix(const aRibeiro::mat4 & m);///< set the transform matrix
    };

}

#endif
