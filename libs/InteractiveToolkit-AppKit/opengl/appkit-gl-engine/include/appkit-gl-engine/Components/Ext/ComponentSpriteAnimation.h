#pragma once

#include <appkit-gl-engine/util/SpriteAtlas.h>
#include <appkit-gl-engine/Components/2d/common.h>

#include <appkit-gl-engine/util/SpritePool.h>

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>

#include <InteractiveToolkit/Platform/Core/SmartVector.h>
#include <InteractiveToolkit/Platform/Core/ObjectQueue.h>

namespace AppKit
{
    namespace GLEngine
    {
        class Transform;
        class ResourceMap;
        namespace Components
        {
            class ComponentMaterial;
            class ComponentMesh;
            class ComponentCamera;
            class ComponentSprite;
        }
    }

    namespace GLEngine
    {
        namespace Components
        {

            struct SpriteAnimationFrames
            {
                std::shared_ptr<AppKit::GLEngine::SpriteAtlas> atlas;

                SpriteAtlas::Entry atlas_entry;

                std::string sprite_name;
                MathCore::vec2f pivot;
                MathCore::vec4f color;
                MathCore::vec2f size_constraint;
                bool x_invert;
                bool y_invert;

                // used if transform cache is enabled
                // std::shared_ptr<AppKit::GLEngine::Transform> transform;
                // std::shared_ptr<AppKit::GLEngine::Components::ComponentSprite> component_sprite;
            };

            struct SpriteAnimation
            {
                std::string name;

                std::vector<SpriteAnimationFrames> frames;

                float frame_duration_seconds;
                bool loop;
                std::string on_end_goes_to_animation_name;

                ITK_DECLARE_CREATE_SHARED(SpriteAnimation)
            };

            struct SpriteAnimationFrameInfo
            {
                std::string name;
                MathCore::vec2f pivot;
                MathCore::vec4f color;
                MathCore::vec2f size_constraint;
                bool x_invert;
                bool y_invert;
            };

            struct SpriteAnimationConfiguration
            {
                std::string name;
                std::vector<SpriteAnimationFrameInfo> frames;
                float frame_duration_seconds;
                bool loop;
                std::string on_end_goes_to_animation_name;
            };

            struct SpriteAnimationAction
            {
                std::string name;
                float start_time;
            };

            class ComponentSpriteAnimation : public Component
            {
                std::weak_ptr<EventHandlerSet> eventHandlerSetRef;

                void OnUpdate(Platform::Time *time);

                void sample_current_frame(bool try_enqueue_first = true);

                // returns true if an action was dequeued and set as current animation
                bool dequeue_next_action(int *new_frame_index, int *total_frames);

            public:
                static const ComponentType Type;

                std::shared_ptr<SpriteAnimation> current_animation;
                float current_time;
                int current_frame_index;
                int sampled_frame_index;

                std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<SpriteAnimation>>> animations;

                EventCore::Callback<void(const std::string &name)> onAnimationStart;
                EventCore::Callback<void(const std::string &name, int from, int to)> onAnimationFrameChanged;
                EventCore::Callback<void(const std::string &name)> onAnimationEnd;
                EventCore::Callback<void(const std::string &name)> onAnimationLoop;

                bool use_transform_cache;
                std::shared_ptr<AppKit::GLEngine::Transform> sprite_child_transform;
                std::shared_ptr<AppKit::GLEngine::Components::ComponentSprite> sprite_child_component_sprite;

                std::shared_ptr<AppKit::GLEngine::SpritePool> sprite_pool;

                Platform::SmartVector<SpriteAnimationAction> actionQueue;
                Platform::ObjectQueue<EventCore::Callback<void(void)>> *mainThreadQueue;

                bool start_playing;
                bool is_playing;

                ComponentSpriteAnimation();
                ~ComponentSpriteAnimation();

                void configureAnimations(
                    std::shared_ptr<AppKit::GLEngine::SpritePool> sprite_pool,
                    const std::string &default_animation_name,
                    const std::vector<SpriteAnimationConfiguration> &animations,
                    bool start_playing = true);

                void createTransformChild(
                    bool use_transform_cache = false,
                    const char *child_name = "_sprite_anim");

                // will change the animation when onAnimationEnd or onAnimationLoop is called
                void enqueue(const std::string &name, bool replace_last_if_exists = true);
                void enqueue_FromFrameTime(const std::string &name, float start_time = 0.0f, bool replace_last_if_exists = true);
                void enqueue_FromFrameIndex(const std::string &name, int start_frame_index = 0, bool replace_last_if_exists = true);

                // change immediately the animation, interrupting the current one
                // it clear the queue
                void play(const std::string &name);
                void play_FromFrameTime(const std::string &name, float start_time = 0.0f);
                void play_FromFrameIndex(const std::string &name, int start_frame_index = 0);

                // remove update callback
                void pause();
                // register update callback
                void resume();

                // component lifecycle

                void attachToTransform(std::shared_ptr<Transform> t) override;
                void detachFromTransform(std::shared_ptr<Transform> t) override;

                void start() override;

                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone) override;
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap) override;

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) override;
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet) override;
            };

        }
    }
}