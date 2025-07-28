#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EaseCore/EaseCore.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/ComponentMesh.h>

#include "ComponentGrow.h"
#include "../App.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            const ComponentType ComponentGrow::Type = "ComponentGrow";

            ComponentGrow::ComponentGrow() : Component(ComponentGrow::Type)
            {
                lrp = 0.0f;
            }

            ComponentGrow::~ComponentGrow()
            {
                auto renderWindowRegion = ToShared(renderWindowRegionRef);
                if (renderWindowRegion == nullptr)
                    return;
                renderWindowRegion->OnUpdate.remove(&ComponentGrow::OnUpdate, this);
            }

            void ComponentGrow::OnUpdate(Platform::Time *time)
            {
                lrp = MathCore::OP<float>::move(lrp, 1.0f, time->deltaTime * 0.6f);
                float expo = EaseCore::Easef32::inExpo(1.0f, 0.0f, lrp);

                auto transform = getTransform();

                if (lrp >= 1.0f - MathCore::EPSILON<float>::low_precision)
                {
                    transform->removeSelf();
                    return;
                }
                transform->setLocalScale(MathCore::vec3f(lrp));
                auto localPos = transform->getLocalPosition();
                transform->setLocalPosition(MathCore::vec3f(localPos.x, localPos.y, -lrp));

                auto mesh = transform->findComponent<ComponentMesh>();
                for (auto &color : mesh->color[0])
                {
                    color.a = expo;
                }

                // mesh->syncVBODynamic();
            }

            void ComponentGrow::attachToTransform(std::shared_ptr<Transform> t)
            {
            }

            void ComponentGrow::detachFromTransform(std::shared_ptr<Transform> t)
            {
                auto renderWindowRegion = ToShared(t->renderWindowRegion);
                if (renderWindowRegion == nullptr)
                    return;
                renderWindowRegion->OnUpdate.remove(&ComponentGrow::OnUpdate, this);
            }

            void ComponentGrow::start()
            {
                auto transform = getTransform();

                renderWindowRegionRef = transform->renderWindowRegion;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);
                renderWindowRegion->OnUpdate.add(&ComponentGrow::OnUpdate, this);
            }

            // always clone
            std::shared_ptr<Component> ComponentGrow::duplicate_ref_or_clone(bool force_clone)
            {
                auto result = Component::CreateShared<ComponentGrow>();
                result->app = app;
                return result;
            }
            void ComponentGrow::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentGrow::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }
            void ComponentGrow::Deserialize(rapidjson::Value &_value,
                                            std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                            std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                            ResourceSet &resourceSet)
            {
            }

        }
    }
}
