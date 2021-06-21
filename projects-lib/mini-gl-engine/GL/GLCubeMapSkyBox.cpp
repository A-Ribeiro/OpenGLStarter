#include "GLCubeMapSkyBox.h"

#include <mini-gl-engine/mini-gl-engine.h>

using namespace openglWrapper;

namespace GLEngine {
    
    void GLCubeMapSkyBox::createVertex(float d){
        vertex.clear();
        
        //front (+z)
        vertex.push_back(aRibeiro::vec3(-d,-d,d));
        vertex.push_back(aRibeiro::vec3( d, d,d));
        vertex.push_back(aRibeiro::vec3( d,-d,d));
        
        vertex.push_back(aRibeiro::vec3(-d,-d,d));
        vertex.push_back(aRibeiro::vec3(-d, d,d));
        vertex.push_back(aRibeiro::vec3( d, d,d));
        
        //back (-z)
        vertex.push_back(aRibeiro::vec3(-d,-d,-d));
        vertex.push_back(aRibeiro::vec3( d,-d,-d));
        vertex.push_back(aRibeiro::vec3( d, d,-d));
        
        vertex.push_back(aRibeiro::vec3(-d,-d,-d));
        vertex.push_back(aRibeiro::vec3( d, d,-d));
        vertex.push_back(aRibeiro::vec3(-d, d,-d));
        
        //left (+x)
        vertex.push_back(aRibeiro::vec3(d,-d,-d));
        vertex.push_back(aRibeiro::vec3(d, d, d));
        vertex.push_back(aRibeiro::vec3(d, d,-d));
        
        vertex.push_back(aRibeiro::vec3(d,-d,-d));
        vertex.push_back(aRibeiro::vec3(d,-d, d));
        vertex.push_back(aRibeiro::vec3(d, d, d));
        
        //right (-x)
        vertex.push_back(aRibeiro::vec3(-d,-d,-d));
        vertex.push_back(aRibeiro::vec3(-d, d,-d));
        vertex.push_back(aRibeiro::vec3(-d, d, d));
        
        vertex.push_back(aRibeiro::vec3(-d,-d,-d));
        vertex.push_back(aRibeiro::vec3(-d, d, d));
        vertex.push_back(aRibeiro::vec3(-d,-d, d));
        
        //up (+y)
        vertex.push_back(aRibeiro::vec3(-d, d,-d));
        vertex.push_back(aRibeiro::vec3( d, d,-d));
        vertex.push_back(aRibeiro::vec3( d, d, d));
        
        vertex.push_back(aRibeiro::vec3(-d, d,-d));
        vertex.push_back(aRibeiro::vec3( d, d, d));
        vertex.push_back(aRibeiro::vec3(-d, d, d));
        
        //down (-y)
        vertex.push_back(aRibeiro::vec3(-d, -d,-d));
        vertex.push_back(aRibeiro::vec3( d, -d, d));
        vertex.push_back(aRibeiro::vec3( d, -d,-d));
        
        vertex.push_back(aRibeiro::vec3(-d, -d,-d));
        vertex.push_back(aRibeiro::vec3(-d, -d, d));
        vertex.push_back(aRibeiro::vec3( d, -d, d));
        
    }
    
    GLCubeMapSkyBox::GLCubeMapSkyBox(bool sRGB, 
                                    const std::string &_negz,const std::string &_posz,
                                     const std::string &_negx,const std::string &_posx,
                                     const std::string &_negy,const std::string &_posy,
                                     float _distance,
                                     int cubeResolution, bool leftHanded):cubeMap(sRGB,cubeResolution) {
        vbo = NULL;
        cubeMap.loadFromFile(_negz,_posz,
                            _negx,_posx,
                            _negy,_posy,
                            leftHanded);
        createVertex(_distance);
    }
    
    GLCubeMapSkyBox::GLCubeMapSkyBox(bool sRGB, 
                                     std::string single_file,
                                     float _distance,
                                     int cubeResolution, bool leftHanded): cubeMap(sRGB, cubeResolution) {
        vbo = NULL;
        cubeMap.loadFromSingleFile(single_file, leftHanded);
        createVertex(_distance);
    }
    
