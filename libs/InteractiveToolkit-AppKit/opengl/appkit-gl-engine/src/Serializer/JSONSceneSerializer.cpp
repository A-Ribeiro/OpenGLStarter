#include <appkit-gl-engine/Serializer/JSONSceneSerializer.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <string>
#include <InteractiveToolkit/ITKCommon/STL_Tools.h>

namespace AppKit
{
    namespace GLEngine
    {

        using RapidJsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;

        template <typename _math_type,
                  typename std::enable_if<
                      MathCore::MathTypeInfo<_math_type>::_is_valid::value &&
                          MathCore::MathTypeInfo<_math_type>::_is_vec::value &&
                          std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value
                          ,
                      bool>::type = true>
        static ITK_INLINE _math_type read(rapidjson::Value &reader)
        {
            if (!reader.IsArray())
                return _math_type();
            if ((int)reader.Size() != (int)_math_type::array_count)
                return _math_type();
                
            using float_type = typename MathCore::MathTypeInfo<_math_type>::_type;
            _math_type result;
            for (int i = 0; i < (int)_math_type::array_count; i++)
            {
                auto &element = reader[i];
                if (!element.IsDouble())
                    continue;
                result[i] = MathCore::CVT<float_type>::toFloat(element.GetDouble());
            }
            return result;
        }

        template <typename _math_type,
                  typename std::enable_if<
                      MathCore::MathTypeInfo<_math_type>::_is_valid::value &&
                          MathCore::MathTypeInfo<_math_type>::_is_vec::value &&
                          std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value
                          ,
                      bool>::type = true>
        static ITK_INLINE void write(RapidJsonWriter &writer, const _math_type &v)
        {
            using float_type = typename MathCore::MathTypeInfo<_math_type>::_type;
            writer.StartArray();
            for (int i = 0; i < (int)_math_type::array_count; i++)
                writer.Double(MathCore::CVT<float_type>::toDouble(v[i]));
            writer.EndArray();
        }

        template <typename _math_type,
                  typename std::enable_if<
                      MathCore::MathTypeInfo<_math_type>::_is_valid::value &&
                          !MathCore::MathTypeInfo<_math_type>::_is_vec::value &&
                          std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value,
                      bool>::type = true>
        static ITK_INLINE void write(RapidJsonWriter &writer, const _math_type &v)
        {
            using float_type = typename MathCore::MathTypeInfo<_math_type>::_type;
            writer.StartArray();
            for (int c = 0; c < (int)_math_type::cols; c++)
            {
                writer.StartArray();
                for (int r = 0; r < (int)_math_type::rows; r++)
                    writer.Double(MathCore::CVT<float_type>::toDouble(v(r, c)));
                writer.EndArray();
            }
            writer.EndArray();
        }

