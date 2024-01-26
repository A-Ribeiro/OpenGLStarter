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

        class ObjectPlaces: public EventCore::HandleCallback
        {

            bool traverse_search_elements(Transform *element, void *userData);

        public:
            std::vector<Components::ComponentLight *> pointLights;
            std::vector<Components::ComponentLight *> sunLights;
            std::vector<Components::ComponentLight *> spotLights;
            // std::vector<Components::ComponentMeshWrapper *> meshWrappers;
            std::vector<Components::ComponentParticleSystem *> sceneParticleSystem;
            std::vector<Components::ComponentColorLine *> debugLines;

            void searchObjects(Transform *root);

            std::vector<Components::ComponentMeshWrapper *> filteredMeshWrappers;
            void filterObjectsAABB(Transform *root, const CollisionCore::AABB<MathCore::vec3f> &aabb);
            void filterObjectsOBB(Transform *root, const CollisionCore::OBB<MathCore::vec3f> &obb);
            void filterObjectsFrustum(Transform *root, const CollisionCore::Frustum<MathCore::vec3f> &frustum);
        };

    }

}