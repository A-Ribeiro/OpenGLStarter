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
        }
        Platform::ObjectBuffer JSONSceneSerializer::End(std::shared_ptr<WriterSet> writerSet){
            std::string out_str = writerSet->stringBuffer.GetString();
            if (out_str.length() > 0)
                return Platform::ObjectBuffer((uint8_t *)&out_str[0], out_str.length(), 32, true);
            return Platform::ObjectBuffer();
        }



        Platform::ObjectBuffer JSONSceneSerializer::serialize(std::shared_ptr<Transform> transform, bool include_root)
        {
            auto writerSet = JSONSceneSerializer::Begin();
            JSONSceneSerializer::Serialize(writerSet->writer, transform, include_root);
            return JSONSceneSerializer::End(writerSet);
        }

        std::shared_ptr<Transform> JSONSceneSerializer::deserialize(const Platform::ObjectBuffer &src)
        {
            rapidjson::Document document;

            std::shared_ptr<Transform> result = Transform::CreateShared();

            Platform::ObjectBuffer buffer;
            if (src.size > 0 && src.data[src.size - 1] != 0x00)
            {
                buffer.setSize(src.size + 1);
                memcpy(buffer.data, src.data, src.size * sizeof(uint8_t));
                buffer.data[src.size] = 0; // add \0 at end
                document.ParseInsitu((char *)buffer.data);
            }
            else
                document.ParseInsitu((char *)src.data);

            if (!document.HasParseError())
            {
            }

            return result;
        }

    }
}