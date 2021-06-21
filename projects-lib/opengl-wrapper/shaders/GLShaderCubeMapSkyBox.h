#ifndef GL_SHADER_CUBEMAP_SKYBOX_h_
#define GL_SHADER_CUBEMAP_SKYBOX_h_

#include <opengl-wrapper/GLShader.h>

namespace openglWrapper {
    
    /// \brief Shader to draw a world aligned sky box using a cubemap as input
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLShaderCubeMapSkyBox : public GLShader {
        
    protected:
        
        void setupAttribLocation() {
            bindAttribLocation(GLShaderCubeMapSkyBox::vPosition, "vPosition");
        }
        
    public:
        //
        // attrib locations
        //
        static const int vPosition = 0;///< vertex atribute layout position
        
        //
        // uniform locations
        //
        int matrix;
        int cubeTexture;
        
        GLShaderCubeMapSkyBox():GLShader(){
            const char vertexShaderCode[] = {
                "attribute vec4 vPosition;"
                "varying vec3 cubeAccess;"
                "uniform mat4 matrix;"
                "void main() {"
                "  cubeAccess = vPosition.rgb;"
                "  gl_Position = matrix * vPosition;"
                "}" };
            
            const char fragmentShaderCode[] = {
                //"precision mediump float;"
                "varying vec3 cubeAccess;"
                "uniform samplerCube cubeTexture;"
                "void main() {"
                "  vec4 texel = textureCube(cubeTexture, cubeAccess);"
                "  gl_FragColor = texel;"
                "}" };
            
            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            
            matrix = getUniformLocation("matrix");
            cubeTexture = getUniformLocation("cubeTexture");
        }
        
        void setMatrix(const aRibeiro::mat4 & m){
            setUniform(matrix, m);
        }
        
        void setCubeTexture(int texunit){
            setUniform(cubeTexture, texunit);
        }
        
        virtual ~GLShaderCubeMapSkyBox(){
            
        }
        
    };
    
}

#endif

