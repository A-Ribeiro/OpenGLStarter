#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/common.h>

#include <appkit-gl-base/VirtualTexture.h>

// Check for C++17 support using standard macros
#if false && (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
#include <variant>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Utils
        {
            // using std::variant
            class ShaderProperty
            {
            public:
                // Define os tipos que a property pode armazenar
                using VariantType = std::variant<
                    int, float,
                    MathCore::vec2f, MathCore::vec3f, MathCore::vec4f,
                    MathCore::mat4f>;

                ShaderProperty() = default;

                // Construtor template para aceitar qualquer tipo suportado
                template <typename T>
                ShaderProperty(const T &value) : value_(value) {}

                // Getter template para obter o valor com tipo específico
                template <typename T>
                const T &get() const
                {
                    return std::get<T>(value_);
                }

                template <typename T>
                T &get()
                {
                    return std::get<T>(value_);
                }

                // Setter template para definir o valor
                template <typename T>
                void set(const T &value)
                {
                    value_ = value;
                }

                // Verifica se a property contém um tipo específico
                template <typename T>
                bool holds() const
                {
                    return std::holds_alternative<T>(value_);
                }

                std::string toString() const;

            private:
                VariantType value_;
            };
        }
    }
}
#else
// C++11 compatible version without std::variant

namespace AppKit
{
    namespace GLEngine
    {
        namespace Utils
        {

            class ShaderProperty
            {
            public:
                enum PropertyType
                {
                    TYPE_INT,
                    TYPE_FLOAT,
                    TYPE_VEC2F,
                    TYPE_VEC3F,
                    TYPE_VEC4F,
                    TYPE_MAT4F,
                    
                    TYPE_VTEX,
                };

            private:
                PropertyType type_;
                union
                {
                    int int_value;
                    float float_value;
                    MathCore::vec2f vec2f_value;
                    MathCore::vec3f vec3f_value;
                    MathCore::vec4f vec4f_value;
                    MathCore::mat4f mat4f_value;
                };

                std::shared_ptr<OpenGL::VirtualTexture> virtual_texture_value;

                inline void clearSharedPtr()
                {
                    if (type_ == TYPE_VTEX)
                        virtual_texture_value.reset();
                }

            public:
                ShaderProperty();
                ShaderProperty(int value);
                ShaderProperty(float value);
                ShaderProperty(const MathCore::vec2f &value);
                ShaderProperty(const MathCore::vec3f &value);
                ShaderProperty(const MathCore::vec4f &value);
                ShaderProperty(const MathCore::mat4f &value);
                
                ShaderProperty(std::shared_ptr<OpenGL::VirtualTexture> value);

                // Copy constructor
                ShaderProperty(const ShaderProperty &other);

                // Assignment operator
                ShaderProperty &operator=(const ShaderProperty &other);

                template <typename T>
                void set(const T &v);

                // Getter methods
                template <typename T>
                const T &get() const;

                template <typename T>
                T &get();

                // Verificar tipo
                PropertyType getType() const { return type_; }

                template <typename T>
                bool holds() const;

                std::string toString() const;
            };


            template <>
            inline void ShaderProperty::set<int>(const int &v)
            {
                clearSharedPtr();
                type_ = TYPE_INT;
                int_value = v;
            }

            template <>
            inline void ShaderProperty::set<float>(const float &v)
            {
                clearSharedPtr();
                type_ = TYPE_FLOAT;
                float_value = v;
            }

            template <>
            inline void ShaderProperty::set<MathCore::vec2f>(const MathCore::vec2f &v)
            {
                clearSharedPtr();
                type_ = TYPE_VEC2F;
                vec2f_value = v;
            }

            template <>
            inline void ShaderProperty::set<MathCore::vec3f>(const MathCore::vec3f &v)
            {
                clearSharedPtr();
                type_ = TYPE_VEC3F;
                vec3f_value = v;
            }

            template <>
            inline void ShaderProperty::set<MathCore::vec4f>(const MathCore::vec4f &v)
            {
                clearSharedPtr();
                type_ = TYPE_VEC4F;
                vec4f_value = v;
            }

            template <>
            inline void ShaderProperty::set<MathCore::mat4f>(const MathCore::mat4f &v)
            {
                clearSharedPtr();
                type_ = TYPE_MAT4F;
                mat4f_value = v;
            }

            template <>
            inline void ShaderProperty::set<std::shared_ptr<OpenGL::VirtualTexture>>(const std::shared_ptr<OpenGL::VirtualTexture> &v)
            {
                type_ = TYPE_VTEX;
                virtual_texture_value = v;
            }

            // Template specializations for get() method
            template <>
            inline const int &ShaderProperty::get<int>() const
            {
                if (type_ != TYPE_INT)
                    throw std::runtime_error("Property type mismatch");
                return int_value;
            }

