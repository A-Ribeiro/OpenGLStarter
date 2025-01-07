#include <appkit-gl-engine/Serializer/JSONSceneSerializer.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <string>
#include <InteractiveToolkit/ITKCommon/STL_Tools.h>

// components
#include <appkit-gl-engine/Components/deprecated/ComponentCameraLookToNode.h>
#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>
#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>

#include <appkit-gl-engine/Components/ComponentAnimationMotion.h>
#include <appkit-gl-engine/Components/ComponentCameraMove.h>
#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/ComponentCameraRotateOnTarget.h>
#include <appkit-gl-engine/Components/ComponentFontToMesh.h>
#include <appkit-gl-engine/Components/ComponentFrustumCulling.h>
#include <appkit-gl-engine/Components/ComponentLight.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>
#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
#include <appkit-gl-engine/Components/ComponentParticleSystem.h>
#include <appkit-gl-engine/Components/ComponentSkinnedMesh.h>
#include <appkit-gl-engine/Components/ComponentThirdPersonCamera.h>
#include <appkit-gl-engine/Components/ComponentThirdPersonPlayerController.h>

namespace AppKit
{
    namespace GLEngine
    {

        using RapidJsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;

        // template <typename _math_type,
        //           typename std::enable_if<
        //               MathCore::MathTypeInfo<_math_type>::_is_valid::value &&
        //                   MathCore::MathTypeInfo<_math_type>::_is_vec::value &&
        //                   std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value
        //                   ,
        //               bool>::type = true>
        // static ITK_INLINE _math_type read(rapidjson::Value &reader)
        // {
        //     if (!reader.IsArray())
        //         return _math_type();
        //     if ((int)reader.Size() != (int)_math_type::array_count)
        //         return _math_type();

        //     using float_type = typename MathCore::MathTypeInfo<_math_type>::_type;
        //     _math_type result;
        //     for (int i = 0; i < (int)_math_type::array_count; i++)
        //     {
        //         auto &element = reader[i];
        //         if (!element.IsDouble())
        //             continue;
        //         result[i] = MathCore::CVT<float_type>::toFloat(element.GetDouble());
        //     }
        //     return result;
        // }

        // template <typename _math_type,
        //           typename std::enable_if<
        //               MathCore::MathTypeInfo<_math_type>::_is_valid::value &&
        //                   MathCore::MathTypeInfo<_math_type>::_is_vec::value &&
        //                   std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value
        //                   ,
        //               bool>::type = true>
        // static ITK_INLINE void write(RapidJsonWriter &writer, const _math_type &v)
        // {
        //     using float_type = typename MathCore::MathTypeInfo<_math_type>::_type;
        //     writer.StartArray();
        //     for (int i = 0; i < (int)_math_type::array_count; i++)
        //         writer.Double(MathCore::CVT<float_type>::toDouble(v[i]));
        //     writer.EndArray();
        // }

        // template <typename _math_type,
        //           typename std::enable_if<
        //               MathCore::MathTypeInfo<_math_type>::_is_valid::value &&
        //                   !MathCore::MathTypeInfo<_math_type>::_is_vec::value &&
        //                   std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value,
        //               bool>::type = true>
        // static ITK_INLINE void write(RapidJsonWriter &writer, const _math_type &v)
        // {
        //     using float_type = typename MathCore::MathTypeInfo<_math_type>::_type;
        //     writer.StartArray();
        //     for (int c = 0; c < (int)_math_type::cols; c++)
        //     {
        //         writer.StartArray();
        //         for (int r = 0; r < (int)_math_type::rows; r++)
        //             writer.Double(MathCore::CVT<float_type>::toDouble(v(r, c)));
        //         writer.EndArray();
        //     }
        //     writer.EndArray();
        // }

