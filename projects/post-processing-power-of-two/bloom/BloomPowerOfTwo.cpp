#include "BloomPowerOfTwo.h"
#include <aribeiro/common.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace openglWrapper;
#include <aribeiro/SetNullAndDelete.h>

#include <stdio.h>

namespace aRibeiro {

    //private copy constructores, to avoid copy...
    BloomPowerOfTwo::BloomPowerOfTwo(const BloomPowerOfTwo& v) {}
    void BloomPowerOfTwo::operator=(const BloomPowerOfTwo& v) {}

    void BloomPowerOfTwo::setSize(int w, int h) {

        w = w / 2;
        h = h / 2;

        if (this->width != w || this->height != h){

            this->width = w;
            this->height = h;

            potWidth = 1;
            potHeight = 1;
            while (potWidth < w)
                potWidth = potWidth << 1;
            while (potHeight < h)
                potHeight = potHeight << 1;

            printf("[%s:%i]\n", __FILE__, __LINE__);
            printf("Resize potsize: %i %i, using resolution: %i %i\n",potWidth, potHeight, width, height);

            texelNeighbor = vec2(1.0f/potWidth,1.0f/potHeight);
            drawQuadMaxUV = vec2((float)width/(float)potWidth, (float)height/(float)potHeight);
            texelMaxUVAccess = drawQuadMaxUV - texelNeighbor * 0.5f;

            fbo1->setSize(potWidth,potHeight);
            fbo2->setSize(potWidth,potHeight);
        }

        //int potWidth, potHeight;// will hold the power of two size
        //vec2 texelNeighbor;
        //vec2 texelMaxUVAccess;
        //vec2 texelMaxUV;

        /*
        if (fbo1->width != w || fbo1->height != h){
            fbo1->setSize(w/2,h/2);
            fbo2->setSize(w/2,h/2);

            neighborhood = vec2(1.0f/(float)fbo1->width, 1.0f/(float)fbo1->height);
        }*/
    }

    BloomPowerOfTwo::BloomPowerOfTwo() {
        potWidth = -1;
        potHeight = -1;
        width = -1;
        height = -1;

        fbo1 = new GLFramebufferObject();
        fbo1->color.push_back(new GLTexture());
        fbo2 = new GLFramebufferObject();
        fbo2->color.push_back(new GLTexture());

        setSize(32,32);
        fbo1->attachTextures();
        fbo2->attachTextures();

        thresholdShader = new BloomPowerOfTwoThresholdShader();
        blurShader = new BloomPowerOfTwoBlurShader();
        compositeShader = new BloomPowerOfTwoCompositeShader();

        threshold = 0.5f;
        intensity = 0.5f;
    }

    BloomPowerOfTwo::~BloomPowerOfTwo() {
        setNullAndDelete(fbo1->color[0]);
        setNullAndDelete(fbo1);
        setNullAndDelete(fbo2->color[0]);
        setNullAndDelete(fbo2);

        setNullAndDelete(thresholdShader);
        setNullAndDelete(blurShader);
        setNullAndDelete(compositeShader);
    }

    void BloomPowerOfTwo::render(GLPostProcessPowerOfTwo *postProcessPOT, GLTexture *colorSrc, GLTexture *depthSrc, GLFramebufferObject *dst){
        setSize(postProcessPOT->width,postProcessPOT->height);



        //
        // 1st pass -- threshold
        //
        fbo1->enable();
        glViewport(0,0,width,height);

        thresholdShader->enable();
        colorSrc->active(0);
        thresholdShader->setTexture(0);

        //max texel access from the source image
        //thresholdShader->setTexelNeighbor( postProcessPOT->texelNeighbor );
        //thresholdShader->setTexelMaxUVAccess( postProcessPOT->texelMaxUVAccess );

        thresholdShader->setThreshold(threshold);

        GLPostProcessPowerOfTwo::drawQuad(thresholdShader,
                                          postProcessPOT->texelNeighbor,
                                          postProcessPOT->texelMaxUVAccess,
                                          postProcessPOT->drawQuadMaxUV);


        //
        // 2nd pass -- horizontal blur
        //
        fbo2->enable();
        glViewport(0,0,width,height);

        blurShader->enable();
        fbo1->color[0]->active(0);
        blurShader->setTexture(0);
        //blurShader->setTextureNeighbor(neighborhood);
        //blurShader->setTexelNeighbor( texelNeighbor );
        //blurShader->setTexelMaxUVAccess( texelMaxUVAccess );
        blurShader->setHorizontal();

        GLPostProcessPowerOfTwo::drawQuad(blurShader,
                                          texelNeighbor,
                                          texelMaxUVAccess,
                                          drawQuadMaxUV);



        //
        // 3th pass -- vertical blur
        //
        fbo1->enable();
        glViewport(0,0,width,height);

        fbo2->color[0]->active(0);
        blurShader->setVertical();

        GLPostProcessPowerOfTwo::drawQuad(blurShader,
                                          texelNeighbor,
                                          texelMaxUVAccess,
                                          drawQuadMaxUV);


        //
        // 4th pass -- composite
        //
        if (dst == NULL)
            GLFramebufferObject::disable();
         else
            dst->enable();

        glViewport(0,0,postProcessPOT->width,postProcessPOT->height);

        compositeShader->enable();

        colorSrc->active(0);
        compositeShader->setSourceTexture(0);
        fbo1->color[0]->active(1);
        compositeShader->setBlurTexture(1);
        compositeShader->setIntensity(intensity);

        compositeShader->setBlurTexelMaxUVAccess(texelMaxUVAccess);//blur max access

        GLPostProcessPowerOfTwo::drawQuad(compositeShader,
                                          postProcessPOT->texelNeighbor,//source neighbor
                                          postProcessPOT->texelMaxUVAccess,//source max access
                                          postProcessPOT->drawQuadMaxUV); //source UV mapping

        fbo1->color[0]->deactive(1);
        colorSrc->deactive(0);

    }

}

