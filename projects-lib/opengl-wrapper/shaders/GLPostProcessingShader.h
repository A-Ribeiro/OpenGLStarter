#ifndef GLPostProcessingShader__H
#define GLPostProcessingShader__H

#include <opengl-wrapper/GLShader.h>

namespace openglWrapper {
    
    /// \brief Shader that any post processing shader need to inherit (be subclass of)
    ///
    /// The requirement is that the vertex atribute layout follows:
    ///
    /// Position (vec2): layout position 0
    /// UV (vec2): layout position 1
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLPostProcessingShader: public GLShader {
    public:
        static const int aVec2Position = 0;///< vertex atribute location for position = 0
        static const int aVec2UV = 1;///< vertex atribute location for position = 1
    };
    
}

#endif
