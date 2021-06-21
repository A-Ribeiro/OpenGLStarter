#include "PlatformPath.h"

#include <stdlib.h>
#include <string>
#include <algorithm>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "mmsystem.h"

#include <Shlwapi.h>
#include <Iphlpapi.h>


    #include "direct.h"
    #define PATH_SEP '\\'
    #define GETCWD _getcwd
    #define CHDIR _chdir


    //#include <windows.h>

    #include <stdio.h>
    #include <tchar.h>


    //#define NTDDI_VERSION NTDDI_LONGHORN
    #define SHGetFolderPath  SHGetFolderPathW
    #ifndef PathAppend
        #define PathAppend              PathAppendW
    #endif
    #ifndef CSIDL_PERSONAL
        #define CSIDL_PERSONAL                  0x0005        // My Documents
    #endif

    //#include <shlobj.h>
    //#include <shlwapi.h>

    #define INITGUID
    //#include <knownfolders.h>

/*
    typedef struct _DLLVERSIONINFO
    {
        DWORD cbSize;
        DWORD dwMajorVersion;                   // Major version
        DWORD dwMinorVersion;                   // Minor version
        DWORD dwBuildNumber;                    // Build number
        DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
    } DLLVERSIONINFO;
*/

    typedef GUID KNOWNFOLDERID;
    #define REFKNOWNFOLDERID const KNOWNFOLDERID &

    /*
    EXTERN_C HRESULT STDAPICALLTYPE
    SHGetKnownFolderPath(REFKNOWNFOLDERID rfid,
                             DWORD dwFlags,
                             HANDLE hToken,
                             PWSTR *ppszPath); // free *ppszPath with CoTaskMemFree
    */
    HRESULT(_stdcall *SHGetKnownFolderPath)(
        REFKNOWNFOLDERID rfid,
        DWORD dwFlags,
        HANDLE hToken,
        PWSTR *ppszPath
        );

    #ifndef __out_ecount
        #define __out_ecount(v)
        #define __inout_ecount(v)
    #endif

    EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
    SHGetFolderPathW(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, __out_ecount(MAX_PATH) LPWSTR pszPath);

    #ifndef LWSTDAPI_
        #define LWSTDAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
    #endif

    #if defined(__MINGW32__)
        LWSTDAPI_(WINBOOL) PathAppendW(LPWSTR pszPath,LPCWSTR pMore);
    #else
        LWSTDAPI_(BOOL)     PathAppendW(__inout_ecount(MAX_PATH) LPWSTR pszPath, LPCWSTR pMore);
    #endif

    bool useKnownFolders(void) {
        static bool result = false;
        static bool count = 0;
        HINSTANCE shell32;
        if (count)
            return result;
        shell32 = LoadLibrary(TEXT("shell32.dll"));
        if (shell32) {
            //FARPROC DllGetVersion;
            int(__stdcall *DllGetVersion)(DLLVERSIONINFO *pdvi);
            DLLVERSIONINFO version;
            version.cbSize = sizeof(DLLVERSIONINFO);
            //DllGetVersion = GetProcAddress(shell32, TEXT("DllGetVersion"));
            SHGetKnownFolderPath = (HRESULT(_stdcall *)(
                REFKNOWNFOLDERID rfid,
                DWORD dwFlags,
                HANDLE hToken,
                PWSTR *ppszPath
                ))GetProcAddress(shell32, "SHGetKnownFolderPath");
            DllGetVersion = (int(__stdcall *)(DLLVERSIONINFO *pdvi))GetProcAddress(shell32, "DllGetVersion");
            if (!DllGetVersion)
                result = false;
            else { // GetProcAddress succeeded
                result = true;
                DllGetVersion(&version);
                if (version.dwMajorVersion >= 6) {
                    if (version.dwMajorVersion == 6 && version.dwMinorVersion == 0) {
                        if (version.dwBuildNumber < 6000) {
                            result = false;
                        }
                    }
                }
                else { // version.dwMajorVersion < 6
                    result = false;
                }
            }
            FreeLibrary(shell32);
        }
        else {
            result = false;
        }
        count = true;
        return result;
    }

    #ifdef INITGUID
    #define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
            EXTERN_C const GUID DECLSPEC_SELECTANY name \
                    = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
    #else
    #define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
            EXTERN_C const GUID name
    #endif // INITGUID

    // {4C5C32FF-BB9D-43b0-B5B4-2D72E54EAAA4}
    DEFINE_KNOWN_FOLDER(FOLDERID_SavedGames, 0x4c5c32ff, 0xbb9d, 0x43b0, 0xb5, 0xb4, 0x2d, 0x72, 0xe5, 0x4e, 0xaa, 0xa4);

    // {FDD39AD0-238F-46AF-ADB4-6C85480369C7}
    DEFINE_KNOWN_FOLDER(FOLDERID_Documents, 0xFDD39AD0, 0x238F, 0x46AF, 0xAD, 0xB4, 0x6C, 0x85, 0x48, 0x03, 0x69, 0xC7);

    #define _TRUNCATE ((size_t)-1)

    #include <stdio.h>
    #include <stdlib.h>
    #include <locale.h>

    /*
     * This function sets the string to either %USERPROFILE%\My Documents\My Games\<baseDirName>\<gameDirName>
     * (Windows XP or older) or to %USERPROFILE\Saved Games\<baseDirName>\<gameDirName> (Vista and newer).
     */
    void getSaveGameDir(char* dest, size_t size, const wchar_t* baseDirName, const wchar_t* gameDirName)
    {
        WCHAR szPath[MAX_PATH];
        if (useKnownFolders()) { // Vista+ has a "Saved Games" known folder
            PWSTR wdest;
            SHGetKnownFolderPath(FOLDERID_SavedGames, 0, NULL, &wdest);
            //copy to szpath
            //wsprintfW(szPath, TEXT("%s"), wdest);
            wsprintfW(szPath, L"%s", wdest);
            CoTaskMemFree(wdest);
        }
        else { // use My Documents\My Games\<baseDirName>
            SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
            //PathAppendW(szPath, TEXT("My Games"));
            PathAppendW(szPath, L"My Games");
            CreateDirectoryW(szPath, NULL);
        }
        PathAppendW(szPath, baseDirName);
        CreateDirectoryW(szPath, NULL);
        PathAppendW(szPath, gameDirName);
        CreateDirectoryW(szPath, NULL);

        memset(dest, 0, size);

        #if __GNUC__

        wcstombs(dest, szPath, size);

        #else

        size_t convertedChars;
        size_t inputLength = wcslen(szPath);
        wcstombs_s( &convertedChars, dest, size, szPath, inputLength);

        #endif
    }

    void getDocumentsDir(char* dest, size_t size, const wchar_t* baseDirName, const wchar_t* gameDirName)
    {
        WCHAR szPath[MAX_PATH];
        if (useKnownFolders()) { // Vista+ has a "Saved Games" known folder
            PWSTR wdest;
            SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &wdest);
            //copy to szpath
            //wsprintfW(szPath, TEXT("%s"), wdest);
            wsprintfW(szPath, L"%s", wdest);
            CoTaskMemFree(wdest);
        }
        else { // use My Documents\My Games\<baseDirName>
            SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
            //PathAppendW(szPath, L"My Games");
            //CreateDirectoryW(szPath, NULL);
        }
        PathAppendW(szPath, baseDirName);
        CreateDirectoryW(szPath, NULL);
        PathAppendW(szPath, gameDirName);
        CreateDirectoryW(szPath, NULL);

        memset(dest, 0, size);

