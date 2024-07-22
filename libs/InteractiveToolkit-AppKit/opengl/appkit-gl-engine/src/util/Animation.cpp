#include <appkit-gl-engine/util/Animation.h>

namespace AppKit
{
    namespace GLEngine
    {

        bool AnimationClip::findRootNode(std::shared_ptr<Transform> t, const void *userData)
        {
            if (root_node != NULL)
                return false;

            const ITKExtension::Model::Animation &animation = *(const ITKExtension::Model::Animation *)userData;

            for (int i = 0; i < animation.channels.size(); i++)
            {
                const ITKExtension::Model::NodeAnimation &node_anim = animation.channels[i];
                if (node_anim.nodeName.compare(t->Name) == 0)
                {
                    root_node = t;
                    return false;
                }
            }

            return true;
        }

        AnimationClip::AnimationClip(const std::string &clip_name, std::shared_ptr<Transform> root, const ITKExtension::Model::Animation &animation)
        {

            base_model = root;
            root_node = NULL;

            root->traversePreOrder_DepthFirst(
                EventCore::CallbackWrapper(&AnimationClip::findRootNode, this),
                 &animation);

            loop = true;
            wait_end_to_transition = false;
            last_sampled_time = -1.0f;
            current_time = 0.0f;
            // name = animation.name;
            name = clip_name;
            duration = animation.durationTicks / animation.ticksPerSecond;

            printf("AnimationClip(%s)\n", name.c_str());
            int root_node_animation_index = -1;
            for (int i = 0; i < animation.channels.size(); i++)
            {
                const ITKExtension::Model::NodeAnimation &node_anim = animation.channels[i];

                auto t_node = root->findTransformByName(node_anim.nodeName);
                if (t_node != nullptr)
                {

                    printf("   --channel %i (node: %s)\n", i, node_anim.nodeName.c_str());
                    printf("     positions: %u\n", (uint32_t)node_anim.positionKeys.size());
                    printf("     scales: %u\n", (uint32_t)node_anim.scalingKeys.size());
                    printf("     rotations: %u\n", (uint32_t)node_anim.rotationKeys.size());

                    NodeAnimation result;

                    if (node_anim.positionKeys.size() > 0)
                        for (int j = 0; j < node_anim.positionKeys.size(); j++)
                        {
                            const ITKExtension::Model::Vec3Key &key = node_anim.positionKeys[j];
                            result.position.addKey(Key<MathCore::vec3f>(key.time / animation.ticksPerSecond, key.value));
                        }

                    if (node_anim.scalingKeys.size() > 0)
                        for (int j = 0; j < node_anim.scalingKeys.size(); j++)
                        {
                            const ITKExtension::Model::Vec3Key &key = node_anim.scalingKeys[j];
                            result.scale.addKey(Key<MathCore::vec3f>(key.time / animation.ticksPerSecond, key.value));
                        }

                    if (node_anim.rotationKeys.size() > 0)
                        for (int j = 0; j < node_anim.rotationKeys.size(); j++)
                        {
                            const ITKExtension::Model::QuatKey &key = node_anim.rotationKeys[j];
                            result.rotation.addKey(Key<MathCore::quatf>(key.time / animation.ticksPerSecond, key.value));
                        }

                    // result.node = t_node;
                    result.setTransform(t_node, t_node == root_node);

                    if (result.isRootNode)
                        root_node_animation_index = (int)channels.size();

                    channels.push_back(result);
                }
                else
                {
                    printf("[AnimationClip] Animation without a scene node...\n");
                }
            }

            ITK_ABORT(root_node_animation_index == -1, "No bone root animation found...");

            root_node_animation = &channels[root_node_animation_index];

            printf("[AnimationClip] Bone root: %s\n", root_node_animation->node->getName().c_str());

            root_node_animation->clampDuration(0, duration);
        }

        void AnimationClip::update(float elapsed_secs)
        {

            current_time += elapsed_secs;

            if (!loop)
            {
                if (current_time > duration)
                    current_time = duration;
            }
            else
            {
                current_time = MathCore::OP<float>::fmod(current_time, duration);
            }

            // doSample();
        }

        void AnimationClip::reset()
        {

            last_sampled_time = -1;

            current_time = 0;
            root_node_animation->resetInterframeInformation();

            // printf("reset %s \n", name.c_str());
            // doSample();
        }

