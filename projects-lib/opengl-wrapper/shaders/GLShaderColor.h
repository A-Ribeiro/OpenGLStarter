#ifndef GL_SHADER_COLOR_h_
#define GL_SHADER_COLOR_h_

#include <opengl-wrapper/GLShader.h>

namespace openglWrapper {

    /// \brief Shader that draws a solid color for an object
    ///
    /// This shader fill all triangles with the same color.
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLShaderColor : public GLShader {

    protected:
        
        void setupAttribLocation();
        
    public:
        //
        // attrib locations
        //
        static const int vPosition = 0;///< vertex atribute layout position

        //
        // uniform locations
        //
        int color;
        int matrix;

        GLShaderColor();

        void setColor(const aRibeiro::vec4 &c);///< set the color to fill the triangles
        void setMatrix(const aRibeiro::mat4 & m);///< set the transform matrix
    };

}

#endif
