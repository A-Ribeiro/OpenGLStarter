#pragma once

#include <InteractiveToolkit/common.h>
// #include <InteractiveToolkit/ITKCommon/StringUtil.h>
// #include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/platform_common.h>

namespace ITKCommon
{

#if defined(_WIN32)

    time_t timegm(struct tm *timeptr)
    {
        return _mkgmtime(timeptr);
    }

    time_t timelocal(struct tm *timeptr)
    {
        time_t utc_time = timegm(timeptr);
        struct tm local_tm;
        localtime_r(&utc_time, &local_tm);
        return timegm(&local_tm);
    }

    struct tm *localtime_r(const time_t *sourceTime, struct tm *tmDest)
    {
        localtime_s(tmDest, sourceTime);
        return tmDest;
    }

    struct tm *gmtime_r(const time_t *sourceTime, struct tm *tmDest)
    {
        gmtime_s(tmDest, sourceTime);
        return tmDest;
    }

#endif

    class Date
    {

    public:
        uint16_t year;
        uint16_t month;
        uint16_t dayOfWeek;
        uint16_t day;
        uint16_t hour;
        uint16_t minute;
        uint16_t second;

        uint32_t nanoseconds;

        Date(uint16_t year = 1970, uint16_t month = 1,
             uint16_t dayOfWeek = 4, uint16_t day = 1,
             uint16_t hour = 0, uint16_t minute = 0,
             uint16_t second = 0, uint32_t nanoseconds = 0)
        {
            this->year = year;
            this->month = month;
            this->dayOfWeek = dayOfWeek;
            this->day = day;
            this->hour = hour;
            this->minute = minute;
            this->second = second;
            this->nanoseconds = nanoseconds;
        }

        inline uint32_t milliseconds() const
        {
            return nanoseconds / 1000000;
        }

        std::string toString(bool convert_to_local = false) const
        {
            char result[64];

            struct timespec unix_time = toTimespecUTC();

            tm local_time;

            if (convert_to_local)
                localtime_r(&unix_time.tv_sec, &local_time);
            else
                gmtime_r(&unix_time.tv_sec, &local_time);

            strftime(result, 64, "%F %T", &local_time);

            return result;
        }

        std::string toISOString() const
        {
            struct timespec unix_time = toTimespecUTC();
            tm utc_time;
            gmtime_r(&unix_time.tv_sec, &utc_time);
            char iso_str[32];
            snprintf(iso_str, 32, "%.4u-%.2u-%.2uT%.2u:%.2u:%.2u.%.3uZ",
                     utc_time.tm_year + 1900,
                     utc_time.tm_mon + 1,
                     utc_time.tm_mday,
                     utc_time.tm_hour,
                     utc_time.tm_min,
                     utc_time.tm_sec,
                     milliseconds());
            return iso_str;
        }

        Date toLocal() const
        {
            struct timespec unix_time = toTimespecUTC();
            struct tm local_time;

            localtime_r(&unix_time.tv_sec, &local_time);

            return Date(
                local_time.tm_year + 1900,
                local_time.tm_mon + 1,
                local_time.tm_wday,
                local_time.tm_mday,
                local_time.tm_hour,
                local_time.tm_min,
                local_time.tm_sec,
                nanoseconds);
        }

        Date toUTC() const
        {
            struct timespec unix_time = toTimespecLocal();
            struct tm utc_time;

            gmtime_r(&unix_time.tv_sec, &utc_time);

            return Date(
                utc_time.tm_year + 1900,
                utc_time.tm_mon + 1,
                utc_time.tm_wday,
                utc_time.tm_mday,
                utc_time.tm_hour,
                utc_time.tm_min,
                utc_time.tm_sec,
                nanoseconds);
        }

        struct timespec toTimespecUTC() const
        {
            struct timespec result;

            struct tm _t;

            _t.tm_year = year - 1900;
            _t.tm_mon = month - 1;
            _t.tm_wday = dayOfWeek;
            _t.tm_mday = day;
            _t.tm_hour = hour;
            _t.tm_min = minute;
            _t.tm_sec = second;

            result.tv_sec = timegm(&_t);
            result.tv_nsec = nanoseconds;

            return result;
        }

        struct timespec toTimespecLocal() const
        {
            struct timespec result;

            struct tm _t;

            _t.tm_year = year - 1900;
            _t.tm_mon = month - 1;
            _t.tm_wday = dayOfWeek;
            _t.tm_mday = day;
            _t.tm_hour = hour;
            _t.tm_min = minute;
            _t.tm_sec = second;

            result.tv_sec = timelocal(&_t);
            result.tv_nsec = nanoseconds;

            return result;
        }

        static Date NowUTC()
        {
            // time elapsed since Jan 1 1970 00:00:00 UTC
            struct timespec res;
            clock_gettime(CLOCK_REALTIME, &res);
            return FromTimeSpecUTC(res);
        }

        static Date NowLocal()
        {
            return Date::NowUTC().toLocal();
        }

        static Date FromTimeSpecUTC(struct timespec &_ts)
        {
            return FromUnixTimestampUTC(_ts.tv_sec, _ts.tv_nsec);
        }

        static Date FromUnixTimestampUTC(const time_t &unix_time_sec, uint32_t _nsec = 0)
        {
            struct tm utc_time;
            gmtime_r(&unix_time_sec, &utc_time);
            return FromTimeStruct(utc_time, _nsec);
        }

        static Date FromTimeStruct(const struct tm &_time, uint32_t _nsec = 0)
        {
            return Date(
                _time.tm_year + 1900,
                _time.tm_mon + 1,
                _time.tm_wday,
                _time.tm_mday,
                _time.tm_hour,
                _time.tm_min,
                _time.tm_sec,
                _nsec);
        }

#if defined(_WIN32)

        static Date FromSystemTime_win32(const SYSTEMTIME &_stime)
        {
            return Date(
                stUTC.wYear,
                stUTC.wMonth,
                stUTC.wDayOfWeek,
                stUTC.wDay,
                stUTC.wHour,
                stUTC.wMinute,
                stUTC.wSecond,
                stUTC.wMilliseconds * 1000000 // nsec
            );
        }
#endif
    };
}
