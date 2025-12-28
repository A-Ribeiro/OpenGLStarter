#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/Platform/Core/ObjectQueue.h>

class App;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            class ComponentGameArea : public Component
            {

                // std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;
                std::weak_ptr<EventHandlerSet> eventHandlerSetRef;

                void OnUpdate(Platform::Time *time);

                void OnStageAreaParameter(const CollisionCore::AABB<MathCore::vec3f> &value, const CollisionCore::AABB<MathCore::vec3f> &oldValue);

            public:
                static const ComponentType Type;

                bool debugDrawEnabled;
                MathCore::vec4f debugDrawColor;
                EventCore::Property<CollisionCore::AABB<MathCore::vec3f>> StageArea;

                bool LockCameraMove;

                App *app;
                // Platform::ObjectQueue<std::shared_ptr<AppKit::GLEngine::Transform>> *transformPool;

                ComponentGameArea();
                ~ComponentGameArea();

                void attachToTransform(std::shared_ptr<Transform> t) override;
                void detachFromTransform(std::shared_ptr<Transform> t) override;

                void start() override;

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone);
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);
            };

        }
    }
}