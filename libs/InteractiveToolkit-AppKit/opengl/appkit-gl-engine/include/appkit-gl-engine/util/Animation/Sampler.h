#pragma once

#include <appkit-gl-engine/util/Animation/common.h>
#include <appkit-gl-engine/util/Animation/Channel.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Animation
        {
            namespace SMART_DELTA_TYPE
            {
                struct POSITION;
                struct QUATERNION;
            }

            template <typename T>
            class Sampler
            {
            public:
                const Channel<T> *channel_src;
                int64_t idx;
                T value;
                T offset;

                inline void resample(Channel<T> *channel_output, size_t new_sample_count)
                {
                    channel_output->keys.resize(new_sample_count);
                    float start_time_s = 0;
                    float end_time_s = 0;
                    if (channel_src->keys.size() > 0)
                    {
                        int64_t first_idx = channel_src->getIndex(0);
                        int64_t last_idx = channel_src->getIndex((int64_t)channel_src->keys.size() - 1);
                        start_time_s = channel_src->keys[first_idx].time;
                        end_time_s = channel_src->keys[last_idx].time;
                    }

                    float div_factor = 1.0f;
                    if (new_sample_count > 1)
                        div_factor = 1.0f / (float)(new_sample_count - 1);
                    
                    resetForward();
                    for (size_t i = 0; i < new_sample_count; i++)
                    {
                        float time_to_sample;
                        if (i == 0)
                            time_to_sample = start_time_s;
                        else if (i == new_sample_count - 1)
                            time_to_sample = end_time_s;
                        else
                            time_to_sample = MathCore::OP<float>::lerp(start_time_s, end_time_s, (float)i * div_factor);
                        sampleForward(time_to_sample);
                        channel_output->keys[i] = Key<T>(time_to_sample, value);
                    }
                }

                inline void resetForward()
                {
                    idx = 0;
                    channel_src->readValue(idx, &value);
                }

                inline void resetForwardDelta(T *new_value, T *old_value)
                {
                    channel_src->readValue((int64_t)channel_src->keys.size() - 1, new_value);
                    *old_value = value;

                    resetForward();
                }

                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::POSITION>::value,
                              bool>::type = true>
                inline void resetForwardSmartDelta(T *delta)
                {
                    T new_value, old_value;
                    resetForwardDelta(&new_value, &old_value);
                    *delta = new_value - old_value;
                }

                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::QUATERNION>::value &&
                                  std::is_same<typename MathCore::MathTypeInfo<T>::_class, MathCore::MathTypeClass::_class_quat>::value,
                              bool>::type = true>
                inline void resetForwardSmartDelta(T *delta)
                {
                    T new_value, old_value;
                    resetForwardDelta(&new_value, &old_value);
                    *delta = MathCore::OP<T>::inverse(old_value) * new_value;
                }

                inline void resetBackward()
                {
                    if (channel_src->keys.size() == 0)
                        idx = 0;
                    else
                        idx = (int64_t)channel_src->keys.size() - 1;
                    channel_src->readValue(idx, &value);
                }

                inline void resetBackwardDelta(T *new_value, T *old_value)
                {
                    channel_src->readValue(0, new_value);
                    *old_value = value;

                    resetBackward();
                }

                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::POSITION>::value,
                              bool>::type = true>
                inline void resetBackwardSmartDelta(T *delta)
                {
                    T new_value, old_value;
                    resetBackwardDelta(&new_value, &old_value);
                    *delta = new_value - old_value;
                }

                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::QUATERNION>::value &&
                                  std::is_same<typename MathCore::MathTypeInfo<T>::_class, MathCore::MathTypeClass::_class_quat>::value,
                              bool>::type = true>
                inline void resetBackwardSmartDelta(T *delta)
                {
                    T new_value, old_value;
                    resetBackwardDelta(&new_value, &old_value);
                    *delta = MathCore::OP<T>::inverse(old_value) * new_value;
                }

                inline void configure(const Channel<T> *channel_src_, const T &initial_value, const T &offset_value)
                {
                    channel_src = channel_src_;
                    value = initial_value;
                    offset = offset_value;

                    resetForward();
                }

                inline void sampleForward(float current_time_s)
                {
                    idx = channel_src->indexForwardTimeSearch(idx, current_time_s);
                    channel_src->interpolate(idx, idx + 1, current_time_s, &value);
                }

                inline void sampleForwardDelta(float current_time_s, T *new_value_, T *old_value_)
                {
                    T new_value = value;
                    idx = channel_src->indexForwardTimeSearch(idx, current_time_s);
                    channel_src->interpolate(idx, idx + 1, current_time_s, &new_value);
                    *new_value_ = new_value;
                    *old_value_ = value;
                    value = new_value;
                }

                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::POSITION>::value,
                              bool>::type = true>
                inline void sampleForwardSmartDelta(T *delta)
                {
                    T new_value, old_value;
                    sampleForwardDelta(&new_value, &old_value);
                    *delta = new_value - old_value;
                }

                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::QUATERNION>::value &&
                                  std::is_same<typename MathCore::MathTypeInfo<T>::_class, MathCore::MathTypeClass::_class_quat>::value,
                              bool>::type = true>
                inline void sampleForwardSmartDelta(T *delta)
                {
                    T new_value, old_value;
                    sampleForwardDelta(&new_value, &old_value);
                    *delta = MathCore::OP<T>::inverse(old_value) * new_value;
                }

                inline void sampleBackward(float current_time_s)
                {
                    idx = channel_src->indexBackwardTimeSearch(idx, current_time_s);
                    channel_src->interpolate(idx - 1, idx, current_time_s, &value);
                }

                inline void sampleBackwardDelta(float current_time_s, T *new_value_, T *old_value_)
                {
                    T new_value = value;
                    idx = channel_src->indexBackwardTimeSearch(idx, current_time_s);
                    channel_src->interpolate(idx - 1, idx, current_time_s, &new_value);
                    *new_value_ = new_value;
                    *old_value_ = value;
                    value = new_value;
                }

                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::POSITION>::value,
                              bool>::type = true>
                inline void sampleBackwardSmartDelta(T *delta)
                {
                    T new_value, old_value;
                    sampleBackwardDelta(&new_value, &old_value);
                    *delta = new_value - old_value;
                }

                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::QUATERNION>::value &&
                                  std::is_same<typename MathCore::MathTypeInfo<T>::_class, MathCore::MathTypeClass::_class_quat>::value,
                              bool>::type = true>
                inline void sampleBackwardSmartDelta(T *delta)
                {
                    T new_value, old_value;
                    sampleBackwardDelta(&new_value, &old_value);
                    *delta = MathCore::OP<T>::inverse(old_value) * new_value;
                }
            };
        }
    }
}
