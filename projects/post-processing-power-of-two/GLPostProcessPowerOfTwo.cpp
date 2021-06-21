#include "GLPostProcessPowerOfTwo.h"
#include <aribeiro/common.h>
#include <aribeiro/SetNullAndDelete.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace openglWrapper;
#include <stdio.h>

namespace aRibeiro {

    //private copy constructores, to avoid copy...
    GLPostProcessPowerOfTwo::GLPostProcessPowerOfTwo(const GLPostProcessPowerOfTwo& v) {}
    void GLPostProcessPowerOfTwo::operator=(const GLPostProcessPowerOfTwo& v) {}

    GLPostProcessPowerOfTwo::GLPostProcessPowerOfTwo() {
        width = -1;
        height = -1;

        potWidth = -1;
        potHeight = -1;

        firstRender = new GLFramebufferObject();
        //firstRender->depth = new GLTexture();
        firstRender->useRenderbufferDepth = true;
        firstRender->color.push_back(new GLTexture());

        for (int i=0;i<2;i++){
            fbo[i] = new GLFramebufferObject();
            fbo[i]->color.push_back(new GLTexture());
        }

        resize(32,32);

        firstRender->attachTextures();
        for (int i=0;i<2;i++)
            fbo[i]->attachTextures();

        current = 0;
    }

    GLPostProcessPowerOfTwo::~GLPostProcessPowerOfTwo() {

        //setNullAndDelete( firstRender->depth );
        setNullAndDelete( firstRender->color[0] );
        setNullAndDelete( firstRender );
        for (int i=0;i<2;i++){
            setNullAndDelete( fbo[i]->color[0] );
            setNullAndDelete( fbo[i] );
        }

    }

    void GLPostProcessPowerOfTwo::resize(int w, int h) {

        if (width == w && height == h)
            return;

        width = w;
        height = h;

        //find the POT width and height

        potWidth = 1;
        potHeight = 1;

        while ( potWidth < width )
            potWidth = potWidth << 1;
        while ( potHeight < height )
            potHeight = potHeight << 1;

        printf("[%s:%i]\n", __FILE__, __LINE__);
        printf("Resize potsize: %i %i, using resolution: %i %i\n",potWidth, potHeight, width, height);

        texelNeighbor = vec2(1.0f/potWidth,1.0f/potHeight);
        drawQuadMaxUV = vec2((float)width/(float)potWidth, (float)height/(float)potHeight);
        texelMaxUVAccess = drawQuadMaxUV - texelNeighbor * 0.5f;

        firstRender->setSize(potWidth, potHeight);
        for (int i=0;i<2;i++){
            fbo[i]->setSize(potWidth, potHeight);
        }
    }

    void GLPostProcessPowerOfTwo::beginDraw() {
        if (pipeline.size() == 0)
            return;

        firstRender->enable();
        glViewport(0,0,width,height);
    }

    void GLPostProcessPowerOfTwo::endDraw() {
        if (pipeline.size() == 0)
            return;

        //glPushAttrib(GL_ENABLE_BIT);
        GLboolean depthTest;
        glGetBooleanv (GL_DEPTH_TEST, &depthTest);

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

        if (depthTest)
            glEnable(GL_DEPTH_TEST);
        //glPopAttrib();
    }



    void GLPostProcessPowerOfTwo::drawQuad(GLPostProcessingPowerOfTwoShader *shader,
                                           const vec2 &texelNeighbor,
                                           const vec2 &texelMaxUVAccess,
                                           const vec2 &drawQuadMaxUV) {
        const vec2 vpos[] = {
            vec2(-1, -1),
            vec2(1, -1),
            vec2(1,  1),
            vec2(-1,  1)
        };
        vec2 vuv[] = {
            vec2(0, 0),
            vec2(drawQuadMaxUV.x, 0),
            vec2(drawQuadMaxUV.x, drawQuadMaxUV.y),
            vec2(0, drawQuadMaxUV.y)
        };
        
        const uint8_t _SSE2_ALIGN_PRE indices[] _SSE2_ALIGN_POS= {
            0,1,2,
            0,2,3
        };

        shader->setTexelNeighbor(texelNeighbor);
        shader->setTexelMaxUVAccess(texelMaxUVAccess);

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

