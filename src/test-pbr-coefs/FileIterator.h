#pragma once

#include <InteractiveToolkit/common.h>
#include <InteractiveToolkit/ITKCommon/StringUtil.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/platform_common.h>

#if defined(_WIN32)

#ifndef timegm
    #define timegm _mkgmtime
#endif

#ifndef localtime_r
#define localtime_r(a,b) localtime_s(b,a)
#endif

#ifndef gmtime_r
#define gmtime_r(a,b) gmtime_s(b,a)
#endif

#endif

// UTC Date Information
struct Date
{

    uint16_t year;
    uint16_t month;
    uint16_t dayOfWeek;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
    uint16_t milliseconds;

    Date()
    {
        year = 0;
        month = 0;
        dayOfWeek = 0;
        day = 0;
        hour = 0;
        minute = 0;
        second = 0;
        milliseconds = 0;
    }

    Date(uint16_t wYear, uint16_t wMonth,
         uint16_t wDayOfWeek, uint16_t wDay,
         uint16_t wHour, uint16_t wMinute,
         uint16_t wSecond, uint16_t wMilliseconds)
    {
        this->year = wYear;
        this->month = wMonth;
        this->dayOfWeek = wDayOfWeek;
        this->day = wDay;
        this->hour = wHour;
        this->minute = wMinute;
        this->second = wSecond;
        this->milliseconds = wMilliseconds;
    }

    time_t toUnixTimeSec() const{
        struct tm _t;

        _t.tm_year = year - 1900;
        _t.tm_mon = month - 1;
        _t.tm_wday = dayOfWeek;
        _t.tm_mday = day;
        _t.tm_hour = hour;
        _t.tm_min = minute;
        _t.tm_sec = second;
        //sb.stx_mtime.tv_nsec/1000000);

        return timegm(&_t);
    }

    std::string toString(bool local=true) const
    {
        char result[64];

        time_t unix_time = toUnixTimeSec();
        tm local_time;
        
        if (local)
            localtime_r(&unix_time, &local_time);
        else
            gmtime_r(&unix_time, &local_time);
        
        strftime(result,64,"%F %T",&local_time);

        return result;
    }

    Date toLocal() const {

        time_t unix_time = toUnixTimeSec();
        tm local_time;

        localtime_r(&unix_time, &local_time);

        return Date(
            local_time.tm_year + 1900,
            local_time.tm_mon + 1,
            local_time.tm_wday,
            local_time.tm_mday,
            local_time.tm_hour,
            local_time.tm_min,
            local_time.tm_sec,
            milliseconds
        );
    }
};

struct FileInfo
{
    std::string base_path;
    std::string full_path;
    std::string name;
    bool isDirectory;
    bool isFile;

    Date creationTime;
    Date lastWriteTime;

    uint64_t size;

    FileInfo()
    {
        isDirectory = false;
        isFile = false;
        size = UINT64_C(0);
    }
};

class Directory
{
public:
    struct const_iterator
    {
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = FileInfo;
        using pointer = const FileInfo *;
        using reference = const FileInfo &;

        const_iterator()
        {
#if defined(_WIN32)
            memset(&findfiledata, 0, sizeof(WIN32_FIND_DATAW));
            hFind = INVALID_HANDLE_VALUE;
#elif defined(__APPLE__) || defined(__linux__)
            entry = NULL;
            dp = NULL;
#endif
        }

        const_iterator(const std::string &base_path)
        {
#if defined(_WIN32)
            memset(&findfiledata, 0, sizeof(WIN32_FIND_DATAW));
            hFind = INVALID_HANDLE_VALUE;

            std::wstring w_base_path = ITKCommon::StringUtil::string_to_WString(base_path + "*");
            hFind = FindFirstFileW((LPCWSTR)w_base_path.c_str(), &findfiledata);
            if (hFind != INVALID_HANDLE_VALUE)
            {
                fileInfo.base_path = base_path;
                processCurrentValue(true);
            }
            // else {
            //	printf("%s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
            // }
#elif defined(__APPLE__) || defined(__linux__)
            entry = NULL;
            dp = NULL;

            dp = opendir(base_path.c_str());
            if (dp != NULL){
                entry = readdir(dp);
                fileInfo.base_path = base_path;
                processCurrentValue( entry != NULL );
            }

#endif
        }

        ~const_iterator()
        {
#if defined(_WIN32)
            if (hFind != INVALID_HANDLE_VALUE)
            {
                FindClose(hFind);
                hFind = INVALID_HANDLE_VALUE;
            }
#elif defined(__APPLE__) || defined(__linux__)
            if (dp != NULL){
                closedir(dp);
                dp = NULL;
                entry = NULL;
            }
#endif
        }

        const reference operator*() const { return fileInfo; }
        const pointer operator->() const { return &fileInfo; }

