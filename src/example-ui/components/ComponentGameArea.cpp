#include "ComponentGameArea.h"
#include "../App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <InteractiveToolkit/CollisionCore/CollisionCore.h>
#include <appkit-gl-engine/Components/2d/ComponentRectangle.h>

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

                if (debugDrawEnabled)
                {
                    auto transform = getTransform();
                    if (transform->findTransformByName("DebugDrawAABB", 1) != nullptr)
                        return; // already created
                    auto self_ref = this->self<ComponentGameArea>();
                    app->executeOnMainThread.enqueue([this, self_ref, transform]()
                                                     {
                                                         auto stage_box = self_ref->StageArea.c_val();
                                                         auto stage_center = (stage_box.min_box + stage_box.max_box) * 0.5f;
                                                         auto stage_size = (stage_box.max_box - stage_box.min_box);

                                                         auto debugDrawTransform = transform->addChild(Transform::CreateShared("DebugDrawAABB"));
                                                         debugDrawTransform->setLocalPosition(MathCore::vec3f(
                                                             stage_box.min_box.x + stage_size.x * 0.5f,
                                                             stage_box.min_box.y + stage_size.y * 0.5f,
                                                             10.0f));
                                                         auto rect = debugDrawTransform->addNewComponent<ComponentRectangle>();
                                                         rect->setQuad(
                                                             &self_ref->app->resourceMap, // use app's resource map
                                                             MathCore::vec2f(
                                                                 stage_size.x,
                                                                 stage_size.y),           // size
                                                             self_ref->debugDrawColor,    // color
                                                             MathCore::vec4f(0, 0, 0, 0), // radius
                                                             StrokeModeGrowOutside,       // stroke mode
                                                             0.0f,                        // stroke thickness
                                                             MathCore::vec4f(0, 0, 0, 0), // stroke color
                                                             0.0f,                        // drop shadow thickness
                                                             MathCore::vec4f(0, 0, 0, 0), // drop shadow color
                                                             MeshUploadMode_Direct,       // mesh upload mode
                                                             4);                          // segment count
                                                        app->gameScene->printHierarchy(); });
                }
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

                result->eventHandlerSetRef = this->eventHandlerSetRef;
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