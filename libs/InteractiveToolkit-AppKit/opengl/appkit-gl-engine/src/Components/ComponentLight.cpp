#include <appkit-gl-engine/Components/ComponentLight.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>

// using namespace aRibeiro;
// using namespace AppKit::GLEngine;
// using namespace AppKit::GLEngine::Components;

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentLight::Type = "ComponentLight";

            ComponentLight::ComponentLight() : Component(ComponentLight::Type)
            {
                type = LightNone;
                cast_shadow = false;
            }

            MathCore::vec3f ComponentLight::getWorldDirection(bool useVisitedFlag)
            {
                auto transform = getTransform();
                return transform->getRotation(useVisitedFlag) * MathCore::vec3f(0, 0, 1);
            }

            void ComponentLight::postProcessing_computeLightParameters()
            {
                switch (type)
                {
                case LightSun:
                    sun.worldDirection = getWorldDirection();
                    sun.finalIntensity = sun.color * sun.intensity;
                    break;
                default:
                    break;
                }
            }

            void ComponentLight::createDebugLines()
            {

                switch (type)
                {
                case LightSun:
                {
                    auto transform = getTransform();

                    MathCore::mat4f &final_transform = transform->getMatrix(false);
                    MathCore::vec3f origin = MathCore::CVT<MathCore::vec4f>::toVec3(final_transform[3]);
                    MathCore::quatf rotation = MathCore::GEN<MathCore::quatf>::fromMat4(final_transform);

                    MathCore::vec3f forward = rotation * MathCore::vec3f(0, 0, 1);
                    MathCore::vec3f up = rotation * MathCore::vec3f(0, 0.25f, 0);
                    MathCore::vec3f right = rotation * MathCore::vec3f(0.25f, 0, 0);

                    auto lines = transform->findComponent<ComponentColorLine>();
                    if (lines == nullptr)
                        lines = transform->addNewComponent<ComponentColorLine>();

                    lines->vertices.clear();

                    lines->color = MathCore::vec4f(1, 0, 0, 1);
                    lines->width = 2.0f;

                    MathCore::vec3f target = origin + forward * 1.0f;

                    MathCore::vec3f target_arrow = origin + forward * 0.8f;

                    lines->vertices.push_back(origin);
                    lines->vertices.push_back(target);

                    lines->vertices.push_back(origin);
                    lines->vertices.push_back(origin + up);

                    lines->vertices.push_back(origin - right);
                    lines->vertices.push_back(origin + right);

                    lines->vertices.push_back(target);
                    lines->vertices.push_back(target_arrow - up);

                    lines->vertices.push_back(target);
                    lines->vertices.push_back(target_arrow + up);

                    lines->vertices.push_back(target);
                    lines->vertices.push_back(target_arrow - right);

                    lines->vertices.push_back(target);
                    lines->vertices.push_back(target_arrow + right);

                    lines->syncVBODynamic();

                    break;
                }
                default:
                    break;
                }
            }

            // always clone
            std::shared_ptr<Component> ComponentLight::duplicate_ref_or_clone(bool force_clone){
                auto result = Component::CreateShared<ComponentLight>();

                result->type = this->type;
                result->sun = this->sun;
                result->cast_shadow = this->cast_shadow;

                return result;
            }
            void ComponentLight::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap){

            }

            void ComponentLight::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentLight::Type);
                writer.String("id");
                writer.Uint64((intptr_t)self().get());
                writer.EndObject();
                
            }
            void ComponentLight::Deserialize(rapidjson::Value &_value, std::unordered_map<intptr_t, std::shared_ptr<Transform>> &transform_map, std::unordered_map<intptr_t, std::shared_ptr<Component>> &component_map){
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), ComponentLight::Type) == 0)
                    return;
                
            }

        }
    }

}