    GLCubeMapSkyBox::~GLCubeMapSkyBox(){
        aRibeiro::setNullAndDelete(vbo);
    }
    
    void GLCubeMapSkyBox::createVBO() {
        if (vbo == NULL)
            vbo = new GLVertexBufferObject();
        vbo->uploadData(&vertex[0], sizeof(aRibeiro::vec3)*vertex.size());
    }
    
    void GLCubeMapSkyBox::draw(const aRibeiro::mat4& viewMatrix,const aRibeiro::mat4& projectionMatrix){
        
        //GLEngine::Engine *engine = GLEngine::Engine::Instance();
        GLEngine::GLRenderState *renderstate = GLEngine::GLRenderState::Instance();
        
        openglWrapper::GLShader* oldShader = renderstate->CurrentShader;
        GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
        bool oldDepthTestEnabled = renderstate->DepthWrite;
        GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;
        
        renderstate->CurrentShader = &cubeSkyShader;
        renderstate->DepthTest = GLEngine::DepthTestDisabled;
        renderstate->DepthWrite = false;
        renderstate->BlendMode = GLEngine::BlendModeDisabled;

        //renderstate->ColorWrite = ColorWriteAll;
        
        cubeSkyShader.setCubeTexture(0);
        cubeSkyShader.setMatrix( projectionMatrix * aRibeiro::extractRotation( viewMatrix ) );
        
        cubeMap.active(0);
        
        if (vbo != NULL){
            vbo->setLayout(cubeSkyShader.vPosition, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
            vbo->drawArrays(GL_TRIANGLES, 36);
            vbo->unsetLayout(cubeSkyShader.vPosition);
        } else {
            // direct draw commands
            OPENGL_CMD(glEnableVertexAttribArray(cubeSkyShader.vPosition));
            OPENGL_CMD(glVertexAttribPointer(cubeSkyShader.vPosition, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &vertex[0]));
            
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 36));
            
            OPENGL_CMD(glDisableVertexAttribArray(cubeSkyShader.vPosition));
        }
        
        renderstate->CurrentShader = oldShader;
        renderstate->DepthTest = oldDepthTest;
        renderstate->DepthWrite = oldDepthTestEnabled;
        renderstate->BlendMode = oldBlendMode;
        
        
        cubeMap.deactive(0);
    }


    void GLCubeMapSkyBox::drawAnotherCube(const aRibeiro::mat4& viewMatrix, const aRibeiro::mat4& projectionMatrix, openglWrapper::GLCubeMap *anotherCubeMap) {
        //GLEngine::Engine *engine = GLEngine::Engine::Instance();
        GLEngine::GLRenderState *renderstate = GLEngine::GLRenderState::Instance();

        openglWrapper::GLShader* oldShader = renderstate->CurrentShader;
        GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
        bool oldDepthTestEnabled = renderstate->DepthWrite;
        GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;

        renderstate->CurrentShader = &cubeSkyShader;
        renderstate->DepthTest = GLEngine::DepthTestDisabled;
        renderstate->DepthWrite = false;
        renderstate->BlendMode = GLEngine::BlendModeDisabled;

        cubeSkyShader.setCubeTexture(0);
        cubeSkyShader.setMatrix(projectionMatrix * aRibeiro::extractRotation(viewMatrix));

        anotherCubeMap->active(0);

        if (vbo != NULL) {
            vbo->setLayout(cubeSkyShader.vPosition, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
            vbo->drawArrays(GL_TRIANGLES, 36);
            vbo->unsetLayout(cubeSkyShader.vPosition);
        }
        else {
            // direct draw commands
            OPENGL_CMD(glEnableVertexAttribArray(cubeSkyShader.vPosition));
            OPENGL_CMD(glVertexAttribPointer(cubeSkyShader.vPosition, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &vertex[0]));

            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 36));

            OPENGL_CMD(glDisableVertexAttribArray(cubeSkyShader.vPosition));
        }

        renderstate->CurrentShader = oldShader;
        renderstate->DepthTest = oldDepthTest;
        renderstate->DepthWrite = oldDepthTestEnabled;
        renderstate->BlendMode = oldBlendMode;


        anotherCubeMap->deactive(0);
    }
}


