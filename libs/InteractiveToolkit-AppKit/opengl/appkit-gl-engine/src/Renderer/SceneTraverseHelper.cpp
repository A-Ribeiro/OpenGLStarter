#include <appkit-gl-engine/Renderer/SceneTraverseHelper.h>

namespace AppKit
{
    namespace GLEngine
    {

        TraverseFilter::TraverseFilter(FilterFlagsBitMask flags, const CollisionCore::AABB<MathCore::vec3f> *aabb)
        {
            this->flags = flags;
            this->opt = TraverseFilterOpt_AABB;
            this->aabb = aabb;
        }

        TraverseFilter::TraverseFilter(FilterFlagsBitMask flags, const CollisionCore::OBB<MathCore::vec3f> *obb)
        {
            this->flags = flags;
            this->opt = TraverseFilterOpt_OBB;
            this->obb = obb;
        }

        TraverseFilter::TraverseFilter(FilterFlagsBitMask flags, const CollisionCore::Frustum<MathCore::vec3f> *frustum)
        {
            this->flags = flags;
            this->opt = TraverseFilterOpt_Frustum;
            this->frustum = frustum;
        }

        void SceneTraverseHelper::traverse(Transform *element, const TraverseFilter &filterOpt)
        {
            if (element->skip_traversing)
                return;

            for (const auto &component : element->getComponents())
            {
                if ((filterOpt.flags & FilterFlags_Fill_SunLight_List) && component->compareType(Components::ComponentLight::Type))
                {
                    auto light = (Components::ComponentLight *)component.get();
                    if (light->type == Components::LightSun)
                        sunLightList.push_back(light);
                }
                else if (component->compareType(Components::ComponentParticleSystem::Type))
                {
                    auto particleSystem = (Components::ComponentParticleSystem *)component.get();
                    bool add = false;
                    if (filterOpt.opt == TraverseFilterOpt_AABB)
                        add = CollisionCore::AABB<MathCore::vec3f>::aabbOverlapsAABB(*filterOpt.aabb, particleSystem->aabb);
                    else if (filterOpt.opt == TraverseFilterOpt_OBB)
                        add = CollisionCore::AABB<MathCore::vec3f>::obbOverlapsAABB(*filterOpt.obb, particleSystem->aabb);
                    else if (filterOpt.opt == TraverseFilterOpt_Frustum)
                        add = CollisionCore::AABB<MathCore::vec3f>::frustumOverlapsAABB(*filterOpt.frustum, particleSystem->aabb);
                    if (add)
                        particleSystemList.push_back(particleSystem);
                }
                else if (component->compareType(Components::ComponentMeshWrapper::Type))
                {
                    auto meshWrapper = (Components::ComponentMeshWrapper *)component.get();
                    const auto &aabb = meshWrapper->aabb;
                    bool add = false;
                    if (filterOpt.opt == TraverseFilterOpt_AABB)
                        add = meshWrapper->isOverlappingAABB(*filterOpt.aabb);
                    else if (filterOpt.opt == TraverseFilterOpt_OBB)
                        add = meshWrapper->isOverlappingOBB(*filterOpt.obb);
                    else if (filterOpt.opt == TraverseFilterOpt_Frustum)
                        add = meshWrapper->isOverlappingFrustum(*filterOpt.frustum);
                    if (add)
                    {
                        transformList.push_back(element);
                        meshWrapperList.push_back(meshWrapper);
                        if (filterOpt.flags & FilterFlags_Compute_Selection_AABB)
                            selection_aabb = CollisionCore::AABB<MathCore::vec3f>::joinAABB(selection_aabb, aabb);
                    }
                    if (filterOpt.flags & FilterFlags_Compute_Whole_Scene_AABB)
                        scene_aabb = CollisionCore::AABB<MathCore::vec3f>::joinAABB(scene_aabb, aabb);
                }
            }

            for (auto &child : element->getChildren())
                traverse(child.get(), filterOpt);
        }

        void SceneTraverseHelper::filterByAABB(Transform *root, const CollisionCore::AABB<MathCore::vec3f> &aabb, FilterFlagsBitMask flags)
        {
            if (flags & FilterFlags_Compute_Whole_Scene_AABB)
                scene_aabb.makeEmpty();
            if (flags & FilterFlags_Compute_Selection_AABB)
                selection_aabb.makeEmpty();
            if (flags & FilterFlags_Fill_SunLight_List)
                sunLightList.clear();

            particleSystemList.clear();
            meshWrapperList.clear();
            transformList.clear();

            SceneTraverseHelper::traverse(root, TraverseFilter(flags, &aabb));
        }

        void SceneTraverseHelper::filterByOBB(Transform *root, const CollisionCore::OBB<MathCore::vec3f> &obb, FilterFlagsBitMask flags)
        {
            if (flags & FilterFlags_Compute_Whole_Scene_AABB)
                scene_aabb.makeEmpty();
            if (flags & FilterFlags_Compute_Selection_AABB)
                selection_aabb.makeEmpty();
            if (flags & FilterFlags_Fill_SunLight_List)
                sunLightList.clear();

            particleSystemList.clear();
            meshWrapperList.clear();
            transformList.clear();

            SceneTraverseHelper::traverse(root, TraverseFilter(flags, &obb));
        }

        void SceneTraverseHelper::filterByFrustum(Transform *root, const CollisionCore::Frustum<MathCore::vec3f> &frustum, FilterFlagsBitMask flags)
        {
            if (flags & FilterFlags_Compute_Whole_Scene_AABB)
                scene_aabb.makeEmpty();
            if (flags & FilterFlags_Compute_Selection_AABB)
                selection_aabb.makeEmpty();
            if (flags & FilterFlags_Fill_SunLight_List)
                sunLightList.clear();

            particleSystemList.clear();
            meshWrapperList.clear();
            transformList.clear();

            SceneTraverseHelper::traverse(root, TraverseFilter(flags, &frustum));
        }

    }
}
