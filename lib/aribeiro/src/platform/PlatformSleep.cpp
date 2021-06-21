#include "PlatformSleep.h"
#include "PlatformTime.h"



namespace aRibeiro {

    void PlatformSleep::sleepSec(int secs) {
        sleepMillis(secs * 1000);
    }

    void PlatformSleep::sleepMillis(int millis) {
#if defined(OS_TARGET_win)
        Sleep(millis);
#else
        usleep(millis * 1000);
#endif
    }

    void PlatformSleep::busySleepMicro(int64_t micros) {

        /*
        int64_t micro = 0;
        PlatformTime time;
        do {
            time.update();
            micro += time.deltaTimeMicro;
        } while (micro < micros);
        */


#if defined(OS_TARGET_win)

        w32PerformanceCounter counter;
        while (counter.GetCounterMicro(false) < micros)
        {

        }

#else

        usleep(micros);

#endif

    }

}
