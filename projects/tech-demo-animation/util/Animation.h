#ifndef Animation__H__
#define Animation__H__

#include <aribeiro/aribeiro.h>
//#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
//#include <mini-gl-engine/Engine.h>
#include <mini-gl-engine/interpolators.h>
#include <data-model/Animation.h>

namespace GLEngine {


    //
    // Root Motion Analyser
    //
    class AnimationClip;
    class NodeAnimation;
    class _SSE2_ALIGN_PRE ClipInfo {
    public:
        AnimationClip* clip;

        // related to the root node of the model
        // use: Transform* root_node = mixer.rootNode();
        //      root_node->setPosition(position);
        aRibeiro::vec3 position;
        aRibeiro::vec3 position_delta;
        aRibeiro::vec3 position_delta_interframe;
        aRibeiro::vec3 start;

        // related to the root bone node in the local space
        // use: Transform* root_node = mixer.rootNode();
        //      root_node->setLocalPosition(local_position);
        aRibeiro::vec3 local_position;
        aRibeiro::vec3 local_position_delta;
        aRibeiro::vec3 local_start;

        float weight;
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    class _SSE2_ALIGN_PRE RootMotionAnalyserData {
    public:

        bool dirty;

        ClipInfo clipInfo[2];
        int clip_count;

        //for internal use
        int clip_to_process;

        RootMotionAnalyserData();
        void setFromNodeAnimation( NodeAnimation *node_animation, float time, float weight );
        void setFromNodeAnimationBlend( 
            NodeAnimation *node_animation_a, float time_a,
            NodeAnimation *node_animation_b, float time_b,
            float lrp 
        );
        void setFromCurrentState();
        bool isClipsBlending() const ;

        void applyMotionLocalExample();

        void applyMotionGlobalExample();

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    DefineMethodPointer(RootMotionAnalyserMethod_ptr, void, RootMotionAnalyserData *data) VoidMethodCall(data);

    class _SSE2_ALIGN_PRE RootMotionAnalyser {
    public:

        RootMotionAnalyserMethod_ptr method;
        RootMotionAnalyserData data;

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    //
    // Node Animation
    //

    class _SSE2_ALIGN_PRE NodeAnimation {
    public:
        // interpolator arrays
        LinearInterpolator<aRibeiro::vec3> position;
        LinearInterpolator<aRibeiro::vec3> scale;
        SlerpInterpolator rotation;

        Transform *node;

        aRibeiro::vec3 start_position;
        aRibeiro::vec3 start_scale;
        aRibeiro::quat start_rotation;

        //aRibeiro::vec3 motion_last_value;

        bool isRootNode;

        NodeAnimation() {
            isRootNode = false;
        }

        // some animations have more keys outside the animation original duration... 
        // this can lead to a wrong root node interpolation
        // we need to clamp the curve to avoid this situation...
        void clampDuration(float min_duration, float max_duration) {

            if (position.keys.size() <= 1)
                return;

            float last_time = position.keys[position.keys.size() - 1].time;

            printf("[NodeAnimation] clampToMaxDuration\n");
            printf("  max_duration to set: %f\n", max_duration);
            printf("  last_time: %f\n", last_time);
            if (last_time > max_duration) {
                printf("  NEED SET LAST FRAME TIME!!!\n");
                aRibeiro::vec3 valueToSet = position.getValue_ForwardLoop(max_duration);
                int i = position.keys.size() - 1;
                //remove all index
                while (i > 0) {
                    if (position.keys[i].time < max_duration)
                        break;
                    else
                        position.keys.erase(position.keys.begin() + i);
                    i--;
                }
                position.keys.push_back( Key<aRibeiro::vec3>(max_duration, valueToSet) );
                //reset delta frame
                position.getValue_ForwardLoop(0);
            }

            if (position.keys.size() <= 1)
                return;

            // check if the first frame starts with 0
            float first_time = position.keys[0].time;
            if (first_time < min_duration) {
                printf("  NEED SET FIRST FRAME TIME!!!\n");
                aRibeiro::vec3 valueToSet = position.getValue_ForwardLoop(min_duration);
                int i = 0;
                //remove all index
                while (i < position.keys.size()) {
                    if (position.keys[i].time > min_duration)
                        break;
                    else
                        position.keys.erase(position.keys.begin());
                    i++;
                }
                position.keys.insert(position.keys.begin(), Key<aRibeiro::vec3>(min_duration, valueToSet));
                //reset delta frame
                position.getValue_ForwardLoop(min_duration);

                //move all times to 0
                for (int i = 0; i < position.keys.size(); i++)
                    position.keys[i].time -= min_duration;

                //reset delta frame
                position.getValue_ForwardLoop(0);
            }


            


        }

        //need this to play new animations starting from zero
        // only valid for root node
        void resetInterframeInformation() {
            position.getValue_ForwardLoop(0);
            scale.getValue_ForwardLoop(0);
            rotation.getValue_ForwardLoop(0);
        }

        void setTransform(Transform *t, bool rootNode) {
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

            //motion_last_value = start_position;
            //motion_last_value = aRibeiro::toVec3(node->getMatrix() * aRibeiro::toPtn4(motion_last_value));
            //motion_last_value = aRibeiro::toVec3(node->getMatrix() * aRibeiro::toPtn4(motion_last_value - start_position));
        }

        ARIBEIRO_INLINE aRibeiro::vec3 samplePos(float secs, aRibeiro::vec3 *interframe_delta) {
            if (position.keys.size() > 0)
                return position.getValue_ForwardLoop(secs, interframe_delta);
            *interframe_delta = aRibeiro::vec3(0);
            return aRibeiro::vec3(0);
        }

        ARIBEIRO_INLINE aRibeiro::vec3 sampleScale(float secs) {
            if (scale.keys.size() > 0)
                return scale.getValue_ForwardLoop(secs);
            return aRibeiro::vec3(1);
        }

        ARIBEIRO_INLINE aRibeiro::quat sampleRotation(float secs) {
            if (rotation.keys.size() > 0)
                return rotation.getValue_ForwardLoop(secs);
            return aRibeiro::quat();
        }

        ARIBEIRO_INLINE void Sample_Lerp_to_Another_Node(float time_a, NodeAnimation *b, float time_b, float lrp, RootMotionAnalyser *rootMotionAnalyser) {
            
            aRibeiro::vec3 scale = aRibeiro::lerp(sampleScale(time_a), b->sampleScale(time_b), lrp);
            aRibeiro::quat rotation = aRibeiro::slerp(sampleRotation(time_a), b->sampleRotation(time_b), lrp);

            ARIBEIRO_ABORT(isRootNode != b->isRootNode, "The animation nodes dont have the same root node...\n");

            if (isRootNode && rootMotionAnalyser->method != NULL) {
            
                ARIBEIRO_ABORT(node != b->node, "The animation root nodes dont have the same transform...\n");

                //aRibeiro::vec3 _a = samplePos(time_a);
                //aRibeiro::vec3 _b = b->samplePos(time_b);

                rootMotionAnalyser->data.setFromNodeAnimationBlend( this, time_a, b, time_b, lrp );

                /*

                node->setLocalPosition(_a);
                motion_last_value = node->getPosition();

                node->setLocalPosition(_b);
                b->motion_last_value = node->getPosition();

                aRibeiro::vec3 _start_lrp = aRibeiro::lerp(start_position, b->start_position, lrp);

                node->setLocalPosition(_start_lrp);

                (*computed_root_delta) = aRibeiro::lerp(
                    motion_last_value,
                    b->motion_last_value,
                    lrp
                );

                */

            }
            else {
                // reset root node interframe information
                //rootMotionAnalyser->data.clipInfo[0].position_delta_interframe = aRibeiro::vec3(0);
                //rootMotionAnalyser->data.clipInfo[1].position_delta_interframe = aRibeiro::vec3(0);

                aRibeiro::vec3 pos = aRibeiro::lerp(samplePos(time_a, NULL), b->samplePos(time_b, NULL), lrp);
                node->setLocalPosition(pos);
            }
            node->setLocalScale(scale);
            node->setLocalRotation(rotation);
        }

        ARIBEIRO_INLINE void sampleTime(float secs, float amount, RootMotionAnalyser *rootMotionAnalyser) {
            if (amount == 1.0f) {
                if (position.keys.size() > 0) {
                    if ( isRootNode && rootMotionAnalyser->method != NULL) {
                        
                        //aRibeiro::vec3 local_position = position.getValue_ForwardLoop(secs);
                        rootMotionAnalyser->data.setFromNodeAnimation( this, secs, 1.0f );
                        
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
                if (scale.keys.size()>0)
                    node->setLocalScale( scale.getValue_ForwardLoop(secs) );
                if (rotation.keys.size()>0)
                    node->setLocalRotation( rotation.getValue_ForwardLoop(secs) );
            } else {
                if (position.keys.size() > 0) {
                    if (isRootNode && rootMotionAnalyser->method != NULL) {

                        //aRibeiro::vec3 local_position = position.getValue_ForwardLoop(secs);
                        rootMotionAnalyser->data.setFromNodeAnimation( this, secs, amount );
                        /*
                        node->setLocalPosition(aRibeiro::lerp(start_position, position.getValue_ForwardLoop(secs), amount));
                        motion_last_value = node->getPosition();
                        (*computed_root_delta) = motion_last_value;

                        node->setLocalPosition(start_position);
                        */
                    }
                    else
                        node->setLocalPosition(aRibeiro::lerp(start_position, position.getValue_ForwardLoop(secs), amount));
                }
                if (scale.keys.size()>0)
                    node->setLocalScale(aRibeiro::lerp(start_scale, scale.getValue_ForwardLoop(secs), amount));
                if (rotation.keys.size()>0)
                    node->setLocalRotation(aRibeiro::slerp(start_rotation, rotation.getValue_ForwardLoop(secs), amount));
            }
        }

        SSE2_CLASS_NEW_OPERATOR

    } _SSE2_ALIGN_POS ;


    class AnimationClip {
        float last_sampled_time;

        bool findRootNode(Transform *t, const void* userData);
    public:
        std::string name;
        float duration;
        aRibeiro::aligned_vector<NodeAnimation> channels;

        float current_time;
        bool loop;
        bool wait_end_to_transition;

        Transform *base_model;
        Transform *root_node;
        NodeAnimation *root_node_animation;

        AnimationClip(const std::string &clip_name, Transform *root, const model::Animation &animation  ) ;

        void update(float elapsed_secs) ;
        void reset() ;

        void forceResample() ;

        void doSample(float amount, RootMotionAnalyser *rootMotionAnalyser) ;

        bool canDoSample() ;

        void didExternalSample() ;

    };

    class AnimationTransitionChannelInformation {
    public:
        std::vector<NodeAnimation*> no_matching_a_elements;
        std::vector<NodeAnimation*> no_matching_b_elements;
        std::vector<NodeAnimation*> matching_a_elements;
        std::vector<NodeAnimation*> matching_b_elements;
    };

    class AnimationMixerLerpTarget {
    public:

        AnimationClip* clip;
        uint32_t index;
        float transition_duration_secs;
        float transition_current;

        AnimationMixerLerpTarget() {
            clip = NULL;
            index = -1;
            transition_duration_secs = 0.0f;
            transition_current = 0.0f;
        }

        AnimationMixerLerpTarget(AnimationClip* _clip, uint32_t _index, float _transition_duration_secs) {
            clip = _clip;
            index = _index;
            transition_duration_secs = _transition_duration_secs;
            transition_current = 0.0f;
        }

        bool isInstantaneous() {
            return transition_duration_secs <= aRibeiro::EPSILON;
        }

        void incrementTransitionTime(float elapsed) {
            transition_current += elapsed;
        }

        bool reachedEnd() {
            return transition_current >= transition_duration_secs;
        }

        float computeLerp() {
            return transition_current / transition_duration_secs;
        }
    };

    class _SSE2_ALIGN_PRE AnimationMixer {

        std::map<std::string,uint32_t> clips_index;
        std::vector<AnimationClip*> clips_array;
        std::vector<AnimationTransitionChannelInformation> transitions;

        AnimationClip* current_clip;
        uint32_t current_index;

        std::vector<AnimationMixerLerpTarget > request_queue;

        //
        // Root Motion Analyser
        //
        RootMotionAnalyser rootMotionAnalyser;

        AnimationTransitionChannelInformation& getTransition(uint32_t x, uint32_t y);
    public:
        

        AnimationMixer() ;
        virtual ~AnimationMixer();

        void addClip(AnimationClip* clip) ;

        void computeTransitions();

        //void update_old(float elapsed_secs);

        void update(float elapsed_secs);

        void play(const std::string &clipname, float blend_time = 0.5f) ;

        Transform * rootNode();


        void setRootMotionAnalyserCallback(const RootMotionAnalyserMethod_ptr &callback) {
            rootMotionAnalyser.method = callback;

            if (rootMotionAnalyser.method != NULL && rootMotionAnalyser.data.clip_count > 0) {
                //set from current state
                rootMotionAnalyser.data.setFromCurrentState();
                rootMotionAnalyser.method(&rootMotionAnalyser.data);
            }

        }


        SSE2_CLASS_NEW_OPERATOR

    }_SSE2_ALIGN_POS;

};

#endif