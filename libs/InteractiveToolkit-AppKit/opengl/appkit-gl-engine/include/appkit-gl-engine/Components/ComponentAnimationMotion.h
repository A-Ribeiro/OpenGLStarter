#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/ComponentSkinnedMesh.h>

#include <appkit-gl-engine/util/Animation.h>
// #include <appkit-gl-engine/SharedPointer/SharedPointer.h>

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
                            if (influence != nullptr)
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
                            if (influence != nullptr)
                            {
                                MathCore::vec3f aux = MathCore::vec3f(influence->right, influence->up, influence->forward);
                                MathCore::vec3f weighted_delta = clipInfo_a.position_delta; // *clipInfo_a.weight; // 100%
                                influence_a = weighted_delta * (1.0f - aux);
                            }
                        }

                        MathCore::vec3f influence_b;

                        {
                            ClipMotionInfluence *influence = GetInfluence(clipInfo_b.clip->name);
                            if (influence != nullptr)
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

                        auto node = clipInfo_a.clip->root_node;
                        node->setPosition(final_position);
                    }

                    // external move calculator
                    {
                        MathCore::vec3f move_external_delta = MathCore::vec3f(0);

                        for (int i = 0; i < data->clip_count; i++)
                        {
                            ClipInfo &_clipInfo = data->clipInfo[i];
                            ClipMotionInfluence *influence = GetInfluence(_clipInfo.clip->name);
                            if (influence != nullptr)
                            {
                                MathCore::vec3f aux = MathCore::vec3f(influence->right, influence->up, influence->forward);
                                MathCore::vec3f weighted_delta_interframe = _clipInfo.position_delta_interframe * _clipInfo.weight;
                                move_external_delta += weighted_delta_interframe * aux;
                            }
                        }

                        auto _transform = getTransform();
                        auto skinnedMesh = ToShared(skinnedMeshRef);
                        auto skinnedMesh_Transform = skinnedMesh->getTransform();

                        MathCore::vec3f local_delta = MathCore::CVT<MathCore::vec4f>::toVec3(
                            _transform->worldToLocalMatrix() *
                            (skinnedMesh_Transform->localToWorldMatrix() *
                             MathCore::CVT<MathCore::vec3f>::toVec4(move_external_delta)));

                        MathCore::vec3f local_final = _transform->getLocalPosition() + local_delta;

                        // local_final = MathCore::CVT<MathCore::vec4f>::toVec3(transform[0]->localToWorldMatrix() * MathCore::CVT<MathCore::vec3f>::toPtn4(local_final));

                        if (OnMove != nullptr)
                        {
                            OnMove(local_delta.x, local_delta.y, local_delta.z, local_final);
                        }
                        else
                        {
                            _transform->setLocalPosition(local_final);
                        }
                    }
                }

            public:
                static const ComponentType Type;

                std::vector<ClipMotionInfluence> motionInfluence;
                std::weak_ptr<ComponentSkinnedMesh> skinnedMeshRef; // becomes nullptr when skinned mesh is deleted

                EventCore::Event<void(float right, float up, float forward, const MathCore::vec3f &localPosition)> OnMove;

                ComponentAnimationMotion() : Component(ComponentAnimationMotion::Type)
                {
                }

                void start()
                {
                    auto _transform = getTransform();

                    auto skinnedMeshComponent = _transform->findComponent<ComponentSkinnedMesh>();

                    auto skinnedMesh = _transform->findComponent<ComponentSkinnedMesh>();
                    if (skinnedMesh == nullptr)
                        skinnedMesh = _transform->findComponentInChildren<ComponentSkinnedMesh>();

                    ITK_ABORT(skinnedMesh == nullptr, "Failed to query skinned mesh\n.");
                    skinnedMeshRef = skinnedMesh;

                    skinnedMesh->mixer.setRootMotionAnalyserCallback(EventCore::CallbackWrapper(&ComponentAnimationMotion::OnRootMotionAnalyser, this));
                }

                ClipMotionInfluence *GetInfluence(const std::string &name)
                {
                    for (int i = 0; i < motionInfluence.size(); i++)
                    {
                        if (name.compare(motionInfluence[i].name) == 0)
                            return &motionInfluence[i];
                    }
                    return nullptr;
                }

                void TriggerClip(const std::string &name)
                {
                    // auto skinnedMesh = std::shared_ptr<ComponentSkinnedMesh>(skinnedMeshRef);
                    auto skinnedMesh = ToShared(skinnedMeshRef);
                    skinnedMesh->mixer.play(name);
                }

                ~ComponentAnimationMotion()
                {
                    auto skinnedMesh = ToShared(skinnedMeshRef);
                    if (skinnedMesh != nullptr)
                    {
                        skinnedMesh->mixer.setRootMotionAnalyserCallback(nullptr);
                        // skinnedMesh = nullptr;
                        skinnedMeshRef.reset();
                    }
                }

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone)
                {
                    auto result = Component::CreateShared<ComponentAnimationMotion>();

                    result->motionInfluence = this->motionInfluence;
                    result->skinnedMeshRef = this->skinnedMeshRef;

                    return result;
                }
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
                {
                    {
                        auto found = componentMap.find(ToShared(this->skinnedMeshRef));
                        if (found != componentMap.end())
                            this->skinnedMeshRef = std::dynamic_pointer_cast<ComponentSkinnedMesh>(found->second);
                    }
                }

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                    writer.StartObject();
                    writer.String("type");
                    writer.String(ComponentAnimationMotion::Type);
                    writer.String("id");
                    writer.Uint64((intptr_t)self().get());
                    writer.EndObject();
                    
                }
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet){
                    if (!_value.HasMember("type") || !_value["type"].IsString())
                        return;
                    if (!strcmp(_value["type"].GetString(), ComponentAnimationMotion::Type) == 0)
                        return;
                    
                }

            };

        }

    }

}
