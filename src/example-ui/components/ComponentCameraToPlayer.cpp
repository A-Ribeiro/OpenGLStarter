#include "ComponentCameraToPlayer.h"
#include "../App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <InteractiveToolkit/CollisionCore/CollisionCore.h>
#include <appkit-gl-engine/Components/2d/ComponentRectangle.h>
#include "./ComponentPlayer.h"

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {
            const ComponentType ComponentCameraToPlayer::Type = "ComponentCameraToPlayer";

            ComponentCameraToPlayer::ComponentCameraToPlayer() : Component(ComponentCameraToPlayer::Type)
            {
                app = nullptr;
            }

            ComponentCameraToPlayer::~ComponentCameraToPlayer()
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentCameraToPlayer::OnUpdate, this);
            }

            void ComponentCameraToPlayer::start()
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.add(&ComponentCameraToPlayer::OnUpdate, this);
                
            }

            void ComponentCameraToPlayer::attachToTransform(std::shared_ptr<Transform> t)
            {
                eventHandlerSetRef = t->eventHandlerSet;
            }
            void ComponentCameraToPlayer::detachFromTransform(std::shared_ptr<Transform> t)
            {
                if (auto eventHandlerSet = eventHandlerSetRef.lock())
                    eventHandlerSet->OnUpdate.remove(&ComponentCameraToPlayer::OnUpdate, this);
                eventHandlerSetRef.reset();
            }

            void ComponentCameraToPlayer::OnStageAreaParameter(const CollisionCore::AABB<MathCore::vec3f> &value, const CollisionCore::AABB<MathCore::vec3f> &oldValue)
            {
            }

            void ComponentCameraToPlayer::OnUpdate(Platform::Time *time)
            {
            }

            // always clone
            std::shared_ptr<Component> ComponentCameraToPlayer::duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
            {
                auto result = Component::CreateShared<ComponentCameraToPlayer>();

                result->eventHandlerSetRef = this->eventHandlerSetRef;
                result->app = this->app;

                return result;
            }

            void ComponentCameraToPlayer::fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentCameraToPlayer::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }

            void ComponentCameraToPlayer::Deserialize(rapidjson::Value &_value,
                                                std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                ResourceSet &resourceSet)
            {
            }

        }
    }
}