#if __GNUC__

        wcstombs(dest, szPath, size);

#else

        size_t convertedChars;
        size_t inputLength = wcslen(szPath);
        wcstombs_s(&convertedChars, dest, size, szPath, inputLength);

#endif
    }

#else


#include "unistd.h"
#define PATH_SEP '/'
#define GETCWD getcwd
#define CHDIR chdir


#include <sys/types.h>
#include <pwd.h>

#include <errno.h>

std::string unixHomeDirectory() {
    size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)
        bufsize = 0x4000; // = all zeroes with the 14th bit set (1 << 14)

    char *buf = (char*)malloc(bufsize);

    ARIBEIRO_ABORT(buf == NULL, "malloc\n");
    //if (buf == NULL) {
    //    perror("malloc");
    //    exit(EXIT_FAILURE);
    //}

    passwd pwd;
    passwd *result;

    int s = getpwuid_r(getuid(), &pwd, buf, bufsize, &result);

    ARIBEIRO_ABORT(result == NULL, "getpwnam_r errorcode: %i\n", s);
    //if (result == NULL) {
    //    if (s == 0)
    //        printf("Not found\n");
    //    else {
    //        errno = s;
    //        perror("getpwnam_r");
    //    }
    //    exit(EXIT_FAILURE);
    //}

    std::string resultstr = std::string(result->pw_dir);

    free(buf);

    return resultstr;
}

#include <sys/time.h>
#include <time.h>
//#include <android/log.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>

bool unixIsDirectory(const char* path) {
    if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0)
        return false;
    struct stat st;
    bool pathExists = (stat(path, &st) == 0);
    return pathExists && S_ISDIR(st.st_mode);
}

bool unixIsFile(const char* path) {
    struct stat st;
    bool pathExists = (stat(path, &st) == 0);
    return pathExists && S_ISREG(st.st_mode);
}

bool unixCreateDirectory(const char* path) {
    return (mkdir(path, 0777) == 0);
}

void unixCheckPathCreation(const char* path) {
    //LOGI("Checking path (%s)",path);
    if (!unixIsDirectory(path)) {
        //LOGI("  Path does not exists...");
        if (unixCreateDirectory(path)) {
            //LOGI("  Created!!!");
        }
        else {
            //LOGI("  Error to create!!!");
        }
    }
    else {
        //LOGI("  Path exists!!!");
    }
}

