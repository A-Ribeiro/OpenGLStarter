#include <appkit-gl-engine/Renderer/SortingHelper.h>
#include <InteractiveToolkit/AlgorithmCore/Sorting/RadixCountingSort.h>
#include <InteractiveToolkit/AlgorithmCore/Sorting/ParallelRadixCountingSort.h>

// uses this definition to select the parallel algorithm or the single-thread algorithm
const uint64_t MIN_ITEMS_PER_THREAD = 256;

namespace AppKit
{
    namespace GLEngine
    {

        SortingHelper::SortingHelper() : completion_semaphore(0)
        {
            threadpool = nullptr;
        }

        std::vector<AlgorithmCore::Sorting::SortIndexu64> &SortingHelper::sort_by_material(std::vector<Transform *> &v)
        {
            using namespace AlgorithmCore::Sorting;

            sort_u64.resize(v.size());
            sort_u64_tmp.resize(v.size());

            uint64_t n_proc = 0;
            uint64_t amount_to_sort = 0;
            uint64_t thread_count = 0;
            uint64_t block_count = 0;

            if (threadpool != nullptr)
            {
                n_proc = (uint64_t)threadpool->threadCount() * 4ULL;
                amount_to_sort = v.size();
                thread_count = MathCore::OP<uint64_t>::div_ceil(amount_to_sort, n_proc);
                block_count = n_proc;
            }

            // min 1k workload per thread
            if (threadpool != nullptr && thread_count >= MIN_ITEMS_PER_THREAD)
            {
#define ParallelForBegin(...)                                        \
    for (uint64_t blk = 0; blk < block_count; blk++)                 \
    {                                                                \
        threadpool->postTask( \
            [this, blk, thread_count, amount_to_sort, __VA_ARGS__]() \
            { \
                uint64_t data_index_start = blk * thread_count; \
                uint64_t thread_count_end = data_index_start + thread_count; \
                if (thread_count_end > amount_to_sort) \
                    thread_count_end = amount_to_sort; \
                for (uint64_t i = data_index_start; i < thread_count_end; i++)
#define ParallelForEnd                               \
    completion_semaphore.release();                  \
    });                                              \
    }                                                \
    for (uint64_t blk = 0; blk < block_count; blk++) \
        completion_semaphore.blockingAcquire();

                ParallelForBegin(&v)
                // for (size_t i = 0; i < v.size(); i++)
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
                ParallelForEnd;

                ParallelRadixCountingSortu64::sortIndex(sort_u64.data(), (uint32_t)sort_u64.size(), threadpool, sort_u64_tmp.data(), &completion_semaphore);

#undef ParallelForBegin
#undef ParallelForEnd
            }
            else
            {
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
            }
            return sort_u64;
        }