        std::shared_ptr<WriterSet> JSONSceneSerializer::Begin(){
            return std::make_shared<WriterSet>();
        }
        void JSONSceneSerializer::Serialize(RapidJsonWriter &writer, std::shared_ptr<Transform> transform, bool include_root){

            if (!include_root)
                writer.StartArray(); // start children

            struct itemT
            {
                std::shared_ptr<Transform> transform;
                int child_idx;
            };
            std::vector<itemT> stack;

            itemT root = {transform, 0};

            while (root.transform != nullptr || stack.size() > 0)
            {
                // walk down the max possible nodes on left side
                // after that, the stack will hold the parent -> child relationship only
                // between each level.
                // Each place on stack is a parent element.
                while (root.transform != nullptr)
                {
                    // pre order traversing on root.transform
                    if (include_root || root.transform != transform)
                    {
                        // writer.String("transform");
                        writer.StartObject(); // start transform

                        writer.String("N");
                        writer.String(root.transform->getName().c_str());

                        writer.String("T");
                        write(writer, root.transform->getLocalPosition());
                        writer.String("R");
                        write(writer, root.transform->getLocalRotation());
                        writer.String("S");
                        write(writer, root.transform->getLocalScale());

                        writer.String("C");
                        writer.StartArray(); // start children
                    }

                    stack.push_back(root);
                    if (root.transform->getChildCount() > 0)
                        root = {root.transform->getChildAt(0), 0};
                    else
                        root = {nullptr, 0};
                }

                // remove the lowest child node value from stack and
                // save the current info from it
                auto item = stack.back();
                stack.pop_back();

                // post order traversing on item.transform
                if (include_root || item.transform != transform)
                {
                    writer.EndArray();  // end children
                    writer.EndObject(); // end transform
                }

                // while the removed element is the lastest children,
                // walk to parent, making it the new child
                while (stack.size() > 0 &&
                       // stack.back().transform->children.size() > 0 &&
                       item.transform == stack.back().transform->getChildren().back())
                {
                    item = stack.back();
                    stack.pop_back();

                    // post order traversing on item.transform
                    if (include_root || item.transform != transform)
                    {
                        writer.EndArray();
                        writer.EndObject();
                    }
                }

                // if there is any element in the stack, it means that
                // this is a parent with more children to compute
                if (stack.size() > 0)
                {
                    int next_child_idx = item.child_idx + 1;
                    // if (next_child_idx < (int)stack.back().transform->children.size() - 1)
                    root = {stack.back().transform->getChildAt(next_child_idx), next_child_idx};
                }
            }

            if (!include_root)
                writer.EndArray(); // end children
        }
        Platform::ObjectBuffer JSONSceneSerializer::End(std::shared_ptr<WriterSet> writerSet){
            std::string out_str = writerSet->stringBuffer.GetString();
            if (out_str.length() > 0)
                return Platform::ObjectBuffer((uint8_t *)&out_str[0], out_str.length(), 32, true);
            return Platform::ObjectBuffer();
        }

        std::shared_ptr<ReaderSet> JSONSceneDeserializer::Begin(Platform::ObjectBuffer *src) {
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

        void JSONSceneDeserializer::Deserialize(rapidjson::Value &_value, bool include_root, std::shared_ptr<Transform> target_root){

            struct itemT
            {
                std::shared_ptr<Transform> parent;
                std::shared_ptr<Transform> to_set;
                rapidjson::Value &value;
            };

            std::vector<itemT> to_traverse;

            if (!include_root){
                // push array
                if (_value.IsArray()){
                    for (int i = (int)_value.Size()-1; i >= 0; i--)  {
                        // auto new_transform = Transform::CreateShared();
                        // target_root->addChild(new_transform);
                        to_traverse.push_back({target_root, nullptr, _value[i]});
                    }
                }
            }
            else // push root
                to_traverse.push_back({nullptr,target_root, _value});

            while (to_traverse.size() > 0) {
                auto front = to_traverse.back();
                to_traverse.pop_back();
                
                // pre order depth first processing
                {
                    if (front.parent != nullptr){
                        front.to_set = Transform::CreateShared();
                        front.parent->addChild(front.to_set);
                    }
                }

                if (front.value.IsObject()){

                    auto &name = front.value["N"];
                    if (name.IsString())
                        front.to_set->setName(name.GetString());

                    front.to_set->setLocalPosition( read<MathCore::vec3f>(front.value["T"]) );
                    front.to_set->setLocalRotation( read<MathCore::quatf>(front.value["R"]) );
                    front.to_set->setLocalScale( read<MathCore::vec3f>(front.value["S"]) );
                    
                    auto &children = front.value["C"];
                    if (children.IsArray()){
                        for (int i = (int)children.Size()-1; i >= 0; i--)  {
                            // auto new_transform = Transform::CreateShared();
                            //front.to_set->addChild(new_transform);
                            to_traverse.push_back({front.to_set, nullptr, children[i]});
                        }
                    }

                }
            }

        }


    }
}