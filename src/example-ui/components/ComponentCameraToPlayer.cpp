#include "ComponentCameraToPlayer.h"
#include "../App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <InteractiveToolkit/CollisionCore/CollisionCore.h>
#include <appkit-gl-engine/Components/2d/ComponentRectangle.h>
#include "./ComponentPlayer.h"

// #include <appkit-physics/core/Box2D.h>

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
                auto player_ref = player.lock();
                auto camera_ref = camera.lock();
                if (player_ref == nullptr || camera_ref == nullptr)
                    return;
                auto player_transform = player_ref->getTransform();
                auto camera_transform = camera_ref->getTransform();

                using namespace MathCore;
                // using namespace AppKit::Physics::Core;

                vec2f windowSize_2 = camera_ref->projectionAreaSizePx * 0.5f;

                vec2f stage_camera_min_limit = app->gameScene->container2D->game_area.min + windowSize_2;
                vec2f stage_camera_max_limit = app->gameScene->container2D->game_area.max - windowSize_2;

                vec2f game_area_center = app->gameScene->container2D->game_area.getCenter();

                for(int i=0;i<2;i++)
                {
                    if (stage_camera_max_limit[i] < stage_camera_min_limit[i])
                    {
                        // If the game area is smaller than the camera viewport, center the camera on the game area
                        float center = game_area_center[i];
                        stage_camera_min_limit[i] = center;
                        stage_camera_max_limit[i] = center;
                    }
                }

                vec3f camera_look_to = player_transform->getLocalPosition() + vec3f(player_ref->Offset.c_val(), 0.0f);

                camera_look_to = OP<vec3f>::clamp(camera_look_to, 
                    vec3f(stage_camera_min_limit, camera_look_to.z), 
                    vec3f(stage_camera_max_limit, camera_look_to.z));

                camera_transform->setLocalPosition(camera_look_to);
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