        void AnimationClip::forceResample()
        {
            last_sampled_time = -1;
        }

        void AnimationClip::doSample(float amount, RootMotionAnalyser *rootMotionAnalyser)
        {
            if (last_sampled_time != current_time)
            {
                rootMotionAnalyser->data.clip_count = 1;
                rootMotionAnalyser->data.clip_to_process = 0;
                rootMotionAnalyser->data.clipInfo[0].clip = this;
                rootMotionAnalyser->data.clipInfo[0].weight = 1.0f; // force weight to be 1.0...

                for (int i = 0; i < channels.size(); i++)
                    channels[i].sampleTime(current_time, amount, rootMotionAnalyser);

                last_sampled_time = current_time;
            }
        }

        bool AnimationClip::canDoSample()
        {
            return last_sampled_time != current_time;
        }

        void AnimationClip::didExternalSample()
        {
            last_sampled_time = current_time;
        }

        AnimationTransitionChannelInformation &AnimationMixer::getTransition(uint32_t x, uint32_t y)
        {
            return transitions[x + y * clips_array.size()];
        }

        AnimationMixer::AnimationMixer()
        {
            current_clip = NULL;
            current_index = 0;
        }

        AnimationMixer::~AnimationMixer()
        {

            for (int i = 0; i < clips_array.size(); i++)
            {
                delete clips_array[i];
            }

            clips_index.clear();
            clips_array.clear();
            transitions.clear();

            current_clip = NULL;
        }

        void AnimationMixer::addClip(AnimationClip *clip)
        {

            ITK_ABORT(transitions.size() > 0, "trying to add clip after the animation transition computation.\n");

            clips_index[clip->name] = (uint32_t)clips_array.size();
            clips_array.push_back(clip);
        }

        void AnimationMixer::computeTransitions()
        {
            ITK_ABORT(transitions.size() > 0, "trying to initialize the animation twice.\n");

            // normalize the scale of all clips

            auto model_from_file = clips_array[0]->base_model->getChildAt(0);
            MathCore::vec3f scale = model_from_file->getScale();
            printf("[AnimationMixer] Model scale: %f %f %f.\n", scale.x, scale.y, scale.z);

            if (current_clip == NULL)
            {
                current_clip = clips_array[0];

                current_clip->doSample(1.0f, &rootMotionAnalyser);

                if (rootMotionAnalyser.method != nullptr && rootMotionAnalyser.data.dirty)
                {
                    rootMotionAnalyser.data.dirty = false;
                    rootMotionAnalyser.method(&rootMotionAnalyser.data);
                }
            }

            // check the clips root node
            for (int i = 1; i < clips_array.size(); i++)
            {
                ITK_ABORT(clips_array[i]->root_node != current_clip->root_node, "All animations need to share the root node with an animation attached to it.\n");
            }

            transitions.resize(clips_array.size() * clips_array.size());

            for (int index_a = 0; index_a < clips_array.size(); index_a++)
            {
                for (int index_b = 0; index_b < clips_array.size(); index_b++)
                {

                    if (index_a >= index_b)
                        continue;

                    AnimationClip *clip_a = clips_array[index_a];
                    AnimationClip *clip_b = clips_array[index_b];
                    AnimationTransitionChannelInformation &transition = getTransition(index_a, index_b);

                    for (int i = 0; i < clip_a->channels.size(); i++)
                    {
                        auto aNode = clip_a->channels[i].node;

                        NodeAnimation *channel_b = NULL;

                        bool found = false;
                        for (int j = 0; j < clip_b->channels.size(); j++)
                        {
                            auto bNode = clip_b->channels[j].node;
                            if (aNode == bNode)
                            {
                                found = true;
                                channel_b = &clip_b->channels[j];
                                break;
                            }
                        }

                        if (!found)
                        {
                            transition.no_matching_a_elements.push_back(&clip_a->channels[i]);
                        }
                        else
                        {
                            transition.matching_a_elements.push_back(&clip_a->channels[i]);
                            // transition.matching_b_elements.push_back(&clip_b->channels[i]);
                            transition.matching_b_elements.push_back(channel_b);
                        }
                    }

                    for (int i = 0; i < clip_b->channels.size(); i++)
                    {
                        auto bNode = clip_b->channels[i].node;

                        bool found = false;
                        for (int j = 0; j < clip_a->channels.size(); j++)
                        {
                            auto aNode = clip_a->channels[j].node;
                            if (aNode == bNode)
                            {
                                found = true;
                                break;
                            }
                        }

                        if (!found)
                        {
                            transition.no_matching_b_elements.push_back(&clip_b->channels[i]);
                        }
                        else
                        {
                            // transition.matching_b_elements.push_back(&clip_b->channels[i]);
                        }
                    }

                    ITK_ABORT(transition.matching_a_elements.size() != transition.matching_b_elements.size(), "Error on compute animation transition matrix.\n");
                }
            }
        }