            template <>
            inline int &ShaderProperty::get<int>()
            {
                if (type_ != TYPE_INT)
                    throw std::runtime_error("Property type mismatch");
                return int_value;
            }

            template <>
            inline const float &ShaderProperty::get<float>() const
            {
                if (type_ != TYPE_FLOAT)
                    throw std::runtime_error("Property type mismatch");
                return float_value;
            }

            template <>
            inline float &ShaderProperty::get<float>()
            {
                if (type_ != TYPE_FLOAT)
                    throw std::runtime_error("Property type mismatch");
                return float_value;
            }

            template <>
            inline const MathCore::vec2f &ShaderProperty::get<MathCore::vec2f>() const
            {
                if (type_ != TYPE_VEC2F)
                    throw std::runtime_error("Property type mismatch");
                return vec2f_value;
            }

            template <>
            inline MathCore::vec2f &ShaderProperty::get<MathCore::vec2f>()
            {
                if (type_ != TYPE_VEC2F)
                    throw std::runtime_error("Property type mismatch");
                return vec2f_value;
            }

            template <>
            inline const MathCore::vec3f &ShaderProperty::get<MathCore::vec3f>() const
            {
                if (type_ != TYPE_VEC3F)
                    throw std::runtime_error("Property type mismatch");
                return vec3f_value;
            }

            template <>
            inline MathCore::vec3f &ShaderProperty::get<MathCore::vec3f>()
            {
                if (type_ != TYPE_VEC3F)
                    throw std::runtime_error("Property type mismatch");
                return vec3f_value;
            }

            template <>
            inline const MathCore::vec4f &ShaderProperty::get<MathCore::vec4f>() const
            {
                if (type_ != TYPE_VEC4F)
                    throw std::runtime_error("Property type mismatch");
                return vec4f_value;
            }

            template <>
            inline MathCore::vec4f &ShaderProperty::get<MathCore::vec4f>()
            {
                if (type_ != TYPE_VEC4F)
                    throw std::runtime_error("Property type mismatch");
                return vec4f_value;
            }

            template <>
            inline const MathCore::mat4f &ShaderProperty::get<MathCore::mat4f>() const
            {
                if (type_ != TYPE_MAT4F)
                    throw std::runtime_error("Property type mismatch");
                return mat4f_value;
            }

            template <>
            inline MathCore::mat4f &ShaderProperty::get<MathCore::mat4f>()
            {
                if (type_ != TYPE_MAT4F)
                    throw std::runtime_error("Property type mismatch");
                return mat4f_value;
            }

            template <>
            inline const std::shared_ptr<OpenGL::VirtualTexture> &ShaderProperty::get<std::shared_ptr<OpenGL::VirtualTexture>>() const
            {
                if (type_ != TYPE_VTEX)
                    throw std::runtime_error("Property type mismatch");
                return virtual_texture_value;
            }

            template <>
            inline std::shared_ptr<OpenGL::VirtualTexture> &ShaderProperty::get<std::shared_ptr<OpenGL::VirtualTexture>>()
            {
                if (type_ != TYPE_VTEX)
                    throw std::runtime_error("Property type mismatch");
                return virtual_texture_value;
            }

            // Template specializations for holds() method
            template <>
            inline bool ShaderProperty::holds<int>() const
            {
                return type_ == TYPE_INT;
            }

            template <>
            inline bool ShaderProperty::holds<float>() const
            {
                return type_ == TYPE_FLOAT;
            }

            template <>
            inline bool ShaderProperty::holds<MathCore::vec2f>() const
            {
                return type_ == TYPE_VEC2F;
            }

            template <>
            inline bool ShaderProperty::holds<MathCore::vec3f>() const
            {
                return type_ == TYPE_VEC3F;
            }

            template <>
            inline bool ShaderProperty::holds<MathCore::vec4f>() const
            {
                return type_ == TYPE_VEC4F;
            }

            template <>
            inline bool ShaderProperty::holds<MathCore::mat4f>() const
            {
                return type_ == TYPE_MAT4F;
            }

            template <>
            inline bool ShaderProperty::holds<std::shared_ptr<OpenGL::VirtualTexture>>() const
            {
                return type_ == TYPE_VTEX;
            }
        }
    }
}
#endif

namespace AppKit
{
    namespace GLEngine
    {
        namespace Utils
        {
            class ShaderPropertyBag
            {
            private:
                std::unordered_map<std::string, ShaderProperty> properties_;

            public:
                void addProperty(const std::string &key, const ShaderProperty &property);
                ShaderProperty &getProperty(const std::string &key);
                const ShaderProperty &getProperty(const std::string &key) const;

                template <typename T>
                T getProperty(const std::string &key) const
                {
                    return getProperty(key).get<T>();
                }

                bool hasProperty(const std::string &key) const;
                void removeProperty(const std::string &key);
                void clear();
            };

        }
    }
}

