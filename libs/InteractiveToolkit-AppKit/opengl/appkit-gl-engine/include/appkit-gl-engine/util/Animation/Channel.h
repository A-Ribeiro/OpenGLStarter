#pragma once

#include <appkit-gl-engine/util/Animation/common.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Animation
        {
            template <typename T>
            class Channel
            {
            public:
                std::vector<Key<T>> keys;

                Channel() = default;

                Channel(const Channel<T> &other)
                    : keys(other.keys)
                {
                }

                Channel<T> &operator=(const Channel<T> &other)
                {
                    if (this != &other)
                    {
                        keys = other.keys;
                    }
                    return *this;
                }

                Channel(Channel<T> &&other) noexcept
                    : keys(std::move(other.keys))
                {
                }

                Channel<T> &operator=(Channel<T> &&other) noexcept
                {
                    if (this != &other)
                    {
                        keys = std::move(other.keys);
                    }

                    return *this;
                }

                inline void clamp_time(float time_min_s, float time_max_s)
                {
                    if (keys.size() == 0)
                        return;
                    int64_t min_idx = indexForwardTimeSearch(0, time_min_s);
                    int64_t min_idx_a = getIndex(min_idx);
                    int64_t min_idx_b = getIndex(min_idx + 1);
                    if (time_min_s > keys[min_idx_a].time)
                    {
                        T zero_value;
                        interpolate(min_idx_a, min_idx_b, time_min_s, &zero_value);
                        keys[min_idx_a] = Key<T>(time_min_s, zero_value);
                    }
                    if (min_idx_a > 0)
                        keys.erase(keys.begin(), keys.begin() + min_idx_a);

                    int64_t last_item_idx = (int64_t)keys.size() - 1;
                    int64_t max_idx = indexBackwardTimeSearch(last_item_idx, time_max_s);
                    int64_t max_idx_a = getIndex(max_idx - 1);
                    int64_t max_idx_b = getIndex(max_idx);
                    if (time_max_s < keys[max_idx_b].time)
                    {
                        T last_value;
                        interpolate(max_idx_a, max_idx_b, time_max_s, &last_value);
                        keys[max_idx_b] = Key<T>(time_max_s, last_value);
                    }
                    if (max_idx_b < last_item_idx)
                        keys.erase(keys.begin() + max_idx_b + 1, keys.end());
                }

                inline int64_t getIndex(int64_t idx) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return 0;
                    return MathCore::OP<int64_t>::clamp(idx, 0, keys_size - 1);
                }

                inline void readValue(int64_t idx, T *output) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return;
                    idx = MathCore::OP<int64_t>::clamp(idx, 0, keys_size - 1);
                    *output = keys[idx].value;
                }

                inline void readTime(int64_t idx, float *output) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return;
                    idx = MathCore::OP<int64_t>::clamp(idx, 0, keys_size - 1);
                    *output = keys[idx].time;
                }

                inline int64_t indexForwardTimeSearch(int64_t start_idx, float curr_time_s) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return 0;
                    if (start_idx >= keys_size)
                        return keys_size - 1;
                    else if (start_idx < 0)
                        start_idx = 0;

                    // to_use index time will have time equal or greater than curr_time_s
                    int64_t to_use = start_idx;
                    for (int64_t i = start_idx; i < keys_size && keys[i].time <= curr_time_s; i++)
                        to_use = i;

                    return to_use;
                }

                inline int64_t indexBackwardTimeSearch(int64_t start_idx, float curr_time_s) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return 0;
                    if (start_idx >= keys_size)
                        return keys_size - 1;
                    else if (start_idx < 0)
                        start_idx = 0;

                    // to_use index time will be less than curr_time_s
                    int64_t to_use = start_idx;
                    for (int64_t i = start_idx; i >= 0 && keys[i].time >= curr_time_s; i--)
                        to_use = i;

                    return to_use;
                }

                inline void interpolate(int64_t idx_a, int64_t idx_b, float curr_time_s, T *output) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return;

                    int64_t max_idx = keys_size - 1;

                    idx_a = MathCore::OP<int64_t>::clamp(idx_a, 0, max_idx);
                    idx_b = MathCore::OP<int64_t>::clamp(idx_b, 0, max_idx);

                    if (idx_a == idx_b)
                    {
                        *output = keys[idx_a].value;
                        return;
                    }

                    const auto &itemA = keys[idx_a];
                    const auto &itemB = keys[idx_b];

                    float delta_inv = itemB.time - itemA.time;
                    delta_inv = MathCore::OP<float>::maximum(delta_inv, MathCore::FloatTypeInfo<float>::min);
                    delta_inv = 1.0f / delta_inv;
                    float lrp = (curr_time_s - itemA.time) * delta_inv;
                    lrp = MathCore::OP<float>::clamp(lrp, 0.0f, 1.0f);

                    *output = InterpolateSpecialization<T>::doInterpolate(itemA.value, itemB.value, lrp);
                }
            };

        }
    }
}
