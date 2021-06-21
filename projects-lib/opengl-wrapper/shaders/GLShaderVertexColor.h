#ifndef GL_SHADER_VERTEX_COLOR_h_
#define GL_SHADER_VERTEX_COLOR_h_

#include <opengl-wrapper/GLShader.h>

namespace openglWrapper {

    /// \brief Shader that draws colored triangles (vertex color)
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLShaderVertexColor : public GLShader {

    protected:
        
        void setupAttribLocation();
        
    public:
        //
        // attrib locations
        //
        static const int vPosition = 0;///< vertex atribute layout position
        static const int vColor = 1;///< vertex atribute layout color

        //
        // uniform locations
        //
        int matrix;

        GLShaderVertexColor();

        void setMatrix(const aRibeiro::mat4 & m);///< set the transform matrix
    };

}

#endif
