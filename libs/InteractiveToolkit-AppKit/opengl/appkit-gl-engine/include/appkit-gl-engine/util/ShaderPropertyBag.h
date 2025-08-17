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
        class Component;
        class Transform;
        namespace Utils
        {

            class ShaderProperty
            {
            public:
                enum PropertyType
                {
                    TYPE_BOOL,
                    TYPE_INT,
                    TYPE_FLOAT,
                    TYPE_VEC2F,
                    TYPE_VEC3F,
                    TYPE_VEC4F,
                    TYPE_MAT4F,

                    TYPE_VTEX,

                    TYPE_WEAK_COMPONENT,
                    TYPE_WEAK_TRANSFORM,
                };

            private:
                PropertyType type_;
                union
                {
                    bool bool_value;
                    int int_value;
                    float float_value;
                    MathCore::vec2f vec2f_value;
                    MathCore::vec3f vec3f_value;
                    MathCore::vec4f vec4f_value;
                    MathCore::mat4f mat4f_value;
                };

                std::shared_ptr<OpenGL::VirtualTexture> virtual_texture_value;

                std::weak_ptr<Component> component_value;
                std::weak_ptr<Transform> transform_value;

                inline void clearSharedPtr(PropertyType new_type)
                {
                    if (type_ == TYPE_VTEX && new_type != TYPE_VTEX)
                        virtual_texture_value.reset();
                    else if (type_ == TYPE_WEAK_COMPONENT && new_type != TYPE_WEAK_COMPONENT)
                        component_value.reset();
                    else if (type_ == TYPE_WEAK_TRANSFORM && new_type != TYPE_WEAK_TRANSFORM)
                        transform_value.reset();
                }

            public:
                ShaderProperty();
                ShaderProperty(bool value);
                ShaderProperty(int value);
                ShaderProperty(float value);
                ShaderProperty(const MathCore::vec2f &value);
                ShaderProperty(const MathCore::vec3f &value);
                ShaderProperty(const MathCore::vec4f &value);
                ShaderProperty(const MathCore::mat4f &value);

                ShaderProperty(std::shared_ptr<OpenGL::VirtualTexture> value);
                ShaderProperty(std::weak_ptr<Component> value);
                ShaderProperty(std::weak_ptr<Transform> value);

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
            inline void ShaderProperty::set<bool>(const bool &v)
            {
                clearSharedPtr(TYPE_BOOL);
                type_ = TYPE_BOOL;
                bool_value = v;
            }

            template <>
            inline void ShaderProperty::set<int>(const int &v)
            {
                clearSharedPtr(TYPE_INT);
                type_ = TYPE_INT;
                int_value = v;
            }

            template <>
            inline void ShaderProperty::set<float>(const float &v)
            {
                clearSharedPtr(TYPE_FLOAT);
                type_ = TYPE_FLOAT;
                float_value = v;
            }

            template <>
            inline void ShaderProperty::set<MathCore::vec2f>(const MathCore::vec2f &v)
            {
                clearSharedPtr(TYPE_VEC2F);
                type_ = TYPE_VEC2F;
                vec2f_value = v;
            }

            template <>
            inline void ShaderProperty::set<MathCore::vec3f>(const MathCore::vec3f &v)
            {
                clearSharedPtr(TYPE_VEC3F);
                type_ = TYPE_VEC3F;
                vec3f_value = v;
            }

            template <>
            inline void ShaderProperty::set<MathCore::vec4f>(const MathCore::vec4f &v)
            {
                clearSharedPtr(TYPE_VEC4F);
                type_ = TYPE_VEC4F;
                vec4f_value = v;
            }

            template <>
            inline void ShaderProperty::set<MathCore::mat4f>(const MathCore::mat4f &v)
            {
                clearSharedPtr(TYPE_MAT4F);
                type_ = TYPE_MAT4F;
                mat4f_value = v;
            }

            template <>
            inline void ShaderProperty::set<std::shared_ptr<OpenGL::VirtualTexture>>(const std::shared_ptr<OpenGL::VirtualTexture> &v)
            {
                clearSharedPtr(TYPE_VTEX);
                type_ = TYPE_VTEX;
                virtual_texture_value = v;
            }

            template <>
            inline void ShaderProperty::set<std::weak_ptr<Component>>(const std::weak_ptr<Component> &v)
            {
                clearSharedPtr(TYPE_WEAK_COMPONENT);
                type_ = TYPE_WEAK_COMPONENT;
                component_value = v;
            }

            template <>
            inline void ShaderProperty::set<std::weak_ptr<Transform>>(const std::weak_ptr<Transform> &v)
            {
                clearSharedPtr(TYPE_WEAK_TRANSFORM);
                type_ = TYPE_WEAK_TRANSFORM;
                transform_value = v;
            }

            // Template specializations for get() method
            template <>
            inline const bool &ShaderProperty::get<bool>() const
            {
                ITK_ABORT(type_ != TYPE_BOOL, "Property type mismatch");
                return bool_value;
            }

            template <>
            inline bool &ShaderProperty::get<bool>()
            {
                ITK_ABORT(type_ != TYPE_BOOL, "Property type mismatch");
                return bool_value;
            }
            template <>
            inline const int &ShaderProperty::get<int>() const
            {
                ITK_ABORT(type_ != TYPE_INT, "Property type mismatch");
                return int_value;
            }

            template <>
            inline int &ShaderProperty::get<int>()
            {
                ITK_ABORT(type_ != TYPE_INT, "Property type mismatch");
                return int_value;
            }

            template <>
            inline const float &ShaderProperty::get<float>() const
            {
                ITK_ABORT(type_ != TYPE_FLOAT, "Property type mismatch");
                return float_value;
            }

            template <>
            inline float &ShaderProperty::get<float>()
            {
                ITK_ABORT(type_ != TYPE_FLOAT, "Property type mismatch");
                return float_value;
            }

            template <>
            inline const MathCore::vec2f &ShaderProperty::get<MathCore::vec2f>() const
            {
                ITK_ABORT(type_ != TYPE_VEC2F, "Property type mismatch");
                return vec2f_value;
            }

            template <>
            inline MathCore::vec2f &ShaderProperty::get<MathCore::vec2f>()
            {
                ITK_ABORT(type_ != TYPE_VEC2F, "Property type mismatch");
                return vec2f_value;
            }

            template <>
            inline const MathCore::vec3f &ShaderProperty::get<MathCore::vec3f>() const
            {
                ITK_ABORT(type_ != TYPE_VEC3F, "Property type mismatch");
                return vec3f_value;
            }

            template <>
            inline MathCore::vec3f &ShaderProperty::get<MathCore::vec3f>()
            {
                ITK_ABORT(type_ != TYPE_VEC3F, "Property type mismatch");
                return vec3f_value;
            }

            template <>
            inline const MathCore::vec4f &ShaderProperty::get<MathCore::vec4f>() const
            {
                ITK_ABORT(type_ != TYPE_VEC4F, "Property type mismatch");
                return vec4f_value;
            }

            template <>
            inline MathCore::vec4f &ShaderProperty::get<MathCore::vec4f>()
            {
                ITK_ABORT(type_ != TYPE_VEC4F, "Property type mismatch");
                return vec4f_value;
            }

            template <>
            inline const MathCore::mat4f &ShaderProperty::get<MathCore::mat4f>() const
            {
                ITK_ABORT(type_ != TYPE_MAT4F, "Property type mismatch");
                return mat4f_value;
            }

            template <>
            inline MathCore::mat4f &ShaderProperty::get<MathCore::mat4f>()
            {
                ITK_ABORT(type_ != TYPE_MAT4F, "Property type mismatch");
                return mat4f_value;
            }

            template <>
            inline const std::shared_ptr<OpenGL::VirtualTexture> &ShaderProperty::get<std::shared_ptr<OpenGL::VirtualTexture>>() const
            {
                ITK_ABORT(type_ != TYPE_VTEX, "Property type mismatch");
                return virtual_texture_value;
            }

            template <>
            inline std::shared_ptr<OpenGL::VirtualTexture> &ShaderProperty::get<std::shared_ptr<OpenGL::VirtualTexture>>()
            {
                ITK_ABORT(type_ != TYPE_VTEX, "Property type mismatch");
                return virtual_texture_value;
            }

            template <>
            inline const std::weak_ptr<Component> &ShaderProperty::get<std::weak_ptr<Component>>() const
            {
                ITK_ABORT(type_ != TYPE_WEAK_COMPONENT, "Property type mismatch");
                return component_value;
            }

            template <>
            inline std::weak_ptr<Component> &ShaderProperty::get<std::weak_ptr<Component>>()
            {
                ITK_ABORT(type_ != TYPE_WEAK_COMPONENT, "Property type mismatch");
                return component_value;
            }

            template <>
            inline const std::weak_ptr<Transform> &ShaderProperty::get<std::weak_ptr<Transform>>() const
            {
                ITK_ABORT(type_ != TYPE_WEAK_TRANSFORM, "Property type mismatch");
                return transform_value;
            }

            template <>
            inline std::weak_ptr<Transform> &ShaderProperty::get<std::weak_ptr<Transform>>()
            {
                ITK_ABORT(type_ != TYPE_WEAK_TRANSFORM, "Property type mismatch");
                return transform_value;
            }

            // Template specializations for holds() method
            template <>
            inline bool ShaderProperty::holds<bool>() const
            {
                return type_ == TYPE_BOOL;
            }
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

            template <>
            inline bool ShaderProperty::holds<std::weak_ptr<Component>>() const
            {
                return type_ == TYPE_WEAK_COMPONENT;
            }

            template <>
            inline bool ShaderProperty::holds<std::weak_ptr<Transform>>() const
            {
                return type_ == TYPE_WEAK_TRANSFORM;
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
                const T &getProperty(const std::string &key) const
                {
                    return getProperty(key).get<T>();
                }

                template <typename T>
                T &getProperty(const std::string &key)
                {
                    return getProperty(key).get<T>();
                }

                bool hasProperty(const std::string &key) const;
                void removeProperty(const std::string &key);
                void clear();

                std::unordered_map<std::string, ShaderProperty> &getProperties();
                const std::unordered_map<std::string, ShaderProperty> &getProperties() const;
            };

        }
    }
}
