#ifndef GL_SKY_BOX_h
#define GL_SKY_BOX_h

#include <string>
#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLVertexBufferObject.h>
#include <opengl-wrapper/GLShaderTextureColor.h>

namespace GLEngine {
    
    /// \brief This definition uses 6 plain 2D textures to draw a skybox.
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
    /// GLSkyBox *skybox = GLSkyBox(
    ///     true && Engine::Instance()->isRGBCapable, // load sRGB
    ///     "negz.jpg","posz.jpg",
    ///     "negx.jpg","posx.jpg",
    ///     "negy.jpg","posy.jpg",
    ///     100.0f, // distance
    ///     0.0f // rotation
    /// );
    ///
    /// ...
    ///
    /// skybox->draw(viewMatrix,projectionMatrix);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE GLSkyBox {
        
    public:
        openglWrapper::GLTexture *back;
        openglWrapper::GLTexture *front;
        openglWrapper::GLTexture *left;
        openglWrapper::GLTexture *right;
        openglWrapper::GLTexture *bottom;
        openglWrapper::GLTexture *top;
        
        aRibeiro::aligned_vector<aRibeiro::vec3> vertex;
        aRibeiro::aligned_vector<aRibeiro::vec2> uv;
        
        openglWrapper::GLShaderTextureColor shader;
        
        GLSkyBox(bool sRGB, 
                 const std::string &_negz,const std::string &_posz,
                 const std::string &_negx,const std::string &_posx,
                 const std::string &_negy,const std::string &_posy,
                 float _distance, float rotation);
        
        virtual ~GLSkyBox();
        
        void draw(const aRibeiro::mat4& viewMatrix,const aRibeiro::mat4& projectionMatrix, bool leftHanded = true);
        
        SSE2_CLASS_NEW_OPERATOR
    } _SSE2_ALIGN_POS ;
    
}

#endif