bool unixRemovePath(const char* directory_name) {
    DIR*            dp;
    struct dirent*  ep;
    char            p_buf[512] = { 0 };

    dp = opendir(directory_name);

    while ((ep = readdir(dp)) != NULL) {
        sprintf(p_buf, "%s/%s", directory_name, ep->d_name);
        if (unixIsDirectory(p_buf))
            unixRemovePath(p_buf);
        else
            unlink(p_buf);
    }

    closedir(dp);
    rmdir(directory_name);

    return true;
}

void unixCleanUpPath(const char* path) {
    //LOGI("Cleaning up path (%s)",path);
    if (unixRemovePath(path)) {
        //LOGI("  Path removed...");
        unixCheckPathCreation(path);
    }
    else {
        //LOGI("  Cannot remove path!!!");
    }
}



#endif

namespace aRibeiro {

#ifdef _WIN32
    std::string PlatformPath::SEPARATOR = "\\";
#else
    std::string PlatformPath::SEPARATOR = "/";
#endif

    std::string PlatformPath::getExecutablePath(const char* argv0) {
        return getExecutablePath(std::string(argv0));
    }

    std::string PlatformPath::getExecutablePath(const std::string &arg0) {
        //std::string path = argv0;
        size_t path_directory_index = arg0.find_last_of(PATH_SEP);
        return arg0.substr(0, path_directory_index);
    }

    std::string PlatformPath::getWorkingPath() {
        const int BUFSIZE = 4096;
        char buf[BUFSIZE];
        memset(buf, 0, BUFSIZE);
        GETCWD(buf, BUFSIZE - 1);
        return std::string(buf);
    }

    bool PlatformPath::setWorkingPath(const std::string &path) {
        return CHDIR(path.c_str()) == 0;
    }

    std::string PlatformPath::getSaveGamePath(const std::string &rootFolder, const std::string &gameName) {

#ifdef _WIN32
        std::wstring base = std::wstring(rootFolder.begin(), rootFolder.end());
        std::wstring game = std::wstring(gameName.begin(), gameName.end());

        char resultChar[4096];

        getSaveGameDir(resultChar, 4096, base.c_str(), game.c_str());

        return std::string(resultChar);
#else

        std::string home = unixHomeDirectory();

        home += std::string("/.") + rootFolder;

        unixCheckPathCreation(home.c_str());

        home += std::string("/") + gameName;

        unixCheckPathCreation(home.c_str());


        return  home;

#endif

    }

    std::string PlatformPath::getDocumentsPath(const std::string &rootFolder, const std::string &appName) {

#ifdef _WIN32
        std::wstring base = std::wstring(rootFolder.begin(), rootFolder.end());
        std::wstring app = std::wstring(appName.begin(), appName.end());

        char resultChar[4096];

        getDocumentsDir(resultChar, 4096, base.c_str(), app.c_str());

        return std::string(resultChar);
#else

        std::string home = unixHomeDirectory();

        home += std::string("/.") + rootFolder;

        unixCheckPathCreation(home.c_str());

        home += std::string("/") + appName;

        unixCheckPathCreation(home.c_str());


        return  home;

#endif

    }


    bool PlatformPath::isDirectory(const std::string &path) {
#ifdef _WIN32
        return PathIsDirectoryA(path.c_str()) == TRUE;
#else
        return unixIsDirectory(path.c_str());
#endif
    }

    bool PlatformPath::isFile(const std::string &path) {
#ifdef _WIN32
        return PathFileExistsA(path.c_str()) && !isDirectory(path);
#else
        return unixIsFile(path.c_str());
#endif
    }


    void PlatformPath::splitPathString(const std::string &_input, std::string *outFolder, std::string *outFilename, std::string *outFileWOExt, std::string *outFileExt) {

        //copy the parameter to use replace afterwards
        std::string input = _input;

        //
        // normalize path separator
        //
        std::replace(input.begin(), input.end(), '\\', PlatformPath::SEPARATOR[0]);
        std::replace(input.begin(), input.end(), '/', PlatformPath::SEPARATOR[0]);


        std::string folder, filename, filename_wo_ext, ext;
        size_t path_directory_index = input.find_last_of(PlatformPath::SEPARATOR[0]);
        if (path_directory_index == -1) {
            folder = ".";
            filename = input;
        }
        else {
            folder = input.substr(0, path_directory_index);
            filename = input.substr(path_directory_index + 1, input.size() - 1 - path_directory_index);
        }


        path_directory_index = filename.find_last_of('.');
        if (path_directory_index == -1) {
            filename_wo_ext = filename;
            ext = "";
        }
        else {
            filename_wo_ext = filename.substr(0, path_directory_index);
            ext = filename.substr(path_directory_index + 1, filename.size() - 1 - path_directory_index);
        }



        *outFolder = folder;
        *outFilename = filename;
        *outFileWOExt = filename_wo_ext;
        *outFileExt = ext;

    }

}
