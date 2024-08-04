#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

// #include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
// #include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/util/Interpolators.h>

#include <InteractiveToolkit-Extension/model/Animation.h>

namespace AppKit
{
    namespace GLEngine
    {

        //
        // Root Motion Analyser
        //
        class AnimationClip;
        class NodeAnimation;
        class ClipInfo
        {
        public:
            AnimationClip *clip;

            // related to the root node of the model
            // use: Transform* root_node = mixer.rootNode();
            //      root_node->setPosition(position);
            MathCore::vec3f position;
            MathCore::vec3f position_delta;
            MathCore::vec3f position_delta_interframe;
            MathCore::vec3f start;

            // related to the root bone node in the local space
            // use: Transform* root_node = mixer.rootNode();
            //      root_node->setLocalPosition(local_position);
            MathCore::vec3f local_position;
            MathCore::vec3f local_position_delta;
            MathCore::vec3f local_start;

            float weight;
        };

        class RootMotionAnalyserData
        {
        public:
            bool dirty;

            ClipInfo clipInfo[2];
            int clip_count;

            // for internal use
            int clip_to_process;

            RootMotionAnalyserData();
            void setFromNodeAnimation(NodeAnimation *node_animation, float time, float weight);
            void setFromNodeAnimationBlend(
                NodeAnimation *node_animation_a, float time_a,
                NodeAnimation *node_animation_b, float time_b,
                float lrp);
            void setFromCurrentState();
            bool isClipsBlending() const;

            void applyMotionLocalExample();

            void applyMotionGlobalExample();
        };

        //DefineMethodPointer(RootMotionAnalyserMethod_ptr, void, RootMotionAnalyserData *data) VoidMethodCall(data);

        class RootMotionAnalyser
        {
            // avoid copy, using copy constructors
            RootMotionAnalyser(const RootMotionAnalyser &) {}
            void operator=(const RootMotionAnalyser &) {}
        public:

            EventCore::Callback<void(RootMotionAnalyserData *data)> method;
            RootMotionAnalyserData data;

            RootMotionAnalyser(){}

            void copy(std::unordered_map<AnimationClip *, AnimationClip *> &clipMap,
                      const RootMotionAnalyser &rootMotionAnalyser);
        };

        //
        // Node Animation
        //

        class NodeAnimation
        {
        public:
            // interpolator arrays
            LinearInterpolator<MathCore::vec3f> position;
            LinearInterpolator<MathCore::vec3f> scale;
            SlerpInterpolator rotation;

            std::shared_ptr<Transform> node;

            MathCore::vec3f start_position;
            MathCore::vec3f start_scale;
            MathCore::quatf start_rotation;

            // MathCore::vec3f motion_last_value;

            bool isRootNode;

            NodeAnimation()
            {
                isRootNode = false;
            }

            // some animations have more keys outside the animation original duration...
            // this can lead to a wrong root node interpolation
            // we need to clamp the curve to avoid this situation...
            void clampDuration(float min_duration, float max_duration)
            {

                if (position.keys.size() <= 1)
                    return;

                float last_time = position.keys[position.keys.size() - 1].time;

                printf("[NodeAnimation] clampToMaxDuration\n");
                printf("  max_duration to set: %f\n", max_duration);
                printf("  last_time: %f\n", last_time);
                if (last_time > max_duration)
                {
                    printf("  NEED SET LAST FRAME TIME!!!\n");
                    MathCore::vec3f valueToSet = position.getValue_ForwardLoop(max_duration);
                    int i = (int)position.keys.size() - 1;
                    // remove all index
                    while (i > 0)
                    {
                        if (position.keys[i].time < max_duration)
                            break;
                        else
                            position.keys.erase(position.keys.begin() + i);
                        i--;
                    }
                    position.keys.push_back(Key<MathCore::vec3f>(max_duration, valueToSet));
                    // reset delta frame
                    position.getValue_ForwardLoop(0);
                }

                if (position.keys.size() <= 1)
                    return;

                // check if the first frame starts with 0
                float first_time = position.keys[0].time;
                if (first_time < min_duration)
                {
                    printf("  NEED SET FIRST FRAME TIME!!!\n");
                    MathCore::vec3f valueToSet = position.getValue_ForwardLoop(min_duration);
                    int i = 0;
                    // remove all index
                    while (i < position.keys.size())
                    {
                        if (position.keys[i].time > min_duration)
                            break;
                        else
                            position.keys.erase(position.keys.begin());
                        i++;
                    }
                    position.keys.insert(position.keys.begin(), Key<MathCore::vec3f>(min_duration, valueToSet));
                    // reset delta frame
                    position.getValue_ForwardLoop(min_duration);

                    // move all times to 0
                    for (int i = 0; i < position.keys.size(); i++)
                        position.keys[i].time -= min_duration;

                    // reset delta frame
                    position.getValue_ForwardLoop(0);
                }
            }

            // need this to play new animations starting from zero
            //  only valid for root node
            void resetInterframeInformation()
            {
                position.getValue_ForwardLoop(0);
                scale.getValue_ForwardLoop(0);
                rotation.getValue_ForwardLoop(0);
            }

