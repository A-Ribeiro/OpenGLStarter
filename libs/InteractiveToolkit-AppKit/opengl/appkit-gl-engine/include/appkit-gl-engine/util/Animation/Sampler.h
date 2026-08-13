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

            /// \brief Samples an animation Channel forward or backward in time.
            ///
            /// Sampler maintains a cursor over a \c Channel<T> and provides incremental
            /// time-stepping utilities. It is designed for per-frame animation evaluation
            /// where the time advances monotonically (forward) or reverses (backward).
            ///
            /// The sampler tracks the current keyframe index (\c idx), the interpolated
            /// value (\c value), and the last sampled time (\c sampled_time_s). Methods
            /// such as \c sampleForward and \c sampleBackward advance or retreat the
            /// cursor and update \c value accordingly.
            ///
            /// Delta variants (\c *Delta, \c *SmartDelta) expose the change between the
            /// previous and new sampled values, which is useful for computing motion
            /// deltas in physics or networking.
            ///
            /// \author Alessandro Ribeiro
            ///
            /// \tparam T The value type stored in the channel. Typically a scalar,
            /// vector, or quaternion type supported by \c MathCore interpolation.
            ///
            template <typename T>
            class Sampler
            {
            public:
                /// \brief Pointer to the source Channel being sampled.
                ///
                /// Set via \c configure or left null until then.
                ///
                const Channel<T> *channel_src;

                /// \brief Current keyframe index in the source Channel.
                ///
                int64_t idx;

                /// \brief The currently interpolated value.
                ///
                T value;

                /// \brief An offset value applied to sampled results.
                ///
                T offset;

                /// \brief The last time (in seconds) that was sampled.
                ///
                float sampled_time_s;

                /// \brief Construct a Sampler in the default (unconfigured) state.
                ///
                /// All members are zero-initialized: \c channel_src is null, \c idx is 0,
                /// and \c sampled_time_s is 0.
                ///
                /// Example:
                ///
                /// \code
                /// Sampler<float> sampler;
                /// sampler.configure(&channel, 0.0f, 0.0f);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                ///
                Sampler()
                {
                    channel_src = nullptr;
                    idx = 0;
                    value = T();
                    offset = T();
                    sampled_time_s = 0;
                }

                /// \brief Resample the source Channel into \c channel_output with a new keyframe count.
                ///
                /// Produces \p new_sample_count evenly spaced samples between the first and
                /// last keyframe times of the source Channel, writing them into
                /// \c channel_output->keys. The first and last output keys are pinned to
                /// the exact start and end times of the source.
                ///
                /// Example:
                ///
                /// \code
                /// Channel<float> source;
                /// Sampler<float> sampler;
                /// sampler.configure(&source, 0.0f, 0.0f);
                /// Channel<float> result;
                /// sampler.resample(&result, 100);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param channel_output Pointer to the output Channel to fill.
                /// \param new_sample_count Number of keyframes to generate.
                ///
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

                /// \brief Reset the sampler cursor to the first keyframe.
                ///
                /// Sets \c idx to 0 and reads the value and time of the first key.
                ///
                /// Example:
                ///
                /// \code
                /// sampler.resetForward();
                /// float first_value = sampler.value;
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                ///
                inline void resetForward()
                {
                    idx = 0;
                    channel_src->readValue(idx, &value);
                    channel_src->readTime(idx, &sampled_time_s);
                }

                /// \brief Reset to the first keyframe and output the delta from the previous last key.
                ///
                /// Reads the last key's value into \p new_value, stores the current value
                /// into \p old_value, then resets the cursor forward.
                ///
                /// Example:
                ///
                /// \code
                /// float new_val, old_val;
                /// sampler.resetForwardDelta(&new_val, &old_val);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param new_value Pointer to store the last key's value.
                /// \param old_value Pointer to store the current value before reset.
                ///
                inline void resetForwardDelta(T *new_value, T *old_value)
                {
                    channel_src->readValue((int64_t)channel_src->keys.size() - 1, new_value);
                    *old_value = value;

                    resetForward();
                }

                /// \brief Reset forward and compute the positional delta (POSITION specialization).
                ///
                /// Computes \c delta = new_value - old_value.
                ///
                /// Example:
                ///
                /// \code
                /// vec3 delta;
                /// sampler.resetForwardSmartDelta(&delta);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param delta Pointer to store the computed delta.
                ///
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

                /// \brief Reset forward and compute the quaternion delta (QUATERNION specialization).
                ///
                /// Computes \c delta = inverse(old_value) * new_value, representing the
                /// relative rotation from the old to the new orientation.
                ///
                /// Example:
                ///
                /// \code
                /// quat delta;
                /// sampler.resetForwardSmartDelta<SMART_DELTA_TYPE::QUATERNION>(&delta);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param delta Pointer to store the computed delta.
                ///
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

                /// \brief Reset the sampler cursor to the last keyframe.
                ///
                /// Sets \c idx to the last keyframe index and reads its value and time.
                /// No-op (leaves \c idx at 0) when the Channel is empty.
                ///
                /// Example:
                ///
                /// \code
                /// sampler.resetBackward();
                /// float last_value = sampler.value;
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                ///
                inline void resetBackward()
                {
                    if (channel_src->keys.size() == 0)
                        idx = 0;
                    else
                        idx = (int64_t)channel_src->keys.size() - 1;
                    channel_src->readValue(idx, &value);
                    channel_src->readTime(idx, &sampled_time_s);
                }

                /// \brief Reset to the last keyframe and output the delta from the first key.
                ///
                /// Reads the first key's value into \p new_value, stores the current value
                /// into \p old_value, then resets the cursor backward.
                ///
                /// Example:
                ///
                /// \code
                /// float new_val, old_val;
                /// sampler.resetBackwardDelta(&new_val, &old_val);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param new_value Pointer to store the first key's value.
                /// \param old_value Pointer to store the current value before reset.
                ///
                inline void resetBackwardDelta(T *new_value, T *old_value)
                {
                    channel_src->readValue(0, new_value);
                    *old_value = value;

                    resetBackward();
                }

                /// \brief Reset backward and compute the positional delta (POSITION specialization).
                ///
                /// Computes \c delta = new_value - old_value.
                ///
                /// Example:
                ///
                /// \code
                /// vec3 delta;
                /// sampler.resetBackwardSmartDelta(&delta);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param delta Pointer to store the computed delta.
                ///
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

                /// \brief Reset backward and compute the quaternion delta (QUATERNION specialization).
                ///
                /// Computes \c delta = inverse(old_value) * new_value.
                ///
                /// Example:
                ///
                /// \code
                /// quat delta;
                /// sampler.resetBackwardSmartDelta<SMART_DELTA_TYPE::QUATERNION>(&delta);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param delta Pointer to store the computed delta.
                ///
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

                /// \brief Configure the Sampler with a Channel and initial state.
                ///
                /// Sets the source Channel, initial value, and offset, then resets the
                /// cursor to the first keyframe.
                ///
                /// Example:
                ///
                /// \code
                /// Channel<float> channel;
                /// Sampler<float> sampler;
                /// sampler.configure(&channel, 0.0f, 0.0f);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param channel_src_ Pointer to the source Channel.
                /// \param initial_value The initial value to set before resetting.
                /// \param offset_value The offset value to store.
                ///
                inline void configure(const Channel<T> *channel_src_, const T &initial_value, const T &offset_value)
                {
                    channel_src = channel_src_;
                    value = initial_value;
                    offset = offset_value;

                    resetForward();
                }

                /// \brief Advance the sampler to \p current_time_s using forward interpolation.
                ///
                /// If \p current_time_s is not ahead of \c sampled_time_s, this is a no-op.
                /// Otherwise, searches forward for the appropriate keyframe pair and
                /// interpolates the value.
                ///
                /// Example:
                ///
                /// \code
                /// sampler.configure(&channel, 0.0f, 0.0f);
                /// sampler.sampleForward(1.5f);
                /// float value = sampler.value;
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param current_time_s The time to sample (seconds).
                ///
                inline void sampleForward(float current_time_s)
                {
                    if (current_time_s <= sampled_time_s)
                        return;
                    idx = channel_src->indexForwardTimeSearch(idx, current_time_s);
                    channel_src->interpolate(idx, idx + 1, current_time_s, &value);
                    sampled_time_s = current_time_s;
                }

                /// \brief Advance the sampler to \p current_time_s and output both values.
                ///
                /// Advances the cursor forward if \p current_time_s is ahead of
                /// \c sampled_time_s, then writes the new sampled value to \p new_value_
                /// and the previous value to \p old_value_. This is useful when the
                /// delta between frames is needed alongside the new value.
                ///
                /// Example:
                ///
                /// \code
                /// float new_val, old_val;
                /// sampler.sampleForwardDelta(1.5f, &new_val, &old_val);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param current_time_s The time to sample (seconds).
                /// \param new_value_ Pointer to store the new sampled value.
                /// \param old_value_ Pointer to store the previous value.
                ///
                inline void sampleForwardDelta(float current_time_s, T *new_value_, T *old_value_)
                {
                    if (current_time_s <= sampled_time_s)
                        return;
                    T new_value = value;
                    idx = channel_src->indexForwardTimeSearch(idx, current_time_s);
                    channel_src->interpolate(idx, idx + 1, current_time_s, &new_value);
                    *new_value_ = new_value;
                    *old_value_ = value;
                    value = new_value;
                    sampled_time_s = current_time_s;
                }

                /// \brief Advance forward and compute the positional delta (POSITION specialization).
                ///
                /// Advances the cursor forward to \p current_time_s, then computes and
                /// stores the positional delta (\c new_value - old_value) into \p delta.
                ///
                /// Example:
                ///
                /// \code
                /// vec3 delta;
                /// sampler.sampleForwardSmartDelta(1.5f, &delta);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param current_time_s The time to sample (seconds).
                /// \param delta Pointer to store the computed delta.
                ///
                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::POSITION>::value,
                              bool>::type = true>
                inline void sampleForwardSmartDelta(float current_time_s, T *delta)
                {
                    T new_value, old_value;
                    sampleForwardDelta(current_time_s, &new_value, &old_value);
                    *delta = new_value - old_value;
                }

                /// \brief Advance forward and compute the quaternion delta (QUATERNION specialization).
                ///
                /// Advances the cursor forward to \p current_time_s, then computes and
                /// stores the quaternion delta (\c inverse(old_value) * new_value) into
                /// \p delta, representing the relative rotation from the old to the new
                /// orientation.
                ///
                /// Example:
                ///
                /// \code
                /// quat delta;
                /// sampler.sampleForwardSmartDelta<SMART_DELTA_TYPE::QUATERNION>(1.5f, &delta);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param current_time_s The time to sample (seconds).
                /// \param delta Pointer to store the computed delta.
                ///
                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::QUATERNION>::value &&
                                  std::is_same<typename MathCore::MathTypeInfo<T>::_class, MathCore::MathTypeClass::_class_quat>::value,
                              bool>::type = true>
                inline void sampleForwardSmartDelta(float current_time_s, T *delta)
                {
                    T new_value, old_value;
                    sampleForwardDelta(current_time_s, &new_value, &old_value);
                    *delta = MathCore::OP<T>::inverse(old_value) * new_value;
                }

                /// \brief Retreat the sampler to \p current_time_s using backward interpolation.
                ///
                /// If \p current_time_s is not behind \c sampled_time_s, this is a no-op.
                /// Otherwise, searches backward for the appropriate keyframe pair and
                /// interpolates the value.
                ///
                /// Example:
                ///
                /// \code
                /// sampler.sampleBackward(0.5f);
                /// float value = sampler.value;
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param current_time_s The time to sample (seconds).
                ///
                inline void sampleBackward(float current_time_s)
                {
                    if (current_time_s >= sampled_time_s)
                        return;
                    idx = channel_src->indexBackwardTimeSearch(idx, current_time_s);
                    channel_src->interpolate(idx - 1, idx, current_time_s, &value);
                    sampled_time_s = current_time_s;
                }

                /// \brief Retreat the sampler to \p current_time_s and output both values.
                ///
                /// Retreats the cursor backward if \p current_time_s is behind
                /// \c sampled_time_s, then writes the new sampled value to \p new_value_
                /// and the previous value to \p old_value_. This is useful when the
                /// delta between frames is needed alongside the new value.
                ///
                /// Example:
                ///
                /// \code
                /// float new_val, old_val;
                /// sampler.sampleBackwardDelta(0.5f, &new_val, &old_val);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param current_time_s The time to sample (seconds).
                /// \param new_value_ Pointer to store the new sampled value.
                /// \param old_value_ Pointer to store the previous value.
                ///
                inline void sampleBackwardDelta(float current_time_s, T *new_value_, T *old_value_)
                {
                    if (current_time_s >= sampled_time_s)
                        return;
                    T new_value = value;
                    idx = channel_src->indexBackwardTimeSearch(idx, current_time_s);
                    channel_src->interpolate(idx - 1, idx, current_time_s, &new_value);
                    *new_value_ = new_value;
                    *old_value_ = value;
                    value = new_value;
                    sampled_time_s = current_time_s;
                }

                /// \brief Retreat backward and compute the positional delta (POSITION specialization).
                ///
                /// Retreats the cursor backward to \p current_time_s, then computes and
                /// stores the positional delta (\c new_value - old_value) into \p delta.
                ///
                /// Example:
                ///
                /// \code
                /// vec3 delta;
                /// sampler.sampleBackwardSmartDelta(0.5f, &delta);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param current_time_s The time to sample (seconds).
                /// \param delta Pointer to store the computed delta.
                ///
                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::POSITION>::value,
                              bool>::type = true>
                inline void sampleBackwardSmartDelta(float current_time_s, T *delta)
                {
                    T new_value, old_value;
                    sampleBackwardDelta(current_time_s, &new_value, &old_value);
                    *delta = new_value - old_value;
                }

                /// \brief Retreat backward and compute the quaternion delta (QUATERNION specialization).
                ///
                /// Retreats the cursor backward to \p current_time_s, then computes and
                /// stores the quaternion delta (\c inverse(old_value) * new_value) into
                /// \p delta, representing the relative rotation from the old to the new
                /// orientation.
                ///
                /// Example:
                ///
                /// \code
                /// quat delta;
                /// sampler.sampleBackwardSmartDelta<SMART_DELTA_TYPE::QUATERNION>(0.5f, &delta);
                /// \endcode
                ///
                /// \author Alessandro Ribeiro
                /// \param current_time_s The time to sample (seconds).
                /// \param delta Pointer to store the computed delta.
                ///
                template <typename DeltaType = SMART_DELTA_TYPE::POSITION,
                          typename std::enable_if<
                              std::is_same<DeltaType, SMART_DELTA_TYPE::QUATERNION>::value &&
                                  std::is_same<typename MathCore::MathTypeInfo<T>::_class, MathCore::MathTypeClass::_class_quat>::value,
                              bool>::type = true>
                inline void sampleBackwardSmartDelta(float current_time_s, T *delta)
                {
                    T new_value, old_value;
                    sampleBackwardDelta(current_time_s, &new_value, &old_value);
                    *delta = MathCore::OP<T>::inverse(old_value) * new_value;
                }
            };
        }
    }
}