        void SortingHelper::sort_by_z(std::vector<Transform *> &v, SortingModeEnum mode, bool sort_by_material_p)
        {
            using namespace AlgorithmCore::Sorting;

            sort_u32.resize(v.size());
            sort_u32_tmp.resize(v.size());

            uint64_t n_proc = 0;
            uint64_t amount_to_sort = 0;
            uint64_t thread_count = 0;
            uint64_t block_count = 0;

            if (threadpool != nullptr)
            {
                n_proc = (uint64_t)threadpool->threadCount() * 4ULL;
                amount_to_sort = v.size();
                thread_count = MathCore::OP<uint64_t>::div_ceil(amount_to_sort, n_proc);
                block_count = n_proc;
            }

            // min 1k workload per thread
            if (threadpool != nullptr && thread_count >= MIN_ITEMS_PER_THREAD)
            {
#define ParallelForBegin(...)                                        \
    for (uint64_t blk = 0; blk < block_count; blk++)                 \
    {                                                                \
        threadpool->postTask( \
            [this, blk, thread_count, amount_to_sort, __VA_ARGS__]() \
            { \
                uint64_t data_index_start = blk * thread_count; \
                uint64_t thread_count_end = data_index_start + thread_count; \
                if (thread_count_end > amount_to_sort) \
                    thread_count_end = amount_to_sort; \
                for (uint64_t i = data_index_start; i < thread_count_end; i++)
#define ParallelForEnd                               \
    completion_semaphore.release();                  \
    });                                              \
    }                                                \
    for (uint64_t blk = 0; blk < block_count; blk++) \
        completion_semaphore.blockingAcquire();

                if (sort_by_material_p)
                {
                    const auto &material_sorted = sort_by_material(v);
                    if (mode == SortingMode_Desc)
                    {
                        ParallelForBegin(&material_sorted, &v)
                        // for (size_t i = 0; i < material_sorted.size(); i++)
                        {
                            uint32_t index = material_sorted[i].index;
                            auto transform = v[index];
                            MathCore::vec3f pos = transform->getPosition(true);
                            sort_u32[i] = SortIndexu32::Create((uint32_t)index, SortToolu32::floatToInt(-pos.z));
                        }
                        ParallelForEnd
                    }
                    else
                    {
                        ParallelForBegin(&material_sorted, &v)
                        // for (size_t i = 0; i < material_sorted.size(); i++)
                        {
                            uint32_t index = material_sorted[i].index;
                            auto transform = v[index];
                            MathCore::vec3f pos = transform->getPosition(true);
                            sort_u32[i] = SortIndexu32::Create((uint32_t)index, SortToolu32::floatToInt(pos.z));
                        }
                        ParallelForEnd
                    }
                }
                else
                {
                    if (mode == SortingMode_Desc)
                    {
                        ParallelForBegin(&v)
                        // for (size_t i = 0; i < v.size(); i++)
                        {
                            auto transform = v[i];
                            MathCore::vec3f pos = transform->getPosition(true);
                            sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(-pos.z));
                        }
                        ParallelForEnd
                    }
                    else
                    {
                        ParallelForBegin(&v) for (size_t i = 0; i < v.size(); i++)
                        {
                            auto transform = v[i];
                            MathCore::vec3f pos = transform->getPosition(true);
                            sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(pos.z));
                        }
                        ParallelForEnd
                    }
                }

                ParallelRadixCountingSortu32::sortIndex(sort_u32.data(), (uint32_t)sort_u32.size(), threadpool, sort_u32_tmp.data(), &completion_semaphore);

                transform_tmp.resize(v.size());
                ParallelForBegin(&v)
                // for (size_t i = 0; i < v.size(); i++)
                {
                    auto index = sort_u32[i].index;
                    transform_tmp[i] = v[index];
                }
                ParallelForEnd

#undef ParallelForBegin
#undef ParallelForEnd
            }
            else
            {

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
            }

