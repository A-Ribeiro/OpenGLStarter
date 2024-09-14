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
                          MathCore::MathTypeInfo<_math_type>::_is_vec::value,
                      bool>::type = true>
        static ITK_INLINE void write(RapidJsonWriter &writer, const _math_type &v)
        {
            writer.StartArray();
            for (int i = 0; i < (int)_math_type::array_count; i++)
                writer.Double((double)v[i]);
            writer.EndArray();
        }

        template <typename _math_type,
                  typename std::enable_if<
                      MathCore::MathTypeInfo<_math_type>::_is_valid::value &&
                          !MathCore::MathTypeInfo<_math_type>::_is_vec::value,
                      bool>::type = true>
        static ITK_INLINE void write(RapidJsonWriter &writer, const _math_type &v)
        {
            writer.StartArray();
            for (int c = 0; c < (int)_math_type::cols; c++) {
                writer.StartArray();
                for (int r = 0; r < (int)_math_type::rows; r++)
                    writer.Double((double)v(r, c));
                writer.EndArray();
            }
            writer.EndArray();
        }

        Platform::ObjectBuffer JSONSceneSerializer::serialize(std::shared_ptr<Transform> transform, bool include_root)
        {

            rapidjson::StringBuffer sb;
            RapidJsonWriter writer(sb);
            // writer.SetIndent(' ', 0);
            
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
                    {
                        // writer.String("transform");
                        writer.StartObject(); // start transform

                        writer.String("Name");
                        writer.String(root.transform->getName().c_str());

                        writer.String("Translate");
                        write(writer, root.transform->getLocalPosition());
                        writer.String("Rotate");
                        write(writer, root.transform->getLocalRotation());
                        writer.String("Scale");
                        write(writer, root.transform->getLocalScale());
                        
                        writer.String("Children");
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
                {
                    writer.EndArray(); // end children
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
                    //if (next_child_idx < (int)stack.back().transform->children.size() - 1)
                    root = {stack.back().transform->getChildAt(next_child_idx), next_child_idx};
                }
            }

            std::string out_str = sb.GetString();

            Platform::ObjectBuffer result;
            if (out_str.length() > 0)
                result = Platform::ObjectBuffer((uint8_t *)&out_str[0], out_str.length(), 32, true);// write the '\0'
            
            return result;
        }

        std::shared_ptr<Transform> JSONSceneSerializer::deserialize(const Platform::ObjectBuffer &src)
        {
            rapidjson::Document document;

            std::shared_ptr<Transform> result = Transform::CreateShared();

            Platform::ObjectBuffer buffer;
            if (src.size > 0 && src.data[src.size-1] != 0x00)
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