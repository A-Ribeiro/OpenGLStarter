#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/ComponentSkinnedMesh.h>

#include <appkit-gl-engine/util/Animation.h>
#include <appkit-gl-engine/SharedPointer/SharedPointer.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            class ClipMotionInfluence
            {
            public:
                std::string name;
                float right;
                float up;
                float forward;

                ClipMotionInfluence()
                {
                }

                ClipMotionInfluence(const std::string &_name, float _right, float _up, float _forward)
                {
                    name = _name;
                    right = _right;
                    up = _up;
                    forward = _forward;
                }
            };

            // BEGIN_DECLARE_DELEGATE(AnimationMotionOnMoveEvent, float right, float up, float forward, const MathCore::vec3f &localPosition)
            // CALL_PATTERN(right, up, forward, localPosition) END_DECLARE_DELEGATE;

            class ComponentAnimationMotion : public Component
            {

                void OnRootMotionAnalyser(RootMotionAnalyserData *data)
                {

                    // animation node calculator
                    if (!data->isClipsBlending())
                    {
                        for (int i = 0; i < data->clip_count; i++)
                        {
                            ClipInfo &_clipInfo = data->clipInfo[i];

                            ClipMotionInfluence *influence = GetInfluence(_clipInfo.clip->name);
                            if (influence != NULL)
                            {
                                MathCore::vec3f aux = MathCore::vec3f(influence->right, influence->up, influence->forward);
                                MathCore::vec3f weighted_delta = _clipInfo.position_delta * _clipInfo.weight;
                                MathCore::vec3f final_position = _clipInfo.start + weighted_delta * (1.0f - aux);
                                _clipInfo.clip->root_node->setPosition(final_position);
                            }
                            else
                            {
                                MathCore::vec3f final_local_position = MathCore::OP<MathCore::vec3f>::lerp(_clipInfo.local_start, _clipInfo.local_position, _clipInfo.weight);
                                _clipInfo.clip->root_node->setLocalPosition(final_local_position);
                            }
                        }
                    }
                    else
                    {
                        // 2 clips, and they have the same node
                        ClipInfo &clipInfo_a = data->clipInfo[0];
                        ClipInfo &clipInfo_b = data->clipInfo[1];

                        MathCore::vec3f influence_a;

                        {
                            ClipMotionInfluence *influence = GetInfluence(clipInfo_a.clip->name);
                            if (influence != NULL)
                            {
                                MathCore::vec3f aux = MathCore::vec3f(influence->right, influence->up, influence->forward);
                                MathCore::vec3f weighted_delta = clipInfo_a.position_delta; // *clipInfo_a.weight; // 100%
                                influence_a = weighted_delta * (1.0f - aux);
                            }
                        }

                        MathCore::vec3f influence_b;

                        {
                            ClipMotionInfluence *influence = GetInfluence(clipInfo_b.clip->name);
                            if (influence != NULL)
                            {
                                MathCore::vec3f aux = MathCore::vec3f(influence->right, influence->up, influence->forward);
                                MathCore::vec3f weighted_delta = clipInfo_b.position_delta; // *clipInfo_b.weight;// 100%
                                influence_b = weighted_delta * (1.0f - aux);
                            }
                        }

                        /*
                        MathCore::vec3f final_position = MathCore::OP<MathCore::vec3f>::lerp(
                            clipInfo_a.start,
                            clipInfo_b.start,
                            clipInfo_b.weight
                        );

                        final_position = final_position + MathCore::OP<MathCore::vec3f>::lerp(influence_a, influence_b, clipInfo_b.weight);
                        */
                        MathCore::vec3f final_position = MathCore::OP<MathCore::vec3f>::lerp(
                            clipInfo_a.start + influence_a,
                            clipInfo_b.start + influence_b,
                            clipInfo_b.weight);

                        Transform *node = clipInfo_a.clip->root_node;
                        node->setPosition(final_position);
                    }

                    // external move calculator
                    {
                        MathCore::vec3f move_external_delta = MathCore::vec3f(0);

                        for (int i = 0; i < data->clip_count; i++)
                        {
                            ClipInfo &_clipInfo = data->clipInfo[i];
                            ClipMotionInfluence *influence = GetInfluence(_clipInfo.clip->name);
                            if (influence != NULL)
                            {
                                MathCore::vec3f aux = MathCore::vec3f(influence->right, influence->up, influence->forward);
                                MathCore::vec3f weighted_delta_interframe = _clipInfo.position_delta_interframe * _clipInfo.weight;
                                move_external_delta += weighted_delta_interframe * aux;
                            }
                        }

                        MathCore::vec3f local_delta = MathCore::CVT<MathCore::vec4f>::toVec3(
                            transform[0]->worldToLocalMatrix() *
                            (skinnedMesh->transform[0]->localToWorldMatrix() *
                             MathCore::CVT<MathCore::vec3f>::toVec4(move_external_delta)));

                        MathCore::vec3f local_final = transform[0]->getLocalPosition() + local_delta;

                        // local_final = MathCore::CVT<MathCore::vec4f>::toVec3(transform[0]->localToWorldMatrix() * MathCore::CVT<MathCore::vec3f>::toPtn4(local_final));

                        if (OnMove != nullptr)
                        {
                            OnMove(local_delta.x, local_delta.y, local_delta.z, local_final);
                        }
                        else
                        {
                            transform[0]->setLocalPosition(local_final);
                        }
                    }
                }

            public:
                static const ComponentType Type;

                std::vector<ClipMotionInfluence> motionInfluence;
                SharedPointer<ComponentSkinnedMesh> skinnedMesh; // becomes null when skinned mesh is deleted

                EventCore::Event<void(float right, float up, float forward, const MathCore::vec3f &localPosition)> OnMove;

                ComponentAnimationMotion() : Component(ComponentAnimationMotion::Type)
                {
                }

                void start()
                {
                    skinnedMesh = (ComponentSkinnedMesh *)transform[0]->findComponent(ComponentSkinnedMesh::Type);
                    if (skinnedMesh == NULL)
                        skinnedMesh = (ComponentSkinnedMesh *)transform[0]->findComponentInChildren(ComponentSkinnedMesh::Type);
                    ITK_ABORT(skinnedMesh == NULL, "Failed to query skinned mesh\n.");

                    skinnedMesh->mixer.setRootMotionAnalyserCallback(EventCore::CallbackWrapper(&ComponentAnimationMotion::OnRootMotionAnalyser, this));
                }

                ClipMotionInfluence *GetInfluence(const std::string &name)
                {
                    for (int i = 0; i < motionInfluence.size(); i++)
                    {
                        if (name.compare(motionInfluence[i].name) == 0)
                            return &motionInfluence[i];
                    }
                    return NULL;
                }

                void TriggerClip(const std::string &name)
                {
                    skinnedMesh->mixer.play(name);
                }

                ~ComponentAnimationMotion()
                {
                    if (skinnedMesh != NULL)
                    {
                        skinnedMesh->mixer.setRootMotionAnalyserCallback(NULL);
                        skinnedMesh = NULL;
                    }
                }
            };

        }

    }

}
