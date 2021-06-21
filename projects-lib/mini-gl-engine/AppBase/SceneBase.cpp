#include "SceneBase.h"

#include <mini-gl-engine/mini-gl-engine.h>
#include <aribeiro/aribeiro.h>

namespace GLEngine {

    SceneBase::SceneBase(
        aRibeiro::PlatformTime *_time,
        RenderPipeline *_renderPipeline,
        ResourceHelper *_resourceHelper){
        
        time = _time;
        renderPipeline = _renderPipeline;
        resourceHelper = _resourceHelper;

        camera = NULL;
        root = NULL;

    }

    void SceneBase::load() {
        loadResources();
        loadGraph();
        bindResourcesToGraph();
    }

    void SceneBase::unload() {
        unloadAll();
    }

    SceneBase::~SceneBase(){
    }

    void SceneBase::draw(){
        GLEngine::Engine *engine = GLEngine::Engine::Instance();
        if (engine->sRGBCapable)
            glEnable(GL_FRAMEBUFFER_SRGB);
            
        renderPipeline->runSinglePassPipeline(root, camera, true);
            
        if (engine->sRGBCapable)
            glDisable(GL_FRAMEBUFFER_SRGB);
    }

    void SceneBase::precomputeSceneGraphAndCamera(){
        root->resetVisited();
        root->preComputeTransforms();
        camera->precomputeViewProjection(true);
    }

}