        // Prefix increment
        const_iterator &operator++()
        {
#if defined(_WIN32)
            if (hFind == INVALID_HANDLE_VALUE)
            {
                fileInfo = FileInfo();
                return *this;
            }

            bool next_valid = FindNextFileW(hFind, &findfiledata) == TRUE;
            processCurrentValue(next_valid);
#elif defined(__APPLE__) || defined(__linux__)
            if (dp == NULL){
                fileInfo = FileInfo();
                return *this;
            }
            
            entry = readdir(dp);
            bool next_valid = entry != NULL;
            processCurrentValue(next_valid);
#endif
            return *this;
        }

        // Postfix increment
        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const const_iterator &a, const const_iterator &b)
        {
            return
#if defined(_WIN32)
                a.hFind == b.hFind &&
#elif defined(__APPLE__) || defined(__linux__)
                a.dp == b.dp &&
#endif
                a.fileInfo.full_path.compare(b.fileInfo.full_path) == 0;
        }
        friend bool operator!=(const const_iterator &a, const const_iterator &b)
        {
            return !(a == b);
        }

    private:
        void processCurrentValue(bool next_valid)
        {
#if defined(_WIN32)

            if (hFind == INVALID_HANDLE_VALUE)
                return;

            // skip . and ..
            while (next_valid &&
                   (findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
                   (wcscmp(findfiledata.cFileName, L".") == 0 ||
                    wcscmp(findfiledata.cFileName, L"..") == 0))
            {
                next_valid = FindNextFileW(hFind, &findfiledata) == TRUE;
            }

            if (!next_valid)
            {
                FindClose(hFind);
                hFind = INVALID_HANDLE_VALUE;
                fileInfo = FileInfo();
            }
            else
            {
                fileInfo.isDirectory = (findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                fileInfo.isFile = !fileInfo.isDirectory;
                fileInfo.name = ITKCommon::StringUtil::wString_to_String(findfiledata.cFileName);
                fileInfo.full_path = fileInfo.base_path + fileInfo.name;
                if (fileInfo.isDirectory)
                    fileInfo.full_path += "/";

                // date processing
                SYSTEMTIME stUTC;//, stLocal;
                FileTimeToSystemTime(&findfiledata.ftLastWriteTime, &stUTC);
                //SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
                fileInfo.lastWriteTime = Date(
                    stUTC.wYear,
                    stUTC.wMonth,
                    stUTC.wDayOfWeek,
                    stUTC.wDay,
                    stUTC.wHour,
                    stUTC.wMinute,
                    stUTC.wSecond,
                    stUTC.wMilliseconds);

                FileTimeToSystemTime(&findfiledata.ftCreationTime, &stUTC);
                //SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
                fileInfo.creationTime = Date(
                    stUTC.wYear,
                    stUTC.wMonth,
                    stUTC.wDayOfWeek,
                    stUTC.wDay,
                    stUTC.wHour,
                    stUTC.wMinute,
                    stUTC.wSecond,
                    stUTC.wMilliseconds);

                fileInfo.size =
                    ((uint64_t)findfiledata.nFileSizeHigh << 32) | (uint64_t)findfiledata.nFileSizeLow & UINT64_C(0xffffffff);
            }
#elif defined(__APPLE__) || defined(__linux__)
            if (dp == NULL)
                return;

            // skip . and ..
            struct statx sb;
            int dirfd = AT_FDCWD;
            //unsigned int mask = STATX_ALL;
            unsigned int mask = STATX_MODE | STATX_MTIME | STATX_SIZE | STATX_BTIME;
            int flags = AT_SYMLINK_NOFOLLOW;
            bool stat_success = false;

            if (next_valid) {

                fileInfo.full_path = fileInfo.base_path + entry->d_name;
                stat_success = statx(dirfd, fileInfo.full_path.c_str(), flags, mask, &sb) == 0;

                // read next until a valid stat file stated
                while (next_valid && !stat_success) {
                    //printf("cannot stat: %s\n", entry->d_name);
                    entry = readdir(dp);
                    next_valid = entry != NULL;
                    if (next_valid) {
                        fileInfo.full_path = fileInfo.base_path + entry->d_name;
                        stat_success = statx(dirfd, fileInfo.full_path.c_str(), flags, mask, &sb) == 0;
                    }
                }

            }

            while (next_valid &&
                   (sb.stx_mode & S_IFDIR != 0) &&
                   (strcmp(entry->d_name, ".") == 0 ||
                    strcmp(entry->d_name, "..") == 0))
            {
                entry = readdir(dp);
                next_valid = entry != NULL;
                if (next_valid) {
                    fileInfo.full_path = fileInfo.base_path + entry->d_name;
                    stat_success = statx(dirfd, fileInfo.full_path.c_str(), flags, mask, &sb) == 0;
                    
                    // read next until a valid stat file stated
                    while (next_valid && !stat_success) {
                        //printf("cannot stat: %s\n", entry->d_name);
                        entry = readdir(dp);
                        next_valid = entry != NULL;
                        if (next_valid) {
                            fileInfo.full_path = fileInfo.base_path + entry->d_name;
                            stat_success = statx(dirfd, fileInfo.full_path.c_str(), flags, mask, &sb) == 0;
                        }
                    }
                }
            }

            if (!next_valid || !stat_success)
            {
                closedir(dp);
                dp = NULL;
                entry = NULL;
                fileInfo = FileInfo();
            } else {
                // use sb to fill the file properties
                fileInfo.isDirectory = sb.stx_mode & S_IFDIR;
                fileInfo.isFile = !fileInfo.isDirectory;
                fileInfo.name = entry->d_name;
                //fileInfo.full_path = fileInfo.base_path + fileInfo.name;
                if (fileInfo.isDirectory)
                    fileInfo.full_path += "/";

                // date processing
                struct tm localTime;
                //localtime_r((const time_t*)&sb.stx_mtime.tv_sec,&localTime);
                gmtime_r((const time_t*)&sb.stx_mtime.tv_sec,&localTime);
                fileInfo.lastWriteTime = Date(
                    localTime.tm_year + 1900,
                    localTime.tm_mon + 1,
                    localTime.tm_wday,
                    localTime.tm_mday,
                    localTime.tm_hour,
                    localTime.tm_min,
                    localTime.tm_sec,
                    sb.stx_mtime.tv_nsec/1000000);

                // gmtime
                //localtime_r((const time_t*)&sb.stx_btime.tv_sec,&localTime);
                gmtime_r((const time_t*)&sb.stx_btime.tv_sec,&localTime);

                fileInfo.creationTime = Date(
                    localTime.tm_year + 1900,
                    localTime.tm_mon + 1,
                    localTime.tm_wday,
                    localTime.tm_mday,
                    localTime.tm_hour,
                    localTime.tm_min,
                    localTime.tm_sec,
                    sb.stx_btime.tv_nsec/1000000);

                fileInfo.size = (uint64_t)sb.stx_size;
            }
#endif
        }

        const_iterator(const const_iterator &v)
        {
#if defined(_WIN32)
            memset(&findfiledata, 0, sizeof(WIN32_FIND_DATAW));
            hFind = INVALID_HANDLE_VALUE;
#elif defined(__APPLE__) || defined(__linux__)
            entry = NULL;
            dp = NULL;
#endif

            fileInfo = v.fileInfo;
        }
        void operator=(const const_iterator &v)
        {
#if defined(_WIN32)
            memset(&findfiledata, 0, sizeof(WIN32_FIND_DATAW));
            hFind = INVALID_HANDLE_VALUE;
#elif defined(__APPLE__) || defined(__linux__)
            entry = NULL;
            dp = NULL;
#endif

            fileInfo = v.fileInfo;
        }


        const_iterator(const_iterator&& v) noexcept
        {
#if defined(_WIN32)
            findfiledata = v.findfiledata;
            hFind = v.hFind;
#elif defined(__APPLE__) || defined(__linux__)
            entry = v.entry;
            dp = v.dp;
#endif
            fileInfo = v.fileInfo;


#if defined(_WIN32)
            memset(&v.findfiledata, 0, sizeof(WIN32_FIND_DATAW));
            v.hFind = INVALID_HANDLE_VALUE;
#elif defined(__APPLE__) || defined(__linux__)
            v.entry = NULL;
            v.dp = NULL;
#endif
            v.fileInfo = FileInfo();
        }

        void operator=(const_iterator&& v) noexcept
        {
#if defined(_WIN32)
            findfiledata = v.findfiledata;
            hFind = v.hFind;
#elif defined(__APPLE__) || defined(__linux__)
            entry = v.entry;
            dp = v.dp;
#endif
            fileInfo = v.fileInfo;


#if defined(_WIN32)
            memset(&v.findfiledata, 0, sizeof(WIN32_FIND_DATAW));
            v.hFind = INVALID_HANDLE_VALUE;
#elif defined(__APPLE__) || defined(__linux__)
            v.entry = NULL;
            v.dp = NULL;
#endif
            v.fileInfo = FileInfo();
        }

#if defined(_WIN32)
        WIN32_FIND_DATAW findfiledata;
        HANDLE hFind;
#elif defined(__APPLE__) || defined(__linux__)
        struct dirent *entry;
        DIR *dp;
#endif

        FileInfo fileInfo;
    };

    const_iterator begin() const { return const_iterator(base_path); }
    const_iterator end() const { return const_iterator(); }

    std::string base_path;

    Directory(const std::string &base_path = "./", bool keep_base_path_relative = false)
    {
        this->base_path = base_path;
        if (!keep_base_path_relative)
            this->base_path = ITKCommon::Path::getAbsolutePath(this->base_path);
        ITKCommon::StringUtil::replaceAll(&this->base_path, "\\", "/");
        //ITKCommon::StringUtil::replaceAll(&this->base_path, "/", "/");
        if (!ITKCommon::StringUtil::endsWith(this->base_path, "/"))
            this->base_path += "/";
    }
};
