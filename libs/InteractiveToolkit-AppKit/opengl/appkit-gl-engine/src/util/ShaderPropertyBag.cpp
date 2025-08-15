#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>

// Check for C++17 support using standard macros
#if false && (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))

namespace AppKit
{
    namespace GLEngine
    {
        namespace Utils
        {
            std::string ShaderProperty::toString() const
            {
                return std::visit([](const auto &v) -> std::string
                                  {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, int>)
                        return ITKCommon::PrintfToStdString("%d", v);
                    else if constexpr (std::is_same_v<T, float>)
                        return ITKCommon::PrintfToStdString("%f", v);
                    else if constexpr (std::is_same_v<T, MathCore::vec2f>)
                        return ITKCommon::PrintfToStdString("(%f, %f)", v.x, v.y);
                    else if constexpr (std::is_same_v<T, MathCore::vec3f>)
                        return ITKCommon::PrintfToStdString("(%f, %f, %f)", v.x, v.y, v.z);
                    else if constexpr (std::is_same_v<T, MathCore::vec4f>)
                        return ITKCommon::PrintfToStdString("(%f, %f, %f, %f)", v.x, v.y, v.z, v.w);
                    else if constexpr (std::is_same_v<T, MathCore::mat4f>)
                        return ITKCommon::PrintfToStdString(
                            "\n"
                            "|%f, %f, %f, %f|\n"
                            "|%f, %f, %f, %f|\n"
                            "|%f, %f, %f, %f|\n"
                            "|%f, %f, %f, %f|\n",
                            v.a1, v.b1, v.c1, v.d1,
                            v.a2, v.b2, v.c2, v.d2,
                            v.a3, v.b3, v.c3, v.d3,
                            v.a4, v.b4, v.c4, v.d4
                        );
                    else
                        return "<type not supported>"; }, value_);
            }
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
            ShaderProperty::ShaderProperty() : type_(TYPE_INT), int_value(0) {}

            ShaderProperty::ShaderProperty(bool value) : type_(TYPE_BOOL), bool_value(value) {}

            ShaderProperty::ShaderProperty(int value) : type_(TYPE_INT), int_value(value) {}

            ShaderProperty::ShaderProperty(float value) : type_(TYPE_FLOAT), float_value(value) {}

            ShaderProperty::ShaderProperty(const MathCore::vec2f &value) : type_(TYPE_VEC2F), vec2f_value(value) {}

            ShaderProperty::ShaderProperty(const MathCore::vec3f &value) : type_(TYPE_VEC3F), vec3f_value(value) {}

            ShaderProperty::ShaderProperty(const MathCore::vec4f &value) : type_(TYPE_VEC4F), vec4f_value(value) {}

            ShaderProperty::ShaderProperty(const MathCore::mat4f &value) : type_(TYPE_MAT4F), mat4f_value(value) {}

            ShaderProperty::ShaderProperty(std::shared_ptr<OpenGL::VirtualTexture> value) : type_(TYPE_VTEX), virtual_texture_value(value) {}

            ShaderProperty::ShaderProperty(std::weak_ptr<AppKit::GLEngine::Component> value) : type_(TYPE_WEAK_COMPONENT), component_value(value) {}

            ShaderProperty::ShaderProperty(std::weak_ptr<AppKit::GLEngine::Transform> value) : type_(TYPE_WEAK_TRANSFORM), transform_value(value) {}

            ShaderProperty::ShaderProperty(const ShaderProperty &other) : type_(other.type_)
            {
                switch (type_)
                {
                case TYPE_BOOL:
                    bool_value = other.bool_value;
                    break;
                case TYPE_INT:
                    int_value = other.int_value;
                    break;
                case TYPE_FLOAT:
                    float_value = other.float_value;
                    break;
                case TYPE_VEC2F:
                    vec2f_value = other.vec2f_value;
                    break;
                case TYPE_VEC3F:
                    vec3f_value = other.vec3f_value;
                    break;
                case TYPE_VEC4F:
                    vec4f_value = other.vec4f_value;
                    break;
                case TYPE_MAT4F:
                    mat4f_value = other.mat4f_value;
                    break;
                case TYPE_VTEX:
                    virtual_texture_value = other.virtual_texture_value;
                    break;
                case TYPE_WEAK_COMPONENT:
                    component_value = other.component_value;
                    break;
                case TYPE_WEAK_TRANSFORM:
                    transform_value = other.transform_value;
                    break;
                }
            }

