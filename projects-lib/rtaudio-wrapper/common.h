/// \file
#ifndef RTAUDIO_COMMON_H_
#define RTAUDIO_COMMON_H_

#include <string.h> // memcpy
#include "int_lerp.h"
#include "int24_t.h"
#include "uint32_gcd.h"

#include <aribeiro/aribeiro.h>
//using namespace aRibeiro;

namespace rtaudioWrapper {
    /// \class RTAUDIO_OnAudioDataMethodPtrT
    /// \brief Callback pattern with samples array and amount of frames.
    ///
    /// Each frame in this callback could have the number of channels as one element in the data array.
    ///
    /// For example: 2 channels float buffer --> in this situation each frame is composed by a 4 * 2 bytes.
    ///
    /// 4 bytes is the float number and the 2 is the amount of channels related to this frame.
    ///
    /// Example of use with functions:
    ///
    /// \code
    ///    void callbackFunction(const void* data, uint32_t frames){
    ///        ...
    ///    }
    ///
    ///    RTAUDIO_OnAudioDataMethodPtrT OnData;
    ///
    ///    OnData = &callbackFunction;
    ///
    ///    int channels = 2;
    ///    float data[1024*channels];
    ///    uint32_t frames = 1024 * channels;
    ///
    ///    ...
    ///
    ///    if (OnData != NULL)
    ///        OnData(data,frames);
    /// \endcode
    ///
    /// Example of use with method:
    ///
    /// \code
    ///    class ExampleClass {
    ///    public:
    ///        void callbackFunction(const void* data, uint32_t frames){
    ///            ...
    ///        }
    ///    };
    ///
    ///    ExampleClass obj;
    ///
    ///    RTAUDIO_OnAudioDataMethodPtrT OnData;
    ///
    ///    OnData = RTAUDIO_OnAudioDataMethodPtrT( &obj, &ExampleClass::callbackFunction );
    ///
    ///    int channels = 2;
    ///    float data[1024*channels];
    ///    uint32_t frames = 1024 * channels;
    ///
    ///    ...
    ///
    ///    if (OnData != NULL)
    ///        OnData(data,frames);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    DefineMethodPointer(RTAUDIO_OnAudioDataMethodPtrT, void, const void* data, uint32_t frames) VoidMethodCall(data, frames)
}

#endif
