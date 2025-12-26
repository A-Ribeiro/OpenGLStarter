#pragma once

#include <appkit-gl-engine/Components/Core/ComponentCamera.h>
#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/Renderer/RenderPipeline.h>
#include <appkit-gl-engine/util/ResourceHelper.h>

// #include <aRibeiroCore/aRibeiroCore.h>

#include <InteractiveToolkit/Platform/Time.h>
#include <appkit-gl-engine/ResourceMap.h>

#include <appkit-gl-engine/AppBase/EventHandlerSet.h>

namespace AppKit
{
    namespace GLEngine
    {

        class SceneBase : public EventHandlerSet
        {

            std::weak_ptr<SceneBase> mSelf;

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

            SceneBase(Platform::Time *_time,
                      RenderPipeline *_renderPipeline,
                      ResourceHelper *_resourceHelper,
                      ResourceMap *_resourceMap,
                      std::shared_ptr<RenderWindowRegion> renderWindow);

        public:
            Platform::Time *time;
            RenderPipeline *renderPipeline;
            ResourceHelper *resourceHelper;
            ResourceMap *resourceMap;
            std::shared_ptr<RenderWindowRegion> renderWindow;

            void load();
            void unload();

            virtual ~SceneBase();

            virtual void draw();

            // Precompute the modified scene graph transforms
            //   after this call, can use the visited flag to true when using transforms
            //  Need to call this before the draw from RenderPipeline
            virtual void precomputeSceneGraphAndCamera();


            inline std::shared_ptr<SceneBase> self()
            {
                return std::shared_ptr<SceneBase>(mSelf);
            }

            template <typename _SceneType,
                      typename std::enable_if<
                          std::is_base_of<SceneBase, _SceneType>::value,
                          bool>::type = true>
            inline std::shared_ptr<_SceneType> self()
            {
                return std::dynamic_pointer_cast<_SceneType>(self());
            }

            template <typename _SceneType, typename... _param_args,
                      typename std::enable_if<
                          std::is_base_of<SceneBase, _SceneType>::value,
                          bool>::type = true>
            static inline std::shared_ptr<_SceneType> CreateShared(_param_args &&...args)
            {
                //auto result = std::make_shared<_SceneType>(std::forward<_param_args>(args)...);
                auto result = std::shared_ptr<_SceneType>( new  _SceneType(std::forward<_param_args>(args)...) );
                result->mSelf = std::weak_ptr<SceneBase>(result);
                return result;
            }
        };

    }

}