            void setTransform(std::shared_ptr<Transform> t, bool rootNode)
            {
                node = t;

                isRootNode = rootNode;

                start_position = node->getLocalPosition();
                start_scale = node->getLocalScale();
                start_rotation = node->getLocalRotation();

                if (position.keys.size() > 0)
                    start_position = position.getValue_ForwardLoop(0);
                if (scale.keys.size() > 0)
                    start_scale = scale.getValue_ForwardLoop(0);
                if (rotation.keys.size() > 0)
                    start_rotation = rotation.getValue_ForwardLoop(0);

                // motion_last_value = start_position;
                // motion_last_value = MathCore::CVT<MathCore::vec4f>::toVec3(node->getMatrix() * MathCore::CVT<MathCore::vec3f>::toPtn4(motion_last_value));
                // motion_last_value = MathCore::CVT<MathCore::vec4f>::toVec3(node->getMatrix() * MathCore::CVT<MathCore::vec3f>::toPtn4(motion_last_value - start_position));
            }

            ITK_INLINE MathCore::vec3f samplePos(float secs, MathCore::vec3f *interframe_delta)
            {
                if (position.keys.size() > 0)
                    return position.getValue_ForwardLoop(secs, interframe_delta);
                *interframe_delta = MathCore::vec3f(0);
                return MathCore::vec3f(0);
            }

            ITK_INLINE MathCore::vec3f sampleScale(float secs)
            {
                if (scale.keys.size() > 0)
                    return scale.getValue_ForwardLoop(secs);
                return MathCore::vec3f(1);
            }

            ITK_INLINE MathCore::quatf sampleRotation(float secs)
            {
                if (rotation.keys.size() > 0)
                    return rotation.getValue_ForwardLoop(secs);
                return MathCore::quatf();
            }

            ITK_INLINE void Sample_Lerp_to_Another_Node(float time_a, NodeAnimation *b, float time_b, float lrp, RootMotionAnalyser *rootMotionAnalyser)
            {

                MathCore::vec3f scale = MathCore::OP<MathCore::vec3f>::lerp(sampleScale(time_a), b->sampleScale(time_b), lrp);
                MathCore::quatf rotation = MathCore::OP<MathCore::quatf>::slerp(sampleRotation(time_a), b->sampleRotation(time_b), lrp);

                ITK_ABORT(isRootNode != b->isRootNode, "The animation nodes dont have the same root node...\n");

                if (isRootNode && rootMotionAnalyser->method != nullptr)
                {

                    ITK_ABORT(node != b->node, "The animation root nodes dont have the same transform...\n");

                    // MathCore::vec3f _a = samplePos(time_a);
                    // MathCore::vec3f _b = b->samplePos(time_b);

                    rootMotionAnalyser->data.setFromNodeAnimationBlend(this, time_a, b, time_b, lrp);

                    /*

                    node->setLocalPosition(_a);
                    motion_last_value = node->getPosition();

                    node->setLocalPosition(_b);
                    b->motion_last_value = node->getPosition();

                    MathCore::vec3f _start_lrp = aRibeiro::lerp(start_position, b->start_position, lrp);

                    node->setLocalPosition(_start_lrp);

                    (*computed_root_delta) = aRibeiro::lerp(
                        motion_last_value,
                        b->motion_last_value,
                        lrp
                    );

                    */
                }
                else
                {
                    // reset root node interframe information
                    // rootMotionAnalyser->data.clipInfo[0].position_delta_interframe = MathCore::vec3f(0);
                    // rootMotionAnalyser->data.clipInfo[1].position_delta_interframe = MathCore::vec3f(0);

                    MathCore::vec3f pos = MathCore::OP<MathCore::vec3f>::lerp(samplePos(time_a, nullptr), b->samplePos(time_b, nullptr), lrp);
                    node->setLocalPosition(pos);
                }
                node->setLocalScale(scale);
                node->setLocalRotation(rotation);
            }

            ITK_INLINE void sampleTime(float secs, float amount, RootMotionAnalyser *rootMotionAnalyser)
            {
                if (amount == 1.0f)
                {
                    if (position.keys.size() > 0)
                    {
                        if (isRootNode && rootMotionAnalyser->method != nullptr)
                        {

                            // MathCore::vec3f local_position = position.getValue_ForwardLoop(secs);
                            rootMotionAnalyser->data.setFromNodeAnimation(this, secs, 1.0f);

                            /*
                            node->setLocalPosition(position.getValue_ForwardLoop(secs));
                            motion_last_value = node->getPosition();
                            (*computed_root_delta) = motion_last_value;

                            node->setLocalPosition(start_position);
                            */
                        }
                        else
                            node->setLocalPosition(position.getValue_ForwardLoop(secs));
                    }
                    if (scale.keys.size() > 0)
                        node->setLocalScale(scale.getValue_ForwardLoop(secs));
                    if (rotation.keys.size() > 0)
                        node->setLocalRotation(rotation.getValue_ForwardLoop(secs));
                }
                else
                {
                    if (position.keys.size() > 0)
                    {
                        if (isRootNode && rootMotionAnalyser->method != nullptr)
                        {

                            // MathCore::vec3f local_position = position.getValue_ForwardLoop(secs);
                            rootMotionAnalyser->data.setFromNodeAnimation(this, secs, amount);
                            /*
                            node->setLocalPosition(aRibeiro::lerp(start_position, position.getValue_ForwardLoop(secs), amount));
                            motion_last_value = node->getPosition();
                            (*computed_root_delta) = motion_last_value;

                            node->setLocalPosition(start_position);
                            */
                        }
                        else
                            node->setLocalPosition(MathCore::OP<MathCore::vec3f>::lerp(start_position, position.getValue_ForwardLoop(secs), amount));
                    }
                    if (scale.keys.size() > 0)
                        node->setLocalScale(MathCore::OP<MathCore::vec3f>::lerp(start_scale, scale.getValue_ForwardLoop(secs), amount));
                    if (rotation.keys.size() > 0)
                        node->setLocalRotation(MathCore::OP<MathCore::quatf>::slerp(start_rotation, rotation.getValue_ForwardLoop(secs), amount));
                }
            }
        };

