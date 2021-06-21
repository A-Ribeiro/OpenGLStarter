#ifndef PLATFORM_MUTEX_H
#define PLATFORM_MUTEX_H

#include <aribeiro/common.h>


#if defined(OS_TARGET_win)
    #define WIN32_LEAN_AND_MEAN

    #include <WinSock2.h>
    #include <WS2tcpip.h>

    #include <windows.h>
    typedef CRITICAL_SECTION MutexObj;
#elif defined(OS_TARGET_linux) || defined(OS_TARGET_mac)
    #include <pthread.h>
    typedef pthread_mutex_t MutexObj;
#else
    #error Mutex for this platform is not implemented...
#endif

namespace aRibeiro {

    /// \brief Mutex implementation.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// PlatformMutex mutex;
    ///
    /// // critical section begin 
    /// mutex.lock();
    /// ...
    /// // critical section end
    /// mutex.unlock();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class PlatformMutex {
    private:
        MutexObj mLock;

        //private copy constructores, to avoid copy...
        PlatformMutex(const PlatformMutex& v);
        void operator=(const PlatformMutex& v);

    public:

        /// \brief Construct the mutex in the current platform (windows, linux, mac)
        ///
        /// \author Alessandro Ribeiro
        ///
        PlatformMutex();

        /// \brief Destroy the mutex
        ///
        /// \author Alessandro Ribeiro
        ///
        virtual ~PlatformMutex();

        /// \brief Lock the critical section
        ///
        /// \author Alessandro Ribeiro
        ///
        void lock();

        /// \brief Unlock the critical section
        ///
        /// \author Alessandro Ribeiro
        ///
        void unlock();
    };

}

#endif