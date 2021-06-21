#include "GLSkyBox.h"

#include <mini-gl-engine/mini-gl-engine.h>

using namespace openglWrapper;

namespace GLEngine {
    GLSkyBox::GLSkyBox(bool sRGB, 
                       const std::string &_negz,const std::string &_posz,
                       const std::string &_negx,const std::string &_posx,
                       const std::string &_negy,const std::string &_posy,
                       float d, float rotation) {
        
        back = NULL;
        front = NULL;
        left = NULL;
        right = NULL;
        bottom = NULL;
        top = NULL;
        
        back = GLTexture::loadFromFile(_negz.c_str());
        back->generateMipMap();
        front = GLTexture::loadFromFile(_posz.c_str());
        front->generateMipMap();
        
        left = GLTexture::loadFromFile(_negx.c_str());
        left->generateMipMap();
        right = GLTexture::loadFromFile(_posx.c_str());
        right->generateMipMap();
        
        bottom = GLTexture::loadFromFile(_negy.c_str());
        bottom->generateMipMap();
        top = GLTexture::loadFromFile(_posy.c_str());
        top->generateMipMap();
        
        //front (+z)
        vertex.push_back(aRibeiro::vec3(-d,-d,d));
        vertex.push_back(aRibeiro::vec3( d, d,d));
        vertex.push_back(aRibeiro::vec3( d,-d,d));
        
        vertex.push_back(aRibeiro::vec3(-d,-d,d));
        vertex.push_back(aRibeiro::vec3(-d, d,d));
        vertex.push_back(aRibeiro::vec3( d, d,d));
        
        uv.push_back(aRibeiro::vec2(0,1));
        uv.push_back(aRibeiro::vec2(1,0));
        uv.push_back(aRibeiro::vec2(1,1));
        
        uv.push_back(aRibeiro::vec2(0,1));
        uv.push_back(aRibeiro::vec2(0,0));
        uv.push_back(aRibeiro::vec2(1,0));
        
        
        //back (-z)
        vertex.push_back(aRibeiro::vec3(-d,-d,-d));
        vertex.push_back(aRibeiro::vec3( d,-d,-d));
        vertex.push_back(aRibeiro::vec3( d, d,-d));
        
        vertex.push_back(aRibeiro::vec3(-d,-d,-d));
        vertex.push_back(aRibeiro::vec3( d, d,-d));
        vertex.push_back(aRibeiro::vec3(-d, d,-d));
        
        uv.push_back(aRibeiro::vec2(1,1));
        uv.push_back(aRibeiro::vec2(0,1));
        uv.push_back(aRibeiro::vec2(0,0));
        
        uv.push_back(aRibeiro::vec2(1,1));
        uv.push_back(aRibeiro::vec2(0,0));
        uv.push_back(aRibeiro::vec2(1,0));
        
        
        //left (+x)
        vertex.push_back(aRibeiro::vec3(d,-d,-d));
        vertex.push_back(aRibeiro::vec3(d, d, d));
        vertex.push_back(aRibeiro::vec3(d, d,-d));
        
        vertex.push_back(aRibeiro::vec3(d,-d,-d));
        vertex.push_back(aRibeiro::vec3(d,-d, d));
        vertex.push_back(aRibeiro::vec3(d, d, d));
        
        uv.push_back(aRibeiro::vec2(1,1));
        uv.push_back(aRibeiro::vec2(0,0));
        uv.push_back(aRibeiro::vec2(1,0));
        
        uv.push_back(aRibeiro::vec2(1,1));
        uv.push_back(aRibeiro::vec2(0,1));
        uv.push_back(aRibeiro::vec2(0,0));
        
        //right (-x)
        vertex.push_back(aRibeiro::vec3(-d,-d,-d));
        vertex.push_back(aRibeiro::vec3(-d, d,-d));
        vertex.push_back(aRibeiro::vec3(-d, d, d));
        
        vertex.push_back(aRibeiro::vec3(-d,-d,-d));
        vertex.push_back(aRibeiro::vec3(-d, d, d));
        vertex.push_back(aRibeiro::vec3(-d,-d, d));
        
        uv.push_back(aRibeiro::vec2(0,1));
        uv.push_back(aRibeiro::vec2(0,0));
        uv.push_back(aRibeiro::vec2(1,0));
        
        uv.push_back(aRibeiro::vec2(0,1));
        uv.push_back(aRibeiro::vec2(1,0));
        uv.push_back(aRibeiro::vec2(1,1));
        
        //up (+y)
        vertex.push_back(aRibeiro::vec3(-d, d,-d));
        vertex.push_back(aRibeiro::vec3( d, d,-d));
        vertex.push_back(aRibeiro::vec3( d, d, d));
        
        vertex.push_back(aRibeiro::vec3(-d, d,-d));
        vertex.push_back(aRibeiro::vec3( d, d, d));
        vertex.push_back(aRibeiro::vec3(-d, d, d));
        
        uv.push_back(aRibeiro::vec2(0,0));
        uv.push_back(aRibeiro::vec2(1,0));
        uv.push_back(aRibeiro::vec2(1,1));
        
        uv.push_back(aRibeiro::vec2(0,0));
        uv.push_back(aRibeiro::vec2(1,1));
        uv.push_back(aRibeiro::vec2(0,1));
        
        
        //down (-y)
        vertex.push_back(aRibeiro::vec3(-d, -d,-d));
        vertex.push_back(aRibeiro::vec3( d, -d, d));
        vertex.push_back(aRibeiro::vec3( d, -d,-d));
        
        vertex.push_back(aRibeiro::vec3(-d, -d,-d));
        vertex.push_back(aRibeiro::vec3(-d, -d, d));
        vertex.push_back(aRibeiro::vec3( d, -d, d));
        
        uv.push_back(aRibeiro::vec2(0,1));
        uv.push_back(aRibeiro::vec2(1,0));
        uv.push_back(aRibeiro::vec2(1,1));
        
        uv.push_back(aRibeiro::vec2(0,1));
        uv.push_back(aRibeiro::vec2(0,0));
        uv.push_back(aRibeiro::vec2(1,0));
        
        aRibeiro::quat rot = aRibeiro::quatFromEuler(0, -DEG2RAD(rotation) ,0);
        for(size_t i=0;i<vertex.size();i++){
            vertex[i] = rot * vertex[i];
        }
        
    }
    