        void AnimationMixer::update(float elapsed_secs)
        {
            if (request_queue.size() == 0)
            {
                current_clip->update(elapsed_secs);
                current_clip->doSample(1.0f, &rootMotionAnalyser);
                if (rootMotionAnalyser.method != nullptr && rootMotionAnalyser.data.dirty)
                {
                    rootMotionAnalyser.data.dirty = false;
                    rootMotionAnalyser.method(&rootMotionAnalyser.data);
                }
            }
            else
            {
                AnimationMixerLerpTarget *anim_target = &request_queue[0];

                anim_target->incrementTransitionTime(elapsed_secs);

                if (anim_target->reachedEnd())
                {
                    // set new current clip and index... and remove the first element from queue
                    current_clip = anim_target->clip;
                    current_index = anim_target->index;

                    request_queue.erase(request_queue.begin());

                    current_clip->update(elapsed_secs);
                    current_clip->doSample(1.0f, &rootMotionAnalyser);
                    if (rootMotionAnalyser.method != nullptr && rootMotionAnalyser.data.dirty)
                    {
                        rootMotionAnalyser.data.dirty = false;
                        rootMotionAnalyser.method(&rootMotionAnalyser.data);
                    }
                }
                else
                {
                    // custom sample clips acording the weight (lerp)
                    float lrp = anim_target->computeLerp();
                    float one_minus_lrp = 1.0f - lrp;

                    current_clip->update(elapsed_secs);
                    anim_target->clip->update(elapsed_secs);

                    bool current_clip_can_do_sample = current_clip->canDoSample();
                    bool blend_clip_can_do_sample = anim_target->clip->canDoSample();

                    // query transition Node animation information

                    uint32_t index_a = current_index;
                    uint32_t index_b = anim_target->index;
                    bool swap_index = false;

                    if (index_a > index_b)
                        swap_index = true;

                    std::vector<NodeAnimation *> *no_matching_a_elements;
                    std::vector<NodeAnimation *> *no_matching_b_elements;
                    std::vector<NodeAnimation *> *matching_a_elements;
                    std::vector<NodeAnimation *> *matching_b_elements;

                    if (swap_index)
                    {
                        AnimationTransitionChannelInformation &transition = getTransition(index_b, index_a);

                        no_matching_a_elements = &transition.no_matching_b_elements;
                        no_matching_b_elements = &transition.no_matching_a_elements;
                        matching_a_elements = &transition.matching_b_elements;
                        matching_b_elements = &transition.matching_a_elements;
                    }
                    else
                    {
                        AnimationTransitionChannelInformation &transition = getTransition(index_a, index_b);

                        no_matching_a_elements = &transition.no_matching_a_elements;
                        no_matching_b_elements = &transition.no_matching_b_elements;
                        matching_a_elements = &transition.matching_a_elements;
                        matching_b_elements = &transition.matching_b_elements;
                    }

                    ITK_ABORT(matching_a_elements->size() != matching_b_elements->size(), "Matching elements different sizes.\n");

                    float anim_time_a = current_clip->current_time,
                          anim_time_b = anim_target->clip->current_time;

                    rootMotionAnalyser.data.clip_count = 2;
                    rootMotionAnalyser.data.clipInfo[0].clip = current_clip;
                    rootMotionAnalyser.data.clipInfo[1].clip = anim_target->clip;

                    //
                    // Independent transforms update
                    //

                    rootMotionAnalyser.data.clip_to_process = 0;
                    if (current_clip_can_do_sample)
                    {
                        for (int i = 0; i < no_matching_a_elements->size(); i++)
                            no_matching_a_elements->at(i)->sampleTime(anim_time_a, one_minus_lrp, &rootMotionAnalyser);
                        current_clip->didExternalSample();
                    }

                    rootMotionAnalyser.data.clip_to_process = 1;
                    if (blend_clip_can_do_sample)
                    {
                        for (int i = 0; i < no_matching_b_elements->size(); i++)
                            no_matching_b_elements->at(i)->sampleTime(anim_time_b, lrp, &rootMotionAnalyser);
                        anim_target->clip->didExternalSample();
                    }

                    //
                    // shared transforms update
                    //
                    for (int i = 0; i < matching_a_elements->size(); i++)
                    {
                        NodeAnimation *node_a = matching_a_elements->at(i);
                        NodeAnimation *node_b = matching_b_elements->at(i);
                        ITK_ABORT(node_a->node != node_b->node, "Node A transform different from node B transform.\n");

                        node_a->Sample_Lerp_to_Another_Node(anim_time_a, node_b, anim_time_b, lrp, &rootMotionAnalyser);
                    }

                    // all blend frames could generate a dirty flag
                    if (rootMotionAnalyser.method != nullptr && rootMotionAnalyser.data.dirty)
                    {
                        rootMotionAnalyser.data.dirty = false;
                        rootMotionAnalyser.method(&rootMotionAnalyser.data);
                    }
                }
            }
        }

