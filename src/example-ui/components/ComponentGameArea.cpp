#include "ComponentGameArea.h"
#include "../App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {
            const ComponentType ComponentGameArea::Type = "ComponentGameArea";

            ComponentGameArea::ComponentGameArea() : Component(ComponentGameArea::Type)
            {

                debugDrawEnabled = false;
                debugDrawColor = ui::colorFromHex("#FF0000FF");

                LockCameraMove = false;
                app = nullptr;

                StageArea.OnChange.add(&ComponentGameArea::OnStageAreaParameter, this);
            }

            ComponentGameArea::~ComponentGameArea()
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentGameArea::OnUpdate, this);
            }

            void ComponentGameArea::start()
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.add(&ComponentGameArea::OnUpdate, this);
            }

            void ComponentGameArea::attachToTransform(std::shared_ptr<Transform> t)
            {
                eventHandlerSetRef = t->eventHandlerSet;
            }
            void ComponentGameArea::detachFromTransform(std::shared_ptr<Transform> t)
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentGameArea::OnUpdate, this);
                eventHandlerSetRef.reset();
            }

            void ComponentGameArea::OnStageAreaParameter(const CollisionCore::AABB<MathCore::vec3f> &value, const CollisionCore::AABB<MathCore::vec3f> &oldValue)
            {
            }

            void ComponentGameArea::OnUpdate(Platform::Time *time)
            {
            }

            // always clone
            std::shared_ptr<Component> ComponentGameArea::duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
            {
                auto result = Component::CreateShared<ComponentGameArea>();

                result->debugDrawEnabled = this->debugDrawEnabled;
                result->debugDrawColor = this->debugDrawColor;

                result->LockCameraMove = this->LockCameraMove;
                result->app = this->app;

                result->StageArea.setValueNoCallback(this->StageArea.c_val());

                return result;
            }

            void ComponentGameArea::fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentGameArea::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }

            void ComponentGameArea::Deserialize(rapidjson::Value &_value,
                                                std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                ResourceSet &resourceSet)
            {
            }

        }
    }
}