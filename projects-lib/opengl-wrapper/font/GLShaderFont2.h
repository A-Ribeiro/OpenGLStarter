#ifndef GL_SHADER_FONT2_h_
#define GL_SHADER_FONT2_h_

#include <opengl-wrapper/GLShader.h>

namespace openglWrapper {

    /// \brief Shader that draws alpha-map font definitions
    ///
    /// It uses the default transform multiplication to the vertex information of the font.
    ///
    /// This way it can be used in 2D or 3D projections.
    ///
    /// It is defined to use vertex color information also.
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLShaderFont2 : public GLShader {

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

        GLShaderFont2();

        void setTexture(int activeTextureUnit);///< set the OpenGL texture unit to be used by the texture sampler.
        void setMatrix(const aRibeiro::mat4 & m);///< set the transform matrix
    };

}

#endif
