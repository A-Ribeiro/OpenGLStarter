#include <appkit-gl-engine/AppBase/SceneBase.h>

// #include <appkit-gl-engine/mini-gl-engine.h>
// #include <aRibeiroCore/aRibeiroCore.h>

#include <appkit-gl-engine/Engine.h>


namespace AppKit
{
    namespace GLEngine
    {

        SceneBase::SceneBase(
            Platform::Time *_time,
            RenderPipeline *_renderPipeline,
            ResourceHelper *_resourceHelper,
            ResourceMap *_resourceMap,
            std::shared_ptr<RenderWindowRegion> renderWindow)
        {

            this->time = _time;
            this->renderPipeline = _renderPipeline;
            this->resourceHelper = _resourceHelper;
            this->resourceMap = _resourceMap;
            this->renderWindow = renderWindow;

            // camera = nullptr;
            // root = nullptr;
        }

        void SceneBase::load()
        {
            resourceMap->ensure_default_texture_creation();
            loadResources();
            loadGraph();
            bindResourcesToGraph();
        }

        void SceneBase::unload()
        {
            resourceMap->clear_refcount_equals_1();
            unloadAll();
        }

        SceneBase::~SceneBase()
        {
            resourceMap->clear();
            renderWindow.reset();
        }

        void SceneBase::draw()
        {
            AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
            if (engine->sRGBCapable)
                glEnable(GL_FRAMEBUFFER_SRGB);

            renderPipeline->runSinglePassPipeline(root, camera, true);

            if (engine->sRGBCapable)
                glDisable(GL_FRAMEBUFFER_SRGB);
        }

        void SceneBase::precomputeSceneGraphAndCamera()
        {
            root->resetVisited();
            root->preComputeTransforms();
            camera->precomputeViewProjection(true);
        }

    }
}
