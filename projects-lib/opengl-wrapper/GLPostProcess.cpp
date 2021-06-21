#include "GLPostProcess.h"
#include <aribeiro/common.h>
#include <aribeiro/SetNullAndDelete.h>
#include <opengl-wrapper/PlatformGL.h>

using namespace aRibeiro;

namespace openglWrapper {

    //private copy constructores, to avoid copy...
    GLPostProcess::GLPostProcess(const GLPostProcess& v) {}
    void GLPostProcess::operator=(const GLPostProcess& v) {}

    GLPostProcess::GLPostProcess() {
        width = -1;
        height = -1;

        firstRender = new GLFramebufferObject();
        //firstRender->depth = new GLTexture();
        firstRender->useRenderbufferDepth = true;
        firstRender->color.push_back(new GLTexture());

        for (int i=0;i<2;i++){
            fbo[i] = new GLFramebufferObject();
            fbo[i]->color.push_back(new GLTexture());
        }

        resize(32, 32);

        firstRender->attachTextures();
        for (int i=0;i<2;i++){
            fbo[i]->attachTextures();
        }

        current = 0;
    }

    GLPostProcess::~GLPostProcess() {

        //setNullAndDelete( firstRender->depth );
        setNullAndDelete( firstRender->color[0] );
        setNullAndDelete( firstRender );
        for (int i=0;i<2;i++){
            setNullAndDelete( fbo[i]->color[0] );
            setNullAndDelete( fbo[i] );
        }

    }

    void GLPostProcess::resize(int w, int h) {

        if (w == width && h == height)
            return;

        width = w;
        height = h;

        firstRender->setSize(w, h);
        for (int i=0;i<2;i++){
            fbo[i]->setSize(w, h);
        }
    }

    void GLPostProcess::beginDraw() {
        if (pipeline.size() == 0)
            return;

        firstRender->enable();
        glViewport(0,0,firstRender->width,firstRender->height);
    }

    void GLPostProcess::endDraw() {
        if (pipeline.size() == 0)
            return;

        GLboolean depthTest;
        glGetBooleanv (GL_DEPTH_TEST, &depthTest);

        //glPushAttrib(GL_ENABLE_BIT);
        if (depthTest)
            glDisable(GL_DEPTH_TEST);

        GLFramebufferObject* src = firstRender;
        GLFramebufferObject* dst = fbo[current];
        current = (current + 1) % 2;

        for (int i=0; i<pipeline.size(); i++) {
            if (i == pipeline.size() - 1){
                pipeline[i]->render( this, src->color[0], firstRender->depth, NULL);
            } else {
                pipeline[i]->render( this, src->color[0], firstRender->depth, dst);
                src = dst;
                dst = fbo[current];
                current = (current + 1) % 2;
            }
        }

        //glPopAttrib();
        if (depthTest)
            glEnable(GL_DEPTH_TEST);
    }


    void GLPostProcess::drawQuad(GLPostProcessingShader *shader) {
        const vec2 vpos[] = {
            vec2(-1, -1),
            vec2(1, -1),
            vec2(1,  1),
            vec2(-1,  1)
        };
        const vec2 vuv[] = {
            vec2(0, 0),
            vec2(1, 0),
            vec2(1, 1),
            vec2(0, 1)
        };

        const uint8_t _SSE2_ALIGN_PRE indices[] _SSE2_ALIGN_POS = {
            0,1,2,
            0,2,3
        };

        OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2Position));
        OPENGL_CMD(glVertexAttribPointer(shader->aVec2Position, 2, GL_FLOAT, false, sizeof(vec2), &vpos[0]));
        OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
        OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(vec2), &vuv[0]));

        //OPENGL_CMD(glDrawArrays(GL_QUADS, 0, 4));
        OPENGL_CMD(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices));

        OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2Position));
        OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));
    }

}

