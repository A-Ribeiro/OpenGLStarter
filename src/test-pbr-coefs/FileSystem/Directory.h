#pragma once

#include <InteractiveToolkit/common.h>
#include <InteractiveToolkit/ITKCommon/StringUtil.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/platform_common.h>

#include "../Date.h"
#include "File.h"

namespace ITKCommon
{
    namespace FileSystem
    {
        class Directory
        {
        public:
            struct const_iterator
            {
                using iterator_category = std::input_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = FileSystem::File;
                using pointer = const FileSystem::File *;
                using reference = const FileSystem::File &;

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
                    if (dp != NULL)
                    {
                        entry = readdir(dp);
                        fileInfo.base_path = base_path;
                        processCurrentValue(entry != NULL);
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
                    if (dp != NULL)
                    {
                        closedir(dp);
                        dp = NULL;
                        entry = NULL;
                    }
#endif
                }

                const reference operator*() const { return fileInfo; }
                const pointer operator->() const { return &fileInfo; }

                // Postfix increment
                const_iterator &operator++(int)
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
                    if (dp == NULL)
                    {
                        fileInfo = value_type();
                        return *this;
                    }

                    entry = readdir(dp);
                    bool next_valid = entry != NULL;
                    processCurrentValue(next_valid);
#endif
                    return *this;
                }

                // Prefix increment
                const_iterator operator++()
                {
                    const_iterator tmp = *this;
                    (*this)++;
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
                        SYSTEMTIME stUTC; //, stLocal;
                        FileTimeToSystemTime(&findfiledata.ftLastWriteTime, &stUTC);
                        // SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
                        fileInfo.lastWriteTime = Date::FromSystemTime_win32(stUTC);

                        FileTimeToSystemTime(&findfiledata.ftCreationTime, &stUTC);
                        // SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
                        fileInfo.creationTime = Date::FromSystemTime_win32(stUTC);

                        fileInfo.size =
                            ((uint64_t)findfiledata.nFileSizeHigh << 32) | (uint64_t)findfiledata.nFileSizeLow & UINT64_C(0xffffffff);
                    }
#elif defined(__APPLE__) || defined(__linux__)
                    if (dp == NULL)
                        return;

                    // skip . and ..
                    struct statx sb;
                    int dirfd = AT_FDCWD;
                    // unsigned int mask = STATX_ALL;
                    unsigned int mask = STATX_MODE | STATX_MTIME | STATX_SIZE | STATX_BTIME;
                    int flags = AT_SYMLINK_NOFOLLOW;
                    bool stat_success = false;

                    if (next_valid)
                    {

                        fileInfo.full_path = fileInfo.base_path + entry->d_name;
                        stat_success = statx(dirfd, fileInfo.full_path.c_str(), flags, mask, &sb) == 0;

                        // read next until a valid stat file stated
                        while (next_valid && !stat_success)
                        {
                            // printf("cannot stat: %s\n", entry->d_name);
                            entry = readdir(dp);
                            next_valid = entry != NULL;
                            if (next_valid)
                            {
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
                        if (next_valid)
                        {
                            fileInfo.full_path = fileInfo.base_path + entry->d_name;
                            stat_success = statx(dirfd, fileInfo.full_path.c_str(), flags, mask, &sb) == 0;

                            // read next until a valid stat file stated
                            while (next_valid && !stat_success)
                            {
                                // printf("cannot stat: %s\n", entry->d_name);
                                entry = readdir(dp);
                                next_valid = entry != NULL;
                                if (next_valid)
                                {
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
                        fileInfo = value_type();
                    }
                    else
                    {
                        // use sb to fill the file properties
                        fileInfo.isDirectory = sb.stx_mode & S_IFDIR;
                        fileInfo.isFile = !fileInfo.isDirectory;
                        fileInfo.name = entry->d_name;
                        // fileInfo.full_path = fileInfo.base_path + fileInfo.name;
                        if (fileInfo.isDirectory)
                            fileInfo.full_path += "/";

                        // date processing
                        fileInfo.lastWriteTime = Date::FromUnixTimestampUTC(
                            sb.stx_mtime.tv_sec,
                            sb.stx_mtime.tv_nsec
                        );

                        fileInfo.creationTime = Date::FromUnixTimestampUTC(
                            sb.stx_btime.tv_sec,
                            sb.stx_btime.tv_nsec
                        );

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

                const_iterator(const_iterator &&v) noexcept
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
                    v.fileInfo = value_type();
                }

                void operator=(const_iterator &&v) noexcept
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
                    v.fileInfo = value_type();
                }

#if defined(_WIN32)
                WIN32_FIND_DATAW findfiledata;
                HANDLE hFind;
#elif defined(__APPLE__) || defined(__linux__)
                struct dirent *entry;
                DIR *dp;
#endif

                value_type fileInfo;
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
                // ITKCommon::StringUtil::replaceAll(&this->base_path, "/", "/");
                if (!ITKCommon::StringUtil::endsWith(this->base_path, "/"))
                    this->base_path += "/";
            }

            static Directory FromFile(const FileSystem::File &file) {
                if (file.isDirectory)
                    return Directory(file.full_path);
                return Directory(file.base_path);
            }
        };

    }
}