            v.swap(transform_tmp);
        }

        void SortingHelper::sort_by_direction(std::vector<Transform *> &v, const MathCore::vec3f &dir, SortingModeEnum mode, bool sort_by_material_p)
        {
            using namespace AlgorithmCore::Sorting;

            sort_u32.resize(v.size());
            sort_u32_tmp.resize(v.size());

            uint64_t n_proc = 0;
            uint64_t amount_to_sort = 0;
            uint64_t thread_count = 0;
            uint64_t block_count = 0;

            if (threadpool != nullptr)
            {
                n_proc = (uint64_t)threadpool->threadCount() * 4ULL;
                amount_to_sort = v.size();
                thread_count = MathCore::OP<uint64_t>::div_ceil(amount_to_sort, n_proc);
                block_count = n_proc;
            }

            // min 1k workload per thread
            if (threadpool != nullptr && thread_count >= MIN_ITEMS_PER_THREAD)
            {
#define ParallelForBegin(...)                                        \
    for (uint64_t blk = 0; blk < block_count; blk++)                 \
    {                                                                \
        threadpool->postTask( \
            [this, blk, thread_count, amount_to_sort, __VA_ARGS__]() \
            { \
                uint64_t data_index_start = blk * thread_count; \
                uint64_t thread_count_end = data_index_start + thread_count; \
                if (thread_count_end > amount_to_sort) \
                    thread_count_end = amount_to_sort; \
                for (uint64_t i = data_index_start; i < thread_count_end; i++)
#define ParallelForEnd                               \
    completion_semaphore.release();                  \
    });                                              \
    }                                                \
    for (uint64_t blk = 0; blk < block_count; blk++) \
        completion_semaphore.blockingAcquire();

                if (sort_by_material_p)
                {
                    const auto &material_sorted = sort_by_material(v);
                    if (mode == SortingMode_Desc)
                    {
                        ParallelForBegin(&material_sorted, &v, dir)
                        // for (size_t i = 0; i < material_sorted.size(); i++)
                        {
                            uint32_t index = material_sorted[i].index;
                            auto transform = v[index];
                            MathCore::vec3f pos = transform->getPosition(true);
                            float projection = MathCore::OP<MathCore::vec3f>::dot(pos, dir);
                            sort_u32[i] = SortIndexu32::Create((uint32_t)index, SortToolu32::floatToInt(-projection));
                        }
                        ParallelForEnd
                    }
                    else
                    {
                        ParallelForBegin(&material_sorted, &v, dir)
                        // for (size_t i = 0; i < material_sorted.size(); i++)
                        {
                            uint32_t index = material_sorted[i].index;
                            auto transform = v[index];
                            MathCore::vec3f pos = transform->getPosition(true);
                            float projection = MathCore::OP<MathCore::vec3f>::dot(pos, dir);
                            sort_u32[i] = SortIndexu32::Create((uint32_t)index, SortToolu32::floatToInt(projection));
                        }
                        ParallelForEnd
                    }
                }
                else
                {
                    if (mode == SortingMode_Desc)
                    {
                        ParallelForBegin(&v, dir)
                        // for (size_t i = 0; i < v.size(); i++)
                        {
                            auto transform = v[i];
                            MathCore::vec3f pos = transform->getPosition(true);
                            float projection = MathCore::OP<MathCore::vec3f>::dot(pos, dir);
                            sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(-projection));
                        }
                        ParallelForEnd
                    }
                    else
                    {
                        ParallelForBegin(&v, dir)
                        // for (size_t i = 0; i < v.size(); i++)
                        {
                            auto transform = v[i];
                            MathCore::vec3f pos = transform->getPosition(true);
                            float projection = MathCore::OP<MathCore::vec3f>::dot(pos, dir);
                            sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(projection));
                        }
                        ParallelForEnd
                    }
                }

                ParallelRadixCountingSortu32::sortIndex(sort_u32.data(), (uint32_t)sort_u32.size(), threadpool, sort_u32_tmp.data(), &completion_semaphore);

                transform_tmp.resize(v.size());
                ParallelForBegin(&v)
                // for (size_t i = 0; i < v.size(); i++)
                {
                    auto index = sort_u32[i].index;
                    transform_tmp[i] = v[index];
                }
                ParallelForEnd

#undef ParallelForBegin
#undef ParallelForEnd
            }
            else
            {
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
            }

            v.swap(transform_tmp);
        }

        void SortingHelper::sort_by_direction(std::vector<Components::ComponentParticleSystem *> &v, const MathCore::vec3f &dir, SortingModeEnum mode)
        {
            using namespace AlgorithmCore::Sorting;

            sort_u32.resize(v.size());
            sort_u32_tmp.resize(v.size());

            uint64_t n_proc = 0;
            uint64_t amount_to_sort = 0;
            uint64_t thread_count = 0;
            uint64_t block_count = 0;

            if (threadpool != nullptr)
            {
                n_proc = (uint64_t)threadpool->threadCount() * 4ULL;
                amount_to_sort = v.size();
                thread_count = MathCore::OP<uint64_t>::div_ceil(amount_to_sort, n_proc);
                block_count = n_proc;
            }

            // min 1k workload per thread
            if (threadpool != nullptr && thread_count >= MIN_ITEMS_PER_THREAD)
            {
#define ParallelForBegin(...)                                        \
    for (uint64_t blk = 0; blk < block_count; blk++)                 \
    {                                                                \
        threadpool->postTask( \
            [this, blk, thread_count, amount_to_sort, __VA_ARGS__]() \
            { \
                uint64_t data_index_start = blk * thread_count; \
                uint64_t thread_count_end = data_index_start + thread_count; \
                if (thread_count_end > amount_to_sort) \
                    thread_count_end = amount_to_sort; \
                for (uint64_t i = data_index_start; i < thread_count_end; i++)
#define ParallelForEnd                               \
    completion_semaphore.release();                  \
    });                                              \
    }                                                \
    for (uint64_t blk = 0; blk < block_count; blk++) \
        completion_semaphore.blockingAcquire();

                if (mode == SortingMode_Desc)
                {
                    ParallelForBegin(&v, dir)
                    // for (size_t i = 0; i < v.size(); i++)
                    {
                        float projection = MathCore::OP<MathCore::vec3f>::dot(v[i]->aabb_center, dir);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(-projection));
                    }
                    ParallelForEnd
                }
                else
                {
                    ParallelForBegin(&v, dir)
                    // for (size_t i = 0; i < v.size(); i++)
                    {
                        float projection = MathCore::OP<MathCore::vec3f>::dot(v[i]->aabb_center, dir);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(projection));
                    }
                    ParallelForEnd
                }

                ParallelRadixCountingSortu32::sortIndex(sort_u32.data(), (uint32_t)sort_u32.size(), threadpool, sort_u32_tmp.data(), &completion_semaphore);

                particleSystem_tmp.resize(v.size());
                ParallelForBegin(&v)
                // for (size_t i = 0; i < v.size(); i++)
                {
                    auto index = sort_u32[i].index;
                    particleSystem_tmp[i] = v[index];
                }
                ParallelForEnd

