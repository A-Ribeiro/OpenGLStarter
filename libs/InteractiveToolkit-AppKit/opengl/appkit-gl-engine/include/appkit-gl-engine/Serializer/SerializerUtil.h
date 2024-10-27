#pragma once

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <InteractiveToolkit/MathCore/MathCore.h>

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
                    result[i] = MathCore::CVT<float_type>::toFloat(element.GetDouble());
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
                        result(r,c) = MathCore::CVT<float_type>::toFloat(element.GetDouble());
                    }
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
        }

    }
}
