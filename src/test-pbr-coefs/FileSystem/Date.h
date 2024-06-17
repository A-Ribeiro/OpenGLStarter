#pragma once

#include <InteractiveToolkit/common.h>
#include <InteractiveToolkit/ITKCommon/StringUtil.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/platform_common.h>

namespace ITKCommon
{

    namespace FileSystem
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

        struct Date
        {

            uint16_t year;
            uint16_t month;
            uint16_t dayOfWeek;
            uint16_t day;
            uint16_t hour;
            uint16_t minute;
            uint16_t second;

            uint32_t nanoseconds;

            inline uint32_t getMilliseconds() const {
                return nanoseconds / 1000000;
            }

            Date()
            {
                year = 0;
                month = 0;
                dayOfWeek = 0;
                day = 0;
                hour = 0;
                minute = 0;
                second = 0;
                nanoseconds = 0;
            }

            Date(uint16_t wYear, uint16_t wMonth,
                 uint16_t wDayOfWeek, uint16_t wDay,
                 uint16_t wHour, uint16_t wMinute,
                 uint16_t wSecond, uint16_t wNanoseconds)
            {
                this->year = wYear;
                this->month = wMonth;
                this->dayOfWeek = wDayOfWeek;
                this->day = wDay;
                this->hour = wHour;
                this->minute = wMinute;
                this->second = wSecond;
                this->nanoseconds = wNanoseconds;
            }

            std::string toString(bool local = true) const
            {
                char result[64];

                struct timespec unix_time = toTimespecUTC();

                tm local_time;

                if (local)
                    localtime_r(&unix_time.tv_sec, &local_time);
                else
                    gmtime_r(&unix_time.tv_sec, &local_time);

                strftime(result, 64, "%F %T", &local_time);

                return result;
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

            struct timespec toTimespecUTC()const {
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

            struct timespec toTimespecLocal()const {
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

            static Date NowUTC() {
                // time elapsed since Jan 1 1970 00:00:00 UTC
                struct timespec res;
                clock_gettime(CLOCK_REALTIME, &res);
                return FromTimeSpecUTC(res);
            }

            static Date NowLocal() {
                return Date::NowUTC().toLocal();
            }

            static Date FromTimeSpecUTC(struct timespec &_ts){
                return FromUnixTimestampUTC(_ts.tv_sec, _ts.tv_nsec);
            }

            static Date FromUnixTimestampUTC(const time_t &unix_time_sec, uint32_t _nsec = 0){
                struct tm utc_time;
                gmtime_r(&unix_time_sec, &utc_time);
                return FromTimeStruct(utc_time, _nsec);
            }

            static Date FromTimeStruct(const struct tm& _time, uint32_t _nsec = 0) {
                return Date(
                    _time.tm_year + 1900,
                    _time.tm_mon + 1,
                    _time.tm_wday,
                    _time.tm_mday,
                    _time.tm_hour,
                    _time.tm_min,
                    _time.tm_sec,
                    _nsec
                );
            }

#if defined(_WIN32)

            static Date FromSystemTime_win32(const SYSTEMTIME& _stime) {
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

}
