#include <appkit-gl-engine/Renderer/ParticleSystemRenderer.h>

#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

namespace AppKit
{
    namespace GLEngine
    {

        ParticleSystemRenderer::ParticleSystemRenderer()
        {
            debugLinesShader_AttribLocation_Pos = debugLinesShader.queryAttribLocation("aPosition");

            normalShader_pos = normalShader.queryAttribLocation("aPosition");
            normalShader_uv = normalShader.queryAttribLocation("aUV0");
            normalShader_color = normalShader.queryAttribLocation("aColor0");
            normalShader_size = normalShader.queryAttribLocation("aSize");
            normalShader_alpha = normalShader.queryAttribLocation("aAlpha");

            softShader_pos = softShader.queryAttribLocation("aPosition");
            softShader_uv = softShader.queryAttribLocation("aUV0");
            softShader_color = softShader.queryAttribLocation("aColor0");
            softShader_size = softShader.queryAttribLocation("aSize");
            softShader_alpha = softShader.queryAttribLocation("aAlpha");
        }

        void ParticleSystemRenderer::drawDebugPoints(
            const Components::ComponentCamera *camera,
            const Components::ComponentParticleSystem *particleSystem,
            float size)
        {

            size *= 0.5f;
            MathCore::vec3f xneg = MathCore::vec3f(-size, 0, 0);
            MathCore::vec3f xpos = MathCore::vec3f(size, 0, 0);

            MathCore::vec3f yneg = MathCore::vec3f(0, -size, 0);
            MathCore::vec3f ypos = MathCore::vec3f(0, size, 0);

            MathCore::vec3f zneg = MathCore::vec3f(0, 0, -size);
            MathCore::vec3f zpos = MathCore::vec3f(0, 0, size);

            lines.clear();
            for (int i = 0; i < particleSystem->particles.size(); i++)
            {
                const Components::Particle &p = particleSystem->particles[i];
                lines.push_back(p.pos - xneg);
                lines.push_back(p.pos - xpos);

                lines.push_back(p.pos - yneg);
                lines.push_back(p.pos - ypos);

                lines.push_back(p.pos - zneg);
                lines.push_back(p.pos - zpos);
            }

            GLRenderState *state = GLRenderState::Instance();

            state->BlendMode = BlendModeDisabled;
            state->DepthTest = DepthTestLessEqual;
            state->CurrentShader = &debugLinesShader;

            debugLinesShader.setColor(MathCore::vec4f(0.0f, 1.0f, 0.0f, 1.0f));
            debugLinesShader.setMVP(camera->viewProjection);

            OPENGL_CMD(glEnableVertexAttribArray(debugLinesShader_AttribLocation_Pos));
            OPENGL_CMD(glVertexAttribPointer(debugLinesShader_AttribLocation_Pos, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &lines[0]));

            OPENGL_CMD(glDrawArrays(GL_LINES, 0, lines.size()));

            OPENGL_CMD(glDisableVertexAttribArray(debugLinesShader_AttribLocation_Pos));
        }

        void ParticleSystemRenderer::draw(const Components::ComponentCamera *camera,
                                          const Components::ComponentParticleSystem *particleSystem)
        {

            if (particleSystem->texture == NULL)
                return;

            NormalAttributes aux;
            normalVertex.clear();
            for (int i = 0; i < particleSystem->particles.size(); i++)
            {
                const Components::Particle &p = particleSystem->particles[i];

                aux.pos = p.pos;
                aux.color = p.color;
                aux.size = p.size;
                aux.alpha = p.alpha;

                aux.uv = MathCore::vec2f(0, 0);
                normalVertex.push_back(aux);
                aux.uv = MathCore::vec2f(0, 1);
                normalVertex.push_back(aux);
                aux.uv = MathCore::vec2f(1, 1);
                normalVertex.push_back(aux);

                aux.uv = MathCore::vec2f(0, 0);
                normalVertex.push_back(aux);
                aux.uv = MathCore::vec2f(1, 1);
                normalVertex.push_back(aux);
                aux.uv = MathCore::vec2f(1, 0);
                normalVertex.push_back(aux);
            }

            GLRenderState *state = GLRenderState::Instance();

            state->BlendMode = BlendModeAlpha;
            state->DepthTest = DepthTestLessEqual;
            state->DepthWrite = false;
            state->CurrentShader = &normalShader;

            particleSystem->texture->active(0);

            normalShader.setMVP(camera->viewProjection);
            normalShader.setV_inv(camera->viewInv);
            normalShader.setTexture(0);
            normalShader.setColor(particleSystem->textureColor);

            /*
            int normalShader_pos;
            int normalShader_uv;
            int normalShader_color;
            int normalShader_size;
            int normalShader_alpha
            */

            OPENGL_CMD(glEnableVertexAttribArray(normalShader_pos));
            OPENGL_CMD(glVertexAttribPointer(normalShader_pos, 3, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].pos));

            OPENGL_CMD(glEnableVertexAttribArray(normalShader_uv));
            OPENGL_CMD(glVertexAttribPointer(normalShader_uv, 2, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].uv));

