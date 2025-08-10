#pragma once

#include <appkit-gl-engine/Components/Core/ComponentCamera.h>
#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>

// #include <aRibeiroCore/aRibeiroCore.h>

#include <InteractiveToolkit/Platform/Time.h>
#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        class SceneBase : public EventCore::HandleCallback
        {

        protected:
            std::shared_ptr<Components::ComponentCamera> camera;
            std::shared_ptr<AppKit::GLEngine::Transform> root;

            // to load skybox, textures, cubemaps, 3DModels and setup materials
            virtual void loadResources() = 0;
            // to load the scene graph
            virtual void loadGraph() = 0;
            // to bind the resources to the current graph
            virtual void bindResourcesToGraph() = 0;

            // clear all loaded scene
            virtual void unloadAll() = 0;

        public:
            Platform::Time *time;
            RenderPipeline *renderPipeline;
            ResourceHelper *resourceHelper;
            ResourceMap *resourceMap;
            std::shared_ptr<RenderWindowRegion> renderWindow;
            

            SceneBase(Platform::Time *_time,
                      RenderPipeline *_renderPipeline,
                      ResourceHelper *_resourceHelper,
                      ResourceMap *_resourceMap,
                      std::shared_ptr<RenderWindowRegion> renderWindow);

            void load();
            void unload();

            virtual ~SceneBase();

            virtual void draw();

            // Precompute the modified scene graph transforms
            //   after this call, can use the visited flag to true when using transforms
            //  Need to call this before the draw from RenderPipeline
            virtual void precomputeSceneGraphAndCamera();
        };

    }

}