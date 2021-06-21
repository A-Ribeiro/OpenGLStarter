#ifndef RTAudioHelper__H
#define RTAudioHelper__H

#include <rtaudio/RtAudio.h>
namespace rtaudioWrapper {
    class RTAudioHelper {
    public:
        /// \brief Print all devices that could be opened by the rtaudio library.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <rtaudio-wrapper/rtaudio-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace rtaudioWrapper;
        ///
        /// RTAudioHelper::printDevices();
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        static void printDevices(RtAudio::Api api = RtAudio::UNSPECIFIED);
    };
}

#endif