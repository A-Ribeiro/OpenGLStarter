#include <appkit-gl-engine/GL/GLSkyBox.h>

#include <appkit-gl-engine/GL/GLRenderState.h>

// #include <appkit-gl-engine/mini-gl-engine.h>

// using namespace AppKit::OpenGL;

namespace AppKit
{
    namespace GLEngine
    {
        GLSkyBox::GLSkyBox(bool sRGB,
                           const std::string &_negz, const std::string &_posz,
                           const std::string &_negx, const std::string &_posx,
                           const std::string &_negy, const std::string &_posy,
                           float d, float rotation)
        {

            back = NULL;
            front = NULL;
            left = NULL;
            right = NULL;
            bottom = NULL;
            top = NULL;

            back = AppKit::OpenGL::GLTexture::loadFromFile(_negz.c_str());
            back->generateMipMap();
            front = AppKit::OpenGL::GLTexture::loadFromFile(_posz.c_str());
            front->generateMipMap();

            left = AppKit::OpenGL::GLTexture::loadFromFile(_negx.c_str());
            left->generateMipMap();
            right = AppKit::OpenGL::GLTexture::loadFromFile(_posx.c_str());
            right->generateMipMap();

            bottom = AppKit::OpenGL::GLTexture::loadFromFile(_negy.c_str());
            bottom->generateMipMap();
            top = AppKit::OpenGL::GLTexture::loadFromFile(_posy.c_str());
            top->generateMipMap();

            // front (+z)
            vertex.push_back(MathCore::vec3f(-d, -d, d));
            vertex.push_back(MathCore::vec3f(d, d, d));
            vertex.push_back(MathCore::vec3f(d, -d, d));

            vertex.push_back(MathCore::vec3f(-d, -d, d));
            vertex.push_back(MathCore::vec3f(-d, d, d));
            vertex.push_back(MathCore::vec3f(d, d, d));

            uv.push_back(MathCore::vec2f(0, 1));
            uv.push_back(MathCore::vec2f(1, 0));
            uv.push_back(MathCore::vec2f(1, 1));

            uv.push_back(MathCore::vec2f(0, 1));
            uv.push_back(MathCore::vec2f(0, 0));
            uv.push_back(MathCore::vec2f(1, 0));

            // back (-z)
            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, d, -d));

            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, d, -d));
            vertex.push_back(MathCore::vec3f(-d, d, -d));

            uv.push_back(MathCore::vec2f(1, 1));
            uv.push_back(MathCore::vec2f(0, 1));
            uv.push_back(MathCore::vec2f(0, 0));

            uv.push_back(MathCore::vec2f(1, 1));
            uv.push_back(MathCore::vec2f(0, 0));
            uv.push_back(MathCore::vec2f(1, 0));

            // left (+x)
            vertex.push_back(MathCore::vec3f(d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, d, d));
            vertex.push_back(MathCore::vec3f(d, d, -d));

            vertex.push_back(MathCore::vec3f(d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, -d, d));
            vertex.push_back(MathCore::vec3f(d, d, d));

            uv.push_back(MathCore::vec2f(1, 1));
            uv.push_back(MathCore::vec2f(0, 0));
            uv.push_back(MathCore::vec2f(1, 0));

            uv.push_back(MathCore::vec2f(1, 1));
            uv.push_back(MathCore::vec2f(0, 1));
            uv.push_back(MathCore::vec2f(0, 0));

            // right (-x)
            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(-d, d, -d));
            vertex.push_back(MathCore::vec3f(-d, d, d));

            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(-d, d, d));
            vertex.push_back(MathCore::vec3f(-d, -d, d));

            uv.push_back(MathCore::vec2f(0, 1));
            uv.push_back(MathCore::vec2f(0, 0));
            uv.push_back(MathCore::vec2f(1, 0));

            uv.push_back(MathCore::vec2f(0, 1));
            uv.push_back(MathCore::vec2f(1, 0));
            uv.push_back(MathCore::vec2f(1, 1));

            // up (+y)
            vertex.push_back(MathCore::vec3f(-d, d, -d));
            vertex.push_back(MathCore::vec3f(d, d, -d));
            vertex.push_back(MathCore::vec3f(d, d, d));

            vertex.push_back(MathCore::vec3f(-d, d, -d));
            vertex.push_back(MathCore::vec3f(d, d, d));
            vertex.push_back(MathCore::vec3f(-d, d, d));

            uv.push_back(MathCore::vec2f(0, 0));
            uv.push_back(MathCore::vec2f(1, 0));
            uv.push_back(MathCore::vec2f(1, 1));

            uv.push_back(MathCore::vec2f(0, 0));
            uv.push_back(MathCore::vec2f(1, 1));
            uv.push_back(MathCore::vec2f(0, 1));

            // down (-y)
            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(d, -d, d));
            vertex.push_back(MathCore::vec3f(d, -d, -d));

            vertex.push_back(MathCore::vec3f(-d, -d, -d));
            vertex.push_back(MathCore::vec3f(-d, -d, d));
            vertex.push_back(MathCore::vec3f(d, -d, d));

            uv.push_back(MathCore::vec2f(0, 1));
            uv.push_back(MathCore::vec2f(1, 0));
            uv.push_back(MathCore::vec2f(1, 1));

            uv.push_back(MathCore::vec2f(0, 1));
            uv.push_back(MathCore::vec2f(0, 0));
            uv.push_back(MathCore::vec2f(1, 0));

            MathCore::quatf rot = MathCore::GEN<MathCore::quatf>::fromEuler(0, -MathCore::OP<float>::deg_2_rad(rotation), 0);
            for (size_t i = 0; i < vertex.size(); i++)
            {
                vertex[i] = rot * vertex[i];
            }
        }

        GLSkyBox::~GLSkyBox()
        {
            if (back != NULL)
                delete  back;
            if (front != NULL)
                delete  front;
            if (left != NULL)
                delete  left;
            if (right != NULL)
                delete  right;
            if (bottom != NULL)
                delete  bottom;
            if (top != NULL)
                delete  top;
        }

        void GLSkyBox::draw(const MathCore::mat4f &viewMatrix, const MathCore::mat4f &projectionMatrix, bool leftHanded)
        {

            // AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
            AppKit::GLEngine::GLRenderState *renderstate = AppKit::GLEngine::GLRenderState::Instance();

            AppKit::OpenGL::GLShader *oldShader = renderstate->CurrentShader;
            AppKit::GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
            bool oldDepthTestEnabled = renderstate->DepthWrite;
            AppKit::GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;

            renderstate->CurrentShader = &shader;
            renderstate->DepthTest = AppKit::GLEngine::DepthTestDisabled;
            renderstate->DepthWrite = false;
            renderstate->BlendMode = AppKit::GLEngine::BlendModeDisabled;

            shader.setColor(MathCore::vec4f(1, 1, 1, 1));
            shader.setTexture(0);
            shader.setMatrix(projectionMatrix * MathCore::OP<MathCore::mat4f>::extractRotation(viewMatrix));

            OPENGL_CMD(glEnableVertexAttribArray(shader.vPosition));
            OPENGL_CMD(glVertexAttribPointer(shader.vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));

            OPENGL_CMD(glEnableVertexAttribArray(shader.vUV));
            OPENGL_CMD(glVertexAttribPointer(shader.vUV, 2, GL_FLOAT, false, sizeof(MathCore::vec2f), &uv[0]));

            if (leftHanded)
            {
                // front +z
                front->active(0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

                // back -z
                back->active(0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 6, 6));

                // left -x
                left->active(0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 18, 6));

                // right +x
                right->active(0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 12, 6));
            }
            else
            {
                // front +z
                back->active(0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

                // back -z
                front->active(0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 6, 6));

                // left +x
                left->active(0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 12, 6));

                // right -x
                right->active(0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 18, 6));
            }

            // up +y
            top->active(0);
            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 24, 6));

            // down -y
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
}
