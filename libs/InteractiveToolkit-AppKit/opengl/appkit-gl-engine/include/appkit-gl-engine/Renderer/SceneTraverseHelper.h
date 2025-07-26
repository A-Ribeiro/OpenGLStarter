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

        enum FilterFlagsBitMask
        {
            FilterFlags_None = 0,
            FilterFlags_Compute_Whole_Scene_AABB = 1 << 0,
            FilterFlags_Compute_Selection_AABB = 1 << 1,
            FilterFlags_Fill_SunLight_List = 1 << 2,

            FilterFlags_All = FilterFlags_Compute_Whole_Scene_AABB | FilterFlags_Compute_Selection_AABB | FilterFlags_Fill_SunLight_List
        };

        enum TraverseFilterOpt
        {
            TraverseFilterOpt_All,
            TraverseFilterOpt_AABB,
            TraverseFilterOpt_OBB,
            TraverseFilterOpt_Frustum
        };

        struct TraverseFilter
        {
            FilterFlagsBitMask flags;
            TraverseFilterOpt opt;

            union
            {
                const CollisionCore::AABB<MathCore::vec3f> *aabb;
                const CollisionCore::OBB<MathCore::vec3f> *obb;
                const CollisionCore::Frustum<MathCore::vec3f> *frustum;
            };

            TraverseFilter(FilterFlagsBitMask flags, const CollisionCore::AABB<MathCore::vec3f> *aabb);
            TraverseFilter(FilterFlagsBitMask flags, const CollisionCore::OBB<MathCore::vec3f> *obb);
            TraverseFilter(FilterFlagsBitMask flags, const CollisionCore::Frustum<MathCore::vec3f> *frustum);
        };

        class SceneTraverseHelper
        {

            void traverse(Transform *element, const TraverseFilter &filterOpt);

        public:
            std::vector<Components::ComponentLight *> sunLightList;
            CollisionCore::AABB<MathCore::vec3f> scene_aabb;
            CollisionCore::AABB<MathCore::vec3f> selection_aabb;

            std::vector<Components::ComponentParticleSystem *> particleSystemList;
            std::vector<Components::ComponentMeshWrapper *> meshWrapperList;
            std::vector<Transform *> transformList;

            void filterByAABB(Transform *root, const CollisionCore::AABB<MathCore::vec3f> &aabb, FilterFlagsBitMask flags);
            void filterByOBB(Transform *root, const CollisionCore::OBB<MathCore::vec3f> &obb, FilterFlagsBitMask flags);
            void filterByFrustum(Transform *root, const CollisionCore::Frustum<MathCore::vec3f> &frustum, FilterFlagsBitMask flags);
        };

    }

}