        class AnimationClip: public EventCore::HandleCallback
        {
            float last_sampled_time;

            bool findRootNode(std::shared_ptr<Transform> t, const void *userData);

            AnimationClip(){}
        public:
            std::string name;
            float duration;
            std::vector<NodeAnimation> channels;

            float current_time;
            bool loop;
            bool wait_end_to_transition;

            std::shared_ptr<Transform> base_model;
            std::shared_ptr<Transform> root_node;
            NodeAnimation *root_node_animation;

            AnimationClip(const std::string &clip_name, std::shared_ptr<Transform> root, const ITKExtension::Model::Animation &animation);

            void update(float elapsed_secs);
            void reset();

            void forceResample();

            void doSample(float amount, RootMotionAnalyser *rootMotionAnalyser);

            bool canDoSample();

            void didExternalSample();

            AnimationClip* clone(std::unordered_map<NodeAnimation*,NodeAnimation*> *nodeMap);
            void fix_internal_references(Transform::TransformMapT &transformMap);
        };

        class AnimationTransitionChannelInformation
        {
        public:
            std::vector<NodeAnimation *> no_matching_a_elements;
            std::vector<NodeAnimation *> no_matching_b_elements;
            std::vector<NodeAnimation *> matching_a_elements;
            std::vector<NodeAnimation *> matching_b_elements;
        };

        class AnimationMixerLerpTarget
        {
        public:
            AnimationClip *clip;
            uint32_t index;
            float transition_duration_secs;
            float transition_current;

            AnimationMixerLerpTarget()
            {
                clip = nullptr;
                index = -1;
                transition_duration_secs = 0.0f;
                transition_current = 0.0f;
            }

            AnimationMixerLerpTarget(AnimationClip *_clip, uint32_t _index, float _transition_duration_secs)
            {
                clip = _clip;
                index = _index;
                transition_duration_secs = _transition_duration_secs;
                transition_current = 0.0f;
            }

            bool isInstantaneous()
            {
                return transition_duration_secs <= MathCore::EPSILON<float>::high_precision;//aRibeiro::EPSILON;
            }

            void incrementTransitionTime(float elapsed)
            {
                transition_current += elapsed;
            }

            bool reachedEnd()
            {
                return transition_current >= transition_duration_secs;
            }

            float computeLerp()
            {
                return transition_current / transition_duration_secs;
            }
        };

        class AnimationMixer
        {

            std::map<std::string, uint32_t> clips_index;
            std::vector<AnimationClip *> clips_array;
            std::vector<AnimationTransitionChannelInformation> transitions;

            AnimationClip *current_clip;
            uint32_t current_index;

            std::vector<AnimationMixerLerpTarget> request_queue;

            //
            // Root Motion Analyser
            //
            RootMotionAnalyser rootMotionAnalyser;

            AnimationTransitionChannelInformation &getTransition(uint32_t x, uint32_t y);

            // avoid copy, using copy constructors
            AnimationMixer(const AnimationMixer &) {}
            void operator=(const AnimationMixer &) {}

            void clear();
        public:
            AnimationMixer();
            ~AnimationMixer();

            void addClip(AnimationClip *clip);

            void computeTransitions();

            // void update_old(float elapsed_secs);

            void update(float elapsed_secs);

            void play(const std::string &clipname, float blend_time = 0.5f);

            std::shared_ptr<Transform> rootNode();

            void setRootMotionAnalyserCallback(const EventCore::Callback<void(RootMotionAnalyserData *data)> &callback)
            {
                rootMotionAnalyser.method = callback;

                if (rootMotionAnalyser.method != nullptr && rootMotionAnalyser.data.clip_count > 0)
                {
                    // set from current state
                    rootMotionAnalyser.data.setFromCurrentState();
                    rootMotionAnalyser.method(&rootMotionAnalyser.data);
                }
            }

            void copy(const AnimationMixer& src);

            void fix_internal_references(Transform::TransformMapT &transformMap);

        };

    }
}