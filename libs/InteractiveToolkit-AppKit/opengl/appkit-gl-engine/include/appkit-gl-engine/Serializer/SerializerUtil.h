#pragma once

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit-Extension/model/Bone.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace SerializerUtil
        {

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
                    if (std::is_same<float_type, float>::value)
                        result[i] = MathCore::CVT<double>::toFloat(element.GetDouble());
                    else
                        result[i] = element.GetDouble();
                }
                return result;
            }

            template <typename _math_type,
                        typename std::enable_if<
                            MathCore::MathTypeInfo<_math_type>::_is_valid::value &&
                                !MathCore::MathTypeInfo<_math_type>::_is_vec::value &&
                                std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value
                                ,
                            bool>::type = true>
            static ITK_INLINE _math_type read(rapidjson::Value &reader)
            {
                if (!reader.IsArray())
                    return _math_type();
                if ((int)reader.Size() != (int)_math_type::cols)
                    return _math_type();
                    
                using float_type = typename MathCore::MathTypeInfo<_math_type>::_type;
                _math_type result;

                for (int c = 0; c < (int)_math_type::cols; c++)
                {
                    auto &col = reader[c];
                    if (!col.IsArray())
                        return _math_type();
                    if ((int)col.Size() != (int)_math_type::rows)
                        return _math_type();
                    for (int r = 0; r < (int)_math_type::rows; r++)
                    {
                        auto &element = col[r];
                        if (!element.IsDouble())
                            continue;
                        if (std::is_same<float_type, float>::value)
                            result(r,c) = MathCore::CVT<double>::toFloat(element.GetDouble());
                        else
                            result(r,c) = element.GetDouble();
                    }
                }
                return result;
            }

            template <typename _primitive_t,
                        typename std::enable_if<
                            std::is_same<_primitive_t, uint32_t>::value,
                            bool>::type = true>
            static ITK_INLINE _primitive_t read(rapidjson::Value &reader)
            {
                return reader.GetUint();
            }

            template <typename _primitive_t,
                        typename std::enable_if<
                            std::is_same<_primitive_t, ITKExtension::Model::Bone>::value,
                            bool>::type = true>
            static ITK_INLINE _primitive_t read(rapidjson::Value &reader)
            {
                ITKExtension::Model::Bone result;
                
                if (!reader.IsObject())
                    return result;
                
                if (reader.HasMember("name") && reader["name"].IsString())
                    result.name = reader["name"].GetString();

                if (!reader.HasMember("weights") || !reader["weights"].IsArray())
                    return result;

                auto &_weights = reader["weights"];

                result.weights.resize((size_t)_weights.Size());
                for (int i=0;i<(int)_weights.Size();i++){
                    auto &item = _weights[i];
                    if (!item.IsObject())
                        continue;
                    if (!item.HasMember("v") || !item["v"].IsUint())
                        continue;
                    if (!item.HasMember("w") || !item["w"].IsDouble())
                        continue;
                    result.weights[i].vertexID = item["v"].GetUint();
                    result.weights[i].weight = MathCore::CVT<double>::toFloat(item["w"].GetDouble());
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
            static ITK_INLINE void write(rapidjson::Writer<rapidjson::StringBuffer> &writer, const _math_type &v)
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
            static ITK_INLINE void write(rapidjson::Writer<rapidjson::StringBuffer> &writer, const _math_type &v)
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


            template <typename _primitive_t,
                        typename std::enable_if<
                            std::is_same<_primitive_t, uint32_t>::value,
                            bool>::type = true>
            static ITK_INLINE void write(rapidjson::Writer<rapidjson::StringBuffer> &writer, const _primitive_t &v)
            {
                writer.Uint(v);
            }


            template <typename _primitive_t,
                        typename std::enable_if<
                            std::is_same<_primitive_t, ITKExtension::Model::Bone>::value,
                            bool>::type = true>
            static ITK_INLINE void write(rapidjson::Writer<rapidjson::StringBuffer> &writer, const _primitive_t &v)
            {
                writer.StartObject();

                writer.String("name");
                writer.String(v.name.c_str());

                writer.String("weights");
                writer.StartArray();
                for(const auto&item:v.weights){
                    writer.StartObject();

                    writer.String("v");
                    writer.Uint(item.vertexID);

                    writer.String("w");
                    writer.Double(MathCore::CVT<float>::toDouble(item.weight));

                    writer.EndObject();
                }
                writer.EndArray();

                writer.EndObject();
            }




            template <typename T>
            static ITK_INLINE void write_vector(rapidjson::Writer<rapidjson::StringBuffer> &writer, const char* _field , std::vector<T>&_vec)
            {
                if (_vec.size() == 0)
                    return;
                writer.String(_field);
                writer.StartArray();
                for(const auto&item:_vec)
                    SerializerUtil::write(writer, item);
                writer.EndArray();
            }


            template <typename T>
            static ITK_INLINE void read_vector(rapidjson::Value &reader, const char* _field , std::vector<T>&_vec_output)
            {
                _vec_output.clear();
                if (reader.HasMember(_field)){
                    auto &_arr = reader[_field];
                    if (_arr.IsArray()) {
                        _vec_output.resize((size_t)_arr.Size());
                        for (int i = 0; i < (int)_arr.Size(); i++)
                            _vec_output[i] = SerializerUtil::read<T>(_arr[i]);
                    }
                }
            }



        }

    }
}