            OPENGL_CMD(glEnableVertexAttribArray(normalShader_color));
            OPENGL_CMD(glVertexAttribPointer(normalShader_color, 3, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].color));

            OPENGL_CMD(glEnableVertexAttribArray(normalShader_size));
            OPENGL_CMD(glVertexAttribPointer(normalShader_size, 1, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].size));

            OPENGL_CMD(glEnableVertexAttribArray(normalShader_alpha));
            OPENGL_CMD(glVertexAttribPointer(normalShader_alpha, 1, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].alpha));

            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, normalVertex.size()));

            OPENGL_CMD(glDisableVertexAttribArray(normalShader_pos));
            OPENGL_CMD(glDisableVertexAttribArray(normalShader_uv));
            OPENGL_CMD(glDisableVertexAttribArray(normalShader_color));
            OPENGL_CMD(glDisableVertexAttribArray(normalShader_size));
            OPENGL_CMD(glDisableVertexAttribArray(normalShader_alpha));

            particleSystem->texture->deactive(0);

            state->DepthWrite = true;
        }

        void ParticleSystemRenderer::drawSoftDepthComponent24(const Components::ComponentCamera *camera,
                                                              const Components::ComponentParticleSystem *particleSystem,
                                                              const AppKit::OpenGL::GLTexture *depthComponent24)
        {

            if (particleSystem->texture == NULL)
                return;

            NormalAttributes aux;
            normalVertex.clear();
            for (int i = 0; i < particleSystem->particles.size(); i++)
            {
                const Components::Particle &p = particleSystem->particles[i];

                aux.pos = p.pos;
                aux.color = p.color;
                aux.size = p.size;
                aux.alpha = p.alpha;

                aux.uv = MathCore::vec2f(0, 0);
                normalVertex.push_back(aux);
                aux.uv = MathCore::vec2f(0, 1);
                normalVertex.push_back(aux);
                aux.uv = MathCore::vec2f(1, 1);
                normalVertex.push_back(aux);

                aux.uv = MathCore::vec2f(0, 0);
                normalVertex.push_back(aux);
                aux.uv = MathCore::vec2f(1, 1);
                normalVertex.push_back(aux);
                aux.uv = MathCore::vec2f(1, 0);
                normalVertex.push_back(aux);
            }

            GLRenderState *state = GLRenderState::Instance();

            state->BlendMode = BlendModeAlpha;
            state->DepthTest = DepthTestLessEqual;
            state->DepthWrite = false;
            state->CurrentShader = &softShader;

            particleSystem->texture->active(0);
            depthComponent24->active(1);

            softShader.setMVP(camera->viewProjection);
            softShader.setV_inv(camera->viewInv);
            softShader.setTexture(0);
            softShader.setDepthTextureComponent24(1);
            softShader.setColor(particleSystem->textureColor);

            iRect s = particleSystem->transform[0]->renderWindowRegion->Viewport;
            MathCore::vec2f ss = MathCore::vec2f(s.w, s.h);
            softShader.setScreenSize(ss);

            // get texture size...
            /*
            int texDims[2];
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texDims[0]);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texDims[1]);
            softShader.setScreenSize(MathCore::vec2f( texDims[0], texDims[1] ));
            */

            if (camera->compareType(Components::ComponentCameraPerspective::Type))
            {
                Components::ComponentCameraPerspective *perspect = (Components::ComponentCameraPerspective *)camera;
                softShader.setCamera_FMinusN_FPlusN_FTimesNTimes2_N(MathCore::vec4f(
                    perspect->farPlane - perspect->nearPlane,
                    perspect->farPlane + perspect->nearPlane,
                    perspect->farPlane * perspect->nearPlane * 2.0f,
                    perspect->nearPlane));
            }
            else if (camera->compareType(Components::ComponentCameraOrthographic::Type))
            {
                Components::ComponentCameraOrthographic *ortho = (Components::ComponentCameraOrthographic *)camera;
                softShader.setCamera_FMinusN_FPlusN_FTimesNTimes2_N(MathCore::vec4f(
                    ortho->farPlane - ortho->nearPlane,
                    ortho->farPlane + ortho->nearPlane,
                    ortho->farPlane * ortho->nearPlane * 2.0f,
                    ortho->nearPlane));
            }

            /*
            int normalShader_pos;
            int normalShader_uv;
            int normalShader_color;
            int normalShader_size;
            int normalShader_alpha
            */

            OPENGL_CMD(glEnableVertexAttribArray(softShader_pos));
            OPENGL_CMD(glVertexAttribPointer(softShader_pos, 3, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].pos));

            OPENGL_CMD(glEnableVertexAttribArray(softShader_uv));
            OPENGL_CMD(glVertexAttribPointer(softShader_uv, 2, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].uv));

            OPENGL_CMD(glEnableVertexAttribArray(softShader_color));
            OPENGL_CMD(glVertexAttribPointer(softShader_color, 3, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].color));

            OPENGL_CMD(glEnableVertexAttribArray(softShader_size));
            OPENGL_CMD(glVertexAttribPointer(softShader_size, 1, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].size));

            OPENGL_CMD(glEnableVertexAttribArray(softShader_alpha));
            OPENGL_CMD(glVertexAttribPointer(softShader_alpha, 1, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].alpha));

            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, normalVertex.size()));

            OPENGL_CMD(glDisableVertexAttribArray(softShader_pos));
            OPENGL_CMD(glDisableVertexAttribArray(softShader_uv));
            OPENGL_CMD(glDisableVertexAttribArray(softShader_color));
            OPENGL_CMD(glDisableVertexAttribArray(softShader_size));
            OPENGL_CMD(glDisableVertexAttribArray(softShader_alpha));

            depthComponent24->deactive(1);
            particleSystem->texture->deactive(0);

            state->DepthWrite = true;
        }

    }
}