            ShaderProperty &ShaderProperty::operator=(const ShaderProperty &other)
            {
                if (this != &other)
                {
                    if (type_ == TYPE_VTEX && other.type_ != TYPE_VTEX)
                        virtual_texture_value.reset();
                    else if (type_ == TYPE_WEAK_COMPONENT && other.type_ != TYPE_WEAK_COMPONENT)
                        component_value.reset();
                    else if (type_ == TYPE_WEAK_TRANSFORM && other.type_ != TYPE_WEAK_TRANSFORM)
                        transform_value.reset();

                    type_ = other.type_;
                    switch (type_)
                    {
                    case TYPE_BOOL:
                        bool_value = other.bool_value;
                        break;
                    case TYPE_INT:
                        int_value = other.int_value;
                        break;
                    case TYPE_FLOAT:
                        float_value = other.float_value;
                        break;
                    case TYPE_VEC2F:
                        vec2f_value = other.vec2f_value;
                        break;
                    case TYPE_VEC3F:
                        vec3f_value = other.vec3f_value;
                        break;
                    case TYPE_VEC4F:
                        vec4f_value = other.vec4f_value;
                        break;
                    case TYPE_MAT4F:
                        mat4f_value = other.mat4f_value;
                        break;
                    case TYPE_VTEX:
                        virtual_texture_value = other.virtual_texture_value;
                        break;
                    case TYPE_WEAK_COMPONENT:
                        component_value = other.component_value;
                        break;
                    case TYPE_WEAK_TRANSFORM:
                        transform_value = other.transform_value;
                        break;
                    }
                }
                return *this;
            }

            std::string ShaderProperty::toString() const
            {
                switch (type_)
                {
                case TYPE_BOOL:
                    return bool_value ? "true" : "false";
                case TYPE_INT:
                    return ITKCommon::PrintfToStdString("%d", int_value);
                case TYPE_FLOAT:
                    return ITKCommon::PrintfToStdString("%f", float_value);
                case TYPE_VEC2F:
                    return ITKCommon::PrintfToStdString("(%f, %f)", vec2f_value.x, vec2f_value.y);
                case TYPE_VEC3F:
                    return ITKCommon::PrintfToStdString("(%f, %f, %f)", vec3f_value.x, vec3f_value.y, vec3f_value.z);
                case TYPE_VEC4F:
                    return ITKCommon::PrintfToStdString("(%f, %f, %f, %f)", vec4f_value.x, vec4f_value.y, vec4f_value.z, vec4f_value.w);
                case TYPE_MAT4F:
                    return ITKCommon::PrintfToStdString(
                        "\n"
                        "|%f, %f, %f, %f|\n"
                        "|%f, %f, %f, %f|\n"
                        "|%f, %f, %f, %f|\n"
                        "|%f, %f, %f, %f|\n",
                        mat4f_value.a1, mat4f_value.b1, mat4f_value.c1, mat4f_value.d1,
                        mat4f_value.a2, mat4f_value.b2, mat4f_value.c2, mat4f_value.d2,
                        mat4f_value.a3, mat4f_value.b3, mat4f_value.c3, mat4f_value.d3,
                        mat4f_value.a4, mat4f_value.b4, mat4f_value.c4, mat4f_value.d4);

                case TYPE_VTEX:
                    return virtual_texture_value ? "VTEX<valid>" : "VTEX<null>";
                case TYPE_WEAK_COMPONENT:
                    return component_value.lock() ? "COMPONENT<valid>" : "COMPONENT<null>";
                case TYPE_WEAK_TRANSFORM:
                    return transform_value.lock() ? "TRANSFORM<valid>" : "TRANSFORM<null>";
                default:
                    return "<unknown type>";
                }
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
            void ShaderPropertyBag::addProperty(const std::string &key, const ShaderProperty &property)
            {
                properties_[key] = property;
            }

            ShaderProperty &ShaderPropertyBag::getProperty(const std::string &key)
            {
                auto it = properties_.find(key);
                if (it != properties_.end())
                {
                    return it->second;
                }
                ITK_ABORT(true, "Property not found: %s", key.c_str());
                static ShaderProperty dummy;
                return dummy;
            }

            const ShaderProperty &ShaderPropertyBag::getProperty(const std::string &key) const
            {
                auto it = properties_.find(key);
                if (it != properties_.end())
                {
                    return it->second;
                }
                ITK_ABORT(true, "Property not found: %s", key.c_str());
                static ShaderProperty dummy;
                return dummy;
            }

            bool ShaderPropertyBag::hasProperty(const std::string &key) const
            {
                return properties_.find(key) != properties_.end();
            }

            void ShaderPropertyBag::removeProperty(const std::string &key)
            {
                properties_.erase(key);
            }

            void ShaderPropertyBag::clear()
            {
                properties_.clear();
            }
        }
    }
}

// Exemplo de uso (C++11 compatible):
/*
PropertyBag bag;

// Adicionar diferentes tipos de propriedades
bag.addProperty("position", MathCore::vec3f(1.0f, 2.0f, 3.0f));
bag.addProperty("scale", 1.5f);
bag.addProperty("frame", 60);

// Recuperar propriedades
MathCore::vec3f position = bag.getProperty<MathCore::vec3f>("position");
float scale = bag.getProperty<float>("scale");
int frame = bag.getProperty<int>("frame");

// Verificar tipo da propriedade
Property prop = bag.getProperty("scale");
if (prop.holds<float>()) {
    float value = prop.get<float>();
}

// Converter para string
std::string str = prop.toString();
*/