        void AnimationMixer::play(const std::string &clipname, float blend_time)
        {
            if (clips_index.find(clipname) == clips_index.end())
                return;

            uint32_t index = clips_index[clipname];

            if (request_queue.size() == 0)
            {
                if (current_index == index)
                    return;
            }
            else
            {
                if (request_queue[request_queue.size() - 1].index == index)
                    return;
            }

            while (request_queue.size() > 1)
                request_queue.pop_back();

            if (request_queue.size() == 0 || request_queue[0].index != index)
                request_queue.push_back(AnimationMixerLerpTarget(clips_array[index], index, blend_time));
        }

        std::shared_ptr<Transform> AnimationMixer::rootNode()
        {

            return current_clip->root_node;
        }

        RootMotionAnalyserData::RootMotionAnalyserData()
        {
            clipInfo[0].clip = NULL;
            clipInfo[0].weight = 0.0f;
            clipInfo[1].clip = NULL;
            clipInfo[1].weight = 0.0f;
            clip_count = 0;
            clip_to_process = 0;

            dirty = true;
        }

        void RootMotionAnalyserData::setFromCurrentState()
        {
            if (!isClipsBlending())
            {
                for (int i = 0; i < clip_count; i++)
                {
                    clip_to_process = i;
                    setFromNodeAnimation(clipInfo[i].clip->root_node_animation, clipInfo[i].clip->current_time, clipInfo[i].weight);
                }
            }
            else
            {
                setFromNodeAnimationBlend(
                    clipInfo[0].clip->root_node_animation, clipInfo[0].clip->current_time,
                    clipInfo[1].clip->root_node_animation, clipInfo[1].clip->current_time,
                    clipInfo[1].weight);
            }
        }

        void RootMotionAnalyserData::setFromNodeAnimation(NodeAnimation *node_animation, float time, float weight)
        {

            dirty = true;

            auto node = node_animation->node;
            ClipInfo *_clipInfo = &clipInfo[clip_to_process];

            MathCore::vec3f target_local_position = node_animation->samplePos(time, &_clipInfo->position_delta_interframe);

            // make global
            node->setLocalPosition(_clipInfo->position_delta_interframe);
            _clipInfo->position_delta_interframe = node->getPosition();

            node->setLocalPosition(target_local_position);
            _clipInfo->position = node->getPosition();

            node->setLocalPosition(node_animation->start_position);
            _clipInfo->start = node->getPosition();

            _clipInfo->local_position = target_local_position;
            _clipInfo->local_start = node_animation->start_position;

            // compute deltas
            _clipInfo->position_delta = _clipInfo->position - _clipInfo->start;
            _clipInfo->local_position_delta = _clipInfo->local_position - _clipInfo->local_start;

            _clipInfo->weight = weight;
        }

