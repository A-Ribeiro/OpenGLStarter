#include <appkit-gl-engine/Renderer/SortingHelper.h>
#include <InteractiveToolkit/AlgorithmCore/Sorting/RadixCountingSort.h>

namespace AppKit
{
    namespace GLEngine
    {

        std::vector<AlgorithmCore::Sorting::SortIndexu64> &SortingHelper::sort_by_material(std::vector<Transform *> &v)
        {
            using namespace AlgorithmCore::Sorting;

            sort_u64.resize(v.size());
            sort_u64_tmp.resize(v.size());

            for (size_t i = 0; i < v.size(); i++)
            {
                auto transform = v[i];
                uint64_t addr_to_insert = 0;
                for (const auto &component : transform->getComponents())
                {
                    if (component->compareType(Components::ComponentMaterial::Type))
                    {
                        addr_to_insert = (uint64_t)component.get();
                        break;
                    }
                }
                sort_u64[i] = SortIndexu64::Create((uint32_t)i, addr_to_insert);
            }

            RadixCountingSortu64::sortIndex(sort_u64.data(), (uint32_t)sort_u64.size(), sort_u64_tmp.data());

            return sort_u64;
        }

        void SortingHelper::sort_by_z(std::vector<Transform *> &v, SortingModeEnum mode, bool sort_by_material_p)
        {
            using namespace AlgorithmCore::Sorting;

            sort_u32.resize(v.size());
            sort_u32_tmp.resize(v.size());

            if (sort_by_material_p)
            {
                const auto &material_sorted = sort_by_material(v);
                if (mode == SortingMode_Desc)
                {
                    for (size_t i = 0; i < material_sorted.size(); i++)
                    {
                        uint32_t index = material_sorted[i].index;
                        auto transform = v[index];
                        MathCore::vec3f pos = transform->getPosition(true);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)index, SortToolu32::floatToInt(-pos.z));
                    }
                }
                else
                {
                    for (size_t i = 0; i < material_sorted.size(); i++)
                    {
                        uint32_t index = material_sorted[i].index;
                        auto transform = v[index];
                        MathCore::vec3f pos = transform->getPosition(true);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)index, SortToolu32::floatToInt(pos.z));
                    }
                }
            }
            else
            {
                if (mode == SortingMode_Desc)
                {
                    for (size_t i = 0; i < v.size(); i++)
                    {
                        auto transform = v[i];
                        MathCore::vec3f pos = transform->getPosition(true);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(-pos.z));
                    }
                }
                else
                {
                    for (size_t i = 0; i < v.size(); i++)
                    {
                        auto transform = v[i];
                        MathCore::vec3f pos = transform->getPosition(true);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(pos.z));
                    }
                }
            }

            RadixCountingSortu32::sortIndex(sort_u32.data(), (uint32_t)sort_u32.size(), sort_u32_tmp.data());

            transform_tmp.resize(v.size());
            for (size_t i = 0; i < v.size(); i++)
            {
                auto index = sort_u32[i].index;
                transform_tmp[i] = v[index];
            }

            v.swap(transform_tmp);
        }

        void SortingHelper::sort_by_direction(std::vector<Transform *> &v, const MathCore::vec3f &dir, SortingModeEnum mode, bool sort_by_material_p)
        {
            using namespace AlgorithmCore::Sorting;

            sort_u32.resize(v.size());
            sort_u32_tmp.resize(v.size());

            if (sort_by_material_p)
            {
                const auto &material_sorted = sort_by_material(v);
                if (mode == SortingMode_Desc)
                {
                    for (size_t i = 0; i < material_sorted.size(); i++)
                    {
                        uint32_t index = material_sorted[i].index;
                        auto transform = v[index];
                        MathCore::vec3f pos = transform->getPosition(true);
                        float projection = MathCore::OP<MathCore::vec3f>::dot(pos, dir);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)index, SortToolu32::floatToInt(-projection));
                    }
                }
                else
                {
                    for (size_t i = 0; i < material_sorted.size(); i++)
                    {
                        uint32_t index = material_sorted[i].index;
                        auto transform = v[index];
                        MathCore::vec3f pos = transform->getPosition(true);
                        float projection = MathCore::OP<MathCore::vec3f>::dot(pos, dir);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)index, SortToolu32::floatToInt(projection));
                    }
                }
            }
            else
            {
                if (mode == SortingMode_Desc)
                {
                    for (size_t i = 0; i < v.size(); i++)
                    {
                        auto transform = v[i];
                        MathCore::vec3f pos = transform->getPosition(true);
                        float projection = MathCore::OP<MathCore::vec3f>::dot(pos, dir);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(-projection));
                    }
                }
                else
                {
                    for (size_t i = 0; i < v.size(); i++)
                    {
                        auto transform = v[i];
                        MathCore::vec3f pos = transform->getPosition(true);
                        float projection = MathCore::OP<MathCore::vec3f>::dot(pos, dir);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(projection));
                    }
                }
            }

            RadixCountingSortu32::sortIndex(sort_u32.data(), (uint32_t)sort_u32.size(), sort_u32_tmp.data());

            transform_tmp.resize(v.size());
            for (size_t i = 0; i < v.size(); i++)
            {
                auto index = sort_u32[i].index;
                transform_tmp[i] = v[index];
            }

            v.swap(transform_tmp);
        }

        void SortingHelper::sort_by_direction(std::vector<Components::ComponentParticleSystem *> &v, const MathCore::vec3f &dir, SortingModeEnum mode)
        {
            using namespace AlgorithmCore::Sorting;

            sort_u32.resize(v.size());
            sort_u32_tmp.resize(v.size());

            if (mode == SortingMode_Desc)
            {
                for (size_t i = 0; i < v.size(); i++)
                {
                    float projection = MathCore::OP<MathCore::vec3f>::dot(v[i]->aabb_center, dir);
                    sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(-projection));
                }
            }
            else
            {
                for (size_t i = 0; i < v.size(); i++)
                {
                    float projection = MathCore::OP<MathCore::vec3f>::dot(v[i]->aabb_center, dir);
                    sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(projection));
                }
            }

            RadixCountingSortu32::sortIndex(sort_u32.data(), (uint32_t)sort_u32.size(), sort_u32_tmp.data());

            particleSystem_tmp.resize(v.size());
            for (size_t i = 0; i < v.size(); i++)
            {
                auto index = sort_u32[i].index;
                particleSystem_tmp[i] = v[index];
            }

            v.swap(particleSystem_tmp);
        }

        std::vector<AlgorithmCore::Sorting::SortIndexu32> &SortingHelper::sort_by_direction(const std::vector<Components::Particle> &particles, const MathCore::vec3f &dir, SortingModeEnum mode)
        {

            using namespace AlgorithmCore::Sorting;

            sort_u32.resize(particles.size());
            sort_u32_tmp.resize(particles.size());

            if (mode == SortingMode_Desc)
            {
                for (size_t i = 0; i < particles.size(); i++)
                {
                    float projection = MathCore::OP<MathCore::vec3f>::dot(particles[i].pos, dir);
                    sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(-projection));
                }
            }
            else
            {
                for (size_t i = 0; i < particles.size(); i++)
                {
                    float projection = MathCore::OP<MathCore::vec3f>::dot(particles[i].pos, dir);
                    sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(projection));
                }
            }

            RadixCountingSortu32::sortIndex(sort_u32.data(), (uint32_t)sort_u32.size(), sort_u32_tmp.data());

            return sort_u32;
        }

    }
}
