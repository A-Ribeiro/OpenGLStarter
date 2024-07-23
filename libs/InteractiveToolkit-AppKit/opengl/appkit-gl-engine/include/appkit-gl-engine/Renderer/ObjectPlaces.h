#pragma once

#include <appkit-gl-engine/Components/ComponentLight.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
#include <appkit-gl-engine/Components/ComponentParticleSystem.h>

#include <appkit-gl-engine/Transform.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>

namespace AppKit
{
    namespace GLEngine
    {

        class ObjectPlaces : public EventCore::HandleCallback
        {

            bool traverse_search_elements(std::shared_ptr<Transform> element, void *userData);

        public:
            std::vector<std::shared_ptr<Components::ComponentLight>> pointLights;
            std::vector<std::shared_ptr<Components::ComponentLight>> sunLights;
            std::vector<std::shared_ptr<Components::ComponentLight>> spotLights;
            // std::vector<Components::ComponentMeshWrapper *> meshWrappers;
            std::vector<std::shared_ptr<Components::ComponentParticleSystem>> sceneParticleSystem;
            std::vector<std::shared_ptr<Components::ComponentColorLine>> debugLines;

            void searchObjects(std::shared_ptr<Transform> root);

            std::vector<std::shared_ptr<Components::ComponentMeshWrapper>> filteredMeshWrappers;
            void filterObjectsAABB(std::shared_ptr<Transform> root, const CollisionCore::AABB<MathCore::vec3f> &aabb);
            void filterObjectsOBB(std::shared_ptr<Transform> root, const CollisionCore::OBB<MathCore::vec3f> &obb);
            void filterObjectsFrustum(std::shared_ptr<Transform> root, const CollisionCore::Frustum<MathCore::vec3f> &frustum);
        };

    }

}