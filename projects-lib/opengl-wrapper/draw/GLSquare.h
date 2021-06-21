#ifndef GL_SQUARE_COLOR_h
#define GL_SQUARE_COLOR_h

#include <opengl-wrapper/GLShaderColor.h>
#include <opengl-wrapper/GLShaderTextureColor.h>

namespace openglWrapper {

    /// \brief Draw a square using the base shaders
    ///
    /// This class uses the direct draw (draw triangles from main memory buffer).
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLSquare {

    public:

        GLSquare();

        /// \brief Draw a square
        ///
        /// \author Alessandro Ribeiro
        /// \param shader instance of #GLShaderColor
        ///
        void draw(GLShaderColor *shader);

        /// \brief Draw a square
        ///
        /// \author Alessandro Ribeiro
        /// \param shader instance of #GLShaderTextureColor
        ///
        void draw(GLShaderTextureColor *shader);

    };

}

#endif
