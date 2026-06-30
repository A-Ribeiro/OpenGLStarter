#include <appkit-gl-engine/Components/Ext/ComponentSpriteAnimation.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/Components/2d/ComponentSprite.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentSpriteAnimation::Type = "ComponentSpriteAnimation";

            void ComponentSpriteAnimation::OnUpdate(Platform::Time *time)
            {
                if (current_animation == nullptr || animations == nullptr)
                    return;

                current_time += time->deltaTime;

                int full_frames = (int)(current_time / current_animation->frame_duration_seconds);
                if (full_frames == 0)
                {
                    // printf("doing nothing...\n");
                    sample_current_frame();
                    return;
                }

                int new_frame_index = current_frame_index + full_frames;
                int total_frames = (int)current_animation->frames.size();

                while (true)
                {
                    if (!current_animation->loop)
                    {

                        if (new_frame_index >= total_frames)
                        {
                            int remaining_frames = total_frames - current_frame_index;
                            current_time -= remaining_frames * current_animation->frame_duration_seconds;

                            // Store the name of the animation that just ended
                            std::string ended_animation_name = current_animation->name;

                            // Call onAnimationEnd once for the animation that just finished
                            if (onAnimationEnd)
                                onAnimationEnd(ended_animation_name);

                            // on end or loop, dequeue one action from the queue.
                            if (dequeue_next_action(&new_frame_index, &total_frames))
                                continue;

                            // load new animation and recalculate the new frame index
                            auto it = animations->find(current_animation->on_end_goes_to_animation_name);
                            if (it != animations->end())
                            {
                                current_animation = it->second;
                                if (onAnimationStart)
                                    onAnimationStart(current_animation->name);

                                sampled_frame_index = -1; // force sampling of the new animation
                                current_frame_index = 0;
                            }
                            else
                            {
                                // No animation to transition to, stop at last frame
                                current_frame_index = total_frames - 1;

                                pause();
                                break;
                            }

                            full_frames = (int)(current_time / current_animation->frame_duration_seconds);
                            new_frame_index = current_frame_index + full_frames;
                            total_frames = (int)current_animation->frames.size();

                            continue;
                        }

                        if (new_frame_index > 0)
                        {
                            int remaining_frames = new_frame_index - current_frame_index;
                            current_time -= remaining_frames * current_animation->frame_duration_seconds;
                            current_frame_index = new_frame_index;
                        }

                        break;
                    }
                    else
                    {
                        int remaining_frames = new_frame_index - current_frame_index;
                        current_time -= remaining_frames * current_animation->frame_duration_seconds;
                        current_frame_index = new_frame_index;

                        // printf("current_time: %f, new_frame_index: %d, total_frames: %d\n", current_time, new_frame_index, total_frames);

                        bool loop_will_be_called = current_frame_index >= total_frames;
                        while (current_frame_index >= total_frames)
                        {
                            current_frame_index -= total_frames;
                        }

                        if (loop_will_be_called)
                        {
                            if (onAnimationLoop)
                                onAnimationLoop(current_animation->name);

                            // on end or loop, dequeue one action from the queue.
                            if (dequeue_next_action(&new_frame_index, &total_frames))
                                continue;
                        }
                        break;
                    }
                }

                sample_current_frame();
            }

            void ComponentSpriteAnimation::sample_current_frame(bool try_enqueue_first)
            {
                if (current_animation == nullptr || sprite_child_component_sprite == nullptr)
                    return;

                if (sampled_frame_index == current_frame_index)
                    return;

                if (try_enqueue_first && mainThreadQueue != nullptr)
                {
                    auto self_ref = this->self<ComponentSpriteAnimation>();
                    mainThreadQueue->enqueue(
                        [self_ref]()
                        { self_ref->sample_current_frame(false); });
                    return;
                }

                const auto &frame = current_animation->frames[current_frame_index];
                sampled_frame_index = current_frame_index;

                // printf("set texture sample frame %d/%d for animation '%s'\n", current_frame_index, (int)current_animation->frames.size(), current_animation->name.c_str());

                sprite_child_component_sprite->setTextureFromAtlas(
                    sprite_pool->getResourceMap(),
                    frame.atlas,
                    frame.sprite_name,
                    frame.pivot,
                    frame.color,
                    frame.size_constraint,
                    frame.x_invert,
                    frame.y_invert,
                    AppKit::GLEngine::Components::MeshUploadMode::MeshUploadMode_Dynamic);
            }

            bool ComponentSpriteAnimation::dequeue_next_action(int *new_frame_index, int *total_frames)
            {
                // on end or loop, dequeue one action from the queue.
                if (actionQueue.size() > 0)
                {
                    auto action = actionQueue.front();
                    actionQueue.pop_front();

                    auto it = animations->find(action.name);
                    if (it != animations->end())
                    {
                        current_animation = it->second;

                        // remove integer frames from timestamp, keeping intra-frames time
                        int full_frames = (int)(current_time / current_animation->frame_duration_seconds);
                        current_time -= (float)full_frames * current_animation->frame_duration_seconds;

                        if (onAnimationStart)
                            onAnimationStart(current_animation->name);

                        sampled_frame_index = -1; // force sampling of the new animation
                        current_frame_index = 0;

                        // intra-frames + start_time from parameter
                        current_time += action.start_time;
                        *new_frame_index = 0;
                        *total_frames = (int)it->second->frames.size();

                        return true;
                    }
                }

                return false;
            }

            ComponentSpriteAnimation::ComponentSpriteAnimation() : Component(ComponentSpriteAnimation::Type)
            {
                current_time = 0.0f;
                current_frame_index = 0;
                use_transform_cache = false;
                start_playing = false;
                is_playing = false;
                sampled_frame_index = -1; // force sampling on first frame
            }

            ComponentSpriteAnimation::~ComponentSpriteAnimation()
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentSpriteAnimation::OnUpdate, this);
            }

            void ComponentSpriteAnimation::configureAnimations(
                std::shared_ptr<AppKit::GLEngine::SpritePool> sprite_pool,
                const std::string &default_animation_name,
                const std::vector<SpriteAnimationConfiguration> &animations,
                bool start_playing)
            {
                this->sprite_pool = sprite_pool;
                this->start_playing = start_playing;

                current_animation = nullptr;

                this->animations = std::make_shared<std::unordered_map<std::string, std::shared_ptr<SpriteAnimation>>>();

                // mount the animations map
                for (const auto &anim : animations)
                {
                    // Validate animation configuration
                    if (anim.frames.empty())
                        throw std::runtime_error("Animation '" + anim.name + "' has no frames");
                    if (anim.frame_duration_seconds <= 0.0f)
                        throw std::runtime_error("Animation '" + anim.name + "' has invalid frame_duration_seconds: " + std::to_string(anim.frame_duration_seconds));

                    auto new_animation = SpriteAnimation::CreateShared();
                    new_animation->name = anim.name;
                    new_animation->frame_duration_seconds = anim.frame_duration_seconds;
                    new_animation->loop = anim.loop;
                    new_animation->on_end_goes_to_animation_name = anim.on_end_goes_to_animation_name;

                    for (const auto &frame_info : anim.frames)
                    {
                        SpriteAnimationFrames frame;

                        frame.atlas = sprite_pool->getAtlasContainingSprite(frame_info.name);
                        frame.atlas_entry = frame.atlas->getSprite(frame_info.name);

                        frame.sprite_name = frame_info.name;
                        frame.pivot = frame_info.pivot;// sprite_pool->fixPivot(frame.pivot);
                        frame.color = frame_info.color;
                        frame.size_constraint = frame_info.size_constraint;// sprite_pool->fixSize(frame_info.size_constraint);
                        frame.x_invert = frame_info.x_invert;
                        frame.y_invert = frame_info.y_invert;

                        new_animation->frames.push_back(frame);
                    }

                    (*this->animations)[anim.name] = new_animation;
                    if (anim.name == default_animation_name)
                        current_animation = new_animation;
                }

                if (current_animation == nullptr)
                    throw std::runtime_error("Default animation not found: " + default_animation_name);

                current_time = 0.0f;
                current_frame_index = 0;
                sampled_frame_index = -1;
            }

            void ComponentSpriteAnimation::createTransformChild(
                bool use_transform_cache,
                const char *child_name)
            {
                auto transform = getTransform();

                ITK_ABORT(transform == nullptr, "ComponentSpriteAnimation::createTransformChild: no transform attached to this component");

                this->use_transform_cache = use_transform_cache;

                sprite_child_transform = transform->addChild(Transform::CreateShared(child_name));
                sprite_child_component_sprite = sprite_child_transform->addNewComponent<AppKit::GLEngine::Components::ComponentSprite>();

                sample_current_frame();
            }

            // will change the animation when onAnimationEnd or onAnimationLoop is called
            void ComponentSpriteAnimation::enqueue_FromFrameTime(const std::string &name, float start_time, bool replace_last_if_exists)
            {
                if (actionQueue.size() > 0)
                {
                    if (replace_last_if_exists)
                    {
                        actionQueue.back() = SpriteAnimationAction{name, start_time};
                        return;
                    }

                    if (actionQueue.back().name == name)
                    {
                        actionQueue.back().start_time = start_time;
                        return;
                    }
                }
                actionQueue.push_back(SpriteAnimationAction{name, start_time});
            }
            void ComponentSpriteAnimation::enqueue(const std::string &name, bool replace_last_if_exists)
            {
                enqueue_FromFrameTime(name, 0.0f, replace_last_if_exists);
            }
            void ComponentSpriteAnimation::enqueue_FromFrameIndex(const std::string &name, int start_frame_index, bool replace_last_if_exists)
            {
                auto it = animations->find(name);
                if (it != animations->end())
                    enqueue_FromFrameTime(name, (float)start_frame_index * it->second->frame_duration_seconds, replace_last_if_exists);
                else
                    enqueue_FromFrameTime(name, 0.0f, replace_last_if_exists);
            }

            // change immediately the animation, interrupting the current one
            // it clear the queue
            void ComponentSpriteAnimation::play_FromFrameTime(const std::string &name, float start_time)
            {
                if (current_animation != nullptr && current_animation->name == name)
                    return;

                auto it = animations->find(name);
                if (it != animations->end())
                {
                    if (it->second != current_animation)
                    {
                        actionQueue.clear();

                        // Call onAnimationEnd with old animation name
                        if (onAnimationEnd && current_animation != nullptr)
                            onAnimationEnd(current_animation->name);

                        // Change to new animation
                        current_animation = it->second;

                        // remove integer frames from timestamp, keeping intra-frames time
                        int full_frames = (int)(current_time / current_animation->frame_duration_seconds);
                        current_time -= (float)full_frames * current_animation->frame_duration_seconds;

                        if (onAnimationStart)
                            onAnimationStart(current_animation->name);

                        sampled_frame_index = -1; // force sampling of the new animation
                        current_frame_index = 0;

                        // intra-frames + start_time from parameter
                        current_time += start_time;
                    }
                }
                resume();
            }

            void ComponentSpriteAnimation::play(const std::string &name)
            {
                play_FromFrameTime(name, 0.0f);
            }

            void ComponentSpriteAnimation::play_FromFrameIndex(const std::string &name, int start_frame_index)
            {
                auto it = animations->find(name);
                if (it != animations->end())
                    play_FromFrameTime(name, (float)start_frame_index * it->second->frame_duration_seconds);
                else
                    play_FromFrameTime(name, 0.0f);
            }

            // remove update callback
            void ComponentSpriteAnimation::pause()
            {
                if (!is_playing)
                    return;
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentSpriteAnimation::OnUpdate, this);
                is_playing = false;
            }
            // register update callback
            void ComponentSpriteAnimation::resume()
            {
                if (is_playing)
                    return;
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.add(&ComponentSpriteAnimation::OnUpdate, this);
                is_playing = true;
            }

            // component lifecycle

            void ComponentSpriteAnimation::attachToTransform(std::shared_ptr<Transform> t)
            {
                eventHandlerSetRef = t->eventHandlerSet;
            }
            void ComponentSpriteAnimation::detachFromTransform(std::shared_ptr<Transform> t)
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentSpriteAnimation::OnUpdate, this);
                eventHandlerSetRef.reset();
            }

            void ComponentSpriteAnimation::start()
            {
                if (start_playing)
                {
                    if (auto eventHandlerSet = eventHandlerSetRef.lock())
                        eventHandlerSet->OnUpdate.add(&ComponentSpriteAnimation::OnUpdate, this);
                    is_playing = true;

                    if (current_animation != nullptr)
                    {
                        if (onAnimationStart)
                            onAnimationStart(current_animation->name);
                        play(current_animation->name);
                    }
                }
            }

            std::shared_ptr<Component> ComponentSpriteAnimation::duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
            {
                auto result = Component::CreateShared<ComponentSpriteAnimation>();

                result->eventHandlerSetRef = this->eventHandlerSetRef;
                result->current_animation = this->current_animation;
                result->current_time = this->current_time;
                result->current_frame_index = this->current_frame_index;
                result->sampled_frame_index = this->sampled_frame_index;

                result->animations = this->animations;

                // result->onAnimationStart = this->onAnimationStart;
                // result->onAnimationFrameChanged = this->onAnimationFrameChanged;
                // result->onAnimationEnd = this->onAnimationEnd;
                // result->onAnimationLoop = this->onAnimationLoop;

                result->use_transform_cache = this->use_transform_cache;
                result->sprite_child_transform = this->sprite_child_transform;
                result->sprite_child_component_sprite = this->sprite_child_component_sprite;

                result->sprite_pool = this->sprite_pool;

                result->actionQueue = this->actionQueue;

                result->start_playing = this->start_playing;
                result->is_playing = this->is_playing;

                return result;
            }
            void ComponentSpriteAnimation::fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
            {
                if (sprite_child_transform != nullptr)
                {
                    auto it = transformMap.find(sprite_child_transform);
                    if (it != transformMap.end())
                        sprite_child_transform = it->second;
                }
                if (sprite_child_component_sprite != nullptr)
                {
                    auto it = componentMap.find(sprite_child_component_sprite);
                    if (it != componentMap.end())
                        sprite_child_component_sprite = std::dynamic_pointer_cast<AppKit::GLEngine::Components::ComponentSprite>(it->second);
                }
            }

            void ComponentSpriteAnimation::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }
            void ComponentSpriteAnimation::Deserialize(rapidjson::Value &_value,
                                                       std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                       std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                       ResourceSet &resourceSet)
            {
            }

        }
    }
}