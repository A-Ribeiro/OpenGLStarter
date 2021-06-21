#include "PlatformMutex.h"

namespace aRibeiro {

    //private copy constructores, to avoid copy...
    PlatformMutex::PlatformMutex(const PlatformMutex& v) {

    }
    void PlatformMutex::operator=(const PlatformMutex& v) {

    }

#if defined(OS_TARGET_win)

    PlatformMutex::PlatformMutex() {
        InitializeCriticalSection(&mLock);
    }

    PlatformMutex::~PlatformMutex() {
        DeleteCriticalSection(&mLock);
    }

    void PlatformMutex::lock() {
        EnterCriticalSection(&mLock);
    }

    void PlatformMutex::unlock() {
        LeaveCriticalSection(&mLock);
    }

#elif defined(OS_TARGET_linux) || defined(OS_TARGET_mac)

    PlatformMutex::PlatformMutex() {
        // Set it recursive
        pthread_mutexattr_t attributes;
        pthread_mutexattr_init(&attributes);
        pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(&mLock, &attributes);
    }

    PlatformMutex::~PlatformMutex() {
        pthread_mutex_destroy(&mLock);
    }

    void PlatformMutex::lock() {
        pthread_mutex_lock(&mLock);
    }

    void PlatformMutex::unlock() {
        pthread_mutex_unlock(&mLock);
    }

#else
#error Mutex for this platform is not implemented...
#endif

}
