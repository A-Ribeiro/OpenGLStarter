#include "PlatformTime.h"

#include <stdio.h>

#if defined(OS_TARGET_win)

namespace aRibeiro {

    double w32PerformanceCounter::freqMicro;
    double w32PerformanceCounter::freqMillis;
    uint64_t w32PerformanceCounter::freqSec;
    bool w32PerformanceCounter::filledFreqConstants = false;

    w32PerformanceCounter::w32PerformanceCounter()
    {
        if (!filledFreqConstants) {

            LARGE_INTEGER PCFreq;

            QueryPerformanceFrequency(&PCFreq);
            freqSec = PCFreq.QuadPart;
            freqMicro = (double)freqSec / 1000000.0;
            freqMillis = (double)freqSec / 1000.0;

            //printf("frequencySeconds: %I64d\n", freqSec);

            filledFreqConstants = true;
        }

        QueryPerformanceCounter(&CounterStart);
        CounterStartBackup = CounterStart;
        //CounterStart = li.QuadPart;
    }

    void w32PerformanceCounter::UndoReset() {
        CounterStart = CounterStartBackup;
    }

    void w32PerformanceCounter::Reset() {
        CounterStartBackup = CounterStart;
        QueryPerformanceCounter(&CounterStart);
    }
    /*
    uint64_t w32PerformanceCounter::GetCounterMillis(bool reset)
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);

        uint64_t result = li.QuadPart - CounterStart.QuadPart;

        if (reset) {
            CounterStartBackup = CounterStart;
            CounterStart = li;
        }

        return (uint64_t)((double)result / freqMillis);
    }
    */


    int64_t w32PerformanceCounter::GetCounterMicro(bool reset)
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);

        int64_t result = li.QuadPart - CounterStart.QuadPart;

        int64_t micros = (int64_t)((double)result / freqMicro);

        if (reset) {
            CounterStartBackup = CounterStart;
            //CounterStart = li;

            CounterStart.QuadPart += (int64_t)((double)micros * freqMicro);
        }

        return micros;
    }

    /*
    __int64 w32PerformanceCounter::GetCounterSec(bool reset)
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);

        uint64_t result = li.QuadPart - CounterStart.QuadPart;

        if (reset) {
            CounterStartBackup = CounterStart;
            CounterStart = li;
        }

        return result / freqSec;
    }
    */

}

#else

namespace aRibeiro {

    double UnixMicroCounter::now_microseconds_double(void) {

#ifdef __APPLE__

        uint64_t result = mach_absolute_time();
        double resultd = (((double)result * (double)info.numer) / ((double)info.denom)) / 1000.0;
        return resultd;

#else

        struct timespec res;
        clock_gettime(CLOCK_MONOTONIC, &res);
        return (((double)res.tv_sec)*1000000.0 + ((double)res.tv_nsec)*0.001);

#endif

    }

    uint64_t UnixMicroCounter::now_microseconds_uint64_t(void) {

#ifdef __APPLE__

        uint64_t result = mach_absolute_time();
        result *= info.numer;
        result /= info.denom;
        result = result / 1000;
        return result;

#else

        struct timespec res;
        clock_gettime(CLOCK_MONOTONIC, &res);
        return ( ((uint64_t)res.tv_sec) * 1000000ULL + ( ((uint64_t)res.tv_nsec) / 1000ULL ) % 1000000ULL );

#endif
    }

    UnixMicroCounter::UnixMicroCounter() {

#if __APPLE__
        mach_timebase_info(&info);
#endif

        base = now_microseconds_uint64_t();

    }

    int64_t UnixMicroCounter::GetDeltaMicro(bool reset) {
        uint64_t newbase = now_microseconds_uint64_t();
        int64_t delta = newbase - base;

        if (reset)
            base = newbase;

        return delta;
    }

}

#endif

namespace aRibeiro {

    PlatformTime::PlatformTime() {
        deltaTime = 0.0f;
        unscaledDeltaTime = 0.0f;
        timeScale = 1.0f;
        deltaTimeMicro = 0;
    }

    void PlatformTime::update() {

#if defined(OS_TARGET_win)
        deltaTimeMicro = win32_counter.GetCounterMicro(true);
        if (deltaTimeMicro == 0) {
            win32_counter.UndoReset();
        }
#else
        deltaTimeMicro = unix_counter.GetDeltaMicro(true);
#endif

        if (deltaTimeMicro < 0) {
            printf("Time error...Negative Delta Time... \n");
            //fgetc(stdin);
        }

        double sec = (double)deltaTimeMicro / 1000000.0;

        unscaledDeltaTime = (float)(sec);
        deltaTime = (float)(sec*(double)timeScale);
    }
    
    void PlatformTime::reset() {
        unscaledDeltaTime = 0;
        deltaTime = 0;
        deltaTimeMicro = 0;
        
#if defined(OS_TARGET_win)
        win32_counter.GetCounterMicro(true);
#else
        unix_counter.GetDeltaMicro(true);
#endif
        
    }

}
