#include "RotatingCross.h"

#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/Transform.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            const ComponentType RotatingCross::Type = "RotatingCross";

            RotatingCross::RotatingCross() : Component(RotatingCross::Type)
            {
                speed_degrees = -100.0f;
            }

            void RotatingCross::start()
            {
                AppBase *app = Engine::Instance()->app;
                app->screenRenderWindow->OnUpdate.add(&RotatingCross::OnUpdate, this);
            }

            void RotatingCross::OnUpdate(Platform::Time *time)
            {
                auto transform = getTransform();

                transform->LocalRotation =
                    transform->LocalRotation *
                    MathCore::GEN<MathCore::quatf>::fromEuler(
                        0,
                        MathCore::OP<float>::deg_2_rad(speed_degrees) * time->deltaTime,
                        0);
            }

            std::shared_ptr<Component> RotatingCross::duplicate_ref_or_clone(bool force_clone) {
                auto result = Component::CreateShared<RotatingCross>();
                result->speed_degrees = this->speed_degrees;
                return result;
            }
            void RotatingCross::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap){
                
            }

            void RotatingCross::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                writer.StartObject();
                writer.String("type");
                writer.String(RotatingCross::Type);
                writer.String("id");
                writer.Uint64((intptr_t)self().get());
                writer.EndObject();
                
            }
            void RotatingCross::Deserialize(rapidjson::Value &_value,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                  ResourceSet &resourceSet)
            {
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), RotatingCross::Type) == 0)
                    return;
                
            }


            RotatingCross::~RotatingCross()
            {
                AppBase *app = Engine::Instance()->app;
                app->screenRenderWindow->OnUpdate.remove(&RotatingCross::OnUpdate, this);
            }

        }

    }
}
