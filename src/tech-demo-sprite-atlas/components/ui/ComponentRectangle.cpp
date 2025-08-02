// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include "ComponentRectangle.h"

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentRectangle::Type = "ComponentRectangle";

            void ComponentRectangle::checkOrCreateAuxiliaryComponents(
                AppKit::GLEngine::ResourceMap *resourceMap)
            {

                if (material != nullptr || mesh != nullptr || meshWrapper != nullptr)
                    return;
                auto transform = getTransform();

                if (material == nullptr)
                {
                    material = transform->addComponent(resourceMap->defaultUnlitVertexColorAlphaMaterial);
                }

                if (mesh == nullptr)
                {
                    mesh = transform->addNewComponent<ComponentMesh>();
                    mesh->format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_COLOR0;
                }

                if (meshWrapper == nullptr)
                {
                    meshWrapper = transform->addNewComponent<ComponentMeshWrapper>();
                    transform->makeFirstComponent(meshWrapper);
                    // meshWrapper->updateMeshAABB();
                }
            }

            void ComponentRectangle::setQuadFromMinMax(
                AppKit::GLEngine::ResourceMap *resourceMap,
                const MathCore::vec2f &min,
                const MathCore::vec2f &max,
                const MathCore::vec4f &color,
                const MathCore::vec4f &radius)
            {
                checkOrCreateAuxiliaryComponents(resourceMap);

                mesh->pos.clear();
                mesh->pos.push_back(MathCore::vec3f(max.x, max.y, 0.0f));
                mesh->pos.push_back(MathCore::vec3f(max.x, min.y, 0.0f));
                mesh->pos.push_back(MathCore::vec3f(min.x, min.y, 0.0f));
                mesh->pos.push_back(MathCore::vec3f(min.x, max.y, 0.0f));

                mesh->color[0].clear();
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);

                mesh->indices.clear();
                mesh->indices.push_back(0);
                mesh->indices.push_back(2);
                mesh->indices.push_back(1);

                mesh->indices.push_back(0);
                mesh->indices.push_back(3);
                mesh->indices.push_back(2);

                meshWrapper->setShapeAABB(
                    CollisionCore::AABB<MathCore::vec3f>(
                        mesh->pos[0],
                        mesh->pos[2]),
                    true);
            }

            void ComponentRectangle::setQuadFromCenterSize(
                AppKit::GLEngine::ResourceMap *resourceMap,
                const MathCore::vec2f &center,
                const MathCore::vec2f &size,
                const MathCore::vec4f &color,
                const MathCore::vec4f &radius)
            {
                auto size_half = size * 0.5f;
                setQuadFromMinMax(
                    resourceMap,
                    center - size_half,
                    center + size_half,
                    color,
                    radius);
            }

            ComponentRectangle::ComponentRectangle() : Component(ComponentRectangle::Type)
            {
                always_clone = false;
            }

            ComponentRectangle::~ComponentRectangle()
            {
            }

            // always clone
            std::shared_ptr<Component> ComponentRectangle::duplicate_ref_or_clone(bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentRectangle>();

                result->always_clone = this->always_clone;

                return result;
            }
            void ComponentRectangle::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentRectangle::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }
            void ComponentRectangle::Deserialize(rapidjson::Value &_value,
                                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                 ResourceSet &resourceSet)
            {
            }
        }
    }
}