        void RootMotionAnalyserData::setFromNodeAnimationBlend(
            NodeAnimation *node_animation_a, float time_a,
            NodeAnimation *node_animation_b, float time_b,
            float lrp)
        {

            dirty = true;

            ClipInfo *clipInfo_a = &clipInfo[0];
            ClipInfo *clipInfo_b = &clipInfo[1];

            auto node = node_animation_a->node;

            MathCore::vec3f target_local_position_a = node_animation_a->samplePos(time_a, &clipInfo_a->position_delta_interframe);
            MathCore::vec3f target_local_position_b = node_animation_b->samplePos(time_b, &clipInfo_b->position_delta_interframe);

            // make global
            node->setLocalPosition(clipInfo_a->position_delta_interframe);
            clipInfo_a->position_delta_interframe = node->getPosition();

            node->setLocalPosition(target_local_position_a);
            clipInfo_a->position = node->getPosition();

            node->setLocalPosition(node_animation_a->start_position);
            clipInfo_a->start = node->getPosition();

            clipInfo_a->weight = 1.0f - lrp;

            clipInfo_a->local_position = target_local_position_a;
            clipInfo_a->local_start = node_animation_a->start_position;

            clipInfo_a->position_delta = clipInfo_a->position - clipInfo_a->start;
            clipInfo_a->local_position_delta = clipInfo_a->local_position - clipInfo_a->local_start;

            // make global
            node->setLocalPosition(clipInfo_b->position_delta_interframe);
            clipInfo_b->position_delta_interframe = node->getPosition();

            node->setLocalPosition(target_local_position_b);
            clipInfo_b->position = node->getPosition();

            node->setLocalPosition(node_animation_b->start_position);
            clipInfo_b->start = node->getPosition();

            clipInfo_b->weight = lrp;

            clipInfo_b->local_position = target_local_position_b;
            clipInfo_b->local_start = node_animation_b->start_position;

            clipInfo_b->position_delta = clipInfo_b->position - clipInfo_b->start;
            clipInfo_b->local_position_delta = clipInfo_b->local_position - clipInfo_b->local_start;

            node->setLocalPosition(MathCore::OP<MathCore::vec3f>::lerp(clipInfo_a->local_start, clipInfo_b->local_start, lrp));
        }

        bool RootMotionAnalyserData::isClipsBlending() const
        {
            return clip_count == 2 && clipInfo[0].clip->root_node_animation->node == clipInfo[1].clip->root_node_animation->node;
        }

        void RootMotionAnalyserData::applyMotionLocalExample()
        {
            if (!isClipsBlending())
            {
                for (int i = 0; i < clip_count; i++)
                {
                    ClipInfo &_clipInfo = clipInfo[i];
                    _clipInfo.clip->root_node->setLocalPosition(MathCore::OP<MathCore::vec3f>::lerp(
                        _clipInfo.local_start,
                        _clipInfo.local_position,
                        _clipInfo.weight));
                }
            }
            else
            {
                // 2 clips, and they have the same node
                ClipInfo &clipInfo_a = clipInfo[0];
                ClipInfo &clipInfo_b = clipInfo[1];

                auto node = clipInfo_a.clip->root_node;

                node->setLocalPosition(MathCore::OP<MathCore::vec3f>::lerp(
                    clipInfo_a.local_position,
                    clipInfo_b.local_position,
                    clipInfo_b.weight));
            }
        }

        void RootMotionAnalyserData::applyMotionGlobalExample()
        {
            if (!isClipsBlending())
            {
                for (int i = 0; i < clip_count; i++)
                {
                    ClipInfo &_clipInfo = clipInfo[i];
                    _clipInfo.clip->root_node->setPosition(MathCore::OP<MathCore::vec3f>::lerp(
                        _clipInfo.start,
                        _clipInfo.position,
                        _clipInfo.weight));
                }
            }
            else
            {
                // 2 clips, and they have the same node
                ClipInfo &clipInfo_a = clipInfo[0];
                ClipInfo &clipInfo_b = clipInfo[1];

                auto node = clipInfo_a.clip->root_node;

                node->setPosition(MathCore::OP<MathCore::vec3f>::lerp(
                    clipInfo_a.position,
                    clipInfo_b.position,
                    clipInfo_b.weight));
            }
        }
    }
}