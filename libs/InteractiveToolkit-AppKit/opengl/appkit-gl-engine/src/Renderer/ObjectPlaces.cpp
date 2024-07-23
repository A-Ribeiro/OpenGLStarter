#include <appkit-gl-engine/Renderer/ObjectPlaces.h>

namespace AppKit
{
    namespace GLEngine
    {

        const uint32_t FILTER_ADD_LIGHT = 1 << 0;
        const uint32_t FILTER_ADD_PARTICLE_SYSTEM = 1 << 1;
        const uint32_t FILTER_ADD_MESH_WRAPPER = 1 << 2;
        const uint32_t FILTER_ADD_DEBUG_LINES = 1 << 3;

        enum ObjectPlacesFilterOpt
        {
            ObjectPlacesFilterOpt_All,
            ObjectPlacesFilterOpt_AABB,
            ObjectPlacesFilterOpt_OBB,
            ObjectPlacesFilterOpt_Frustum
        };

        struct ObjectPlaces_FilterOptions
        {
            uint32_t add_bitmask;
            ObjectPlacesFilterOpt filter_opt;

            union
            {
                const CollisionCore::AABB<MathCore::vec3f> *aabb;
                const CollisionCore::OBB<MathCore::vec3f> *obb;
                const CollisionCore::Frustum<MathCore::vec3f> *frustum;
            };

            ObjectPlaces_FilterOptions(uint32_t add_bitmask, ObjectPlacesFilterOpt filter_opt)
            {
                this->add_bitmask = add_bitmask;
                this->filter_opt = filter_opt;
            }
        };

        bool ObjectPlaces::traverse_search_elements(std::shared_ptr<Transform> element, void *userData)
        {

            ObjectPlaces_FilterOptions *filter = (ObjectPlaces_FilterOptions *)userData;

            bool addLight = filter->add_bitmask & FILTER_ADD_LIGHT;
            bool addParticleSystem = filter->add_bitmask & FILTER_ADD_PARTICLE_SYSTEM;
            bool addMeshWrapper = filter->add_bitmask & FILTER_ADD_MESH_WRAPPER;
            bool addDebugLines = filter->add_bitmask & FILTER_ADD_DEBUG_LINES;

            for (int i = 0; i < element->getComponentCount(); i++)
            {
                auto component = element->getComponentAt(i);
                if (addLight && component->compareType(Components::ComponentLight::Type))
                {
                    auto light = std::dynamic_pointer_cast<Components::ComponentLight>(component);
                    if (light->type == Components::LightSun)
                        sunLights.push_back(light);
                }
                else if (addParticleSystem && component->compareType(Components::ComponentParticleSystem::Type))
                {

                    auto particleSystem = std::dynamic_pointer_cast<Components::ComponentParticleSystem>(component);

                    bool add = true;
                    if (filter->filter_opt == ObjectPlacesFilterOpt_AABB)
                    {
                        add = CollisionCore::AABB<MathCore::vec3f>::aabbOverlapsAABB(*filter->aabb, particleSystem->aabb);
                    }
                    else if (filter->filter_opt == ObjectPlacesFilterOpt_OBB)
                    {
                        add = CollisionCore::AABB<MathCore::vec3f>::obbOverlapsAABB(*filter->obb, particleSystem->aabb);
                    }
                    else if (filter->filter_opt == ObjectPlacesFilterOpt_Frustum)
                    {
                        add = CollisionCore::AABB<MathCore::vec3f>::frustumOverlapsAABB(*filter->frustum, particleSystem->aabb);
                    }

                    if (add)
                        sceneParticleSystem.push_back(particleSystem);
                }
                else if (addMeshWrapper && component->compareType(Components::ComponentMeshWrapper::Type))
                {

                    auto meshWrapper = std::dynamic_pointer_cast<Components::ComponentMeshWrapper>(component);

                    bool add = true;
                    if (filter->filter_opt == ObjectPlacesFilterOpt_AABB)
                    {
                        add = meshWrapper->isOverlappingAABB(*filter->aabb);
                    }
                    else if (filter->filter_opt == ObjectPlacesFilterOpt_OBB)
                    {
                        add = meshWrapper->isOverlappingOBB(*filter->obb);
                    }
                    else if (filter->filter_opt == ObjectPlacesFilterOpt_Frustum)
                    {
                        add = meshWrapper->isOverlappingFrustum(*filter->frustum);
                    }

                    if (add)
                        filteredMeshWrappers.push_back(meshWrapper);
                }
                else if (addDebugLines && component->compareType(Components::ComponentColorLine::Type))
                {
                    debugLines.push_back(std::dynamic_pointer_cast<Components::ComponentColorLine>(component));
                }
            }

            return true;
        }

