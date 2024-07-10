#include <appkit-gl-engine/GL/GLCubeMapSkyBox.h>

#include <appkit-gl-engine/GL/GLRenderState.h>

// #include <appkit-gl-engine/mini-gl-engine.h>

// using namespace AppKit::OpenGL;

namespace AppKit
{
    namespace GLEngine
    {

        void GLCubeMapSkyBox::createVertex(float d)
        {
            vertex.clear();

            // front (+z)
            vertex.push_back(MathCore::vec3f(-d, -d, d));
            vertex.push_back(MathCore::vec3f(d, d, d));
            vertex.push_back(MathCore::vec3f(d, -d, d));

            vertex.push_back(MathCore::vec3f(-d, -d, d));
            vertex.push_back(MathCore::vec3f(-d, d, d));
            vertex.push_back(MathCore::vec3f(d, d, d));

            // back (-z)
            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, d, -d));

            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, d, -d));
            vertex.push_back(MathCore::vec3f(-d, d, -d));

            // left (+x)
            vertex.push_back(MathCore::vec3f(d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, d, d));
            vertex.push_back(MathCore::vec3f(d, d, -d));

            vertex.push_back(MathCore::vec3f(d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, -d, d));
            vertex.push_back(MathCore::vec3f(d, d, d));

            // right (-x)
            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(-d, d, -d));
            vertex.push_back(MathCore::vec3f(-d, d, d));

            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(-d, d, d));
            vertex.push_back(MathCore::vec3f(-d, -d, d));

            // up (+y)
            vertex.push_back(MathCore::vec3f(-d, d, -d));
            vertex.push_back(MathCore::vec3f(d, d, -d));
            vertex.push_back(MathCore::vec3f(d, d, d));

            vertex.push_back(MathCore::vec3f(-d, d, -d));
            vertex.push_back(MathCore::vec3f(d, d, d));
            vertex.push_back(MathCore::vec3f(-d, d, d));

            // down (-y)
            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, -d, d));
            vertex.push_back(MathCore::vec3f(d, -d, -d));

            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(-d, -d, d));
            vertex.push_back(MathCore::vec3f(d, -d, d));
        }

        GLCubeMapSkyBox::GLCubeMapSkyBox(bool sRGB,
                                         const std::string &_negz, const std::string &_posz,
                                         const std::string &_negx, const std::string &_posx,
                                         const std::string &_negy, const std::string &_posy,
                                         float _distance,
                                         int cubeResolution, bool leftHanded) : cubeMap(0, 0, 0xffffffff, cubeResolution)
        {
            vbo = NULL;
            cubeMap.loadFromFile(_negz, _posz,
                                 _negx, _posx,
                                 _negy, _posy,
                                 sRGB, leftHanded);
            createVertex(_distance);
        }

        GLCubeMapSkyBox::GLCubeMapSkyBox(bool sRGB,
                                         std::string single_file,
                                         float _distance,
                                         int cubeResolution, bool leftHanded) : cubeMap(sRGB, cubeResolution)
        {
            vbo = NULL;
            cubeMap.loadFromSingleFile(single_file, sRGB, leftHanded);
            createVertex(_distance);
        }

        GLCubeMapSkyBox::~GLCubeMapSkyBox()
        {
            if (vbo != NULL)
                delete vbo;
        }

        void GLCubeMapSkyBox::createVBO()
        {
            if (vbo == NULL)
                vbo = new AppKit::OpenGL::GLVertexBufferObject();
            vbo->uploadData(&vertex[0], sizeof(MathCore::vec3f) * (int)vertex.size());
        }

        void GLCubeMapSkyBox::draw(const MathCore::mat4f &viewMatrix, const MathCore::mat4f &projectionMatrix)
        {

            // AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
            AppKit::GLEngine::GLRenderState *renderstate = AppKit::GLEngine::GLRenderState::Instance();

            AppKit::OpenGL::GLShader *oldShader = renderstate->CurrentShader;
            AppKit::GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
            bool oldDepthTestEnabled = renderstate->DepthWrite;
            AppKit::GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;

            renderstate->CurrentShader = &cubeSkyShader;
            renderstate->DepthTest = AppKit::GLEngine::DepthTestDisabled;
            renderstate->DepthWrite = false;
            renderstate->BlendMode = AppKit::GLEngine::BlendModeDisabled;

            // renderstate->ColorWrite = ColorWriteAll;

            cubeSkyShader.setCubeTexture(0);
            cubeSkyShader.setMatrix(projectionMatrix * MathCore::OP<MathCore::mat4f>::extractRotation(viewMatrix));

            cubeMap.active(0);

            if (vbo != NULL)
            {
                vbo->setLayout(cubeSkyShader.vPosition, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                vbo->drawArrays(GL_TRIANGLES, 36);
                vbo->unsetLayout(cubeSkyShader.vPosition);
            }
            else
            {
                // direct draw commands
                OPENGL_CMD(glEnableVertexAttribArray(cubeSkyShader.vPosition));
                OPENGL_CMD(glVertexAttribPointer(cubeSkyShader.vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));

                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 36));

                OPENGL_CMD(glDisableVertexAttribArray(cubeSkyShader.vPosition));
            }

            renderstate->CurrentShader = oldShader;
            renderstate->DepthTest = oldDepthTest;
            renderstate->DepthWrite = oldDepthTestEnabled;
            renderstate->BlendMode = oldBlendMode;

            cubeMap.deactive(0);
        }

        void GLCubeMapSkyBox::drawAnotherCube(const MathCore::mat4f &viewMatrix, const MathCore::mat4f &projectionMatrix, AppKit::OpenGL::GLCubeMap *anotherCubeMap)
        {
            // AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
            AppKit::GLEngine::GLRenderState *renderstate = AppKit::GLEngine::GLRenderState::Instance();

            AppKit::OpenGL::GLShader *oldShader = renderstate->CurrentShader;
            AppKit::GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
            bool oldDepthTestEnabled = renderstate->DepthWrite;
            AppKit::GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;

            renderstate->CurrentShader = &cubeSkyShader;
            renderstate->DepthTest = AppKit::GLEngine::DepthTestDisabled;
            renderstate->DepthWrite = false;
            renderstate->BlendMode = AppKit::GLEngine::BlendModeDisabled;

            cubeSkyShader.setCubeTexture(0);
            cubeSkyShader.setMatrix(projectionMatrix * MathCore::OP<MathCore::mat4f>::extractRotation(viewMatrix));

            anotherCubeMap->active(0);

            if (vbo != NULL)
            {
                vbo->setLayout(cubeSkyShader.vPosition, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                vbo->drawArrays(GL_TRIANGLES, 36);
                vbo->unsetLayout(cubeSkyShader.vPosition);
            }
            else
            {
                // direct draw commands
                OPENGL_CMD(glEnableVertexAttribArray(cubeSkyShader.vPosition));
                OPENGL_CMD(glVertexAttribPointer(cubeSkyShader.vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));

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

}