        std::shared_ptr<WriterSet> JSONSceneSerializer::Begin()
        {
            return std::make_shared<WriterSet>();
        }
        void JSONSceneSerializer::Serialize(RapidJsonWriter &writer, std::shared_ptr<Transform> transform, bool include_root)
        {

            if (!include_root)
                writer.StartArray(); // start children

            std::unordered_map<uint64_t, bool> component_exported;

            transform->traverse_Generic(
                // pre order
                [&](std::shared_ptr<Transform> t, void *userData){
                    if (include_root || t != transform)
                    {
                        // writer.String("transform");
                        writer.StartObject(); // start transform

                        writer.String("id");
                        writer.Uint64((uint64_t)(uintptr_t)t->self().get());

                        writer.String("N");
                        writer.String(t->getName().c_str());

                        auto _T = t->getLocalPosition();
                        if (_T != MathCore::vec3f(0.0f))
                        {
                            writer.String("T");
                            SerializerUtil::write(writer, _T);
                        }

                        auto _R = t->getLocalRotation();
                        if (_R != MathCore::quatf())
                        {
                            writer.String("R");
                            SerializerUtil::write(writer, _R);
                        }

                        auto _S = t->getLocalScale();
                        if (_S != MathCore::vec3f(1.0f))
                        {
                            writer.String("S");
                            SerializerUtil::write(writer, _S);
                        }

                        // serialize all components
                        writer.String("co");
                        writer.StartArray();
                        for (auto comp : t->getComponents())
                        {
                            if (component_exported.find((uint64_t)(uintptr_t)comp.get()) == component_exported.end())
                            {
                                component_exported[(uint64_t)(uintptr_t)comp.get()] = true;
                                comp->Serialize(writer);
                            }
                            else
                            {
                                writer.StartObject();
                                writer.String("_co_ref_");
                                writer.Uint64((uint64_t)(uintptr_t)comp.get());
                                writer.EndObject();
                            }
                        }
                        writer.EndArray();

                        writer.String("C");
                        writer.StartArray(); // start children
                    }
                    return true;
                },
                // post order
                [&](std::shared_ptr<Transform> t, void *userData){
                    if (include_root || t != transform)
                    {
                        writer.EndArray();  // end children
                        writer.EndObject(); // end transform
                    }
                    return true;
                }
            );

            // struct itemT
            // {
            //     std::shared_ptr<Transform> transform;
            //     int child_idx;
            // };
            // std::vector<itemT> stack;

            // itemT root = {transform, 0};

            // while (root.transform != nullptr || stack.size() > 0)
            // {
            //     // walk down the max possible nodes on left side
            //     // after that, the stack will hold the parent -> child relationship only
            //     // between each level.
            //     // Each place on stack is a parent element.
            //     while (root.transform != nullptr)
            //     {
            //         // pre order traversing on root.transform
            //         if (include_root || root.transform != transform)
            //         {
            //             // writer.String("transform");
            //             writer.StartObject(); // start transform

            //             writer.String("id");
            //             writer.Uint64((uint64_t)(uintptr_t)root.transform->self().get());

            //             writer.String("N");
            //             writer.String(root.transform->getName().c_str());

            //             auto _T = root.transform->getLocalPosition();
            //             if (_T != MathCore::vec3f(0.0f))
            //             {
            //                 writer.String("T");
            //                 SerializerUtil::write(writer, _T);
            //             }

            //             auto _R = root.transform->getLocalRotation();
            //             if (_R != MathCore::quatf())
            //             {
            //                 writer.String("R");
            //                 SerializerUtil::write(writer, _R);
            //             }

            //             auto _S = root.transform->getLocalScale();
            //             if (_S != MathCore::vec3f(1.0f))
            //             {
            //                 writer.String("S");
            //                 SerializerUtil::write(writer, _S);
            //             }

            //             // serialize all components
            //             writer.String("co");
            //             writer.StartArray();
            //             for (auto comp : root.transform->getComponents())
            //             {
            //                 if (component_exported.find((uint64_t)(uintptr_t)comp.get()) == component_exported.end())
            //                 {
            //                     component_exported[(uint64_t)(uintptr_t)comp.get()] = true;
            //                     comp->Serialize(writer);
            //                 }
            //                 else
            //                 {
            //                     writer.StartObject();
            //                     writer.String("_co_ref_");
            //                     writer.Uint64((uint64_t)(uintptr_t)comp.get());
            //                     writer.EndObject();
            //                 }
            //             }
            //             writer.EndArray();

            //             writer.String("C");
            //             writer.StartArray(); // start children
            //         }

            //         stack.push_back(root);
            //         if (root.transform->getChildCount() > 0)
            //             root = {root.transform->getChildAt(0), 0};
            //         else
            //             root = {nullptr, 0};
            //     }

            //     // remove the lowest child node value from stack and
            //     // save the current info from it
            //     auto item = stack.back();
            //     stack.pop_back();

            //     // post order traversing on item.transform
            //     if (include_root || item.transform != transform)
            //     {
            //         writer.EndArray();  // end children
            //         writer.EndObject(); // end transform
            //     }

            //     // while the removed element is the lastest children,
            //     // walk to parent, making it the new child
            //     while (stack.size() > 0 &&
            //            // stack.back().transform->children.size() > 0 &&
            //            item.transform == stack.back().transform->getChildren().back())
            //     {
            //         item = stack.back();
            //         stack.pop_back();

            //         // post order traversing on item.transform
            //         if (include_root || item.transform != transform)
            //         {
            //             writer.EndArray();
            //             writer.EndObject();
            //         }
            //     }

            //     // if there is any element in the stack, it means that
            //     // this is a parent with more children to compute
            //     if (stack.size() > 0)
            //     {
            //         int next_child_idx = item.child_idx + 1;
            //         // if (next_child_idx < (int)stack.back().transform->children.size() - 1)
            //         root = {stack.back().transform->getChildAt(next_child_idx), next_child_idx};
            //     }
            // }

            if (!include_root)
                writer.EndArray(); // end children
        }
        Platform::ObjectBuffer JSONSceneSerializer::End(std::shared_ptr<WriterSet> writerSet)
        {
            std::string out_str = writerSet->stringBuffer.GetString();
            if (out_str.length() > 0)
                return Platform::ObjectBuffer((uint8_t *)&out_str[0], out_str.length(), 32, true);
            return Platform::ObjectBuffer();
        }

