#ifndef platform_sleep_h
#define platform_sleep_h

#include <aribeiro/common.h>
#include "PlatformTime.h"

namespace aRibeiro {

    /// \brief Sleep implementation in several platforms.
    ///
    /// Can be used with PlatformThread to make some thread sleep and avoid 100% CPU usage for example.
    ///
    /// \author Alessandro Ribeiro
    ///
    class PlatformSleep {
    public:

        /// \brief Sleep for an amount of seconds.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_function() {
        ///     ...
        ///     PlatformSleep::sleepSec( 1 );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param secs seconds
        ///
        static void sleepSec(int secs);

        /// \brief Sleep for an amount of milliseconds.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_function() {
        ///     ...
        ///     PlatformSleep::sleepMillis( 1 );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param millis milliseconds
        ///
        static void sleepMillis(int millis);

        /// \brief Sleep for an amount of microseconds.
        ///
        /// \warning This method consumes 100% CPU while sleeping when it is in windows OS.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_function() {
        ///     ...
        ///     // 0.1 millisecond sleep
        ///     PlatformSleep::busySleepMicro( 100 );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param micros microseconds
        ///
        static void busySleepMicro(int64_t micros);
    };

}

#endif