#undef ParallelForBegin
#undef ParallelForEnd
            }
            else
            {

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
            }

            v.swap(particleSystem_tmp);
        }

        std::vector<AlgorithmCore::Sorting::SortIndexu32> &SortingHelper::sort_by_direction(const std::vector<Components::Particle> &particles, const MathCore::vec3f &dir, SortingModeEnum mode)
        {

            using namespace AlgorithmCore::Sorting;

            sort_u32.resize(particles.size());
            sort_u32_tmp.resize(particles.size());

            uint64_t n_proc = 0;
            uint64_t amount_to_sort = 0;
            uint64_t thread_count = 0;
            uint64_t block_count = 0;

            if (threadpool != nullptr)
            {
                n_proc = (uint64_t)threadpool->threadCount() * 4ULL;
                amount_to_sort = particles.size();
                thread_count = MathCore::OP<uint64_t>::div_ceil(amount_to_sort, n_proc);
                block_count = n_proc;
            }

            // min 1k workload per thread
            if (threadpool != nullptr && thread_count >= MIN_ITEMS_PER_THREAD)
            {
#define ParallelForBegin(...)                                        \
    for (uint64_t blk = 0; blk < block_count; blk++)                 \
    {                                                                \
        threadpool->postTask( \
            [this, blk, thread_count, amount_to_sort, __VA_ARGS__]() \
            { \
                uint64_t data_index_start = blk * thread_count; \
                uint64_t thread_count_end = data_index_start + thread_count; \
                if (thread_count_end > amount_to_sort) \
                    thread_count_end = amount_to_sort; \
                for (uint64_t i = data_index_start; i < thread_count_end; i++)
#define ParallelForEnd                               \
    completion_semaphore.release();                  \
    });                                              \
    }                                                \
    for (uint64_t blk = 0; blk < block_count; blk++) \
        completion_semaphore.blockingAcquire();

                if (mode == SortingMode_Desc)
                {
                    ParallelForBegin(&particles, dir)
                    // for (size_t i = 0; i < particles.size(); i++)
                    {
                        float projection = MathCore::OP<MathCore::vec3f>::dot(particles[i].pos, dir);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(-projection));
                    }
                    ParallelForEnd
                }
                else
                {
                    ParallelForBegin(&particles, dir)
                    // for (size_t i = 0; i < particles.size(); i++)
                    {
                        float projection = MathCore::OP<MathCore::vec3f>::dot(particles[i].pos, dir);
                        sort_u32[i] = SortIndexu32::Create((uint32_t)i, SortToolu32::floatToInt(projection));
                    }
                    ParallelForEnd
                }

                ParallelRadixCountingSortu32::sortIndex(sort_u32.data(), (uint32_t)sort_u32.size(), threadpool, sort_u32_tmp.data(), &completion_semaphore);

#undef ParallelForBegin
#undef ParallelForEnd
            }
            else
            {

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
            }

            return sort_u32;
        }

    }
}