        bool JSONSceneDeserializer::default_registered = false;
        std::unordered_map<std::string, std::function<std::shared_ptr<Component>(void)>> JSONSceneDeserializer::componentCreator;

        void JSONSceneDeserializer::registerDefaultComponents()
        {
            if (default_registered)
                return;
            default_registered = true;

            AddComponent<Components::ComponentCameraLookToNode>();
            AddComponent<Components::ComponentColorLine>();
            AddComponent<Components::ComponentColorMesh>();

            AddComponent<Components::ComponentAnimationMotion>();
            AddComponent<Components::ComponentCameraMove>();
            AddComponent<Components::ComponentCameraOrthographic>();
            AddComponent<Components::ComponentCameraPerspective>();
            AddComponent<Components::ComponentCameraRotateOnTarget>();
            AddComponent<Components::ComponentFontToMesh>();
            AddComponent<Components::ComponentFrustumCulling>();
            AddComponent<Components::ComponentLight>();
            AddComponent<Components::ComponentMaterial>();
            AddComponent<Components::ComponentMesh>();
            AddComponent<Components::ComponentMeshWrapper>();
            AddComponent<Components::ComponentParticleSystem>();
            AddComponent<Components::ComponentSkinnedMesh>();
            AddComponent<Components::ComponentThirdPersonCamera>();
            AddComponent<Components::ComponentThirdPersonPlayerController>();
        }

        std::shared_ptr<ReaderSet> JSONSceneDeserializer::Begin(Platform::ObjectBuffer *src)
        {
            auto result = std::make_shared<ReaderSet>();

            if (src->size > 0 && src->data[src->size - 1] != 0x00)
            {
                result->objectBuffer.setSize(src->size + 1);
                memcpy(result->objectBuffer.data, src->data, src->size * sizeof(uint8_t));
                result->objectBuffer.data[src->size] = 0; // add \0 at end
            }
            else
                result->objectBuffer = src->clone();

            result->document.ParseInsitu((char *)result->objectBuffer.data);

            if (result->document.HasParseError())
                return nullptr;

            return result;
        }

