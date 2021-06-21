#include "Bloom.h"
#include <aribeiro/common.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace openglWrapper;
#include <aribeiro/SetNullAndDelete.h>

namespace aRibeiro {
    
    //private copy constructores, to avoid copy...
    Bloom::Bloom(const Bloom& v) {}
    void Bloom::operator=(const Bloom& v) {}
    
    void Bloom::setSize(int w, int h) {

        w = w / 2;
        h = h / 2;

        if (fbo1->width != w || fbo1->height != h ){
            fbo1->setSize(w,h);
            fbo2->setSize(w,h);
            
            neighborhood = vec2(1.0f/(float)fbo1->width, 1.0f/(float)fbo1->height);
        }
    }

    Bloom::Bloom() {
        fbo1 = new GLFramebufferObject();
        fbo1->color.push_back(new GLTexture());
        fbo2 = new GLFramebufferObject();
        fbo2->color.push_back(new GLTexture());
        
        setSize(32,32);
        fbo1->attachTextures();
        fbo2->attachTextures();
       
        thresholdShader = new BloomThresholdShader();
        blurShader = new BloomBlurShader();
        compositeShader = new BloomCompositeShader();
        
        threshold = 0.5f;
        intensity = 0.5f;
    }
    
    Bloom::~Bloom() {
        setNullAndDelete(fbo1->color[0]);
        setNullAndDelete(fbo1);
        setNullAndDelete(fbo2->color[0]);
        setNullAndDelete(fbo2);
        
        setNullAndDelete(thresholdShader);
        setNullAndDelete(blurShader);
        setNullAndDelete(compositeShader);
    }
    
    void Bloom::render(GLPostProcess *postProcess, GLTexture *colorSrc, GLTexture *depthSrc, GLFramebufferObject *dst){
        setSize(postProcess->width,postProcess->height);
        
        
        //
        // 1st pass -- threshold
        //
        fbo1->enable();
        glViewport(0,0,fbo1->width,fbo1->height);
        
        thresholdShader->enable();
        colorSrc->active(0);
        thresholdShader->setTexture(0);
        thresholdShader->setThreshold(threshold);
        
        GLPostProcess::drawQuad(thresholdShader);
        
        
        //
        // 2nd pass -- horizontal blur
        //
        fbo2->enable();
        glViewport(0,0,fbo2->width,fbo2->height);
        
        blurShader->enable();
        fbo1->color[0]->active(0);
        blurShader->setTexture(0);
        blurShader->setTextureNeighbor(neighborhood);
        blurShader->setHorizontal();
        
        GLPostProcess::drawQuad(blurShader);
        
        
        //
        // 3th pass -- vertical blur
        //
        fbo1->enable();
        glViewport(0,0,fbo1->width,fbo1->height);
        
        fbo2->color[0]->active(0);
        blurShader->setVertical();
        
        GLPostProcess::drawQuad(blurShader);
        
        
        //
        // 4th pass -- composite
        //
        if (dst == NULL){
            GLFramebufferObject::disable();
            glViewport(0,0,postProcess->width,postProcess->height);
        } else {
            dst->enable();
            glViewport(0,0,dst->width,dst->height);
        }
        
        compositeShader->enable();
        
        colorSrc->active(0);
        compositeShader->setSourceTexture(0);
        fbo1->color[0]->active(1);
        compositeShader->setBlurTexture(1);
        compositeShader->setIntensity(intensity);
        
        GLPostProcess::drawQuad(compositeShader);
        
        fbo1->color[0]->deactive(1);
        colorSrc->deactive(0);
        
    }
    
}

