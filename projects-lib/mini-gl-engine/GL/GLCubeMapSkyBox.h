#ifndef GL_CUBE_MAP_SKY_BOX_h
#define GL_CUBE_MAP_SKY_BOX_h

#include <string>
#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLVertexBufferObject.h>
#include <opengl-wrapper/GLShaderTextureColor.h>

#include <opengl-wrapper/GLCubeMap.h>
#include <opengl-wrapper/GLShaderCubeMapSkyBox.h>

namespace GLEngine {
    
    /// \brief Loads a CubeMap to draw a skybox.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <mini-gl-engine/mini-gl-engine.h>
    /// using namespace aRibeiro;
    /// using namespace openglWrapper;
    /// using namespace GLEngine;
    ///
    /// GLCubeMapSkyBox *cube_skybox = GLCubeMapSkyBox(
    ///     true && Engine::Instance()->isRGBCapable, // load sRGB
    ///     "negz.jpg","posz.jpg",
    ///     "negx.jpg","posx.jpg",
    ///     "negy.jpg","posy.jpg",
    ///     100.0f, // distance
    ///     1024 // max cubeResolution
    /// );
    ///
    /// ...
    ///
    /// cube_skybox->draw(viewMatrix,projectionMatrix);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE GLCubeMapSkyBox {
        
        void createVertex(float distance);
        
    public:
        openglWrapper::GLCubeMap cubeMap;
        openglWrapper::GLShaderCubeMapSkyBox cubeSkyShader;
        
        aRibeiro::aligned_vector<aRibeiro::vec3> vertex;
        openglWrapper::GLVertexBufferObject *vbo;
        //aRibeiro::aligned_vector<aRibeiro::vec2> uv;
        
        GLCubeMapSkyBox(
                        bool sRGB,
                        const std::string &_negz,const std::string &_posz,
                        const std::string &_negx,const std::string &_posx,
                        const std::string &_negy,const std::string &_posy,
                         float _distance, int cubeResolution, bool leftHanded = true);
        
        GLCubeMapSkyBox(bool sRGB, 
                        std::string single_file,
                        float _distance, int cubeResolution, bool leftHanded = true);
        
        virtual ~GLCubeMapSkyBox();
        
        void createVBO();
        
        void draw(const aRibeiro::mat4& viewMatrix,const aRibeiro::mat4& projectionMatrix);

        void drawAnotherCube(const aRibeiro::mat4& viewMatrix, const aRibeiro::mat4& projectionMatrix, openglWrapper::GLCubeMap *cubeMap);
        
        SSE2_CLASS_NEW_OPERATOR
    } _SSE2_ALIGN_POS ;
    
}

#endif