        void JSONSceneDeserializer::Deserialize(rapidjson::Value &_value,
                                                bool include_root,
                                                std::shared_ptr<Transform> target_root,
                                                ResourceSet &resourceSet)
        {

            registerDefaultComponents();

            struct itemT
            {
                std::shared_ptr<Transform> parent;
                std::shared_ptr<Transform> to_set;
                rapidjson::Value &value;
            };

            std::vector<itemT> to_traverse;
            std::unordered_map<uint64_t, std::shared_ptr<Transform>> transform_map;
            std::unordered_map<uint64_t, std::shared_ptr<Component>> component_map;

            //
            // Create all structure on memory (Transform Hierarchy + Components)
            //

            if (!include_root)
            {
                // push array
                if (_value.IsArray())
                {
                    for (int i = (int)_value.Size() - 1; i >= 0; i--)
                        to_traverse.push_back({target_root, nullptr, _value[i]});
                }
            }
            else // push root
                to_traverse.push_back({nullptr, target_root, _value});

            while (to_traverse.size() > 0)
            {
                auto front = to_traverse.back();
                to_traverse.pop_back();

                // pre order depth first processing
                {
                    if (front.parent != nullptr)
                    {
                        front.to_set = Transform::CreateShared();
                        front.parent->addChild(front.to_set);
                    }
                }

                if (front.value.IsObject())
                {

                    if (front.value.HasMember("id"))
                    {
                        auto &id = front.value["id"];
                        if (id.IsUint64())
                            transform_map[id.GetUint64()] = front.to_set->self();
                    }

                    if (front.value.HasMember("N"))
                    {
                        auto &name = front.value["N"];
                        if (name.IsString())
                            front.to_set->setName(name.GetString());
                    }

                    if (front.value.HasMember("T"))
                        front.to_set->setLocalPosition(SerializerUtil::read<MathCore::vec3f>(front.value["T"]));
                    else
                        front.to_set->setLocalPosition(MathCore::vec3f(0.0f));
                    if (front.value.HasMember("R"))
                        front.to_set->setLocalRotation(SerializerUtil::read<MathCore::quatf>(front.value["R"]));
                    else
                        front.to_set->setLocalRotation(MathCore::quatf());
                    if (front.value.HasMember("S"))
                        front.to_set->setLocalScale(SerializerUtil::read<MathCore::vec3f>(front.value["S"]));
                    else
                        front.to_set->setLocalScale(MathCore::vec3f(1.0f));

                    // deserialize all components
                    if (front.value.HasMember("co"))
                    {
                        auto &components = front.value["co"];
                        if (components.IsArray())
                        {
                            for (int i = 0; i < (int)components.Size(); i++)
                            {
                                rapidjson::Value &comp = components[i];

                                if (!comp.IsObject())
                                    continue;

                                if (comp.HasMember("_co_ref_") && comp["_co_ref_"].IsUint64())
                                {
                                    // a reference to a component already created
                                    auto _ref_comp = component_map.find(comp["_co_ref_"].GetUint64());
                                    ITK_ABORT(_ref_comp == component_map.end(), "component ref in the scene file not found.");
                                    front.to_set->addComponent(_ref_comp->second);
                                    continue;
                                }

                                if (!comp.HasMember("type") || !comp["type"].IsString() ||
                                    !comp.HasMember("id") || !comp["id"].IsUint64())
                                    continue;
                                auto it = componentCreator.find(comp["type"].GetString());
                                if (it == componentCreator.end())
                                    continue;
                                auto &fnc_create_shared = it->second;
                                auto new_component_created = fnc_create_shared();
                                component_map[comp["id"].GetUint64()] = new_component_created;

                                front.to_set->addComponent(new_component_created);
                            }
                        }
                    }

                    if (front.value.HasMember("C"))
                    {
                        auto &children = front.value["C"];
                        if (children.IsArray())
                        {
                            for (int i = (int)children.Size() - 1; i >= 0; i--)
                                to_traverse.push_back({front.to_set, nullptr, children[i]});
                        }
                    }
                }
            }

            //
            // Deserialize only the components
            //
            if (!include_root)
            {
                // push array
                if (_value.IsArray())
                {
                    for (int i = (int)_value.Size() - 1; i >= 0; i--)
                        to_traverse.push_back({target_root, nullptr, _value[i]});
                }
            }
            else // push root
                to_traverse.push_back({nullptr, target_root, _value});

            while (to_traverse.size() > 0)
            {
                auto front = to_traverse.back();
                to_traverse.pop_back();

                if (front.value.IsObject())
                {

                    // deserialize all components
                    if (front.value.HasMember("co"))
                    {
                        auto &components = front.value["co"];
                        if (components.IsArray())
                        {
                            for (int i = 0; i < (int)components.Size(); i++)
                            {
                                rapidjson::Value &comp = components[i];
                                if (!comp.IsObject() || !comp.HasMember("type") || !comp["type"].IsString() ||
                                    !comp.HasMember("id") || !comp["id"].IsUint64())
                                    continue;
                                auto it = component_map.find(comp["id"].GetUint64());
                                if (it == component_map.end())
                                    continue;
                                it->second->Deserialize(comp, transform_map, component_map, resourceSet);
                            }
                        }
                    }

                    if (front.value.HasMember("C"))
                    {
                        auto &children = front.value["C"];
                        if (children.IsArray())
                        {
                            for (int i = (int)children.Size() - 1; i >= 0; i--)
                                to_traverse.push_back({front.to_set, nullptr, children[i]});
                        }
                    }
                }
            }
        }

    }
}