        void ObjectPlaces::searchObjects(std::shared_ptr<Transform> root)
        {
            pointLights.clear();
            sunLights.clear();
            spotLights.clear();

            sceneParticleSystem.clear();
            debugLines.clear();
            filteredMeshWrappers.clear();

            ObjectPlaces_FilterOptions opt = ObjectPlaces_FilterOptions(
                FILTER_ADD_LIGHT | FILTER_ADD_PARTICLE_SYSTEM | FILTER_ADD_MESH_WRAPPER | FILTER_ADD_DEBUG_LINES,
                ObjectPlacesFilterOpt_All);

            root->traversePreOrder_DepthFirst(
                EventCore::CallbackWrapper(&ObjectPlaces::traverse_search_elements, this),
                &opt);
        }

        void ObjectPlaces::filterObjectsAABB(std::shared_ptr<Transform> root, const CollisionCore::AABB<MathCore::vec3f> &aabb)
        {
            // lights.clear();
            // debugLines.clear();
            sceneParticleSystem.clear();
            filteredMeshWrappers.clear();

            ObjectPlaces_FilterOptions opt = ObjectPlaces_FilterOptions(
                // FILTER_ADD_LIGHT | FILTER_ADD_DEBUG_LINES
                FILTER_ADD_PARTICLE_SYSTEM | FILTER_ADD_MESH_WRAPPER,
                ObjectPlacesFilterOpt_AABB);

            opt.aabb = &aabb;

            root->traversePreOrder_DepthFirst(
                EventCore::CallbackWrapper( &ObjectPlaces::traverse_search_elements, this),
                &opt);
        }

        void ObjectPlaces::filterObjectsOBB(std::shared_ptr<Transform> root, const CollisionCore::OBB<MathCore::vec3f> &obb)
        {
            // lights.clear();
            // debugLines.clear();
            sceneParticleSystem.clear();
            filteredMeshWrappers.clear();

            ObjectPlaces_FilterOptions opt = ObjectPlaces_FilterOptions(
                // FILTER_ADD_LIGHT | FILTER_ADD_DEBUG_LINES
                FILTER_ADD_PARTICLE_SYSTEM | FILTER_ADD_MESH_WRAPPER,
                ObjectPlacesFilterOpt_OBB);

            opt.obb = &obb;

            root->traversePreOrder_DepthFirst(
                EventCore::CallbackWrapper(&ObjectPlaces::traverse_search_elements, this),
                &opt);
        }

        void ObjectPlaces::filterObjectsFrustum(std::shared_ptr<Transform> root, const CollisionCore::Frustum<MathCore::vec3f> &frustum)
        {
            // lights.clear();
            // debugLines.clear();
            sceneParticleSystem.clear();
            filteredMeshWrappers.clear();
            ObjectPlaces_FilterOptions opt = ObjectPlaces_FilterOptions(
                // FILTER_ADD_LIGHT | FILTER_ADD_DEBUG_LINES
                FILTER_ADD_PARTICLE_SYSTEM | FILTER_ADD_MESH_WRAPPER,
                ObjectPlacesFilterOpt_Frustum);

            opt.frustum = &frustum;

            root->traversePreOrder_DepthFirst(
                EventCore::CallbackWrapper(&ObjectPlaces::traverse_search_elements, this),
                &opt);
        }

    }
}
