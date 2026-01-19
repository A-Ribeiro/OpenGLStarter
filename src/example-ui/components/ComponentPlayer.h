#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/Platform/Core/ObjectQueue.h>

#include "util/PlayerInputState.h"
#include "util/JumpState.h"

class App;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            class ComponentGameArea;

            class ComponentPlayer : public Component
            {

                // std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;
                std::weak_ptr<EventHandlerSet> eventHandlerSetRef;

                void OnUpdate(Platform::Time *time);

                void OnRadiusParameter(const float &value, const float &oldValue);

            public:
                static const ComponentType Type;

                bool debugDrawEnabled;
                float debugDrawThickness;
                MathCore::vec4f debugDrawColor;
                EventCore::Property<float> Radius;

                std::weak_ptr<ComponentGameArea> gameArea;

                MathCore::vec3f velocity;
                MathCore::vec3f acceleration;

                App *app;
                // Platform::ObjectQueue<std::shared_ptr<AppKit::GLEngine::Transform>> *transformPool;

                CollisionCore::Sphere<MathCore::vec3f> getSphere();

                PlayerInputState inputState;
                JumpState jumpState;

                ComponentPlayer();
                ~ComponentPlayer();

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