    GLSkyBox::~GLSkyBox(){
        aRibeiro::setNullAndDelete( back );
        aRibeiro::setNullAndDelete( front );
        aRibeiro::setNullAndDelete( left );
        aRibeiro::setNullAndDelete( right );
        aRibeiro::setNullAndDelete( bottom );
        aRibeiro::setNullAndDelete( top );
    }
    
    void GLSkyBox::draw(const aRibeiro::mat4& viewMatrix,const aRibeiro::mat4& projectionMatrix, bool leftHanded){
        
        //GLEngine::Engine *engine = GLEngine::Engine::Instance();
        GLEngine::GLRenderState *renderstate = GLEngine::GLRenderState::Instance();
        
        openglWrapper::GLShader* oldShader = renderstate->CurrentShader;
        GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
        bool oldDepthTestEnabled = renderstate->DepthWrite;
        GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;
        
        renderstate->CurrentShader = &shader;
        renderstate->DepthTest = GLEngine::DepthTestDisabled;
        renderstate->DepthWrite = false;
        renderstate->BlendMode = GLEngine::BlendModeDisabled;
        
        shader.setColor(aRibeiro::vec4(1,1,1,1));
        shader.setTexture(0);
        shader.setMatrix( projectionMatrix * aRibeiro::extractRotation( viewMatrix ) );
        
        
        OPENGL_CMD(glEnableVertexAttribArray(shader.vPosition));
        OPENGL_CMD(glVertexAttribPointer(shader.vPosition, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &vertex[0]));
        
        OPENGL_CMD(glEnableVertexAttribArray(shader.vUV));
        OPENGL_CMD(glVertexAttribPointer(shader.vUV, 2, GL_FLOAT, false, sizeof(aRibeiro::vec2), &uv[0]));
        
        if (leftHanded) {
            //front +z
            front->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));
            
            //back -z
            back->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 6, 6));
            
            //left -x
            left->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 18, 6));
            
            
            //right +x
            right->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 12, 6));
            
        } else {
            //front +z
            back->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));
            
            //back -z
            front->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 6, 6));
            
            //left +x
            left->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 12, 6));
            
            //right -x
            right->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 18, 6));
        }
        
        //up +y
        top->active(0);
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 24, 6));
        
        //down -y
        bottom->active(0);
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 30, 6));
        
        
        OPENGL_CMD(glDisableVertexAttribArray(shader.vUV));
        OPENGL_CMD(glDisableVertexAttribArray(shader.vPosition));
        
        renderstate->CurrentShader = oldShader;
        renderstate->DepthTest = oldDepthTest;
        renderstate->DepthWrite = oldDepthTestEnabled;
        renderstate->BlendMode = oldBlendMode;
    }
}


