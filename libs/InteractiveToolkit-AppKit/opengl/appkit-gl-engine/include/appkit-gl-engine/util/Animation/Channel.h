#pragma once

#include <appkit-gl-engine/util/Animation/common.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Animation
        {
            /// \brief A time-indexed keyframe channel for animation data.
            ///
            /// Channel stores a sequence of \c Key<T> elements, each associating a time
            /// (in seconds) with a value of type \c T. It provides linear (or slerp for
            /// quaternions) interpolation between keys, time clamping to a sub-range,
            /// and forward/backward time search utilities.
            ///
            /// Typical usage: populate the \c keys vector with time-value pairs, then
            /// call \c interpolate to sample the animated value at an arbitrary time.
            ///
            /// \author Alessandro Ribeiro
            ///
            /// \tparam T The value type stored per key. Must support default
            /// construction and linear interpolation via \c InterpolateSpecialization<T>.
            ///
            template <typename T>
            class Channel
            {
            public:
                /// \brief The keyframe storage.
                ///
                /// Ordered by ascending time. Each element is a \c Key<T> holding a
                /// time stamp (seconds) and the corresponding animated value.
                ///
                std::vector<Key<T>> keys;

                /// \brief Construct a Channel with no keys.
                ///
                /// Example:
                ///
                /// \code
                /// Channel<float> channel;
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                ///
                Channel() = default;

                /// \brief Copy-construct a Channel from another Channel.
                ///
                /// Copies all keys from \p other.
                ///
                /// Example:
                ///
                /// \code
                /// Channel<float> src;
                /// src.keys.push_back(Key<float>(0.0f, 1.0f));
                /// Channel<float> dst(src);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param other The Channel to copy from.
                ///
                Channel(const Channel<T> &other)
                    : keys(other.keys)
                {
                }

                /// \brief Copy-assign a Channel from another Channel.
                ///
                /// Copies all keys from \p other. Self-assignment is a no-op.
                ///
                /// Example:
                ///
                /// \code
                /// Channel<float> a, b;
                /// b.keys.push_back(Key<float>(1.0f, 2.0f));
                /// a = b;
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param other The Channel to copy from.
                /// \return Reference to this Channel.
                ///
                Channel<T> &operator=(const Channel<T> &other)
                {
                    if (this != &other)
                    {
                        keys = other.keys;
                    }
                    return *this;
                }

                /// \brief Move-construct a Channel from another Channel.
                ///
                /// Steals the keyframe storage from \p other.
                ///
                /// Example:
                ///
                /// \code
                /// Channel<float> src;
                /// src.keys.push_back(Key<float>(0.0f, 1.0f));
                /// Channel<float> dst(std::move(src));
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param other The Channel to move from.
                ///
                Channel(Channel<T> &&other) noexcept
                    : keys(std::move(other.keys))
                {
                }

                /// \brief Move-assign a Channel from another Channel.
                ///
                /// Steals the keyframe storage from \p other. Self-assignment is a no-op.
                ///
                /// Example:
                ///
                /// \code
                /// Channel<float> a, b;
                /// b.keys.push_back(Key<float>(1.0f, 2.0f));
                /// a = std::move(b);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param other The Channel to move from.
                /// \return Reference to this Channel.
                ///
                Channel<T> &operator=(Channel<T> &&other) noexcept
                {
                    if (this != &other)
                    {
                        keys = std::move(other.keys);
                    }

                    return *this;
                }

                /// \brief Trim the keyframe timeline to the range [time_min_s, time_max_s].
                ///
                /// Removes all keys strictly before \p time_min_s and all keys strictly
                /// after \p time_max_s. If a boundary key would be removed, a new key is
                /// inserted at the boundary time using interpolated value (zero for the
                /// lower bound, the last key's value for the upper bound).
                ///
                /// Example:
                ///
                /// \code
                /// Channel<float> channel;
                /// channel.keys.push_back(Key<float>(0.0f, 0.0f));
                /// channel.keys.push_back(Key<float>(1.0f, 1.0f));
                /// channel.keys.push_back(Key<float>(2.0f, 2.0f));
                /// channel.clamp_time(0.5f, 1.5f);
                /// // Keys now span [0.5, 1.5] with interpolated boundary values.
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param time_min_s Minimum time to keep (seconds).
                /// \param time_max_s Maximum time to keep (seconds).
                ///
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

                /// \brief Clamp an index to the valid keyframe range [0, size-1].
                ///
                /// Returns 0 when the channel has no keys.
                ///
                /// Example:
                ///
                /// \code
                /// int64_t safe_idx = channel.getIndex(-5); // returns 0
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param idx The index to clamp.
                /// \return The clamped index.
                ///
                inline int64_t getIndex(int64_t idx) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return 0;
                    return MathCore::OP<int64_t>::clamp(idx, 0, keys_size - 1);
                }

                /// \brief Read the value at the given keyframe index.
                ///
                /// The index is clamped to the valid range. No-op when the channel is
                /// empty.
                ///
                /// Example:
                ///
                /// \code
                /// float value;
                /// channel.readValue(0, &value);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param idx The keyframe index.
                /// \param output Pointer to store the retrieved value.
                ///
                inline void readValue(int64_t idx, T *output) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return;
                    idx = MathCore::OP<int64_t>::clamp(idx, 0, keys_size - 1);
                    *output = keys[idx].value;
                }

                /// \brief Read the time (in seconds) at the given keyframe index.
                ///
                /// The index is clamped to the valid range. No-op when the channel is
                /// empty.
                ///
                /// Example:
                ///
                /// \code
                /// float time_s;
                /// channel.readTime(0, &time_s);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param idx The keyframe index.
                /// \param output Pointer to store the retrieved time.
                ///
                inline void readTime(int64_t idx, float *output) const
                {
                    int64_t keys_size = (int64_t)keys.size();
                    if (keys_size == 0)
                        return;
                    idx = MathCore::OP<int64_t>::clamp(idx, 0, keys_size - 1);
                    *output = keys[idx].time;
                }

                /// \brief Find the index of the first key whose time is >= curr_time_s,
                ///        starting the search from start_idx.
                ///
                /// Returns the last index when all keys are before curr_time_s, or 0
                /// when the channel is empty.
                ///
                /// Example:
                ///
                /// \code
                /// int64_t idx = channel.indexForwardTimeSearch(0, 1.5f);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param start_idx Index to begin the search from.
                /// \param curr_time_s The target time in seconds.
                /// \return The index of the first key with time >= curr_time_s.
                ///
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

                /// \brief Find the index of the last key whose time is <= curr_time_s,
                ///        starting the search from start_idx and moving backwards.
                ///
                /// Returns the last index when the channel is empty or start_idx is
                /// out of range.
                ///
                /// Example:
                ///
                /// \code
                /// int64_t idx = channel.indexBackwardTimeSearch(10, 1.5f);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param start_idx Index to begin the backward search from.
                /// \param curr_time_s The target time in seconds.
                /// \return The index of the last key with time <= curr_time_s.
                ///
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

                /// \brief Linearly (or slerp for quaternions) interpolate between two
                ///        keys and write the result to output.
                ///
                /// Both indices are clamped to [0, size-1]. When both indices resolve
                /// to the same key, that key's value is written directly. The blend
                /// factor is computed as a normalized time ratio between the two keys
                /// and clamped to [0, 1].
                ///
                /// Example:
                ///
                /// \code
                /// float result;
                /// channel.interpolate(0, 1, 0.5f, &result);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param idx_a Index of the first keyframe.
                /// \param idx_b Index of the second keyframe.
                /// \param curr_time_s The time at which to sample (seconds).
                /// \param output Pointer to store the interpolated value.
                ///
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
